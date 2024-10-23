/*
  ==============================================================================

    WavePicker.cpp
    Created: 17 Jun 2024 3:49:45pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "WavePicker.h"

WavePicker::WavePicker(juce::ValueTree t_waveTree, const juce::Identifier& t_waveProperty)
: waveTree(t_waveTree)
, waveProperty(t_waveProperty)
, previousSVG(juce::Drawable::createFromImageData(BinaryData::previous_svg, BinaryData::previous_svgSize))
, nextSVG(juce::Drawable::createFromImageData(BinaryData::next_svg, BinaryData::next_svgSize))
, previousButton(previousSVG)
, nextButton(nextSVG)
{
    setOpaque(true);
    
    wave = static_cast<int>(waveTree.getProperty(waveProperty));
    
    waveTree.addListener(this);
    
    sineSVG = juce::Drawable::createFromImageData(BinaryData::sine_wave_svg, BinaryData::sine_wave_svgSize);
    triangleSVG = juce::Drawable::createFromImageData(BinaryData::triangle_wave_svg, BinaryData::triangle_wave_svgSize);
    sawtoothSVG = juce::Drawable::createFromImageData(BinaryData::saw_wave_svg, BinaryData::saw_wave_svgSize);
    squareSVG = juce::Drawable::createFromImageData(BinaryData::square_wave_svg, BinaryData::square_wave_svgSize);
    
    addAndMakeVisible(previousButton);
    addAndMakeVisible(nextButton);
    
    previousButton.callback = [&]
    {
        wave--;
        if (wave == -1)
            wave = 3;
        
        waveTree.setProperty(waveProperty, wave, nullptr);
    };
    nextButton.callback = [&]
    {
        wave++;
        if (wave == 4)
            wave = 0;
        
        waveTree.setProperty(waveProperty, wave, nullptr);
    };
}

WavePicker::~WavePicker()
{
    waveTree.removeListener(this);
}

void WavePicker::paint (juce::Graphics& g)
{
    g.fillAll(MyColors::background);
    
    switch (wave) {
        case 0:
            sineSVG->drawWithin(g, waveArea, 0, 1.0f);
            break;
            
        case 1:
            triangleSVG->drawWithin(g, waveArea, 0, 1.0f);
            break;
        
        case 2:
            sawtoothSVG->drawWithin(g, waveArea, 0, 1.0f);
            break;
            
        case 3:
            squareSVG->drawWithin(g, waveArea, 0, 1.0f);
            break;
            
        default:
            jassertfalse;
            break;
    }
}

void WavePicker::resized()
{
    juce::Rectangle<int> bounds = getLocalBounds();
    
    const float areaWidth = juce::jmin((float) getHeight(), getWidth() * 0.3f);
    
    waveArea = juce::Rectangle<float> (areaWidth, areaWidth);
    waveArea.setCentre(getLocalBounds().getCentre().toFloat());
    waveArea.reduce(2.0f, 2.0f);
    
    juce::Rectangle<int> arrowArea (areaWidth * 0.9f, areaWidth * 0.9f);
    
    previousButton.setBounds(arrowArea.withCentre(bounds.removeFromLeft(areaWidth).getCentre()));
    nextButton.setBounds(arrowArea.withCentre(bounds.removeFromRight(areaWidth).getCentre()));
}

void WavePicker::valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property)
{
    if (treeWhosePropertyHasChanged == waveTree && property == waveProperty)
    {
        wave = static_cast<int>(waveTree.getProperty(waveProperty));
        repaint();
    }
}
