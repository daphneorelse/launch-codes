/*
  ==============================================================================

    FocusBorder.h
    Created: 17 May 2024 1:59:20pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../service/ParamIDs.h"
#include "GUIHelper.h"

class FocusBorder : public juce::Component
{
public:
    FocusBorder()
    {
        setWantsKeyboardFocus(true);
        setMouseCursor(juce::MouseCursor::DraggingHandCursor);
    }
    
    void drawBorder()
    {
        borderPath.clear();
        
        const juce::Rectangle<float> area = getLocalBounds().toFloat().reduced(MyWidths::standardWidth / 2.0f);
        auto radianRotate = 0.0f;

        for (const juce::Point<float> corner : { area.getTopLeft(), area.getTopRight(), area.getBottomRight(), area.getBottomLeft() })
        {
            const int length = 5.0f;
            juce::Path p;
            p.startNewSubPath (corner.x, corner.y + length);
            p.lineTo (corner.x, corner.y);
            p.lineTo (corner.x + length, corner.y);
            p.applyTransform (juce::AffineTransform::rotation (radianRotate, corner.x, corner.y));
            borderPath.addPath (p);

            radianRotate += juce::MathConstants<float>::halfPi;
        }
    }
    
    void focusGained (FocusChangeType) override
    {
        shouldDrawBorder = true;
        repaint();
    };
    
    void focusLost (FocusChangeType) override
    {
        shouldDrawBorder = false;
        repaint();
    };
    
    void paint (juce::Graphics& g) override
    {
        if (shouldDrawBorder || shouldDrawDragBorder)
        {
            g.setColour(shouldDrawDragBorder ? MyColors::lightSecondary : MyColors::lightNeutral);
            g.strokePath(borderPath, juce::PathStrokeType(MyWidths::standardWidth));
        }
    }
    
    void resized() override
    {
        drawBorder();
    }
    
    juce::Path& getPath()
    {
        return borderPath;
    }
    
    void setPaintDragBorder(bool temp)
    {
        shouldDrawDragBorder = temp;
        repaint();
    }
    
private:
    juce::Path borderPath;
    
    bool shouldDrawBorder { false };
    bool shouldDrawDragBorder { false };
};



