/*
  ==============================================================================

    XYControlPad.cpp
    Created: 16 Jul 2024 4:24:10pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "XYControlPad.h"

//==============================================================================
XYControlPad::XYControlPad(juce::RangedAudioParameter& t_widthParameter,
                           juce::RangedAudioParameter& t_sizeParameter,
                           juce::RangedAudioParameter& t_dampParameter)
: widthParameter(t_widthParameter)
, sizeParameter(t_sizeParameter)
, dampParameter(t_dampParameter)
, widthAttachment(widthParameter, [&] (float val) { updateWidth(val); })
, sizeAttachment(sizeParameter, [&] (float val) { updateSize(val); })
, dampAttachment(dampParameter, [&] (float val) { updateDamp(val); })
{
    widthAttachment.sendInitialUpdate();
    sizeAttachment.sendInitialUpdate();
    dampAttachment.sendInitialUpdate();
}

void XYControlPad::mouseDown (const juce::MouseEvent& event)
{
    widthAttachment.beginGesture();
    sizeAttachment.beginGesture();
    
    event.source.enableUnboundedMouseMovement(true);
    lastMousePosition = event.position;
}

void XYControlPad::mouseDrag (const juce::MouseEvent& event)
{
    const float deltaX = (event.position.x - lastMousePosition.x) * dragSensitivity;
    const float deltaY = (lastMousePosition.y - event.position.y) * dragSensitivity;
    
    currentWidth = juce::jlimit(0.0f, 1.0f, currentWidth + deltaX);
    widthAttachment.setValueAsPartOfGesture(widthParameter.convertFrom0to1(currentWidth));
    
    currentSize = juce::jlimit(0.0f, 1.0f, currentSize + deltaY);
    sizeAttachment.setValueAsPartOfGesture(sizeParameter.convertFrom0to1(currentSize));
    
    lastMousePosition = event.position;
}

void XYControlPad::mouseUp (const juce::MouseEvent& event)
{
    widthAttachment.endGesture();
    sizeAttachment.endGesture();
    
    event.source.enableUnboundedMouseMovement(false);
    juce::Desktop::getInstance().getMainMouseSource().setScreenPosition(event.source.getLastMouseDownPosition());
}

void XYControlPad::mouseDoubleClick (const juce::MouseEvent& event)
{
    widthAttachment.setValueAsCompleteGesture(widthParameter.convertFrom0to1(widthParameter.getDefaultValue()));
    sizeAttachment.setValueAsCompleteGesture(sizeParameter.convertFrom0to1(sizeParameter.getDefaultValue()));
}

void XYControlPad::paint (juce::Graphics& g)
{
    g.setColour(MyColors::viewBackground);
    g.fillRect(mainArea);
    
    juce::Path rectangles;
    
    const float largestNormInput = currentSize;
    const float largestAdjustedInput = normToOutputRange.convertFrom0to1(largestNormInput);
    
    const float slope = widthToSlope.convertFrom0to1(currentWidth);
    
    for (int i = 1; i <= numSquares; i++)
    {
        const float fraction = static_cast<float>(i) / numSquares;
        const float adjustedInput = largestAdjustedInput * fraction * fraction;
        const float inverseTransformOutput = GUIHelper::inverseTransformWithSlope( adjustedInput, slope);
        const float xCoordDelta = inverseTransformOutput * mainArea.getWidth() * normToOutputRange.end / 2;
        const float yCoordDelta = adjustedInput * mainArea.getHeight() / 2;
        
        rectangles.startNewSubPath(center.x - xCoordDelta, center.y - yCoordDelta);
        rectangles.lineTo(center.x + xCoordDelta, center.y - yCoordDelta);
        rectangles.lineTo(center.x + xCoordDelta, center.y + yCoordDelta);
        rectangles.lineTo(center.x - xCoordDelta, center.y + yCoordDelta);
        rectangles.closeSubPath();
        
        g.setColour(MyColors::lightPrimary.withBrightness(brightnessRange.convertFrom0to1(1.0f + currentDamp * (fraction - 1.0f))));
        g.strokePath(rectangles, juce::PathStrokeType(MyWidths::thinWidth));
        rectangles.clear();
    }
    
    g.setColour(MyColors::darkPrimary);
    GUIHelper::drawRectangleWithThickness(g, mainArea, MyWidths::standardWidth);
}

void XYControlPad::resized()
{
    mainArea = getLocalBounds().toFloat();
    center = mainArea.getCentre();
}

void XYControlPad::updateWidth (const float newWidth)
{
    currentWidth = widthParameter.convertTo0to1(newWidth);
    repaint();
}

void XYControlPad::updateSize (const float newSize)
{
    currentSize = sizeParameter.convertTo0to1(newSize);
    repaint();
}

void XYControlPad::updateDamp (const float newDamp)
{
    currentDamp = dampParameter.convertTo0to1(newDamp);
    repaint();
}
