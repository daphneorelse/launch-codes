/*
  ==============================================================================

    FilterProcessor.h
    Created: 18 Sep 2024 4:30:59pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include "ProcessorBase.h"
#include "LFO.h"
#include "../service/ParameterHelper.h"
#include "../service/ParamIDs.h"

class FilterProcessor  : public ProcessorBase
                       , public juce::ValueTree::Listener
{
public:
    FilterProcessor(juce::AudioProcessorValueTreeState& apvts,
                    juce::ValueTree paramTree,
                    LFO& lfo1,
                    LFO& lfo2);
    
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void processBlock (juce::AudioSampleBuffer& buffer, juce::MidiBuffer&) override;
    
    void updateFilters(juce::dsp::IIR::Coefficients<float>& lowpassCoeffs, juce::dsp::IIR::Coefficients<float>& highpassCoeffs, const int lowpassMode, const int highpassMode);
    
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;
    
    const juce::String getName() const override { return "Filter Processor"; }
    
private:
    juce::AudioProcessorValueTreeState& apvts;
    juce::ValueTree filterTree;
    juce::ValueTree lfoTree;
    LFO& lfo1;
    LFO& lfo2;
    
    juce::AudioParameterFloat* lowpassCutoff { nullptr };
    juce::AudioParameterFloat* lowpassQ { nullptr };
    juce::AudioParameterFloat* lowpassGain { nullptr };
    juce::AudioParameterFloat* highpassCutoff { nullptr };
    juce::AudioParameterFloat* highpassQ { nullptr };
    juce::AudioParameterFloat* highpassGain { nullptr };
    juce::AudioParameterFloat* mix { nullptr };
    
    std::atomic<int> lowFilterType;
    std::atomic<int> highFilterType;
    
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> lowpassFilter, highpassFilter;
    
    juce::dsp::DryWetMixer<float> mixer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterProcessor);
};
