/*
  ==============================================================================

    ControlLever.cpp
    Created: 5 Oct 2024 2:15:59pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ControlLever.h"

//==============================================================================
ControlLever::ControlLever(juce::RangedAudioParameter& param, const juce::String labelText, juce::ValueTree t_lfoTree)
: SliderBase(param, labelText, t_lfoTree)
, handleSVG(juce::Drawable::createFromImageData(BinaryData::lever_handle_svg, BinaryData::lever_handle_svgSize))
{
    setOpaque(true);
}

ControlLever::~ControlLever()
{
}

void ControlLever::paint (juce::Graphics& g)
{
    g.fillAll(MyColors::background);
    
    const float paramValue = currentValue.load();
    
    g.setColour(MyColors::darkNeutral);
    g.fillPath(basePath);
    g.fillRect(mainArea.getX() + mainArea.getWidth() / 3,
               mainArea.getY() + mainArea.getHeight() / 2 - 5,
               15,
               10);
    
    g.setColour(MyColors::lightNeutral);
    g.strokePath(markingPath, juce::PathStrokeType(MyWidths::thinWidth));
    
    const float valueY = mainArea.getBottom() - mainArea.getHeight() * paramValue;
    
    const float perspectiveMult = sinf(paramValue * juce::MathConstants<float>::pi);
    
    const int leverWidth = mainArea.getWidth() / 5;
    const int leverCenterX = mainArea.getX() + 2 * mainArea.getWidth() / 3;
    const float widthAddition = (0.5f + perspectiveMult) * mainArea.getWidth() / 16;
    const int baseAddition = paramValue > 0.5 ? 5 : -5;
    
    juce::Path leverPath;
    leverPath.startNewSubPath(leverCenterX - leverWidth / 2, mainArea.getCentreY() + baseAddition);
    leverPath.lineTo(leverCenterX + leverWidth / 2, mainArea.getCentreY() + baseAddition);
    leverPath.lineTo(leverCenterX + leverWidth / 2 + widthAddition, valueY);
    leverPath.lineTo(leverCenterX - leverWidth / 2 - widthAddition, valueY);
    leverPath.closeSubPath();
    g.setColour(MyColors::darkNeutral);
    g.fillPath(leverPath);
    
    const float handleWidth = 2 * mainArea.getWidth() / 3 * (1 + perspectiveMult / 3);
    const float handleHeight = 8 * (1 + perspectiveMult / 3);
    juce::Rectangle<float> handleArea (leverCenterX - handleWidth / 2,
                                       valueY - handleHeight / 2,
                                       handleWidth,
                                       handleHeight);
    
    handleSVG->drawWithin(g, handleArea, 0, 1.0f);
}

void ControlLever::resized()
{
    juce::Rectangle<int> bounds = getLocalBounds();
    
    focusBorder.setBounds(bounds);
    
    textBox.setBounds(bounds.removeFromBottom(10));
    textBox.setFont(textBox.proportionOfHeight(0.9f));
    label.setBounds(bounds.removeFromBottom(10));
    label.setFont(label.proportionOfHeight(0.9f));
    mainArea = bounds.reduced(5, 20);
    
    const int numMarkings = 20;
    const int centerX = mainArea.getX() + mainArea.getWidth() / 3;
    bool basePathStarted = false;
    
    for (int i = 0; i <= numMarkings; i++)
    {
        const float normVal = static_cast<float>(i) / numMarkings;
        
        const float width = mainArea.getWidth() / 6 * (1 + sinf(normVal * juce::MathConstants<float>::pi));
        const int valueY = mainArea.getCentre().y + (mainArea.getHeight() / 2) * cosf(normVal * juce::MathConstants<float>::pi);
        markingPath.startNewSubPath(centerX - width, valueY);
        markingPath.lineTo(centerX, valueY);
        
        if (!basePathStarted)
        {
            basePath.startNewSubPath(centerX - width, valueY);
            basePathStarted = true;
        }
        else
            basePath.lineTo(centerX - width, valueY);
    }
    
    basePath.lineTo(centerX, mainArea.getY());
    basePath.lineTo(centerX, mainArea.getBottom());
    basePath.closeSubPath();
}
