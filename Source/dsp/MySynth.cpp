/*
  ==============================================================================

    MySynth.cpp
    Created: 18 Jun 2024 10:59:42am
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#include "MySynth.h"

MyVoice::MyVoice(juce::AudioProcessorValueTreeState& t_apvts,
                   juce::ValueTree nonParamTree,
                   LFO& t_lfo1,
                   LFO& t_lfo2,
                   juce::OwnedArray<juce::dsp::LookupTableTransform<float>>& oscAmpTables,
                   juce::OwnedArray<juce::dsp::LookupTableTransform<float>>& oscPitchTables,
                   juce::OwnedArray<juce::dsp::LookupTableTransform<float>>& noiseAmpTables,
                   juce::OwnedArray<juce::dsp::LookupTableTransform<float>>& noiseFilterTables)
: apvts(t_apvts)
, synthTree(nonParamTree.getChildWithName(ParamIDs::synthTree))
, lfoTree(nonParamTree.getChildWithName(ParamIDs::lfoTree))
, lfo1(t_lfo1)
, lfo2(t_lfo2)
, oscAmpEnv(synthTree, ParamIDs::oscAmpEnvMode, oscAmpTables)
, oscPitchEnv(synthTree, ParamIDs::pitchEnvMode, oscPitchTables)
, noiseAmpEnv(synthTree, ParamIDs::noiseAmpEnvMode, noiseAmpTables)
, noiseFilterEnv(synthTree, ParamIDs::filterEnvMode, noiseFilterTables)
{
    apvts.addParameterListener(ParamIDs::oscAttackAmp, this);
    apvts.addParameterListener(ParamIDs::oscDecayAmp, this);
    apvts.addParameterListener(ParamIDs::oscReleaseAmp, this);
    apvts.addParameterListener(ParamIDs::oscInitialAmp, this);
    apvts.addParameterListener(ParamIDs::oscPeakAmp, this);
    apvts.addParameterListener(ParamIDs::oscSustainAmp, this);
    apvts.addParameterListener(ParamIDs::oscAttackPitch, this);
    apvts.addParameterListener(ParamIDs::oscDecayPitch, this);
    apvts.addParameterListener(ParamIDs::oscReleasePitch, this);
    apvts.addParameterListener(ParamIDs::oscInitialPitch, this);
    apvts.addParameterListener(ParamIDs::oscPeakPitch, this);
    apvts.addParameterListener(ParamIDs::oscSustainPitch, this);
    apvts.addParameterListener(ParamIDs::oscEndPitch, this);
    apvts.addParameterListener(ParamIDs::noiseAttackAmp, this);
    apvts.addParameterListener(ParamIDs::noiseDecayAmp, this);
    apvts.addParameterListener(ParamIDs::noiseReleaseAmp, this);
    apvts.addParameterListener(ParamIDs::noiseInitialAmp, this);
    apvts.addParameterListener(ParamIDs::noisePeakAmp, this);
    apvts.addParameterListener(ParamIDs::noiseSustainAmp, this);
    apvts.addParameterListener(ParamIDs::noiseAttackCutoff, this);
    apvts.addParameterListener(ParamIDs::noiseDecayCutoff, this);
    apvts.addParameterListener(ParamIDs::noiseReleaseCutoff, this);
    apvts.addParameterListener(ParamIDs::noiseInitialCutoff, this);
    apvts.addParameterListener(ParamIDs::noisePeakCutoff, this);
    apvts.addParameterListener(ParamIDs::noiseSustainCutoff, this);
    apvts.addParameterListener(ParamIDs::noiseEndCutoff, this);
    
    synthTree.addListener(this);
    
    ParameterHelper::castParameter(apvts, ParamIDs::oscGain, oscGainParam);
    ParameterHelper::castParameter(apvts, ParamIDs::oscPan, oscPanParam);
    ParameterHelper::castParameter(apvts, ParamIDs::oscOctave, oscOctaveParam);
    ParameterHelper::castParameter(apvts, ParamIDs::oscSemitone, oscSemitoneParam);
    ParameterHelper::castParameter(apvts, ParamIDs::oscFine, oscFineParam);
    ParameterHelper::castParameter(apvts, ParamIDs::oscCoarse, oscCoarseParam);
    ParameterHelper::castParameter(apvts, ParamIDs::noiseGain, noiseGainParam);
    ParameterHelper::castParameter(apvts, ParamIDs::noisePan, noisePanParam);
    ParameterHelper::castParameter(apvts, ParamIDs::noiseQ, noiseQParam);
    ParameterHelper::castParameter(apvts, ParamIDs::noiseFilterCoarse, noiseCoarseParam);
}

MyVoice::~MyVoice()
{
    synthTree.removeListener(this);
    
    delete [] oscOutput;
    delete [] noiseOutput;
}

bool MyVoice::canPlaySound (juce::SynthesiserSound* sound)
{
    return dynamic_cast<MySound*> (sound) != nullptr;
}

void MyVoice::startNote (int midiNoteNumber, float velocity,
                juce::SynthesiserSound* sound, int /*currentPitchWheelPosition*/)
{
    currentMidiNote = midiNoteNumber;
    
    if (oscIO)
    {
        oscAmpEnv.noteOn();
        if (pitchEnvMode.load() != 2)
            oscPitchEnv.noteOn();
        else
            setFrequency(DSPHelper::getFrequencyFromMidi(midiNoteNumber));
    }
    if (noiseIO)
    {
        noiseAmpEnv.noteOn();
        if (filterEnvMode.load() != 2)
            noiseFilterEnv.noteOn();
    }
}

