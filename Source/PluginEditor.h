/*
  ==============================================================================
    PluginEditor.h
    One-Knob Opto Compressor Vintage Rack Faceplate GUI
  ==============================================================================
*/

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

class OneKnobOptoAudioProcessorEditor : public juce::AudioProcessorEditor,
                                        private juce::Timer
{
public:
    explicit OneKnobOptoAudioProcessorEditor (OneKnobOptoAudioProcessor&);
    ~OneKnobOptoAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    OneKnobOptoAudioProcessor& audioProcessor;

    // Primary Knob
    juce::Slider squeezeKnob;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> squeezeAttachment;

    // Switches
    juce::ToggleButton autoMakeupButton { "Auto Makeup" };
    juce::ToggleButton tubeWarmthButton { "Tube Warmth" };
    juce::ToggleButton scFilterButton   { "80Hz HPF" };
    juce::ToggleButton bypassButton     { "Bypass" };

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> autoMakeupAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> tubeWarmthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> scFilterAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttachment;

    // Meter needle animation state
    float meterAngle = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OneKnobOptoAudioProcessorEditor)
};
