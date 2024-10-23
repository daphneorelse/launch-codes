/*
  ==============================================================================

    DelayModule.h
    Created: 19 Sep 2024 12:24:28pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../PluginProcessor.h"
#include "DelayKnob.h"
#include "ValueTreeComponents.h"
#include "TextSlider.h"
#include "DialKnob.h"
#include "DelayFilter.h"
#include "GUIHelper.h"
#include "ComponentIDs.h"
#include "EffectsSlot.h"
#include "CrosshairDragger.h"
#include "../service/ParameterHelper.h"
#include "../service/ParamIDs.h"

class DelayModule  : public EffectsSlot
{
public:
    DelayModule(PluginProcessor& processor,
                juce::AudioProcessorValueTreeState& apvts,
                juce::ValueTree delayTree,
                juce::ValueTree lfoTree,
                juce::ValueTree effectsOrderTree);

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    PluginProcessor& processor;
    juce::AudioProcessorValueTreeState& apvts;
    juce::ValueTree delayTree;
    juce::ValueTree lfoTree;
    juce::ValueTree effectsOrderTree;
    
    juce::Rectangle<int> mainArea;
    
    std::unique_ptr<juce::Drawable> header, timeIcon, syncIcon, unlinkedIcon, linkedIcon;
    
    CrosshairDragger crosshair;
    
    DelayKnob delayLeftTime;
    DelayKnob delayLeftSync;
    DelayKnob delayLeftTimeCopy;
    DelayKnob delayLeftSyncCopy;
    DelayKnob delayRightTime;
    DelayKnob delayRightSync;
    
    ComponentSwitcher<DelayKnob> delayLeftSyncSwitcher;
    ComponentSwitcher<DelayKnob> delayLeftSyncSwitcherCopy;
    ComponentSwitcher<DelayKnob> delayRightSyncSwitcher;
    ComponentSwitcher<ComponentSwitcher<DelayKnob>> delayRightLinkSwitcher;
    
    TextSlider delayOffsetLeft;
    TextSlider delayOffsetRight;
    
    ValueTreeToolButton delayLeftSyncButton;
    ValueTreeToolButton delayLeftSyncButtonCopy;
    ValueTreeToolButton delayRightSyncButton;
    ComponentSwitcher<ValueTreeToolButton> delayRightSyncButtonSwitcher;
    ValueTreeToolButton delayLinkButton;
    
    DialKnob delayFeedbackKnob;
    
    DelayFilter delayFilter;
    
    TextSlider mixSlider;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayModule)
};