void MyVoice::stopNote (float /*velocity*/, bool allowTailOff)
{
    oscAmpEnv.noteOff();
    oscPitchEnv.noteOff();
    noiseAmpEnv.noteOff();
    noiseFilterEnv.noteOff();
    
    if (! allowTailOff || (!oscAmpEnv.isActive() && !noiseAmpEnv.isActive()))
    {
        clearCurrentNote();
    }
}

void MyVoice::prepareToPlay (double t_sampleRate, int samplesPerBlock, int numOutputChannels)
{
    sampleRate = t_sampleRate;
    
    juce::dsp::ProcessSpec spec;

    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = numOutputChannels;
    
    oscBuffer.setSize(2, samplesPerBlock);
    noiseBuffer.setSize(2, samplesPerBlock);
    
    oscDSP = new oscDsp();
    oscDSP->init(sampleRate);
    oscUI = new MapUI();
    oscDSP->buildUserInterface(oscUI);

    setWaveType(synthTree[ParamIDs::oscWaveType]);
    
    noiseDSP = new noiseDsp();
    noiseDSP->init(sampleRate);
    noiseUI = new MapUI();
    noiseDSP->buildUserInterface(noiseUI);
    
    noiseUI->setParamValue("gain", 0.5f);
    setQ(noiseQParam->get());
    setFilterType(synthTree[ParamIDs::filterType]);

    oscOutput = new float*[2];
    noiseOutput = new float*[2];
    for (int channel = 0; channel < 2; ++channel) {
        oscOutput[channel] = new float[samplesPerBlock];
        noiseOutput[channel] = new float[samplesPerBlock];
    }
    
    oscAmpEnv.setSampleRate(sampleRate);
    oscPitchEnv.setSampleRate(sampleRate);
    noiseAmpEnv.setSampleRate(sampleRate);
    noiseFilterEnv.setSampleRate(sampleRate);
    
    oscAmpEnvParams.attack = *apvts.getRawParameterValue(ParamIDs::oscAttackAmp);
    oscAmpEnvParams.decay = *apvts.getRawParameterValue(ParamIDs::oscDecayAmp);
    oscAmpEnvParams.release = *apvts.getRawParameterValue(ParamIDs::oscReleaseAmp);
    oscAmpEnvParams.initial = juce::Decibels::decibelsToGain(apvts.getRawParameterValue(ParamIDs::oscInitialAmp)->load());
    oscAmpEnvParams.peak = juce::Decibels::decibelsToGain(apvts.getRawParameterValue(ParamIDs::oscPeakAmp)->load());
    oscAmpEnvParams.sustain = juce::Decibels::decibelsToGain(apvts.getRawParameterValue(ParamIDs::oscSustainAmp)->load());
    oscAmpEnvParams.stealRelease = 0.008f;
    
    oscAmpEnv.setParameters(oscAmpEnvParams);
    
    oscPitchEnvParams.attack = *apvts.getRawParameterValue(ParamIDs::oscAttackPitch);
    oscPitchEnvParams.decay = *apvts.getRawParameterValue(ParamIDs::oscDecayPitch);
    oscPitchEnvParams.release = *apvts.getRawParameterValue(ParamIDs::oscReleasePitch);
    oscPitchEnvParams.initial = *apvts.getRawParameterValue(ParamIDs::oscInitialPitch);
    oscPitchEnvParams.peak = *apvts.getRawParameterValue(ParamIDs::oscPeakPitch);
    oscPitchEnvParams.sustain = *apvts.getRawParameterValue(ParamIDs::oscSustainPitch);
    oscPitchEnvParams.end = *apvts.getRawParameterValue(ParamIDs::oscEndPitch);
    
    oscPitchEnv.setParameters(oscPitchEnvParams);
    
    noiseAmpEnvParams.attack = *apvts.getRawParameterValue(ParamIDs::noiseAttackAmp);
    noiseAmpEnvParams.decay = *apvts.getRawParameterValue(ParamIDs::noiseDecayAmp);
    noiseAmpEnvParams.release = *apvts.getRawParameterValue(ParamIDs::noiseReleaseAmp);
    noiseAmpEnvParams.initial = juce::Decibels::decibelsToGain(apvts.getRawParameterValue(ParamIDs::noiseInitialAmp)->load());
    noiseAmpEnvParams.peak = juce::Decibels::decibelsToGain(apvts.getRawParameterValue(ParamIDs::noisePeakAmp)->load());
    noiseAmpEnvParams.sustain = juce::Decibels::decibelsToGain(apvts.getRawParameterValue(ParamIDs::noiseSustainAmp)->load());
    noiseAmpEnvParams.stealRelease = 0.008f;
    
    noiseAmpEnv.setParameters(noiseAmpEnvParams);
    
    noiseFilterEnvParams.attack = *apvts.getRawParameterValue(ParamIDs::noiseAttackCutoff);
    noiseFilterEnvParams.decay = *apvts.getRawParameterValue(ParamIDs::noiseDecayCutoff);
    noiseFilterEnvParams.release = *apvts.getRawParameterValue(ParamIDs::noiseReleaseCutoff);
    noiseFilterEnvParams.initial = *apvts.getRawParameterValue(ParamIDs::noiseInitialCutoff);
    noiseFilterEnvParams.peak = *apvts.getRawParameterValue(ParamIDs::noisePeakCutoff);
    noiseFilterEnvParams.sustain = *apvts.getRawParameterValue(ParamIDs::noiseSustainCutoff);
    noiseFilterEnvParams.end = *apvts.getRawParameterValue(ParamIDs::noiseEndCutoff);
    
    noiseFilterEnv.setParameters(noiseFilterEnvParams);
    
    numSamplesIncrement = static_cast<int>(sampleRate * 0.0005f) + 1;
    
    oscGain.prepare(spec);
    oscGain.setRampDurationSeconds(0.0005f);
    oscGain.setGainLinear(juce::Decibels::decibelsToGain(oscGainParam->get()));
    
    noiseGain.prepare(spec);
    noiseGain.setRampDurationSeconds(0.0005f);
    noiseGain.setGainLinear(juce::Decibels::decibelsToGain(noiseGainParam->get()));
    
    oscPanner.prepare(spec);
    oscPanner.setRule(juce::dsp::PannerRule::squareRoot4p5dB);
    oscPanner.setPan(oscPanParam->get());
    
    noisePanner.prepare(spec);
    noisePanner.setRule(juce::dsp::PannerRule::squareRoot4p5dB);
    noisePanner.setPan(noisePanParam->get());
    
    oscIO = synthTree.getProperty(ParamIDs::oscIO);
    pitchEnvMode.store(synthTree.getProperty(ParamIDs::pitchEnvMode));
    noiseIO = synthTree.getProperty(ParamIDs::noiseIO);
    filterEnvMode.store(synthTree.getProperty(ParamIDs::filterEnvMode));
    filterKeyFollowIO.store(synthTree.getProperty(ParamIDs::filterKeyFollowIO));
}

