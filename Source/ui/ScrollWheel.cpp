/*
  ==============================================================================

    ScrollWheel.cpp
    Created: 12 Jul 2024 11:13:52am
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ScrollWheel.h"

ScrollWheel::ScrollWheel (juce::RangedAudioParameter& t_param,
             const juce::String labelText,
             juce::ValueTree t_lfoTree)
: SliderBase(t_param, labelText, t_lfoTree, true)
{
    setOpaque(true);
}

void ScrollWheel::paint (juce::Graphics& g)
{
    g.fillAll(MyColors::background);
    
    const float paramValue = currentValue.load();
    
    /* ----- DRAW WHEEL ----- */
    
    const float centerY = wheelArea.getCentreY();
    
    const float wheelWidth = wheelArea.getWidth();
    const float wheelHeight = wheelArea.getHeight() * 0.85f;
    
    juce::Path tickPath;
    
    for (int i = 0; i < numTicks; i++)
    {
        const float tickXNorm = static_cast<float>(i) / numTicks * 2.0f + (paramValue - 0.5f) * 0.9f - 0.5f;
        if (tickXNorm >= 0.0f && tickXNorm <= 1.0f)
        {
            const float tickX = wheelArea.getX() + wheelWidth * GUIHelper::cubicBezier(tickXNorm);
            float tickHeight = (0.4f + 0.45f * std::sinf(tickXNorm * juce::MathConstants<float>::pi)) * (wheelHeight / 2.0f);
            if (i == numTicks / 2)
                tickHeight *= 1.3f;
            tickPath.startNewSubPath(tickX, centerY - tickHeight);
            tickPath.lineTo(tickX, centerY + tickHeight);
        }
    }
    
    g.setColour(MyColors::lightestNeutral);
    g.strokePath(tickPath, juce::PathStrokeType(MyWidths::standardWidth));
    
    /* ----- DRAW LFO MODULATION RANGE ----- */
    
    if (lfoTree.getChild(currentLFONum).getProperty(ParamIDs::lfoOn))
    {
        const float centerX = lfoPositionRange.convertFrom0to1(paramValue);
        const float leftX = lfoPositionRange.convertFrom0to1((lfoTree.getChild(currentLFONum)[ParamIDs::lfoDirection] == ParamIDs::lfoBidirectional
                                                              ? juce::jlimit(0.0f, 1.0f, paramValue - currentLFORange)
                                                              : paramValue));
        const float rightX = lfoPositionRange.convertFrom0to1(juce::jlimit(0.0f, 1.0f, paramValue + currentLFORange));
        
        g.setColour(MyColors::lightSecondary);
        g.drawLine(leftX, 4.0f, rightX, 4.0f, MyWidths::standardWidth);
        
        g.setColour(MyColors::white);
        g.drawLine(centerX - 1, 4.0f, centerX + 1, 4.0f, MyWidths::standardWidth);
    }
    
    if (lfoTree.getChild((currentLFONum + 1) % 2)[ParamIDs::lfoOn])
    {
        g.setColour(MyColors::lightSecondary);
        g.fillEllipse(getWidth() - 8, 4, 4, 4);
    }
}

void ScrollWheel::resized()
{
    juce::Rectangle<int> bounds = getLocalBounds();
    
    focusBorder.setBounds(bounds);
    
    wheelArea = bounds.removeFromLeft(bounds.getWidth() * 0.6f).reduced(3, 6);
    textBox.setBounds(bounds.withTrimmedBottom(2));
    textBox.setFont(textBox.proportionOfHeight(0.3f));
    
    lfoPositionRange = juce::NormalisableRange<float> (wheelArea.getX(), wheelArea.getRight());
}
