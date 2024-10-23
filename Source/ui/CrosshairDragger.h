/*
  ==============================================================================

    LFODragger.h
    Created: 23 Aug 2024 3:55:43pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GUIHelper.h"

//==============================================================================
/*
*/
class CrosshairDragger  : public juce::Component
{
public:
    CrosshairDragger(const juce::String description);
    
    void mouseDown (const juce::MouseEvent& event) override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    std::unique_ptr<juce::Drawable> crosshair;
    
    const juce::String description;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CrosshairDragger)
};
