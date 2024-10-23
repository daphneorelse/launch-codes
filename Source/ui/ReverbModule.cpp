/*
  ==============================================================================

    ReverbModule.cpp
    Created: 19 Sep 2024 12:24:38pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ReverbModule.h"

ReverbModule::ReverbModule(PluginProcessor& t_processor,
                           juce::AudioProcessorValueTreeState& t_apvts,
                           juce::ValueTree t_lfoTree,
                           juce::ValueTree effectsOrderTree)
: EffectsSlot(ParamIDs::reverbNode, effectsOrderTree)
, processor(t_processor)
, apvts(t_apvts)
, lfoTree(t_lfoTree)
, header(juce::Drawable::createFromImageData(BinaryData::reverb_header_svg, BinaryData::reverb_header_svgSize))
, crosshair(ParamIDs::reverbNode.toString())
, xyPad(*apvts.getParameter(ParamIDs::reverbWidth), *apvts.getParameter(ParamIDs::reverbSize), *apvts.getParameter(ParamIDs::reverbDamping))
, reverbSizeKnob   (*apvts.getParameter(ParamIDs::reverbSize), "Size", lfoTree.getChildWithName(ParamIDs::lfoReverbSize))
, reverbDampingKnob   (*apvts.getParameter(ParamIDs::reverbDamping), "Damping", lfoTree.getChildWithName(ParamIDs::lfoReverbDamping))
, reverbWidthKnob  (*apvts.getParameter(ParamIDs::reverbWidth), "Width", lfoTree.getChildWithName(ParamIDs::lfoReverbWidth))
, mixSlider (*apvts.getParameter(ParamIDs::reverbMix), "Mix", TextSlider::displayColorRange, lfoTree.getChildWithName(ParamIDs::lfoReverbMix))
{
    setOpaque(true);
    
    addAndMakeVisible(crosshair);
    addAndMakeVisible(xyPad);
    addAndMakeVisible(reverbSizeKnob);
    addAndMakeVisible(reverbDampingKnob);
    addAndMakeVisible(reverbWidthKnob);
    addAndMakeVisible(mixSlider);
    
    crosshair.setComponentID(ComponentIDs::effectCrosshair);
    xyPad.setComponentID(ComponentIDs::reverbPad);
    reverbSizeKnob.setComponentID(ComponentIDs::reverbSize);
    reverbDampingKnob.setComponentID(ComponentIDs::reverbDamping);
    reverbWidthKnob.setComponentID(ComponentIDs::reverbWidth);
    mixSlider.setComponentID(ComponentIDs::reverbMix);
}

ReverbModule::~ReverbModule()
{
}

void ReverbModule::paint (juce::Graphics& g)
{
    g.fillAll(MyColors::background);
    
    header->drawWithin(g, getLocalBounds().toFloat(), juce::RectanglePlacement::xRight, 1.0f);
    
    if (paintDragBorder)
    {
        g.setColour(MyColors::lightSecondary);
        GUIHelper::paintOutline(g, mainArea);
    }
}

void ReverbModule::resized()
{
    mainArea = getLocalBounds().withTrimmedLeft(MyWidths::additionalHeaderWidth).withTrimmedTop(MyWidths::additionalHeaderHeight);
    juce::Rectangle<int> bounds = mainArea;
    
    crosshair.setBounds(MyWidths::buttonBounds.withX(4).withY(9));
    mixSlider.setBounds(MyWidths::smallTextSliderBounds.withX(1).withBottomY(getHeight() - 16));
    
    juce::Rectangle<int> xyPadBounds (165, 165);
    xyPad.setBounds(xyPadBounds.withCentre(bounds.removeFromTop(180).getCentre()));
    
    juce::Rectangle<int> knobsArea = bounds.removeFromTop(MyWidths::knobDimension);
    reverbWidthKnob.setBounds(knobsArea.removeFromLeft(MyWidths::knobDimension));
    reverbSizeKnob.setBounds(knobsArea.removeFromLeft(MyWidths::knobDimension));
    reverbDampingKnob.setBounds(knobsArea.removeFromLeft(MyWidths::knobDimension));
}

