/*
  ==============================================================================

    LFOModule.h
    Created: 30 Aug 2024 10:18:51am
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ComponentIDs.h"
#include "GUIHelper.h"
#include "CrosshairDragger.h"
#include "LFOVisualizer.h"
#include "DialKnob.h"
#include "TextSlider.h"
#include "WavePicker.h"
#include "ValueTreeComponents.h"
#include "../service/ParamIDs.h"

class LFOModule  : public juce::Component
                 , public juce::ValueTree::Listener
{
public:
    LFOModule(juce::ValueTree lfoTree, juce::RangedAudioParameter& frequencyTimeParam, juce::RangedAudioParameter& frequencySyncParam, juce::RangedAudioParameter& rangeParam,
        LFOVisualizer& t_visualizer);
    ~LFOModule() override;
    
    void mouseDown (const juce::MouseEvent& event) override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;

private:
    juce::ValueTree lfoTree;
    juce::ValueTree selectionTree;
    
    std::unique_ptr<juce::Drawable> timeIcon, syncIcon;
    
    CrosshairDragger dragger;
    LFOVisualizer& visualizer;
    TextSliderSwitcher frequencySlider;
    TextSlider rangeKnob;
    WavePicker wavePicker;
    ValueTreeToolButton syncButton;
    
    juce::Rectangle<int> visualizerArea;
    
    bool shouldDrawSelected;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LFOModule)
};
