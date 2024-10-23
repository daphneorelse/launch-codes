/*
  ==============================================================================

    ReverbModule.h
    Created: 19 Sep 2024 12:24:38pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../PluginProcessor.h"
#include "EffectsSlot.h"
#include "DialKnob.h"
#include "ComponentIDs.h"
#include "GUIHelper.h"
#include "CrosshairDragger.h"
#include "TextSlider.h"
#include "XYControlPad.h"
#include "../service/ParamIDs.h"

class ReverbModule  : public EffectsSlot
{
public:
    ReverbModule(PluginProcessor& processor,
                juce::AudioProcessorValueTreeState& apvts,
                juce::ValueTree lfoTree,
                juce::ValueTree effectsOrderTree);
    ~ReverbModule() override;
    
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    PluginProcessor& processor;
    juce::AudioProcessorValueTreeState& apvts;
    juce::ValueTree lfoTree;
    juce::ValueTree effectsOrderTree;
    
    juce::Rectangle<int> mainArea;
    
    std::unique_ptr<juce::Drawable> header;
    
    CrosshairDragger crosshair;
    
    XYControlPad xyPad;
    
    DialKnob reverbSizeKnob;
    DialKnob reverbDampingKnob;
    DialKnob reverbWidthKnob;
    
    TextSlider mixSlider;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReverbModule)
};
