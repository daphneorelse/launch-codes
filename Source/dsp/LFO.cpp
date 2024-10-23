/*
  ==============================================================================

    LFO.cpp
    Created: 3 Sep 2024 11:39:41am
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#include "LFO.h"

LFO::LFO (juce::RangedAudioParameter& t_freqTimeParam, juce::RangedAudioParameter& t_freqSyncParam, juce::RangedAudioParameter& t_rangeParam, juce::ValueTree t_lfoTree, std::atomic<float>& t_bpm)
: visualizer()
, freqTimeParam(t_freqTimeParam)
, freqSyncParam(t_freqSyncParam)
, rangeParam(t_rangeParam)
, freqTimeAttachment(freqTimeParam, [&] (float newFreqTime) { frequencyTimeChanged(newFreqTime); })
, freqSyncAttachment(freqSyncParam, [&] (float newFreqSync) { frequencySyncChanged(newFreqSync); })
, rangeAttachment(rangeParam, [&] (float newRange) { rangeChanged(newRange); })
, lfoTree(t_lfoTree)
, bpm(t_bpm)
{
    lfoTree.addListener(this);
    
    updateOsc();
    
    syncState.store(lfoTree[ParamIDs::lfoSync]);
    
    freqTimeAttachment.sendInitialUpdate();
    freqSyncAttachment.sendInitialUpdate();
    rangeAttachment.sendInitialUpdate();
}

LFO::~LFO ()
{
    lfoTree.removeListener(this);
}

void LFO::prepare (juce::dsp::ProcessSpec spec)
{
    sampleRate = spec.sampleRate;
    
    visualizer.setBufferSize(100);
    visualizer.setSamplesPerBlock(slowVisualizerSecondsPerScreenWidth * sampleRate / bufferSize);
    visualizer.setRepaintRate(30);
    
    buffer.setSize(1, spec.maximumBlockSize);
}

void LFO::processToOutput (const int numSamples)
{
    float* buff = buffer.getWritePointer(0);
    
    for (int i = 0; i < numSamples; i++)
    {
        buff[i] = osc.processSample(0.0f) * range;
    }
    
    visualizer.pushBuffer(buffer);
}

void LFO::updateOsc()
{
    switch (waveNum) {
        case 0: // sine wave
            osc.initialise([] (float i) { return juce::dsp::FastMathApproximations::sin(i); });
            break;
            
        case 1: // triangle wave
            osc.initialise([] (float i) {
            if (i < -juce::MathConstants<float>::halfPi)
                return juce::jmap(i, -juce::MathConstants<float>::pi, -juce::MathConstants<float>::halfPi, 0.0f, 1.0f);
            else if (i < juce::MathConstants<float>::halfPi)
                return juce::jmap(i, -juce::MathConstants<float>::halfPi, juce::MathConstants<float>::halfPi, 1.0f, -1.0f);
            else
                return juce::jmap(i, juce::MathConstants<float>::halfPi, juce::MathConstants<float>::pi, -1.0f, 0.0f);
            });
            break;
            
        case 2: // sawtooth wave
            osc.initialise([] (float i) { return juce::jmap (i,
                                                             -juce::MathConstants<float>::pi,
                                                             juce::MathConstants<float>::pi,
                                                             1.0f,
                                                             -1.0f); });
            break;
            
        case 3: // square wave
            osc.initialise([] (float i) { return i < 0.0f ? 1.0f : -1.0f; });
            
        default:
            break;
    }
    
};

void LFO::handleSPBUpdate (float newFreq)
{
    if (newFreq > spbCutoffFreq)
        visualizer.setSamplesPerBlock(fastVisualizerSecondsPerScreenWidth * sampleRate / bufferSize);
    else
        visualizer.setSamplesPerBlock(slowVisualizerSecondsPerScreenWidth * sampleRate / bufferSize);
}

void LFO::frequencyTimeChanged (float newFreqTime)
{
    freqTime.store(newFreqTime);
    
    if (! syncState.load())
    {
        osc.setFrequency(newFreqTime);
        handleSPBUpdate(newFreqTime);
    }
}

void LFO::frequencySyncChanged (float newFreqSync)
{
    const float newSixteenths = ParameterHelper::syncOptions[static_cast<int> (newFreqSync)].getIntValue();
    const float newFreq = (bpm.load() / 60.f * 4.0f) / static_cast<float> (newSixteenths);
    
    freqSync.store(newFreq);

    if (syncState.load())
    {
        osc.setFrequency(newFreq);
        osc.reset();
        handleSPBUpdate(newFreq);
    }
}

void LFO::rangeChanged (float newRange)
{
    range = newRange;
}

void LFO::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
{
    if (property == ParamIDs::lfoWaveType)
    {
        waveNum = static_cast<int> (lfoTree.getProperty(ParamIDs::lfoWaveType));
        updateOsc();
    }
    else if (property == ParamIDs::lfoSync)
    {
        syncState.store(lfoTree[ParamIDs::lfoSync]);
        
        if (syncState.load())
        {
            const float freq = freqSync.load();
            osc.setFrequency(freq);
            handleSPBUpdate(freq);
        }
        else
        {
            const float freq = freqTime.load();
            osc.setFrequency(freq);
            osc.reset();
            handleSPBUpdate(freq);
        }
    }
};
