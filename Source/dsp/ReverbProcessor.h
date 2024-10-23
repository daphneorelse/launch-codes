/*
  ==============================================================================

    ReverbProcessor.h
    Created: 18 Sep 2024 4:30:32pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include "ProcessorBase.h"
#include "LFO.h"
#include "../service/ParameterHelper.h"
#include "../service/ParamIDs.h"

class ReverbProcessor  : public ProcessorBase
{
public:
    ReverbProcessor(juce::AudioProcessorValueTreeState& apvts,
                    juce::ValueTree paramTree,
                    LFO& lfo1,
                    LFO& lfo2);
    
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void processBlock (juce::AudioSampleBuffer& buffer, juce::MidiBuffer&) override;
    
    void updateReverb();
    
    const juce::String getName() const override { return "Reverb Processor"; }
    
private:
    juce::AudioProcessorValueTreeState& apvts;
    juce::ValueTree lfoTree;
    LFO& lfo1;
    LFO& lfo2;
    
    juce::AudioParameterFloat* reverbSize { nullptr };
    juce::AudioParameterFloat* reverbDamping { nullptr };
    juce::AudioParameterFloat* reverbWidth { nullptr };
    juce::AudioParameterFloat* reverbMix { nullptr };
    
    juce::dsp::Reverb reverb;
    juce::Reverb::Parameters reverbParameters;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReverbProcessor);
};
