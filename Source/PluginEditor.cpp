/*
  ==============================================================================
    PluginEditor.cpp
  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

OneKnobOptoAudioProcessorEditor::OneKnobOptoAudioProcessorEditor (OneKnobOptoAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (640, 360);

    // Primary Giant Rotary Knob
    squeezeKnob.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    squeezeKnob.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 24);
    squeezeKnob.setRange (0.0, 100.0, 0.1);
    addAndMakeVisible (squeezeKnob);

    squeezeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.apvts, "squeeze", squeezeKnob);

    // Switches
    auto setupButton = [this](juce::ToggleButton& btn) {
        btn.setColour (juce::ToggleButton::textColourId, juce::Colours::lightgrey);
        addAndMakeVisible (btn);
    };

    setupButton (autoMakeupButton);
    setupButton (tubeWarmthButton);
    setupButton (scFilterButton);
    setupButton (bypassButton);

    autoMakeupAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        audioProcessor.apvts, "automakeup", autoMakeupButton);
    tubeWarmthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        audioProcessor.apvts, "tubewarmth", tubeWarmthButton);
    scFilterAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        audioProcessor.apvts, "scfilter", scFilterButton);
    bypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        audioProcessor.apvts, "bypass", bypassButton);

    startTimerHz (30); // 30fps meter refresh
}

OneKnobOptoAudioProcessorEditor::~OneKnobOptoAudioProcessorEditor()
{
    stopTimer();
}

void OneKnobOptoAudioProcessorEditor::timerCallback()
{
    float grDb = audioProcessor.currentGrDb.load();
    // Needle target angle for Gain Reduction (0dB is right, -20dB is left)
    float target = -juce::jlimit (0.0f, 20.0f, grDb) / 20.0f;
    meterAngle += 0.25f * (target - meterAngle);
    repaint();
}

void OneKnobOptoAudioProcessorEditor::paint (juce::Graphics& g)
{
    // 2U Anodized Charcoal Faceplate
    g.fillAll (juce::Colour (0xff1c1e22));

    // Vintage Top Gold/Silver Accent Strip
    g.setColour (juce::Colour (0xffc2a649));
    g.fillRect (0, 0, getWidth(), 3);

    // Title & Brand
    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (18.0f, juce::Font::bold));
    g.drawText ("OPTO-ONE", 24, 16, 200, 24, juce::Justification::left);

    g.setColour (juce::Colour (0xff8f96a3));
    g.setFont (juce::FontOptions (11.0f));
    g.drawText ("AUTO-MAKEUP OPTICAL COMPRESSOR", 24, 38, 300, 16, juce::Justification::left);

    // Vintage VU Meter Bezel (Right side)
    juce::Rectangle<float> meterBounds (getWidth() - 250.0f, 40.0f, 220.0f, 130.0f);
    g.setColour (juce::Colour (0xff2b241c));
    g.fillRoundedRectangle (meterBounds, 8.0f);

    // Warm Backlit Vintage Dial Face
    juce::Rectangle<float> dialBounds = meterBounds.reduced (6.0f);
    g.setColour (juce::Colour (0xfffae6b8));
    g.fillRoundedRectangle (dialBounds, 6.0f);

    // Meter Needle
    g.setColour (juce::Colour (0xff1a1a1a));
    float pivotX = dialBounds.getCentreX();
    float pivotY = dialBounds.getBottom() + 20.0f;
    float needleLen = 110.0f;
    float rad = (meterAngle * 0.8f + 0.4f); // Angle range
    float endX = pivotX + std::sin (rad) * needleLen;
    float endY = pivotY - std::cos (rad) * needleLen;
    g.drawLine (pivotX, pivotY - 20.0f, endX, endY, 2.0f);

    // Meter Label
    g.setColour (juce::Colour (0xff7a3e20));
    g.setFont (juce::FontOptions (10.0f, juce::Font::bold));
    g.drawText ("GAIN REDUCTION (dB)", meterBounds.getX(), meterBounds.getBottom() - 26, meterBounds.getWidth(), 16, juce::Justification::centred);
}

void OneKnobOptoAudioProcessorEditor::resized()
{
    // Center Giant Squeeze Knob
    squeezeKnob.setBounds (180, 70, 180, 200);

    // Bottom Toggle Switches
    int btnY = getHeight() - 50;
    autoMakeupButton.setBounds (40, btnY, 120, 30);
    tubeWarmthButton.setBounds (170, btnY, 110, 30);
    scFilterButton.setBounds   (290, btnY, 100, 30);
    bypassButton.setBounds     (getWidth() - 110, btnY, 80, 30);
}
