/*
  ==============================================================================

    FunctionalButton.h
    Created: 8 Sep 2024 12:47:47pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GUIHelper.h"

class FunctionalButton  : public juce::Component
{
public:
    std::function<void()> callback;
    
    FunctionalButton(std::unique_ptr<juce::Drawable>& t_icon)
    : icon(t_icon)
    {
        setOpaque(true);
        setMouseCursor(juce::MouseCursor::PointingHandCursor);
    }
    
    void mouseDown (const juce::MouseEvent& event) override
    {
        pressed = true;
        repaint();
    }
    
    void mouseUp   (const juce::MouseEvent& event) override
    {
        pressed = false;
        repaint();
        
        if (mainArea.contains(event.getPosition()))
            callback();
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll(MyColors::background);
        
        g.setColour(MyColors::lightNeutral);
        g.drawRect(mainArea);
        
        g.setColour(pressed ? MyColors::lightSecondary : MyColors::mediumPrimary);
        g.fillRect(mainArea.reduced(2));
        
        icon->drawWithin(g, mainArea.reduced(4).toFloat(), 0, 1.0f);
    }

    void resized() override
    {
        const int dimension = juce::jmin(getWidth(), getHeight()) - 2;
        mainArea = juce::Rectangle<int> (dimension, dimension);
        mainArea.setCentre(getLocalBounds().getCentre());
    }

private:
    std::unique_ptr<juce::Drawable>& icon;
    
    juce::Rectangle<int> mainArea;
    
    bool pressed { false };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FunctionalButton)
};