void MyVoice::renderNextBlock (juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples)
{
    if (! isVoiceActive())
        return;
    
    sampleIndex = 0;
    
    if (oscIO)
    {
        oscBuffer.clear();
        auto oscBuffs = oscBuffer.getArrayOfWritePointers();
        
        const bool useEnvelope = pitchEnvMode.load() != 2;
        const float oscOctaveParamVal = oscOctaveParam->convertTo0to1(oscOctaveParam->get());
        const float oscSemitoneParamVal = oscSemitoneParam->convertTo0to1(oscSemitoneParam->get());
        const float oscFineParamVal = oscFineParam->convertTo0to1(oscFineParam->get());
        const float oscCoarseParamVal = oscCoarseParam->convertTo0to1(oscCoarseParam->get());
        
        for (int startSample = 0; startSample < numSamples; startSample += numSamplesIncrement)
        {
            const float samplesToUse = juce::jmin(numSamplesIncrement, numSamples - sampleIndex);
            
            const float lfo1Val = lfo1.buffer.getSample(0, sampleIndex);
            const float lfo2Val = lfo2.buffer.getSample(0, sampleIndex);

            const float envMidiPitch = useEnvelope ? oscPitchEnv.getNextSamples(samplesToUse) : 0.0f;
            const float octavePitch = LFO::processLFOSample(oscOctaveParam,
                                                            lfoTree.getChildWithName(ParamIDs::lfoOscOctave),
                                                            oscOctaveParamVal,
                                                            lfo1Val,
                                                            lfo2Val) * 12.0f;
            const float semitonePitch = LFO::processLFOSample(oscSemitoneParam,
                                                              lfoTree.getChildWithName(ParamIDs::lfoOscSemitone),
                                                              oscSemitoneParamVal,
                                                              lfo1Val,
                                                              lfo2Val);
            const float finePitch = LFO::processLFOSample(oscFineParam,
                                                          lfoTree.getChildWithName(ParamIDs::lfoOscFine),
                                                          oscFineParamVal,
                                                          lfo1Val,
                                                          lfo2Val) / 100.0f;
            const float coarsePitch = LFO::processLFOSample(oscCoarseParam,
                                                            lfoTree.getChildWithName(ParamIDs::lfoOscCoarse),
                                                            oscCoarseParamVal,
                                                            lfo1Val,
                                                            lfo2Val);
            const float targetFreqMidi = currentMidiNote + envMidiPitch + octavePitch + semitonePitch + finePitch + coarsePitch;
            const float targetFrequency = juce::jlimit(10.0f, sampleRate / 2.0f, DSPHelper::getFrequencyFromMidi(targetFreqMidi));
            setFrequency(targetFrequency);

            oscDSP->compute(samplesToUse, NULL, oscOutput);

            for (int channel = 0; channel < oscBuffer.getNumChannels(); channel++)
            {
                for (int sample = 0; sample < samplesToUse; sample++)
                {
                    oscBuffs[channel][startSample + sample] = oscOutput[channel][sample];
                }
            }

            sampleIndex += samplesToUse;
        }
        
        oscAmpEnv.applyEnvelopeToBuffer(oscBuffer, 0, numSamples);
        juce::dsp::AudioBlock<float> oscBlock(oscBuffer);
        auto oscContext = juce::dsp::ProcessContextReplacing<float>(oscBlock);
        
        const float gainValue = LFO::processLFOSample(oscGainParam,
                                                      lfoTree.getChildWithName(ParamIDs::lfoOscGain),
                                                      oscGainParam->convertTo0to1(oscGainParam->get()),
                                                      lfo1.buffer.getSample(0, 0),
                                                      lfo2.buffer.getSample(0, 0));
        oscGain.setGainLinear(juce::Decibels::decibelsToGain(gainValue));
        oscGain.process(oscContext);
        
        const float panValue = LFO::processLFOSample(oscPanParam,
                                                     lfoTree.getChildWithName(ParamIDs::lfoOscPan),
                                                     oscPanParam->convertTo0to1(oscPanParam->get()),
                                                     lfo1.buffer.getSample(0, 0),
                                                     lfo2.buffer.getSample(0, 0));
        oscPanner.setPan(panValue);
        oscPanner.process(oscContext);
        
        for (int channel = 0; channel < outputBuffer.getNumChannels(); channel++)
        {
            outputBuffer.addFrom(channel, startSample, oscBuffer, channel, 0, numSamples);
        }
    }
    
    sampleIndex = 0;
    
    if (noiseIO)
    {
        noiseBuffer.clear();
        auto noiseBuffs = noiseBuffer.getArrayOfWritePointers();
        
        if (filterEnvMode.load() != 2)
        {
            const float noiseCoarseParamVal = noiseCoarseParam->convertTo0to1(noiseCoarseParam->get());
            float keyFollowMultiplier = 1.0f;
            if (filterKeyFollowIO.load())
                keyFollowMultiplier = DSPHelper::getFrequencyFromMidi(currentMidiNote) / 1000.0f; // center frequency 1000 hz
            
            const float qValue = LFO::processLFOSample(noiseQParam, lfoTree.getChildWithName(ParamIDs::lfoNoiseQ), noiseQParam->convertTo0to1(noiseQParam->get()), lfo1.buffer.getSample(0, 0), lfo2.buffer.getSample(0, 0));
            setQ(qValue);
            
            for (int startSample = 0; startSample < numSamples; startSample += numSamplesIncrement)
            {
                const float samplesToUse = juce::jmin(numSamplesIncrement, numSamples - sampleIndex);

                const float noiseCutoff = noiseFilterEnv.getNextSamples(samplesToUse);
                const float exponent = LFO::processLFOSample(noiseCoarseParam,
                                                             lfoTree.getChildWithName(ParamIDs::lfoNoiseCoarse),
                                                             noiseCoarseParamVal,
                                                             lfo1.buffer.getSample(0, sampleIndex),
                                                             lfo2.buffer.getSample(0, sampleIndex));
                const float lfoCutoffMultiplier = juce::dsp::FastMathApproximations::exp(0.69314718056f * exponent); // roughly equivalent to pow(2, exponent)
                setCutoff(juce::jlimit(30.0f, 20000.0f, noiseCutoff * lfoCutoffMultiplier * keyFollowMultiplier));

                noiseDSP->compute(samplesToUse, NULL, noiseOutput);

                for (int channel = 0; channel < noiseBuffer.getNumChannels(); channel++)
                {
                    for (int sample = 0; sample < samplesToUse; sample++)
                    {
                        noiseBuffs[channel][startSample + sample] = noiseOutput[channel][sample];
                    }
                }

                sampleIndex += samplesToUse;
            }
        }
        else
        {
            for (int channel = 0; channel < noiseBuffer.getNumChannels(); channel++)
            {
                noiseDSP->compute(numSamples, NULL, noiseOutput);
                
                for (int sample = 0; sample < numSamples; sample++)
                {
                    noiseBuffs[channel][sample] = noiseOutput[channel][sample];
                }
            }
        }
        
        noiseAmpEnv.applyEnvelopeToBuffer(noiseBuffer, 0, numSamples);
        juce::dsp::AudioBlock<float> noiseBlock(noiseBuffer);
        auto noiseContext = juce::dsp::ProcessContextReplacing<float>(noiseBlock);
        
        const float gainValue = LFO::processLFOSample(noiseGainParam,
                                                      lfoTree.getChildWithName(ParamIDs::lfoNoiseGain),
                                                      noiseGainParam->convertTo0to1(noiseGainParam->get()),
                                                      lfo1.buffer.getSample(0, 0),
                                                      lfo2.buffer.getSample(0, 0));
        noiseGain.setGainLinear(juce::Decibels::decibelsToGain(gainValue));
        noiseGain.process(noiseContext);
        
        const float panValue = LFO::processLFOSample(noisePanParam,
                                                     lfoTree.getChildWithName(ParamIDs::lfoNoisePan),
                                                     noisePanParam->convertTo0to1(noisePanParam->get()),
                                                     lfo1.buffer.getSample(0, 0),
                                                     lfo2.buffer.getSample(0, 0));
        noisePanner.setPan(panValue);
        noisePanner.process(noiseContext);

        
        for (int channel = 0; channel < outputBuffer.getNumChannels(); channel++)
        {
            outputBuffer.addFrom(channel, startSample, noiseBuffer, channel, 0, numSamples);
        }
    }
    
    if (! oscAmpEnv.isActive() && ! noiseAmpEnv.isActive())
        clearCurrentNote();
}

