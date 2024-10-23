/*
  ==============================================================================

    ReverbProcessor.cpp
    Created: 18 Sep 2024 4:30:32pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#include "ReverbProcessor.h"

ReverbProcessor::ReverbProcessor(juce::AudioProcessorValueTreeState& t_apvts,
                                 juce::ValueTree paramTree,
                                 LFO& t_lfo1,
                                 LFO& t_lfo2)
: apvts(t_apvts)
, lfoTree(paramTree.getChildWithName(ParamIDs::lfoTree))
, lfo1(t_lfo1)
, lfo2(t_lfo2)
{
    ParameterHelper::castParameter (apvts, ParamIDs::reverbSize, reverbSize);
    ParameterHelper::castParameter (apvts, ParamIDs::reverbDamping, reverbDamping);
    ParameterHelper::castParameter (apvts, ParamIDs::reverbWidth, reverbWidth);
    ParameterHelper::castParameter (apvts, ParamIDs::reverbMix, reverbMix);
}

void ReverbProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;

    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    
    reverb.prepare(spec);
}

void ReverbProcessor::processBlock (juce::AudioSampleBuffer& buffer, juce::MidiBuffer&)
{
    updateReverb();
    
    juce::dsp::AudioBlock<float> block (buffer);
    reverb.process(juce::dsp::ProcessContextReplacing<float> (block));
}

void ReverbProcessor::updateReverb()
{
    const float lfo1Val = lfo1.buffer.getSample(0, 0), lfo2Val = lfo2.buffer.getSample(0, 0);
    
    reverbParameters.roomSize = LFO::processLFOSample(reverbSize,
                                                      lfoTree.getChildWithName(ParamIDs::lfoReverbSize),
                                                      reverbSize->convertTo0to1(reverbSize->get()),
                                                      lfo1Val,
                                                      lfo2Val);
    reverbParameters.damping = LFO::processLFOSample(reverbDamping,
                                                     lfoTree.getChildWithName(ParamIDs::lfoReverbDamping),
                                                     reverbDamping->convertTo0to1(reverbDamping->get()),
                                                     lfo1Val,
                                                     lfo2Val);
    reverbParameters.width = LFO::processLFOSample(reverbWidth,
                                                   lfoTree.getChildWithName(ParamIDs::lfoReverbWidth),
                                                   reverbWidth->convertTo0to1(reverbWidth->get()),
                                                   lfo1Val,
                                                   lfo2Val);
    
    const float mixVal = LFO::processLFOSample(reverbMix,
                                               lfoTree.getChildWithName(ParamIDs::lfoReverbMix),
                                               reverbMix->convertTo0to1(reverbMix->get()),
                                               lfo1Val,
                                               lfo2Val);
    reverbParameters.wetLevel = mixVal;
    reverbParameters.dryLevel = 1.0f - mixVal;
    reverb.setParameters(reverbParameters);
}
