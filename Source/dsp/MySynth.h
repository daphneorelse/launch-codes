/*
  ==============================================================================

    OscillatorSynth.h
    Created: 4 Jun 2024 4:36:08pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MyADSR.h"
#include "FaustOscSynth.h"
#include "LFO.h"
#include "DSPHelper.h"
#include "../service/ParameterHelper.h"
#include "../service/ParamIDs.h"

struct MySound : public juce::SynthesiserSound
{
    MySound() {}
 
    bool appliesToNote    (int) override        { return true; }
    bool appliesToChannel (int) override        { return true; }
};

struct MyVoice : public juce::SynthesiserVoice
                , public juce::ValueTree::Listener
                , public juce::AudioProcessorValueTreeState::Listener
{
    MyVoice(juce::AudioProcessorValueTreeState& apvts,
             juce::ValueTree nonParamTree,
             LFO& lfo1,
             LFO& lfo2,
             juce::OwnedArray<juce::dsp::LookupTableTransform<float>>& oscAmpTables,
             juce::OwnedArray<juce::dsp::LookupTableTransform<float>>& oscPitchTables,
             juce::OwnedArray<juce::dsp::LookupTableTransform<float>>& noiseAmpTables,
             juce::OwnedArray<juce::dsp::LookupTableTransform<float>>& noisePitchTables);
    
    ~MyVoice();

    bool canPlaySound (juce::SynthesiserSound* sound) override;

    void startNote (int midiNoteNumber, float velocity,
                    juce::SynthesiserSound* sound, int /*currentPitchWheelPosition*/) override;

    void stopNote (float /*velocity*/, bool allowTailOff) override;

    void pitchWheelMoved (int) override      {}
    void controllerMoved (int, int) override {}
    
    void prepareToPlay (double sampleRate, int samplesPerBlock, int numOutputChannels);

    void renderNextBlock (juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override;
    
    void setWaveType (const int typeNum);
    void setFrequency (const float newFreq);
    void setFilterType (const int typeNum);
    void setCutoff (const float newCutoff);
    void setQ (const float newQ);
    
    void valueTreePropertyChanged (juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;
    
    void parameterChanged (const juce::String& parameterID, float newValue) override;
    
    
private:
    juce::AudioProcessorValueTreeState& apvts;
    juce::ValueTree synthTree;
    juce::ValueTree lfoTree;
    
    int wave = 0;
    int currentMidiNote { -1 };
    
    juce::AudioParameterFloat* oscGainParam { nullptr };
    juce::AudioParameterFloat* oscPanParam { nullptr };
    juce::AudioParameterInt*   oscOctaveParam { nullptr };
    juce::AudioParameterInt*   oscSemitoneParam { nullptr };
    juce::AudioParameterInt*   oscFineParam { nullptr };
    juce::AudioParameterFloat* oscCoarseParam { nullptr };
    juce::AudioParameterFloat* noiseGainParam { nullptr };
    juce::AudioParameterFloat* noisePanParam { nullptr };
    juce::AudioParameterFloat* noiseQParam { nullptr };
    juce::AudioParameterFloat* noiseCoarseParam { nullptr };
    
    juce::AudioBuffer<float> oscBuffer;
    juce::AudioBuffer<float> noiseBuffer;
    
    LFO& lfo1;
    LFO& lfo2;
    
    juce::dsp::Gain<float> oscGain;
    juce::dsp::Gain<float> noiseGain;
    juce::dsp::Panner<float> oscPanner;
    juce::dsp::Panner<float> noisePanner;
    
    MyADSR oscAmpEnv;
    MyADSR::Parameters oscAmpEnvParams;
    
    MyADSR oscPitchEnv;
    MyADSR::Parameters oscPitchEnvParams;
    
    MyADSR noiseAmpEnv;
    MyADSR::Parameters noiseAmpEnvParams;
    
    MyADSR noiseFilterEnv;
    MyADSR::Parameters noiseFilterEnvParams;
    
    float sampleRate;
    int numSamplesIncrement;
    int sampleIndex;

    std::atomic<bool> oscIO;
    std::atomic<int> pitchEnvMode;
    std::atomic<bool> noiseIO;
    std::atomic<int> filterEnvMode;
    std::atomic<bool> filterKeyFollowIO;
    
    MapUI* oscUI;
    dsp* oscDSP;
    float** oscOutput;
    MapUI* noiseUI;
    dsp* noiseDSP;
    float** noiseOutput;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MyVoice);
};