void MyVoice::setWaveType(const int typeNum)
{
    jassert(typeNum >= 0 && typeNum <= 3);
    oscUI->setParamValue("wave", typeNum);
}

void MyVoice::setFrequency(const float newFreq)
{
    oscUI->setParamValue("freq", newFreq);
}

void MyVoice::setFilterType(const int typeNum)
{
    jassert(typeNum >= 0 && typeNum <= 3);
    noiseUI->setParamValue("fil_type", typeNum);
}

void MyVoice::setCutoff(const float newCutoff)
{
    noiseUI->setParamValue("cutoff", newCutoff);
}

void MyVoice::setQ(const float newQ)
{
    noiseUI->setParamValue("q", newQ);
}

void MyVoice::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
{
    if (property == ParamIDs::oscIO)
    {
        oscIO.store(synthTree[property]);
        if (!oscIO.load())
        {
            oscAmpEnv.reset();
            oscPitchEnv.reset();
        }
    }
    else if (property == ParamIDs::oscWaveType)
    {
        int waveNum = synthTree.getProperty(property);
        setWaveType(waveNum);
    }
    else if (property == ParamIDs::pitchEnvMode)
    {
        pitchEnvMode = synthTree.getProperty(property);
        setFrequency(DSPHelper::getFrequencyFromMidi(currentMidiNote));
    }
    else if (property == ParamIDs::noiseIO)
    {
        noiseIO.store(synthTree[property]);
        if (!noiseIO.load())
        {
            noiseAmpEnv.reset();
            noiseFilterEnv.reset();
        }
    }
    else if (property == ParamIDs::filterType)
    {
        setFilterType(synthTree.getProperty(property));
    }
    else if (property == ParamIDs::filterEnvMode)
    {
        filterEnvMode = synthTree.getProperty(property);
        if (filterEnvMode != 2)
            setFilterType(synthTree.getProperty(ParamIDs::filterType));
        else
            setFilterType(3);
    }
    else if (property == ParamIDs::filterKeyFollowIO)
    {
        filterKeyFollowIO.store(synthTree[property]);
    }
}

