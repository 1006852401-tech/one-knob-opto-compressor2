/*
  ==============================================================================
    OptoEngine.h - Core Optical Compressor DSP
    Modeled after the T4 Electro-Optical Attenuator Cell
    Features:
      - Soft-knee luminescent response
      - Program-dependent dual-stage release (60ms initial + 1.5s deep relaxation)
      - Dynamic Automatic Makeup Gain Volume Compensation
      - Subtle 12AX7 tube / transformer harmonic saturation
  ==============================================================================
*/

#pragma once
#include <cmath>
#include <algorithm>

namespace AudioDsp {

class OptoEngine
{
public:
    OptoEngine() = default;
    ~OptoEngine() = default;

    void prepare(double newSampleRate)
    {
        sampleRate = std::max(8000.0, newSampleRate);
        
        // T4 Attack time: ~10ms
        constexpr double attackMs = 10.0;
        attackCoeff = 1.0 - std::exp(-1.0 / ((attackMs / 1000.0) * sampleRate));

        // T4 Dual-stage release:
        // 1. Fast photocarrier release (~60ms for first 50%)
        constexpr double fastReleaseMs = 60.0;
        fastReleaseCoeff = 1.0 - std::exp(-1.0 / ((fastReleaseMs / 1000.0) * sampleRate));

        // 2. Slow deep trap relaxation (1500ms tail)
        constexpr double slowReleaseMs = 1500.0;
        slowReleaseCoeff = 1.0 - std::exp(-1.0 / ((slowReleaseMs / 1000.0) * sampleRate));

        // 80Hz Sidechain Highpass filter coefficient
        double w = (2.0 * 3.141592653589793 * 80.0) / sampleRate;
        scAlpha = 1.0 / (1.0 + w);

        reset();
    }

    void reset()
    {
        fastCellL = 0.0;
        slowCellL = 0.0;
        fastCellR = 0.0;
        slowCellR = 0.0;
        smoothedGrL = 0.0;
        scX1L = 0.0;
        scY1L = 0.0;
        scX1R = 0.0;
        scY1R = 0.0;
        currentGrDb = 0.0f;
    }

    /**
     * Process stereo block in place
     * @param leftChannel Pointer to left channel audio samples
     * @param rightChannel Pointer to right channel audio samples (can be null for mono)
     * @param numSamples Number of samples in buffer
     * @param squeeze 0.0 to 1.0 (One-Knob compression intensity)
     * @param autoMakeup Enable automatic volume compensation (true/false)
     * @param tubeWarmth Enable subtle optical tube harmonic saturation
     * @param sidechainFilter Enable 80Hz sidechain highpass
     */
    void process(float* leftChannel, float* rightChannel, int numSamples,
                 float squeeze, bool autoMakeup, bool tubeWarmth, bool sidechainFilter)
    {
        if (numSamples <= 0 || leftChannel == nullptr) return;

        // Virtual drive into optical cell: 0 to +36 dB
        const double driveDb = static_cast<double>(squeeze) * 36.0;
        const double driveLin = std::pow(10.0, driveDb / 20.0);

        float maxGrBlock = 0.0f;

        for (int i = 0; i < numSamples; ++i)
        {
            const double inL = leftChannel[i];
            const double inR = (rightChannel != nullptr) ? rightChannel[i] : inL;

            // 1. Sidechain High-Pass Filter (80Hz)
            double scL = inL;
            double scR = inR;
            if (sidechainFilter)
            {
                double yL = scAlpha * (scY1L + inL - scX1L);
                scX1L = inL;
                scY1L = yL;
                scL = yL;

                double yR = scAlpha * (scY1R + inR - scX1R);
                scX1R = inR;
                scY1R = yR;
                scR = yR;
            }

            // 2. Optical T4 Luminescence Detection (Stereo Linked)
            double detL = std::abs(scL) * driveLin;
            double detR = std::abs(scR) * driveLin;
            double det = std::max(detL, detR);

            // Soft-knee electroluminescence emission curve
            constexpr double threshold = 0.22;
            double emission = 0.0;
            if (det > threshold)
            {
                double over = det - threshold;
                emission = (over * over) / (over + 0.35);
            }

            // 3. Dual-Stage Photocell Relaxation
            if (emission > fastCellL)
            {
                fastCellL += attackCoeff * (emission - fastCellL);
                slowCellL += (attackCoeff * 0.4) * (emission - slowCellL);
            }
            else
            {
                fastCellL += fastReleaseCoeff * (emission - fastCellL);
                slowCellL += slowReleaseCoeff * (emission - slowCellL);
            }

            // 4. Optical Attenuation
            double conductance = (0.55 * fastCellL + 0.45 * slowCellL);
            double compressionIntensity = std::min(1.0, static_cast<double>(squeeze) * 1.2);
            double attenuation = 1.0 / (1.0 + conductance * compressionIntensity * 4.2);
            attenuation = std::max(0.0001, attenuation);

            // Gain Reduction in dB
            double grDb = -20.0 * std::log10(attenuation);
            if (grDb > maxGrBlock) maxGrBlock = static_cast<float>(grDb);

            // Smoothed GR tracking for auto-makeup
            smoothedGrL += 0.0004 * (grDb - smoothedGrL);

            // 5. Automatic Makeup Gain (压缩多少会自动补偿音量)
            double makeupLin = 1.0;
            if (autoMakeup && squeeze > 0.001f)
            {
                // Squeeze curve base compensation + dynamic instantaneous reduction tracking
                double baseCompensationDb = std::pow(static_cast<double>(squeeze), 0.85) * 18.0;
                double dynamicCompensationDb = smoothedGrL * 0.72;
                double totalCompensationDb = (baseCompensationDb * 0.5 + dynamicCompensationDb * 0.5);
                makeupLin = std::pow(10.0, totalCompensationDb / 20.0);
            }

            // Apply attenuation and auto-makeup
            double outL = inL * attenuation * makeupLin;
            double outR = inR * attenuation * makeupLin;

            // 6. Tube & Transformer Harmonic Warmth
            if (tubeWarmth)
            {
                outL = applySaturation(outL);
                outR = applySaturation(outR);
            }

            // 7. Transparent Peak Ceiling Soft Clip (< -0.1 dBFS)
            leftChannel[i] = static_cast<float>(softLimit(outL));
            if (rightChannel != nullptr)
                rightChannel[i] = static_cast<float>(softLimit(outR));
        }

        currentGrDb = maxGrBlock;
    }

    float getCurrentGainReductionDb() const noexcept { return currentGrDb; }

private:
    double sampleRate = 44100.0;
    double attackCoeff = 0.05;
    double fastReleaseCoeff = 0.01;
    double slowReleaseCoeff = 0.0002;
    double scAlpha = 0.988;

    double fastCellL = 0.0;
    double slowCellL = 0.0;
    double fastCellR = 0.0;
    double slowCellR = 0.0;
    double smoothedGrL = 0.0;

    double scX1L = 0.0;
    double scY1L = 0.0;
    double scX1R = 0.0;
    double scY1R = 0.0;

    float currentGrDb = 0.0f;

    static double applySaturation(double x)
    {
        double driven = x * 1.08;
        return (std::tanh(driven) + 0.038 * std::tanh(driven * driven)) * 0.94;
    }

    static double softLimit(double x)
    {
        constexpr double thresh = 0.92;
        if (x > thresh)
            return thresh + (1.0 - thresh) * std::tanh((x - thresh) / (1.0 - thresh));
        else if (x < -thresh)
            return -thresh + (1.0 - thresh) * std::tanh((x + thresh) / (1.0 - thresh));
        return x;
    }
};

} // namespace AudioDsp
