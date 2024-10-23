/*
  ==============================================================================

    WaveshaperProcessor.h
    Created: 18 Sep 2024 4:30:46pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include "ProcessorBase.h"
#include "LFO.h"
#include "../service/ParameterHelper.h"
#include "../service/ParamIDs.h"

class WaveshaperProcessor  : public ProcessorBase
                           , public juce::AudioProcessorValueTreeState::Listener
                           , public juce::ValueTree::Listener
{
public:
    WaveshaperProcessor(juce::AudioProcessorValueTreeState& apvts,
                        juce::ValueTree paramTree,
                        LFO& lfo1,
                        LFO& lfo2,
                        juce::dsp::LookupTableTransform<float>& waveshaperControlTable,
                        juce::dsp::LookupTableTransform<float>& waveshaperTransformTable);
    
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void processBlock (juce::AudioSampleBuffer& buffer, juce::MidiBuffer&) override;
    
    void updateWaveshaperControlTable();
    void updateWaveshaperTransformTable();
    
    void parameterChanged (const juce::String& parameterID, float newValue) override;
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;
    
    const juce::String getName() const override { return "Waveshaper Processor"; }
    
private:
    juce::AudioProcessorValueTreeState& apvts;
    juce::ValueTree waveshaperNodeTree;
    juce::ValueTree waveshaperToolTree;
    juce::ValueTree lfoTree;
    LFO& lfo1;
    LFO& lfo2;
    
    juce::AudioParameterFloat* waveshaperXTRansform { nullptr };
    juce::AudioParameterFloat* waveshaperYTRansform { nullptr };
    juce::AudioParameterFloat* waveshaperInputGain { nullptr };
    juce::AudioParameterFloat* waveshaperOutputGain { nullptr };
    juce::AudioParameterFloat* waveshaperMix { nullptr };
    
    juce::dsp::Gain<float> waveshaperInputGainDSP;
    juce::dsp::Gain<float> waveshaperOutputGainDSP;
    juce::dsp::DryWetMixer<float> waveshaperMixer;
    
    juce::dsp::LookupTableTransform<float>& waveshaperControlTable;
    juce::dsp::LookupTableTransform<float>& waveshaperTransformTable;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveshaperProcessor);
};
