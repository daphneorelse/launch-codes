/*
  ==============================================================================

    LFOModule.cpp
    Created: 30 Aug 2024 10:18:51am
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "LFOModule.h"

LFOModule::LFOModule(juce::ValueTree t_lfoTree, juce::RangedAudioParameter& frequencyTimeParam, juce::RangedAudioParameter& frequencySyncParam, juce::RangedAudioParameter& rangeParam, LFOVisualizer& t_visualizer)
: lfoTree(t_lfoTree)
, selectionTree(lfoTree.getParent().getChildWithName(ParamIDs::lfoSelectionTree))
, timeIcon(juce::Drawable::createFromImageData(BinaryData::frequencyHertz_svg, BinaryData::frequencyHertz_svgSize))
, syncIcon(juce::Drawable::createFromImageData(BinaryData::frequencySync_svg, BinaryData::frequencySync_svgSize))
, dragger(lfoTree.getType().toString())
, visualizer(t_visualizer)
, frequencySlider(frequencyTimeParam, frequencySyncParam, lfoTree)
, rangeKnob(rangeParam, "Range")
, wavePicker(lfoTree, ParamIDs::lfoWaveType)
, syncButton(lfoTree, ParamIDs::lfoSync, timeIcon, syncIcon)
{
    setOpaque(true);
    
    selectionTree.addListener(this);
    lfoTree.addListener(this);
    
    shouldDrawSelected = static_cast<int>(selectionTree[ParamIDs::lfoSelected]) == (lfoTree.getType() == ParamIDs::lfo1 ? 0 : 1);
    
    addAndMakeVisible(dragger);
    addAndMakeVisible(visualizer);
    addAndMakeVisible(frequencySlider);
    addAndMakeVisible(rangeKnob);
    addAndMakeVisible(wavePicker);
    addAndMakeVisible(syncButton);
    
    dragger.setComponentID(ComponentIDs::lfoDragger);
    visualizer.setComponentID(ComponentIDs::lfoVisualizer);
    frequencySlider.setComponentID(ComponentIDs::lfoFrequency);
    rangeKnob.setComponentID(ComponentIDs::lfoRange);
    wavePicker.setComponentID(ComponentIDs::lfoWavePicker);
    syncButton.setComponentID(ComponentIDs::lfoSync);
    
    for (auto* child: getChildren())
        child->addMouseListener(this, true);
}

LFOModule::~LFOModule()
{
    selectionTree.removeListener(this);
    lfoTree.removeListener(this);
    
    for (auto* child: getChildren())
        child->removeMouseListener(this);
}

void LFOModule::mouseDown (const juce::MouseEvent& event)
{
    int lfoNum = lfoTree.getType() == ParamIDs::lfo1 ? 0 : 1;
    selectionTree.setProperty(ParamIDs::lfoSelected, lfoNum, nullptr);
}

void LFOModule::paint (juce::Graphics& g)
{
    g.fillAll(MyColors::background);
    
    if (shouldDrawSelected)
    {
        g.setColour(MyColors::lightSecondary);
        
        juce::Rectangle<float> bounds = getLocalBounds().toFloat().reduced(MyWidths::standardWidth / 2);
        float rotation = 0.0f;
        
        for (juce::Point<float> p: {bounds.getTopLeft(), bounds.getTopRight(), bounds.getBottomRight(), bounds.getBottomLeft()})
        {
            juce::Path corner;
            corner.startNewSubPath(p.x, p.y + 10.0f);
            corner.lineTo(p);
            corner.lineTo(p.x + 10.0f, p.y);
            corner.applyTransform(juce::AffineTransform::rotation(rotation, p.x, p.y));
            rotation += juce::MathConstants<float>::halfPi;
            g.strokePath(corner, juce::PathStrokeType{MyWidths::standardWidth});
        }
    }
}

void LFOModule::resized()
{
    const int margin = MyWidths::standardMarginInt;
    
    juce::Rectangle<int> totalArea = getLocalBounds().reduced(2, 0);
    
    const int headerHeight = MyWidths::buttonDimension;
    juce::Rectangle<int> headerArea = totalArea.removeFromTop(headerHeight);
    dragger.setBounds(headerArea.removeFromRight(headerHeight));
    headerArea.removeFromRight(margin);
    wavePicker.setBounds(headerArea);
    totalArea.removeFromTop(margin);
    
    const int visualizerHeight = 79;
    visualizerArea = totalArea.removeFromTop(visualizerHeight);
    visualizer.setBounds(visualizerArea);
    totalArea.removeFromTop(margin);
    
    const int sliderHeight = MyWidths::standardTextSliderBounds.getHeight();
    juce::Rectangle<int> slidersArea = totalArea.removeFromTop(sliderHeight);
    syncButton.setBounds(slidersArea.removeFromLeft(MyWidths::buttonDimension));
    slidersArea.removeFromLeft(margin);
    frequencySlider.setBounds(slidersArea.removeFromLeft(slidersArea.proportionOfWidth(0.5f)));
    rangeKnob.setBounds(slidersArea);
}

void LFOModule::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
{
    if (property == ParamIDs::lfoSelected)
    {
        shouldDrawSelected = static_cast<int>(treeWhosePropertyHasChanged[property]) == (lfoTree.getType() == ParamIDs::lfo1 ? 0 : 1);
        repaint();
        
    }
}
