/*
  ==============================================================================

    DelayKnob.cpp
    Created: 22 Jul 2024 9:33:23am
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "DelayKnob.h"

DelayKnob::DelayKnob (juce::RangedAudioParameter& param,
           const juce::String labelText,
           juce::ValueTree lfoTree,
           const int t_numParamChoices)
: SliderBase(param, labelText, lfoTree)
, numParamChoices(t_numParamChoices)
{
    setOpaque(true);
}

void DelayKnob::paint (juce::Graphics& g)
{
    g.fillAll(MyColors::background);
    
    float paramValue = currentValue.load();
    
    // snap value for synced AudioParameterChoice knobs
    if (numParamChoices > 0)
        paramValue = static_cast<float>(juce::roundToInt(paramValue * (numParamChoices - 1))) / (numParamChoices - 1);
    
    const float toAngle = startAngle + paramValue * (endAngle - startAngle);
    
    g.setColour(MyColors::mediumNeutral);
    g.fillPath(spinnerPath, juce::AffineTransform::rotation(toAngle, center.x, center.y));
    
    g.setColour(MyColors::white);
    g.fillPath(notchPath, juce::AffineTransform::rotation(toAngle, center.x, center.y));
    
    g.setColour(MyColors::lightNeutral);
    g.fillPath(dotsPath);
    
    /* ----- DRAW LFO MODULATION RANGES ----- */
    
    if (lfoTree.getChild(currentLFONum).getProperty(ParamIDs::lfoOn))
    {
        juce::Path modRange;
        modRange.addCentredArc(center.x,
                               center.y,
                               modRadius,
                               modRadius,
                               0.0f,
                               juce::MathConstants<float>::halfPi + (lfoTree.getChild(currentLFONum)[ParamIDs::lfoDirection] == ParamIDs::lfoUnidirectional ? toAngle : juce::jlimit(startAngle, endAngle, toAngle - currentLFORange * (endAngle - startAngle))),
                               juce::MathConstants<float>::halfPi + juce::jlimit(startAngle, endAngle, toAngle + currentLFORange * (endAngle - startAngle)),
                               true);
        g.setColour(MyColors::lightSecondary);
        g.strokePath(modRange, juce::PathStrokeType{MyWidths::standardWidth});
    }
    
    if (lfoTree.getChild((currentLFONum + 1) % 2)[ParamIDs::lfoOn])
    {
        g.setColour(MyColors::lightSecondary);
        g.fillEllipse(getWidth() - 8, 4, 4, 4);
    }
}

void DelayKnob::resized()
{
    /* ----- SET BOUNDS ----- */
    
    juce::Rectangle<float> bounds = getLocalBounds().toFloat();
    
    focusBorder.setBounds(bounds.toNearestInt());
    
    label.setBounds(bounds.removeFromBottom(bounds.getHeight() / 5.0f).toNearestInt());
    label.setFont(static_cast<float> (label.getHeight()) * 0.9f);
    
    const float mainAreaDimension = juce::jmin(bounds.getHeight(), bounds.getWidth());
    mainArea = juce::Rectangle<float> (mainAreaDimension, mainAreaDimension + 6.0f);
    mainArea.setCentre(bounds.getCentre());
    mainArea.setY(bounds.getY());
    
    center = mainArea.getCentre().toFloat();
    center.y += 2;
    
    juce::Rectangle<float> textBoxBounds (mainArea.getWidth() * 0.68f, 10.0f);
    textBoxBounds.setCentre(center);
    textBox.setBounds(textBoxBounds.toNearestInt());
    textBox.setFont(static_cast<float> (textBox.getHeight()) * 0.8f);
    
    /* ----- DRAW PATHS ----- */
    
    const float dotsRadius = mainArea.getWidth() / 2;
    const float dotRadius = 1.5f;
    const float spinnerRadius = mainArea.getWidth() / 2 * 0.7f;
    const float spinnerGripRadius = 2.5f;
    const float notchRadius = 2.0f;
    modRadius = dotsRadius + 3.5f;
    
    for (float rad = 0; rad < juce::MathConstants<float>::twoPi; rad += juce::MathConstants<float>::pi / 8.0f)
    {
        const float x = center.x + spinnerRadius * cosf(rad);
        const float y = center.y + spinnerRadius * sinf(rad);
        spinnerPath.addEllipse(x - spinnerGripRadius, y - spinnerGripRadius, spinnerGripRadius * 2, spinnerGripRadius * 2);
    }
    
    spinnerPath.addEllipse(center.x - spinnerRadius, center.y - spinnerRadius, spinnerRadius * 2, spinnerRadius * 2);
    
    const float x = center.x + spinnerRadius - 2.5f;
    const float y = center.y;
    notchPath.addEllipse(x - notchRadius, y - notchRadius, notchRadius * 2, notchRadius * 2);
    
    for (float rad = startAngle; rad <= endAngle + juce::MathConstants<float>::pi / 16.0f; rad += juce::MathConstants<float>::pi / 8.0f)
    {
        const float x = center.x + dotsRadius * cosf(rad);
        const float y = center.y + dotsRadius * sinf(rad);
        dotsPath.addEllipse(x - dotRadius, y - dotRadius, dotRadius * 2, dotRadius * 2);
    }
}
