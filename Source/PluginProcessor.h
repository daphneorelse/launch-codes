#pragma once

#include <JuceHeader.h>
#include "dsp/LFO.h"
#include "dsp/MySynth.h"
#include "dsp/MyADSR.h"
#include "dsp/FaustOscSynth.h"
#include "dsp/DelayProcessor.h"
#include "dsp/ReverbProcessor.h"
#include "dsp/FilterProcessor.h"
#include "dsp/WaveshaperProcessor.h"
#include "service/ParamIDs.h"
#include "service/PresetManager.h"
#include "service/ParameterHelper.h"
#include "service/farbot/fifo.hpp"
#include "ui/LFOVisualizer.h"

class PluginProcessor  : public juce::AudioProcessor,
                         public juce::AudioProcessorValueTreeState::Listener,
                         public juce::ValueTree::Listener
{
public:
    PluginProcessor();
    ~PluginProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    
    float getRMSLevel (const int channel);
    float getSimplePeak (const int channel);
    
    void parameterChanged (const juce::String& parameterID, float newValue) override;
    void valueTreeChildOrderChanged (juce::ValueTree &parentTreeWhoseChildrenHaveMoved, int oldIndex, int newIndex) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    juce::AudioProcessorValueTreeState& getPluginAPVST();
    juce::ValueTree getPluginNonParamTree();
    PresetManager& getPresetManager();
    LFOVisualizer& getLFOVisualizer (const int lfoNum);
    
    // TODO Maybe make private and pass to PluginEditor constructor
    juce::OwnedArray<juce::dsp::LookupTableTransform<float>> oscAmpTables;
    juce::OwnedArray<juce::dsp::LookupTableTransform<float>> oscPitchTables;
    juce::OwnedArray<juce::dsp::LookupTableTransform<float>> noiseAmpTables;
    juce::OwnedArray<juce::dsp::LookupTableTransform<float>> noiseFilterTables;
    juce::dsp::LookupTableTransform<float> waveshaperControlTable;
    juce::dsp::LookupTableTransform<float> waveshaperTransformTable;
    
    farbot::fifo<juce::MidiMessage, farbot::fifo_options::concurrency::single, farbot::fifo_options::concurrency::single, farbot::fifo_options::full_empty_failure_mode::return_false_on_full_or_empty, farbot::fifo_options::full_empty_failure_mode::return_false_on_full_or_empty> midiFifo;
    
    juce::AudioProcessorGraph::Node::Ptr inputNode, outputNode, delayNode, reverbNode, filterNode, waveshaperNode;
    
private:
    void connectEffectsGraph();
    void updateTableWithSlope (juce::dsp::LookupTableTransform<float>* table, const float newSlope);
    
    juce::AudioProcessorValueTreeState apvts;
    juce::ValueTree nonParamStateTree;
    PresetManager presetManager;
    
    juce::AudioProcessorGraph effectsProcessorGraph;
    juce::ReferenceCountedArray<juce::AudioProcessorGraph::Node> effectsOrder;
    juce::ValueTree effectsOrderTree;
    
    std::atomic<bool> shouldUpdateEffectsGraph = false;
    
    juce::UndoManager undoManager;
    
    juce::Synthesiser synth;
    const int numOscVoices { 8 };
    
    std::atomic<float> bpm { 120.0f };
    std::atomic<float> leftRMS { 0.0f }, rightRMS { 0.0f };
    std::atomic<float> leftPeak { 0.0f }, rightPeak { 0.0f };
    
    juce::ValueTree lfoTree;
    LFO lfo1;
    LFO lfo2;
    
    juce::AudioParameterFloat* masterGainParam { nullptr };
    juce::dsp::Gain<float> masterGain;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor);
};
