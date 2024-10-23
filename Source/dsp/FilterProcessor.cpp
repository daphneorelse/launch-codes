/*
  ==============================================================================

    FilterProcessor.cpp
    Created: 18 Sep 2024 4:30:59pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#include "FilterProcessor.h"

FilterProcessor::FilterProcessor(juce::AudioProcessorValueTreeState& t_apvts,
                                 juce::ValueTree paramTree,
                                 LFO& t_lfo1,
                                 LFO& t_lfo2)
: apvts(t_apvts)
, filterTree(paramTree.getChildWithName(ParamIDs::filterTree))
, lfoTree(paramTree.getChildWithName(ParamIDs::lfoTree))
, lfo1(t_lfo1)
, lfo2(t_lfo2)
{
    ParameterHelper::castParameter (apvts, ParamIDs::filterLowpassCutoff, lowpassCutoff);
    ParameterHelper::castParameter (apvts, ParamIDs::filterLowpassQ, lowpassQ);
    ParameterHelper::castParameter (apvts, ParamIDs::filterLowpassGain, lowpassGain);
    ParameterHelper::castParameter (apvts, ParamIDs::filterHighpassCutoff, highpassCutoff);
    ParameterHelper::castParameter (apvts, ParamIDs::filterHighpassQ, highpassQ);
    ParameterHelper::castParameter (apvts, ParamIDs::filterHighpassGain, highpassGain);
    ParameterHelper::castParameter (apvts, ParamIDs::filterMix, mix);
    
    filterTree.addListener(this);
    lowFilterType.store(filterTree.getProperty(ParamIDs::filterLowpassMode));
    highFilterType.store(filterTree.getProperty(ParamIDs::filterHighpassMode));
}

void FilterProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;

    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    
    lowpassFilter.prepare(spec);
    highpassFilter.prepare(spec);
    
    mixer.prepare(spec);
    mixer.setWetLatency(0.0f);
    mixer.setMixingRule(juce::dsp::DryWetMixingRule::squareRoot4p5dB);
}

void FilterProcessor::processBlock (juce::AudioSampleBuffer& buffer, juce::MidiBuffer&)
{
    juce::dsp::AudioBlock<float> block (buffer);
    
    mixer.pushDrySamples(block);
    
    updateFilters(*lowpassFilter.state, *highpassFilter.state, lowFilterType.load(), highFilterType.load());
    
    if (lowFilterType.load() != ParameterHelper::off)
        lowpassFilter.process(juce::dsp::ProcessContextReplacing<float> (block));
    if (highFilterType.load() != ParameterHelper::off)
        highpassFilter.process(juce::dsp::ProcessContextReplacing<float> (block));
    
    const float mixVal = LFO::processLFOSample(mix,
                                               lfoTree.getChildWithName(ParamIDs::lfoFilterMix),
                                               mix->convertTo0to1(mix->get()),
                                               lfo1.buffer.getSample(0, 0),
                                               lfo2.buffer.getSample(0, 0));
    mixer.setWetMixProportion(mixVal);
    mixer.mixWetSamples(buffer);
}

void FilterProcessor::updateFilters(juce::dsp::IIR::Coefficients<float>& lowpassCoeffs, juce::dsp::IIR::Coefficients<float>& highpassCoeffs, const int lowpassMode, const int highpassMode)
{
    using CoeffArray = juce::dsp::IIR::ArrayCoefficients<float>;
    
    const float lfo1Val = lfo1.buffer.getSample(0, 0), lfo2Val = lfo2.buffer.getSample(0, 0);
    
    const float lowpassCutoffVal = LFO::processLFOSample(lowpassCutoff,
                                                         lfoTree.getChildWithName(ParamIDs::lfoFilterLowpassCutoff),
                                                         lowpassCutoff->convertTo0to1(lowpassCutoff->get()),
                                                         lfo1Val,
                                                         lfo2Val);
    const float lowpassQVal = LFO::processLFOSample(lowpassQ,
                                                    lfoTree.getChildWithName(ParamIDs::lfoFilterLowpassQ),
                                                    lowpassQ->convertTo0to1(lowpassQ->get()),
                                                    lfo1Val,
                                                    lfo2Val);
    const float lowpassGainVal = LFO::processLFOSample(lowpassGain,
                                                       lfoTree.getChildWithName(ParamIDs::lfoFilterLowpassGain),
                                                       lowpassGain->convertTo0to1(lowpassGain->get()),
                                                       lfo1Val,
                                                       lfo2Val);
    const float highpassCutoffVal = LFO::processLFOSample(highpassCutoff,
                                                          lfoTree.getChildWithName(ParamIDs::lfoFilterHighpassCutoff),
                                                          highpassCutoff->convertTo0to1(highpassCutoff->get()),
                                                          lfo1Val,
                                                          lfo2Val);
    const float highpassQVal = LFO::processLFOSample(highpassQ,
                                                     lfoTree.getChildWithName(ParamIDs::lfoFilterHighpassQ),
                                                     highpassQ->convertTo0to1(highpassQ->get()),
                                                     lfo1Val,
                                                     lfo2Val);
    const float highpassGainVal = LFO::processLFOSample(highpassGain,
                                                        lfoTree.getChildWithName(ParamIDs::lfoFilterHighpassGain),
                                                        highpassGain->convertTo0to1(highpassGain->get()),
                                                        lfo1Val,
                                                        lfo2Val);
    
    if (lowpassMode == ParameterHelper::pass)
       lowpassCoeffs = CoeffArray::makeLowPass(getSampleRate(),
                                               DSPHelper::mapNormalizedToFrequency(lowpassCutoffVal),
                                               lowpassQVal);
    else if (lowpassMode == ParameterHelper::shelf)
        lowpassCoeffs = CoeffArray::makeHighShelf(getSampleRate(),
                                                  DSPHelper::mapNormalizedToFrequency(lowpassCutoffVal),
                                                  lowpassQVal,
                                                  juce::Decibels::decibelsToGain(lowpassGainVal));
    
    if (highpassMode == ParameterHelper::pass)
        highpassCoeffs = CoeffArray::makeHighPass(getSampleRate(),
                                                  DSPHelper::mapNormalizedToFrequency(highpassCutoffVal),
                                                  highpassQVal);
    else if (highpassMode == ParameterHelper::shelf)
        highpassCoeffs = CoeffArray::makeLowShelf(getSampleRate(),
                                                  DSPHelper::mapNormalizedToFrequency(highpassCutoffVal),
                                                  highpassQVal,
                                                  juce::Decibels::decibelsToGain(highpassGainVal));
}

void FilterProcessor::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
{
    if (property == ParamIDs::filterLowpassMode)
        lowFilterType.store(treeWhosePropertyHasChanged[property]);
    
    else if (property == ParamIDs::filterHighpassMode)
        highFilterType.store(treeWhosePropertyHasChanged[property]);
}
