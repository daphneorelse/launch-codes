/*
  ==============================================================================

    EditorContent.h
    Created: 23 Aug 2024 2:57:21pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../PluginProcessor.h"
#include "ComponentIDs.h"
#include "KeyKnob.h"
#include "ValueTreeComponents.h"
#include "TextSlider.h"
#include "EnvelopeView.h"
#include "WavePicker.h"
#include "FilterPicker.h"
#include "ScrollWheel.h"
#include "PresetPanel.h"
#include "LFOModule.h"
#include "DescriptionBox.h"
#include "MidiButton.h"
#include "FilterModule.h"
#include "DelayModule.h"
#include "ReverbModule.h"
#include "WaveshaperModule.h"
#include "LevelMeter.h"
#include "ControlLever.h"
#include "lnfs/EditorLnf.h"

class EditorContent  : public juce::Component
                     , public juce::DragAndDropContainer
                     , public juce::ValueTree::Listener
{
public:
    EditorContent (PluginProcessor&, juce::UndoManager&);
    ~EditorContent() override;
    
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void setEffectsBounds();
    void valueTreeChildOrderChanged (juce::ValueTree &parentTreeWhoseChildrenHaveMoved, int oldIndex, int newIndex) override;

private:
    PluginProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& apvts;
    juce::ValueTree nonParamTree;
    juce::ValueTree lfoTree;
    juce::ValueTree effectsOrderTree;
    
    juce::Rectangle<int> logoArea;
    std::unique_ptr<juce::Drawable> logoGroup, timeIcon, syncIcon, unlinkedIcon, linkedIcon, oneshotIcon, loopIcon, offIcon, pianoIcon;
    std::unique_ptr<juce::Drawable> oscillatorHeader, noiseHeader, lfoHeader, buttonHeader, meterHeader, bottomRightMarkings;
    juce::Rectangle<int> oscillatorHeaderArea, noiseHeaderArea, lfoHeaderArea, buttonHeaderArea, meterHeaderArea, bottomRightMarkingsArea;
    
    const int effectWidth = 200;
    const int moduleHeight = 275;
    const int additionalHeaderWidth = MyWidths::additionalHeaderWidth;
    const int additionalHeaderHeight = MyWidths::additionalHeaderHeight;
    juce::Rectangle<int> effectsBounds;
    
    DefaultLNF defaultLNF;
    
    PresetPanel presetPanel;
    
    ValueTreeToggleSwitch oscSwitch;
    KeyKnob oscGainKnob;
    ScrollWheel oscPanWheel;
    
    WavePicker oscWavePicker;
    
    EnvelopeView oscAmpEnv;
    EnvelopeView oscPitchEnv;
    
    TextSlider oscOctaveSlider;
    TextSlider oscSemitoneSlider;
    TextSlider oscFineSlider;
    TextSlider oscCoarseSlider;
    
    ValueTreeToolButton oscAmpEnvToggler;
    ValueTreeToolMultiToggler pitchEnvToggler;
    
    ValueTreeToggleSwitch noiseSwitch;
    KeyKnob noiseGainKnob;
    ScrollWheel noisePanWheel;
    
    FilterPicker noiseFilterPicker;
    TextSlider filterQSlider;
    
    EnvelopeView noiseAmpEnv;
    EnvelopeView noiseFilterEnv;
    
    TextSlider noiseCoarseSlider;
    
    ValueTreeToolButton noiseAmpEnvToggler;
    ValueTreeToolMultiToggler filterEnvToggler;
    ValueTreeToolButton filterKeyFollowButton;
    
    DelayModule delayModule;
    ReverbModule reverbModule;
    FilterModule filterModule;
    WaveshaperModule waveshaperModule;
    
    LFOModule lfo1Module;
    LFOModule lfo2Module;
    
    MidiButton bigRedButton;
    TextSlider midiNoteSlider;
    
    MeterPair levelMeters;
    ControlLever volumeLever;
    
    DescriptionBox descriptionBox;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EditorContent)
};
