/*
  ==============================================================================

    DelayProcessor.cpp
    Created: 18 Sep 2024 4:30:05pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#include "DelayProcessor.h"

DelayProcessor::DelayProcessor(juce::AudioProcessorValueTreeState& t_apvts,
                               juce::ValueTree paramTree,
                               LFO& t_lfo1,
                               LFO& t_lfo2,
                               std::atomic<float>& t_bpm)
: apvts(t_apvts)
, delayTree(paramTree.getChildWithName(ParamIDs::delayTree))
, lfoTree(paramTree.getChildWithName(ParamIDs::lfoTree))
, lfo1(t_lfo1)
, lfo2(t_lfo2)
, bpm(t_bpm)
{
    ParameterHelper::castParameter (apvts, ParamIDs::leftDelayTime, leftDelayTime);
    ParameterHelper::castParameter (apvts, ParamIDs::rightDelayTime, rightDelayTime);
    ParameterHelper::castParameter (apvts, ParamIDs::leftDelaySync, leftDelaySync);
    ParameterHelper::castParameter (apvts, ParamIDs::rightDelaySync, rightDelaySync);
    ParameterHelper::castParameter (apvts, ParamIDs::leftDelayOffset, leftDelayOffset);
    ParameterHelper::castParameter (apvts, ParamIDs::rightDelayOffset, rightDelayOffset);
    ParameterHelper::castParameter (apvts, ParamIDs::delayFilterCenter, delayFilterCenter);
    ParameterHelper::castParameter (apvts, ParamIDs::delayFilterWidth, delayFilterWidth);
    ParameterHelper::castParameter (apvts, ParamIDs::delayFeedback, delayFeedback);
    ParameterHelper::castParameter (apvts, ParamIDs::delayMix, delayMix);
    
    delayTree.addListener(this);
    linkState.store(delayTree[ParamIDs::delayLink]);
    leftSyncState.store(delayTree[ParamIDs::delaySyncLeft]);
    rightSyncState.store(delayTree[ParamIDs::delaySyncRight]);
    filterOn.store(delayTree[ParamIDs::delayFilterIO]);
}

void DelayProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;

    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    
    delayL.prepare(spec);
    delayR.prepare(spec);
    delayL.setMaximumDelayInSamples(maxDelayInSeconds * sampleRate + 100.0f);
    delayR.setMaximumDelayInSamples(maxDelayInSeconds * sampleRate + 100.0f);
    
    delayMixer.prepare(spec);
    delayMixer.setWetLatency(0.0f);
    delayMixer.setMixingRule(juce::dsp::DryWetMixingRule::squareRoot4p5dB); // TODO try new rules
    
    delayBuffer.setSize(2, samplesPerBlock);
    
    leftDelaySmoothed.reset(sampleRate, smoothingTimeInSeconds);
    rightDelaySmoothed.reset(sampleRate, smoothingTimeInSeconds);
    
    lowpassDelayFilter.prepare(spec);
    highpassDelayFilter.prepare(spec);
    lowpassDelayFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    highpassDelayFilter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
    lowpassDelayFilter.setResonance(0.72f);
    highpassDelayFilter.setResonance(0.72f);
}

void DelayProcessor::processBlock (juce::AudioSampleBuffer& buffer, juce::MidiBuffer&)
{
    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> context (block);
    
    delayMixer.pushDrySamples(block);
    
    for (int channel = 0; channel < buffer.getNumChannels(); channel++)
        delayBuffer.copyFrom(channel, 0, buffer, channel, 0, buffer.getNumSamples());
    
    float* dryL = delayBuffer.getWritePointer(0);
    float* dryR = delayBuffer.getWritePointer(1);
    float* wetL = buffer.getWritePointer(0);
    float* wetR = buffer.getWritePointer(1);
    
    updateDelay();
    
    for (int sample = 0; sample < buffer.getNumSamples(); sample++)
    {
        delayL.setDelay(leftDelaySmoothed.getNextValue());
        delayR.setDelay(rightDelaySmoothed.getNextValue());
        wetL[sample] = delayL.popSample(0);
        wetR[sample] = delayR.popSample(1);
    }
    
    if (filterOn.load())
    {
        updateDelayFilter();
        
        lowpassDelayFilter.process(context);
        highpassDelayFilter.process(context);
    }
    
    const float feedbackVal = LFO::processLFOSample(delayFeedback,
                                                    lfoTree.getChildWithName(ParamIDs::lfoDelayFeedback),
                                                    delayFeedback->convertTo0to1(delayFeedback->get()),
                                                    lfo1.buffer.getSample(0, 0),
                                                    lfo2.buffer.getSample(0, 0));
    
    for (int sample = 0; sample < buffer.getNumSamples(); sample++)
    {
        delayL.pushSample(0, wetL[sample] * feedbackVal + dryL[sample]);
        delayR.pushSample(1, wetR[sample] * feedbackVal + dryR[sample]);
    }
    
    delayMixer.setWetMixProportion(LFO::processLFOSample(delayMix,
                                                         lfoTree.getChildWithName(ParamIDs::lfoDelayMix),
                                                         delayMix->convertTo0to1(delayMix->get()),
                                                         lfo1.buffer.getSample(0, 0),
                                                         lfo2.buffer.getSample(0, 0)));
    delayMixer.mixWetSamples(buffer);
}

void DelayProcessor::updateDelay()
{
    float leftDelay, rightDelay;
    const float bpmToUse = bpm.load();
    
    const float lfo1Val = lfo1.buffer.getSample(0, 0), lfo2Val = lfo2.buffer.getSample(0, 0);
    
    if (linkState)
    {
        if (leftSyncState)
        {
            int leftDelaySyncIndex = LFO::processLFOSample(leftDelaySync,
                                                           lfoTree.getChildWithName(ParamIDs::lfoLeftDelaySync),
                                                           leftDelaySync->convertTo0to1(leftDelaySync->getIndex()),
                                                           lfo1Val,
                                                           lfo2Val);
            int numSixteenths = ParameterHelper::syncOptions[leftDelaySyncIndex].getIntValue();
            leftDelay = numSixteenths / (bpmToUse / 60.f * 4.0f);
            rightDelay = leftDelay;
        }
        else
        {
            leftDelay = LFO::processLFOSample(leftDelayTime,
                                              lfoTree.getChildWithName(ParamIDs::lfoLeftDelayTime),
                                              leftDelayTime->convertTo0to1(leftDelayTime->get()),
                                              lfo1Val,
                                              lfo2Val);
            rightDelay = leftDelay;
        }
    }
    else
    {
        if (leftSyncState)
        {
            int leftDelaySyncIndex = LFO::processLFOSample(leftDelaySync,
                                                           lfoTree.getChildWithName(ParamIDs::lfoLeftDelaySync),
                                                           leftDelaySync->convertTo0to1(leftDelaySync->getIndex()),
                                                           lfo1Val,
                                                           lfo2Val);
            int numSixteenths = ParameterHelper::syncOptions[leftDelaySyncIndex].getIntValue();
            leftDelay = numSixteenths / (bpmToUse / 60.f * 4.0f);
        }
        else
        {
            leftDelay = LFO::processLFOSample(leftDelayTime,
                                              lfoTree.getChildWithName(ParamIDs::lfoLeftDelayTime),
                                              leftDelayTime->convertTo0to1(leftDelayTime->get()),
                                              lfo1Val,
                                              lfo2Val);
        }
        
        if (rightSyncState)
        {
            int rightDelaySyncIndex = LFO::processLFOSample(rightDelaySync,
                                                            lfoTree.getChildWithName(ParamIDs::lfoRightDelaySync),
                                                            rightDelaySync->convertTo0to1(rightDelaySync->getIndex()),
                                                            lfo1Val,
                                                            lfo2Val);
            int numSixteenths = ParameterHelper::syncOptions[rightDelaySyncIndex].getIntValue();
            rightDelay = numSixteenths / (bpmToUse / 60.f * 4.0f);
        }
        else
        {
            rightDelay = LFO::processLFOSample(rightDelayTime,
                                               lfoTree.getChildWithName(ParamIDs::lfoRightDelayTime),
                                               rightDelayTime->convertTo0to1(rightDelayTime->get()),
                                               lfo1Val,
                                               lfo2Val);
        }
    }
    
    const float leftOffsetValue = LFO::processLFOSample(leftDelayOffset,
                                                        lfoTree.getChildWithName(ParamIDs::lfoLeftDelayOffset),
                                                        leftDelayOffset->convertTo0to1(leftDelayOffset->get()),
                                                        lfo1Val,
                                                        lfo2Val);
    const float rightOffsetValue = LFO::processLFOSample(rightDelayOffset,
                                                         lfoTree.getChildWithName(ParamIDs::lfoRightDelayOffset),
                                                         rightDelayOffset->convertTo0to1(rightDelayOffset->get()),
                                                         lfo1Val,
                                                         lfo2Val);
    
    leftDelaySmoothed.setTargetValue(juce::jmin(leftDelay * (1.0f + leftOffsetValue / 100.0f), maxDelayInSeconds) * getSampleRate());
    rightDelaySmoothed.setTargetValue(juce::jmin(rightDelay * (1.0f + rightOffsetValue / 100.0f), maxDelayInSeconds) * getSampleRate());;
}

void DelayProcessor::updateDelayFilter()
{
    const float centerLFOVal = LFO::processLFOSample(delayFilterCenter,
                                                     lfoTree.getChildWithName(ParamIDs::lfoDelayFilterCenter),
                                                     delayFilterCenter->convertTo0to1(delayFilterCenter->get()),
                                                     lfo1.buffer.getSample(0, 0),
                                                     lfo2.buffer.getSample(0, 0));
    const float center = DSPHelper::mapNormalizedToFrequency(centerLFOVal);
    const float width = LFO::processLFOSample(delayFilterWidth,
                                              lfoTree.getChildWithName(ParamIDs::lfoDelayFilterWidth),
                                              delayFilterWidth->convertTo0to1(delayFilterWidth->get()),
                                              lfo1.buffer.getSample(0, 0),
                                              lfo2.buffer.getSample(0, 0));
    
    lowpassDelayFilter.setCutoffFrequency(DSPHelper::getDelayLowpassCutoff(center, width));
    highpassDelayFilter.setCutoffFrequency(DSPHelper::getDelayHighpassCutoff(center, width));
}

void DelayProcessor::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
{
    if (property == ParamIDs::delayLink)
        linkState.store(delayTree[property]);
    
    else if (property == ParamIDs::delaySyncLeft)
        leftSyncState.store(delayTree[property]);
    
    else if (property == ParamIDs::delaySyncRight)
        rightSyncState.store(delayTree[property]);
    
    else if (property == ParamIDs::delayFilterIO)
        filterOn.store(delayTree[property]);

}
