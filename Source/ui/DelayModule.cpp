/*
  ==============================================================================

    DelayModule.cpp
    Created: 19 Sep 2024 12:24:28pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "DelayModule.h"

//==============================================================================
DelayModule::DelayModule(PluginProcessor& t_processor,
                         juce::AudioProcessorValueTreeState& t_apvts,
                         juce::ValueTree t_delayTree,
                         juce::ValueTree t_lfoTree,
                         juce::ValueTree effectsOrderTree)
: EffectsSlot(ParamIDs::delayNode, effectsOrderTree)
, processor(t_processor)
, apvts(t_apvts)
, delayTree(t_delayTree)
, lfoTree(t_lfoTree)
, header(juce::Drawable::createFromImageData(BinaryData::delay_header_svg, BinaryData::delay_header_svgSize))
, timeIcon(juce::Drawable::createFromImageData(BinaryData::frequencyTime_svg, BinaryData::frequencyTime_svgSize))
, syncIcon(juce::Drawable::createFromImageData(BinaryData::frequencySync_svg, BinaryData::frequencySync_svgSize))
, unlinkedIcon(juce::Drawable::createFromImageData(BinaryData::unlinked_svg, BinaryData::unlinked_svgSize))
, linkedIcon(juce::Drawable::createFromImageData(BinaryData::linked_svg, BinaryData::linked_svgSize))
, crosshair(ParamIDs::delayNode.toString())
, delayLeftTime(*apvts.getParameter(ParamIDs::leftDelayTime), "Time L", lfoTree.getChildWithName(ParamIDs::lfoLeftDelayTime))
, delayLeftSync(*apvts.getParameter(ParamIDs::leftDelaySync), "Sync L", lfoTree.getChildWithName(ParamIDs::lfoLeftDelaySync), ParameterHelper::syncOptions.size())
, delayLeftTimeCopy(*apvts.getParameter(ParamIDs::leftDelayTime), "Time R", lfoTree.getChildWithName(ParamIDs::lfoLeftDelayTime))
, delayLeftSyncCopy(*apvts.getParameter(ParamIDs::leftDelaySync), "Sync R", lfoTree.getChildWithName(ParamIDs::lfoLeftDelaySync), ParameterHelper::syncOptions.size())
, delayRightTime(*apvts.getParameter(ParamIDs::rightDelayTime), "Time R", lfoTree.getChildWithName(ParamIDs::lfoRightDelayTime))
, delayRightSync(*apvts.getParameter(ParamIDs::rightDelaySync), "Sync R", lfoTree.getChildWithName(ParamIDs::lfoRightDelaySync), ParameterHelper::syncOptions.size())
, delayLeftSyncSwitcher(delayLeftTime, delayLeftSync, delayTree, ParamIDs::delaySyncLeft)
, delayLeftSyncSwitcherCopy(delayLeftTimeCopy, delayLeftSyncCopy, delayTree, ParamIDs::delaySyncLeft)
, delayRightSyncSwitcher(delayRightTime, delayRightSync, delayTree, ParamIDs::delaySyncRight)
, delayRightLinkSwitcher(delayRightSyncSwitcher, delayLeftSyncSwitcherCopy, delayTree, ParamIDs::delayLink)
, delayOffsetLeft(*apvts.getParameter(ParamIDs::leftDelayOffset), "Offset L", 0, delayTree.getChildWithName(ParamIDs::lfoLeftDelayOffset))
, delayOffsetRight(*apvts.getParameter(ParamIDs::rightDelayOffset), "Offset R", 0, delayTree.getChildWithName(ParamIDs::lfoRightDelayOffset))
, delayLeftSyncButton (delayTree, ParamIDs::delaySyncLeft, timeIcon, syncIcon)
, delayLeftSyncButtonCopy (delayTree, ParamIDs::delaySyncLeft, timeIcon, syncIcon)
, delayRightSyncButton (delayTree, ParamIDs::delaySyncRight, timeIcon, syncIcon)
, delayRightSyncButtonSwitcher(delayRightSyncButton, delayLeftSyncButtonCopy, delayTree, ParamIDs::delayLink)
, delayLinkButton (delayTree, ParamIDs::delayLink, unlinkedIcon, linkedIcon)
, delayFeedbackKnob(*apvts.getParameter(ParamIDs::delayFeedback), "Feedback", lfoTree.getChildWithName(ParamIDs::lfoDelayFeedback))
, delayFilter(*apvts.getParameter(ParamIDs::delayFilterCenter), *apvts.getParameter(ParamIDs::delayFilterWidth), delayTree, lfoTree, processor.getSampleRate())
, mixSlider(*apvts.getParameter(ParamIDs::delayMix), "Mix", TextSlider::displayColorRange, lfoTree.getChildWithName(ParamIDs::lfoDelayMix))
{
    setOpaque(true);
    
    addAndMakeVisible(crosshair);
    addAndMakeVisible(delayLeftSyncSwitcher);
    addAndMakeVisible(delayRightLinkSwitcher);
    addAndMakeVisible(delayOffsetLeft);
    addAndMakeVisible(delayOffsetRight);
    addAndMakeVisible(delayLeftSyncButton);
    addAndMakeVisible(delayRightSyncButtonSwitcher);
    addAndMakeVisible(delayLinkButton);
    addAndMakeVisible(delayFeedbackKnob);
    addAndMakeVisible(delayFilter);
    addAndMakeVisible(mixSlider);
    
    crosshair.setComponentID(ComponentIDs::effectCrosshair);
    delayLeftSyncSwitcher.setComponentID(ComponentIDs::delayLeft);
    delayRightLinkSwitcher.setComponentID(ComponentIDs::delayRight);
    delayLinkButton.setComponentID(ComponentIDs::delayLink);
    delayOffsetLeft.setComponentID(ComponentIDs::delayOffsetLeft);
    delayOffsetRight.setComponentID(ComponentIDs::delayOffsetRight);
    delayLeftSyncButton.setComponentID(ComponentIDs::delaySyncLeft);
    delayRightSyncButton.setComponentID(ComponentIDs::delaySyncRight);
    delayFeedbackKnob.setComponentID(ComponentIDs::delayFeedback);
    mixSlider.setComponentID(ComponentIDs::delayMix);
    delayFilter.setComponentID(ComponentIDs::delayFilter);
}

void DelayModule::paint (juce::Graphics& g)
{
    g.fillAll(MyColors::background);
    
    header->drawWithin(g, getLocalBounds().toFloat(), juce::RectanglePlacement::xRight, 1.0f);
    
    if (paintDragBorder)
    {
        g.setColour(MyColors::lightSecondary);
        GUIHelper::paintOutline(g, mainArea);
    }
}

void DelayModule::resized()
{
    mainArea = getLocalBounds().withTrimmedLeft(MyWidths::additionalHeaderWidth).withTrimmedTop(MyWidths::additionalHeaderHeight);
    juce::Rectangle<int> bounds = mainArea;
    
    crosshair.setBounds(MyWidths::buttonBounds.withX(4).withY(9));
    mixSlider.setBounds(MyWidths::smallTextSliderBounds.withX(1).withBottomY(getHeight() - 16));
    
    const int margin = MyWidths::standardMarginInt;
    
    bounds.removeFromTop(margin);
    juce::Rectangle<int> delayKnobsArea = bounds.removeFromTop(MyWidths::knobDimension);
    delayLinkButton.setBounds(MyWidths::buttonBounds.withCentre(delayKnobsArea.getCentre()));
    juce::Rectangle<int> delayKnobLeftArea = delayKnobsArea.removeFromLeft(delayKnobsArea.proportionOfWidth(0.5f));
    delayLeftSyncSwitcher.setBounds(MyWidths::knobBounds.withCentre(delayKnobLeftArea.getCentre()));
    juce::Rectangle<int> delayKnobRightArea = delayKnobsArea;
    delayRightLinkSwitcher.setBounds(MyWidths::knobBounds.withCentre(delayKnobRightArea.getCentre()));
    bounds.removeFromTop(margin);
    
    static const int delayOffsetHeight = MyWidths::buttonDimension;
    juce::Rectangle<int> delayOffsetsArea = bounds.removeFromTop(delayOffsetHeight);
    juce::Rectangle<int> delayLeftOffsetsArea = delayOffsetsArea.removeFromLeft(delayOffsetsArea.proportionOfWidth(0.5f));
    delayLeftSyncButton.setBounds(delayLeftOffsetsArea.removeFromRight(delayOffsetHeight));
    delayOffsetsArea.removeFromRight(margin);
    delayOffsetLeft.setBounds(delayLeftOffsetsArea);
    juce::Rectangle<int> delayRightOffsetsArea = delayOffsetsArea;
    delayRightSyncButtonSwitcher.setBounds(delayRightOffsetsArea.removeFromRight(delayOffsetHeight));
    delayOffsetsArea.removeFromRight(margin);
    delayOffsetRight.setBounds(delayRightOffsetsArea);
    
    bounds.removeFromTop(margin);
    
    static const int delayFilterHeight = 90;
    delayFilter.setBounds(bounds.removeFromTop(delayFilterHeight));
    bounds.removeFromTop(margin);
    
    delayFeedbackKnob.setBounds(MyWidths::knobBounds.withCentre(bounds.removeFromTop(MyWidths::knobDimension).getCentre()));
}
