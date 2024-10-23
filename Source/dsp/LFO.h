/*
  ==============================================================================

    LFO.h
    Created: 3 Sep 2024 11:39:41am
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../service/ParamIDs.h"
#include "../service/ParameterHelper.h"
#include "../ui/LFOVisualizer.h"

class LFO : public juce::ValueTree::Listener
{
public:
    
    LFO (juce::RangedAudioParameter& t_freqTimeParam, juce::RangedAudioParameter& t_freqSyncParam, juce::RangedAudioParameter& t_rangeParam, juce::ValueTree t_lfoTree, std::atomic<float>& t_bpm);
    
    ~LFO () override;
    
    void prepare (juce::dsp::ProcessSpec spec);
    
    void processToOutput (const int numSamples);
    
    template <typename AudioParameterType>
    static float processLFOSample (AudioParameterType* parameter, juce::ValueTree lfoModTree, const float& parameterValueNorm, const float& lfo1Value, const float& lfo2Value)
    {
        float lfo1Offset = 0.0f, lfo2Offset = 0.0f;
        
        juce::ValueTree lfo1ModTree = lfoModTree.getChildWithName(ParamIDs::lfo1);
        juce::ValueTree lfo2ModTree = lfoModTree.getChildWithName(ParamIDs::lfo2);
        
        if (lfo1ModTree.getProperty(ParamIDs::lfoOn))
        {
            const float lfo1Range = lfo1ModTree.getProperty(ParamIDs::lfoRange);
            lfo1Offset = lfo1Value * lfo1Range;
            
            if (lfo1ModTree.getProperty(ParamIDs::lfoDirection) == ParamIDs::lfoUnidirectional)
                lfo1Offset = (lfo1Offset + lfo1Range) / 2.0f;
        }
        if (lfo2ModTree.getProperty(ParamIDs::lfoOn))
        {
            const float lfo2Range = lfo2ModTree.getProperty(ParamIDs::lfoRange);
            lfo2Offset = lfo2Value * lfo2Range;
            
            if (lfo2ModTree.getProperty(ParamIDs::lfoDirection) == ParamIDs::lfoUnidirectional)
                lfo2Offset = (lfo2Offset + lfo2Range) / 2.0f;
        }
        
        return parameter->convertFrom0to1(juce::jlimit(0.0f, 1.0f, parameterValueNorm + lfo1Offset + lfo2Offset));
    }
    
    LFOVisualizer visualizer;
    juce::AudioBuffer<float> buffer;
    
private:
    void updateOsc();
    void handleSPBUpdate (float newFreq);
    
    void frequencyTimeChanged (float newFreqTime);
    void frequencySyncChanged (float newFreqSync);
    void rangeChanged (float newRange);
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;
    
    juce::RangedAudioParameter& freqTimeParam;
    juce::RangedAudioParameter& freqSyncParam;
    juce::RangedAudioParameter& rangeParam;
    juce::ParameterAttachment freqTimeAttachment;
    juce::ParameterAttachment freqSyncAttachment;
    juce::ParameterAttachment rangeAttachment;
    
    juce::ValueTree lfoTree;
    
    juce::dsp::Oscillator<float> osc;
    int waveNum = 0;
    std::atomic<bool> syncState { false };
    std::atomic<float> freqTime, freqSync;
    float range = 1.0f;
    std::atomic<float>& bpm;
    float sampleRate = 0.0f;
    int bufferSize = 100;
    float slowVisualizerSecondsPerScreenWidth { 2.0f }, fastVisualizerSecondsPerScreenWidth { 0.7f };
    const float spbCutoffFreq = 15.0f;
};