void MyVoice::parameterChanged (const juce::String& parameterID, float newValue)
{
    if (parameterID == ParamIDs::oscAttackAmp)
    {
        oscAmpEnvParams.attack = newValue;
        oscAmpEnv.setParameters(oscAmpEnvParams);
    }
    else if (parameterID == ParamIDs::oscDecayAmp)
    {
        oscAmpEnvParams.decay = newValue;
        oscAmpEnv.setParameters(oscAmpEnvParams);
    }
    else if (parameterID == ParamIDs::oscReleaseAmp)
    {
        oscAmpEnvParams.release = newValue;
        oscAmpEnv.setParameters(oscAmpEnvParams);
    }
    else if (parameterID == ParamIDs::oscInitialAmp)
    {
        oscAmpEnvParams.initial = juce::Decibels::decibelsToGain(newValue);
        oscAmpEnv.setParameters(oscAmpEnvParams);
    }
    else if (parameterID == ParamIDs::oscPeakAmp)
    {
        oscAmpEnvParams.peak = juce::Decibels::decibelsToGain(newValue);
        oscAmpEnv.setParameters(oscAmpEnvParams);
    }
    else if (parameterID == ParamIDs::oscSustainAmp)
    {
        oscAmpEnvParams.sustain = juce::Decibels::decibelsToGain(newValue);
        oscAmpEnv.setParameters(oscAmpEnvParams);
    }
    else if (parameterID == ParamIDs::oscAttackPitch)
    {
        oscPitchEnvParams.attack = newValue;
        oscPitchEnv.setParameters(oscPitchEnvParams);
    }
    else if (parameterID == ParamIDs::oscDecayPitch)
    {
        oscPitchEnvParams.decay = newValue;
        oscPitchEnv.setParameters(oscPitchEnvParams);
    }
    else if (parameterID == ParamIDs::oscReleasePitch)
    {
        oscPitchEnvParams.release = newValue;
        oscPitchEnv.setParameters(oscPitchEnvParams);
    }
    else if (parameterID == ParamIDs::oscInitialPitch)
    {
        oscPitchEnvParams.initial = newValue;
        oscPitchEnv.setParameters(oscPitchEnvParams);
    }
    else if (parameterID == ParamIDs::oscPeakPitch)
    {
        oscPitchEnvParams.peak = newValue;
        oscPitchEnv.setParameters(oscPitchEnvParams);
    }
    else if (parameterID == ParamIDs::oscSustainPitch)
    {
        oscPitchEnvParams.sustain = newValue;
        oscPitchEnv.setParameters(oscPitchEnvParams);
    }
    else if (parameterID == ParamIDs::oscEndPitch)
    {
        oscPitchEnvParams.end = newValue;
        oscPitchEnv.setParameters(oscPitchEnvParams);
    }
    else if (parameterID == ParamIDs::noiseAttackAmp)
    {
        noiseAmpEnvParams.attack = newValue;
        noiseAmpEnv.setParameters(noiseAmpEnvParams);
    }
    else if (parameterID == ParamIDs::noiseDecayAmp)
    {
        noiseAmpEnvParams.decay = newValue;
        noiseAmpEnv.setParameters(noiseAmpEnvParams);
    }
    else if (parameterID == ParamIDs::noiseReleaseAmp)
    {
        noiseAmpEnvParams.release = newValue;
        noiseAmpEnv.setParameters(noiseAmpEnvParams);
    }
    else if (parameterID == ParamIDs::noiseInitialAmp)
    {
        noiseAmpEnvParams.initial = juce::Decibels::decibelsToGain(newValue);
        noiseAmpEnv.setParameters(noiseAmpEnvParams);
    }
    else if (parameterID == ParamIDs::noisePeakAmp)
    {
        noiseAmpEnvParams.peak = juce::Decibels::decibelsToGain(newValue);
        noiseAmpEnv.setParameters(noiseAmpEnvParams);
    }
    else if (parameterID == ParamIDs::noiseSustainAmp)
    {
        noiseAmpEnvParams.sustain = juce::Decibels::decibelsToGain(newValue);
        noiseAmpEnv.setParameters(noiseAmpEnvParams);
    }
    else if (parameterID == ParamIDs::noiseAttackCutoff)
    {
        noiseFilterEnvParams.attack = newValue;
        noiseFilterEnv.setParameters(noiseFilterEnvParams);
    }
    else if (parameterID == ParamIDs::noiseDecayCutoff)
    {
        noiseFilterEnvParams.decay = newValue;
        noiseFilterEnv.setParameters(noiseFilterEnvParams);
    }
    else if (parameterID == ParamIDs::noiseReleaseCutoff)
    {
        noiseFilterEnvParams.release = newValue;
        noiseFilterEnv.setParameters(noiseFilterEnvParams);
    }
    else if (parameterID == ParamIDs::noiseInitialCutoff)
    {
        noiseFilterEnvParams.initial = newValue;
        noiseFilterEnv.setParameters(noiseFilterEnvParams);
    }
    else if (parameterID == ParamIDs::noisePeakCutoff)
    {
        noiseFilterEnvParams.peak = newValue;
        noiseFilterEnv.setParameters(noiseFilterEnvParams);
    }
    else if (parameterID == ParamIDs::noiseSustainCutoff)
    {
        noiseFilterEnvParams.sustain = newValue;
        noiseFilterEnv.setParameters(noiseFilterEnvParams);
    }
    else if (parameterID == ParamIDs::noiseEndCutoff)
    {
        noiseFilterEnvParams.end = newValue;
        noiseFilterEnv.setParameters(noiseFilterEnvParams);
    }
}
