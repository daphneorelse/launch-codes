/*
  ==============================================================================

    DialKnob.cpp
    Created: 16 May 2024 10:33:01am
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "DialKnob.h"

DialKnob::DialKnob(juce::RangedAudioParameter& param,
         const juce::String labelText,
         juce::ValueTree lfoTree,
         juce::UndoManager* um)
: SliderBase(param, labelText, lfoTree, false, um)
{
    setOpaque(true);
}

void DialKnob::paint (juce::Graphics& g)
{
    g.fillAll(MyColors::background);
    
    const float paramValue = currentValue.load();
    
    /* ----- INITIALIZE GEOMETRY ----- */
    
    const auto center = mainArea.getCentre();
    const float toAngle = startAngle + paramValue * (endAngle - startAngle);
    
    const float outerRadius = juce::jmin(mainArea.getWidth(), mainArea.getHeight()) / 2.0f - 2.0f;
    const float innerRadius = outerRadius - 7.0f;
    
    const float modRadius = outerRadius + 2.0f;
    
    const int numColoredTicks = juce::jmin((int) (paramValue * numTicks) + 1, numTicks);
    
    const int numUncoloredTicks = numTicks - numColoredTicks;
    const float coloredEndAngle = startAngle + (float) ((float) numColoredTicks / (numTicks)) * (endAngle - startAngle);
    
    /* ----- DRAW COLORED TICK MARKS ----- */
    
    juce::Path levelMarks;
    
    for (int i = 0; i < numColoredTicks; i++)
    {
        
        const float angle = startAngle + ((float) i / (numColoredTicks)) * (coloredEndAngle - startAngle);
        juce::Point<float> innerPoint (center.x + innerRadius * cosf(angle),
                                       center.y + innerRadius * sinf(angle));
        juce::Point<float> outerPoint (center.x + outerRadius * cosf(angle),
                                       center.y + outerRadius * sinf(angle));;
        levelMarks.startNewSubPath(innerPoint);
        levelMarks.lineTo(outerPoint);
    }
    
    g.setColour(GUIHelper::mapValueToColor(0.0f, 1.0f, paramValue));
    g.strokePath(levelMarks, juce::PathStrokeType(MyWidths::mediumWidth));
    levelMarks.clear();
    
    /* ----- DRAW UNCOLORED TICK MARKS ----- */
    
    for (int i = 0; i < numUncoloredTicks; i++)
    {
        const float angle = coloredEndAngle + ((float) i / (numUncoloredTicks)) * (endAngle - coloredEndAngle);
        juce::Point<float> innerPoint (center.x + innerRadius * cosf(angle),
                                       center.y + innerRadius * sinf(angle));
        juce::Point<float> outerPoint (center.x + outerRadius * cosf(angle),
                                       center.y + outerRadius * sinf(angle));;
        levelMarks.startNewSubPath(innerPoint);
        levelMarks.lineTo(outerPoint);
    }
    
    g.setColour(MyColors::lightNeutral);
    g.strokePath(levelMarks, juce::PathStrokeType(MyWidths::standardWidth));
    
    /* ----- DRAW INNER NEEDLE DIAL ----- */
    
    juce::Path needle;
    needle.startNewSubPath(center.x + innerRadius, center.y);
    needle.lineTo(center.x + innerRadius - 7.0f, center.y);
    
    for (int i = 1; i < numNeedleTicks; i++)
    {
        const float angle = ((float) i / numNeedleTicks) * juce::MathConstants<float>::twoPi;
        juce::Point<float> innerPoint (center.x + (innerRadius - 4.0f) * cosf(angle),
                                       center.y + (innerRadius - 4.0f) * sinf(angle));
        juce::Point<float> outerPoint (center.x + (innerRadius - 2.0f) * cosf(angle),
                                       center.y + (innerRadius - 2.0f) * sinf(angle));;
        needle.startNewSubPath(innerPoint);
        needle.lineTo(outerPoint);
    }
    
    needle.applyTransform(juce::AffineTransform::rotation(toAngle, center.x, center.y));
    g.strokePath(needle, juce::PathStrokeType(MyWidths::standardWidth));
    
    /* ----- DRAW LFO MODULATION RANGES ----- */
    
    if (lfoTree.getChild(currentLFONum).getProperty(ParamIDs::lfoOn))
    {
        juce::Path modRange;
        modRange.addCentredArc(center.x, center.y, modRadius, modRadius, 0.0f,
                               juce::MathConstants<float>::halfPi + ((lfoTree.getChild(currentLFONum)[ParamIDs::lfoDirection] == ParamIDs::lfoUnidirectional) ? toAngle : juce::jlimit(startAngle, endAngle, toAngle - currentLFORange * (endAngle - startAngle))),
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

void DialKnob::resized()
{
    juce::Rectangle<float> bounds = getLocalBounds().toFloat();
    
    focusBorder.setBounds(bounds.toNearestInt());
    
    label.setBounds(bounds.removeFromBottom(bounds.getHeight() / 5.0f).toNearestInt());
    label.setFont(static_cast<float> (label.getHeight()) * 0.9f);
    
    mainArea = bounds.expanded(2.0f).withY(bounds.getY() + 2.0f);
    
    juce::Rectangle<float> textBoxBounds (mainArea.getWidth() - 32.0f, 10.0f);
    textBoxBounds.setCentre(mainArea.getCentre());
    textBox.setBounds(textBoxBounds.toNearestInt());
    textBox.setFont(static_cast<float> (textBox.getHeight()) * 0.9f);
}
