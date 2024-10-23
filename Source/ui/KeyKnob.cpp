/*
  ==============================================================================

    KeyKnob.cpp
    Created: 10 May 2024 12:17:59pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "KeyKnob.h"

KeyKnob::KeyKnob (juce::RangedAudioParameter& param,
         const juce::String labelText,
         juce::ValueTree lfoTree,
         juce::UndoManager* um)
: SliderBase(param, labelText, lfoTree, false, um)
{
    setOpaque(true);
    
    label.setVisible(false);
    
    textBox.setColour(juce::Label::textColourId, MyColors::background);
    textBox.setColour(juce::Label::textWhenEditingColourId, MyColors::background);
}

void KeyKnob::paint (juce::Graphics& g)
{
    g.fillAll(MyColors::background);
    
    const float paramValue = currentValue.load();
    const float toAngle = startAngle + paramValue * (endAngle - startAngle);
    
    /* ----- DRAW DIAL BAKGROUND ----- */
    
    g.setColour(MyColors::lightNeutral);
    g.fillEllipse(center.x - outerRadius, center.y - outerRadius, outerRadius * 2, outerRadius * 2);
    
    /* ----- DRAW FILLED ARC ----- */
    
    juce::Path gaugePath;
    gaugePath.addCentredArc(center.x, center.y, gaugeRadius, gaugeRadius, 0.0f, startAngle, endAngle, true);
    g.setColour(MyColors::white);
    g.strokePath(gaugePath, juce::PathStrokeType(gaugeWidth));
    
    gaugePath.clear();
    gaugePath.addCentredArc(center.x, center.y, gaugeRadius, gaugeRadius, 0.0f, startAngle, toAngle, true);
    g.setColour(MyColors::mediumPrimary);
    g.strokePath(gaugePath, juce::PathStrokeType(gaugeWidth));
    
    /* ----- DRAW GAUGE TICKS ----- */
    
    juce::Path gaugeTicksPath;
    for (int i = 0; i < numGaugeTicks; i++)
    {
        const float angle = ((float) i / numGaugeTicks) * juce::MathConstants<float>::twoPi;
        juce::Point<float> innerPoint (center.x + (gaugeRadius - 2.5f) * cosf(angle),
                                       center.y + (gaugeRadius - 2.5f) * sinf(angle));
        juce::Point<float> outerPoint (center.x + (gaugeRadius + 2.5f) * cosf(angle),
                                       center.y + (gaugeRadius + 2.5f) * sinf(angle));
        gaugeTicksPath.startNewSubPath(innerPoint);
        gaugeTicksPath.lineTo(outerPoint);
    }
    
    gaugeTicksPath.applyTransform(juce::AffineTransform::rotation(toAngle / 2.0f, center.x, center.y));
    g.setColour(MyColors::lightNeutral);
    g.strokePath(gaugeTicksPath, juce::PathStrokeType(tickWidth));
    
    g.setColour(MyColors::white);
    g.fillEllipse(center.x - innerRadius, center.y - innerRadius, innerRadius * 2, innerRadius * 2);
    
    /* ----- DRAW KEYHOLE ----- */
    
    g.setColour(MyColors::darkNeutral);
    g.fillPath(holePath, juce::AffineTransform::rotation(toAngle, center.x, center.y));
    
    /* ----- DRAW LFO MODULATION RANGES ----- */
    
    if (lfoTree.getChild(currentLFONum).getProperty(ParamIDs::lfoOn))
    {
        juce::Path modRange;
        modRange.addCentredArc(center.x,
                               center.y,
                               baseRadius,
                               baseRadius,
                               0.0f,
                               lfoTree.getChild(currentLFONum)[ParamIDs::lfoDirection] == ParamIDs::lfoUnidirectional ? toAngle : juce::jlimit(startAngle, endAngle, toAngle - currentLFORange * (endAngle - startAngle)),
                               juce::jlimit(startAngle, endAngle, toAngle + currentLFORange * (endAngle - startAngle)),
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

void KeyKnob::resized() 
{
    /* ----- SET BOUNDS ----- */
    
    juce::Rectangle<int> bounds = getLocalBounds();

    focusBorder.setBounds(bounds);

    mainArea = bounds.reduced(2);

    textBoxBounds.setWidth(mainArea.getWidth() * 0.6f);
    textBoxBounds.setHeight(9.75f);
    textBoxBounds.setCentre(mainArea.getCentre().x, mainArea.getCentre().y + juce::jmin(mainArea.getWidth(), mainArea.getHeight()) * 0.31f);
    textBox.setBounds(textBoxBounds.toNearestInt());
    textBox.setFont(static_cast<float> (textBox.getHeight()) * 0.9f);
    
    /* ----- INITIALIZE GEOMETRY ----- */
    
    center = mainArea.getCentre().toFloat();
    baseRadius  = juce::jmin(mainArea.getWidth(), mainArea.getHeight()) / 2.0f - 2.0f;
    innerRadius = baseRadius * 0.43f;
    outerRadius = baseRadius - 2.0f;
    gaugeRadius = baseRadius * 0.75f;
    
    /* ----- DRAW KEYHOLE PATH ----- */
    
    juce::Rectangle<float> holeArea (innerRadius * 0.5f, innerRadius * 1.5f);
    holeArea.setCentre(center);
    
    holePath.startNewSubPath(holeArea.getBottomRight());
    holePath.lineTo(holeArea.getBottomLeft());
    holePath.lineTo(holeArea.getX(), holeArea.getCentreY());
    holePath.lineTo(holeArea.getCentre());
    holePath.lineTo(holeArea.getCentreX(), holeArea.getY() + 0.35 * holeArea.getHeight());
    holePath.lineTo(holeArea.getX() + 0.15 * holeArea.getWidth(), holeArea.getY() + 0.2 * holeArea.getHeight());
    holePath.lineTo(holeArea.getX() + 0.15 * holeArea.getWidth(), holeArea.getY());
    holePath.lineTo(holeArea.getX() + 0.7 * holeArea.getWidth(), holeArea.getY());
    holePath.lineTo(holeArea.getX() + 0.7 * holeArea.getWidth(), holeArea.getY() + 0.15 * holeArea.getHeight());
    holePath.lineTo(holeArea.getRight(), holeArea.getY() + 0.35 * holeArea.getHeight());
    holePath.lineTo(holeArea.getRight(), holeArea.getY() + 0.6 * holeArea.getHeight());
    holePath.lineTo(holeArea.getCentreX(), holeArea.getY() + 0.8 * holeArea.getHeight());
    holePath.lineTo(holeArea.getRight(), holeArea.getY() + 0.8 * holeArea.getHeight());
    holePath.closeSubPath();
}
