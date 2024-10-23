/*
  ==============================================================================

    DelayProcessor.h
    Created: 18 Sep 2024 4:30:05pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include "ProcessorBase.h"
#include "LFO.h"
#include "../service/ParameterHelper.h"
#include "../service/ParamIDs.h"

class DelayProcessor  : public ProcessorBase
                      , public juce::ValueTree::Listener
{
public:
    DelayProcessor(juce::AudioProcessorValueTreeState& apvts,
                   juce::ValueTree paramTree,
                   LFO& lfo1,
                   LFO& lfo2,
                   std::atomic<float>& bpm);
    
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void processBlock (juce::AudioSampleBuffer& buffer, juce::MidiBuffer&) override;
    
    void updateDelay();
    void updateDelayFilter();
    
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;
    
    const juce::String getName() const override { return "Delay Processor"; }
    
private:
    juce::AudioProcessorValueTreeState& apvts;
    juce::ValueTree delayTree;
    juce::ValueTree lfoTree;
    LFO& lfo1;
    LFO& lfo2;
    
    juce::AudioParameterFloat* leftDelayTime { nullptr };
    juce::AudioParameterFloat* rightDelayTime { nullptr };
    juce::AudioParameterChoice* leftDelaySync { nullptr };
    juce::AudioParameterChoice* rightDelaySync { nullptr };
    juce::AudioParameterFloat* leftDelayOffset { nullptr };
    juce::AudioParameterFloat* rightDelayOffset { nullptr };
    juce::AudioParameterFloat* delayFilterCenter { nullptr };
    juce::AudioParameterFloat* delayFilterWidth { nullptr };
    juce::AudioParameterFloat* delayFeedback { nullptr };
    juce::AudioParameterFloat* delayMix { nullptr };
    
    std::atomic<float>& bpm;
    
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd> delayL;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd> delayR;
    const float maxDelayInSeconds { 3.0f };
    
    juce::dsp::DryWetMixer<float> delayMixer;
    juce::AudioBuffer<float> delayBuffer;
    
    juce::SmoothedValue<float> leftDelaySmoothed;
    juce::SmoothedValue<float> rightDelaySmoothed;
    const float smoothingTimeInSeconds { 0.1f };
    
    std::atomic<bool> linkState;
    std::atomic<bool> leftSyncState;
    std::atomic<bool> rightSyncState;
    
    juce::dsp::StateVariableTPTFilter<float> lowpassDelayFilter;
    juce::dsp::StateVariableTPTFilter<float> highpassDelayFilter;
    std::atomic<bool> filterOn;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayProcessor);
};
