/*
  ==============================================================================
    PluginProcessor.h
    One-Knob Opto Compressor with Auto Makeup Gain
    Compatible with:
      - Apple AudioUnit v2 / AUv3 (Logic Pro, GarageBand)
      - Steinberg VST3 (Ableton, Cubase, Studio One, Reaper)
      - macOS 10.9 Mavericks through macOS 15+ Sequoia
      - Universal Binary (Apple Silicon arm64 + Intel x86_64)
  ==============================================================================
*/

#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "OptoEngine.h"

class OneKnobOptoAudioProcessor : public juce::AudioProcessor
{
public:
    OneKnobOptoAudioProcessor();
    ~OneKnobOptoAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Parameter Tree
    juce::AudioProcessorValueTreeState apvts;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Metering
    std::atomic<float> currentGrDb { 0.0f };
    std::atomic<float> currentInDb { -60.0f };
    std::atomic<float> currentOutDb { -60.0f };

private:
    AudioDsp::OptoEngine optoEngine;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OneKnobOptoAudioProcessor)
};
