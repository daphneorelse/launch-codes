/*
  ==============================================================================

    WaveshaperProcessor.cpp
    Created: 18 Sep 2024 4:30:46pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#include "WaveshaperProcessor.h"

WaveshaperProcessor::WaveshaperProcessor(juce::AudioProcessorValueTreeState& t_apvts,
                                         juce::ValueTree paramTree,
                                         LFO& t_lfo1,
                                         LFO& t_lfo2,
                                         juce::dsp::LookupTableTransform<float>& t_waveshaperControlTable,
                                         juce::dsp::LookupTableTransform<float>& t_waveshaperTransformTable)
: apvts(t_apvts)
, waveshaperNodeTree(paramTree.getChildWithName(ParamIDs::waveshaperNodeTree))
, waveshaperToolTree(paramTree.getChildWithName(ParamIDs::waveshaperToolTree))
, lfoTree(paramTree.getChildWithName(ParamIDs::lfoTree))
, lfo1(t_lfo1)
, lfo2(t_lfo2)
, waveshaperControlTable(t_waveshaperControlTable)
, waveshaperTransformTable(t_waveshaperTransformTable)
{
    ParameterHelper::castParameter (apvts, ParamIDs::waveshaperXTransform, waveshaperXTRansform);
    ParameterHelper::castParameter (apvts, ParamIDs::waveshaperYTransform, waveshaperYTRansform);
    ParameterHelper::castParameter (apvts, ParamIDs::waveshaperInputGain, waveshaperInputGain);
    ParameterHelper::castParameter (apvts, ParamIDs::waveshaperOutputGain, waveshaperOutputGain);
    ParameterHelper::castParameter (apvts, ParamIDs::waveshaperMix, waveshaperMix);
    
    apvts.addParameterListener(ParamIDs::waveshaperXTransform, this);
    apvts.addParameterListener(ParamIDs::waveshaperYTransform, this);
    
    updateWaveshaperControlTable();
    
    waveshaperNodeTree.addListener(this);
    waveshaperToolTree.addListener(this);
}

void WaveshaperProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;

    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    
    waveshaperInputGainDSP.prepare(spec);
    waveshaperOutputGainDSP.prepare(spec);
    
    waveshaperMixer.prepare(spec);
    waveshaperMixer.setWetLatency(0.0f);
    waveshaperMixer.setMixingRule(juce::dsp::DryWetMixingRule::squareRoot4p5dB);
}

void WaveshaperProcessor::processBlock (juce::AudioSampleBuffer& buffer, juce::MidiBuffer&)
{
    float* wetL = buffer.getWritePointer(0);
    float* wetR = buffer.getWritePointer(1);
    
    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> context (block);
    
    waveshaperMixer.pushDrySamples(block);
    
    const float inputGainVal = LFO::processLFOSample(waveshaperInputGain,
                                                     lfoTree.getChildWithName(ParamIDs::lfoWaveshaperInputGain),
                                                     waveshaperInputGain->convertTo0to1(waveshaperInputGain->get()),
                                                     lfo1.buffer.getSample(0, 0),
                                                     lfo2.buffer.getSample(0, 0));
    waveshaperInputGainDSP.setGainLinear(inputGainVal);
    waveshaperInputGainDSP.process(context);
    
    waveshaperTransformTable.process(wetL, wetL, buffer.getNumSamples());
    waveshaperTransformTable.process(wetR, wetR, buffer.getNumSamples());
    
    const float outputGainVal = LFO::processLFOSample(waveshaperOutputGain,
                                                      lfoTree.getChildWithName(ParamIDs::lfoWaveshaperOutputGain),
                                                      waveshaperOutputGain->convertTo0to1(waveshaperOutputGain->get()),
                                                      lfo1.buffer.getSample(0, 0),
                                                      lfo2.buffer.getSample(0, 0));
    waveshaperOutputGainDSP.setGainLinear(outputGainVal);
    waveshaperOutputGainDSP.process(context);
    
    const float mixVal = LFO::processLFOSample(waveshaperMix,
                                               lfoTree.getChildWithName(ParamIDs::lfoWaveshaperMix),
                                               waveshaperMix->convertTo0to1(waveshaperMix->get()),
                                               lfo1.buffer.getSample(0, 0),
                                               lfo2.buffer.getSample(0, 0));
    waveshaperMixer.setWetMixProportion(mixVal);
    waveshaperMixer.mixWetSamples(buffer);
}

void WaveshaperProcessor::updateWaveshaperControlTable()
{
    // initialize to last enabled node in tree
    juce::ValueTree node = waveshaperNodeTree.getChild(0);
    while (node.getSibling(1).getProperty(ParamIDs::nodeEnabled))
        node = node.getSibling(1);
    
    waveshaperControlTable.initialise([&] (float input) {
        
        const float absInput = std::abs(input);
        
        while (node.isValid())
        {
            juce::Point<float> startPoint (0.0f, 0.0f);
            juce::Point<float> endPoint   (1.0f, 1.0f);
            
            juce::ValueTree nextNode = node.getSibling(1);

            if (nextNode.isValid() && nextNode[ParamIDs::nodeEnabled])
            {
                endPoint.x = nextNode[ParamIDs::nodeX];
                endPoint.y = nextNode[ParamIDs::nodeY];
            }

            if (node.hasProperty(ParamIDs::nodeX))
            {
                startPoint.x = node[ParamIDs::nodeX];
                startPoint.y = node[ParamIDs::nodeY];
            }

            if (absInput > startPoint.x && absInput <= endPoint.x)
            {
                const float slope = node.getProperty(ParamIDs::nodeSlope);
                const float segmentWidth = endPoint.x - startPoint.x;
                const float segmentHeight = endPoint.y - startPoint.y;
                const float inputNorm = (absInput - startPoint.x) / segmentWidth;
                const float mult = input == 0.0f ? 1.0f : absInput / input;

                if (slope == 0.0f)
                    return (startPoint.y + segmentHeight * inputNorm) * mult;
                else if (slope < -10.0f)
                    return endPoint.y * mult;
                else if (slope > 10.0f)
                    return startPoint.y * mult;
                else
                    return (startPoint.y + segmentHeight * (expf(slope * inputNorm) - 1.0f) / (expf(slope) - 1.0f)) * mult;
            }
            else
            {
                node = node.getSibling(input < 0.0f ? -1 : 1);
            }
        }
        
        node = waveshaperNodeTree.getChild(0);
        return 0.0f;
        
    }, -1.0f, 1.0f, 129);
    
    updateWaveshaperTransformTable();
}

void WaveshaperProcessor::updateWaveshaperTransformTable()
{
    waveshaperTransformTable.initialise([&] (float input) {
        
        const bool isOdd = static_cast<bool>(waveshaperToolTree.getProperty(ParamIDs::oddEven));
        
        const float absInput = std::abs(input);
        const float multX = (isOdd && input != 0.0f) ? input / absInput : 1.0f;
        
        const float xSlope = waveshaperXTRansform->get();
        const float xTransformedOutput = waveshaperControlTable.processSample(xSlope == 0.0f ? absInput : (expf(xSlope * absInput) - 1.0f) / (expf(xSlope) - 1.0f));
        
        const float multY = xTransformedOutput != 0.0f ? xTransformedOutput / std::abs(xTransformedOutput) : 1.0f;
        
        const float ySlope = waveshaperYTRansform->get();
        const float yTransformedOutput = ySlope == 0.0f ? std::abs(xTransformedOutput) : (expf(ySlope * std::abs(xTransformedOutput)) - 1.0f) / (expf(ySlope) - 1.0f);
        return yTransformedOutput * multX * multY;
        
    }, -1.0f, 1.0f, 129);
}

void WaveshaperProcessor::parameterChanged (const juce::String& parameterID, float newValue)
{
    if (parameterID == ParamIDs::waveshaperXTransform || parameterID == ParamIDs::waveshaperYTransform)
        updateWaveshaperTransformTable();
}

void WaveshaperProcessor::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
{
    updateWaveshaperControlTable();
}
