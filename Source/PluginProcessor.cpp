/*
  ==============================================================================
    PluginProcessor.cpp
  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

OneKnobOptoAudioProcessor::OneKnobOptoAudioProcessor()
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
       apvts (*this, nullptr, "Parameters", createParameterLayout())
{
}

OneKnobOptoAudioProcessor::~OneKnobOptoAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout OneKnobOptoAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // 1. One-Knob Squeeze (0% to 100%)
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID ("squeeze", 1),
        "Squeeze",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f, 0.8f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel ("%")));

    // 2. Auto Makeup Volume Compensation (Default ON)
    layout.add (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID ("automakeup", 1),
        "Auto Makeup",
        true));

    // 3. Vintage Tube / Transformer Warmth
    layout.add (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID ("tubewarmth", 1),
        "Tube Warmth",
        true));

    // 4. 80Hz Sidechain High-Pass Filter
    layout.add (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID ("scfilter", 1),
        "Sidechain Filter",
        true));

    // 5. Bypass
    layout.add (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID ("bypass", 1),
        "Bypass",
        false));

    return layout;
}

const juce::String OneKnobOptoAudioProcessor::getName() const { return "OneKnobOpto"; }
bool OneKnobOptoAudioProcessor::acceptsMidi() const { return false; }
bool OneKnobOptoAudioProcessor::producesMidi() const { return false; }
bool OneKnobOptoAudioProcessor::isMidiEffect() const { return false; }
double OneKnobOptoAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int OneKnobOptoAudioProcessor::getNumPrograms() { return 1; }
int OneKnobOptoAudioProcessor::getCurrentProgram() { return 0; }
void OneKnobOptoAudioProcessor::setCurrentProgram (int) {}
const juce::String OneKnobOptoAudioProcessor::getProgramName (int) { return {}; }
void OneKnobOptoAudioProcessor::changeProgramName (int, const juce::String&) {}

void OneKnobOptoAudioProcessor::prepareToPlay (double sampleRate, int /*samplesPerBlock*/)
{
    optoEngine.prepare (sampleRate);
}

void OneKnobOptoAudioProcessor::releaseResources()
{
    optoEngine.reset();
}

bool OneKnobOptoAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void OneKnobOptoAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    const bool bypass = apvts.getRawParameterValue ("bypass")->load() > 0.5f;
    if (bypass)
    {
        currentGrDb.store (0.0f);
        return;
    }

    const float squeezeNorm = apvts.getRawParameterValue ("squeeze")->load() / 100.0f;
    const bool autoMakeup   = apvts.getRawParameterValue ("automakeup")->load() > 0.5f;
    const bool tubeWarmth   = apvts.getRawParameterValue ("tubewarmth")->load() > 0.5f;
    const bool scFilter     = apvts.getRawParameterValue ("scfilter")->load() > 0.5f;

    float* leftChannel = buffer.getWritePointer (0);
    float* rightChannel = (totalNumInputChannels > 1) ? buffer.getWritePointer (1) : nullptr;

    // Process audio through Opto Engine
    optoEngine.process (leftChannel, rightChannel, buffer.getNumSamples(),
                        squeezeNorm, autoMakeup, tubeWarmth, scFilter);

    currentGrDb.store (optoEngine.getCurrentGainReductionDb());
}

bool OneKnobOptoAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* OneKnobOptoAudioProcessor::createEditor()
{
    return new OneKnobOptoAudioProcessorEditor (*this);
}

void OneKnobOptoAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void OneKnobOptoAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr && xmlState->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

// Plugin instantiation export
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OneKnobOptoAudioProcessor();
}
