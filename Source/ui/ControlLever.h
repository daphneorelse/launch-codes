/*
  ==============================================================================

    ControlLever.h
    Created: 5 Oct 2024 2:15:59pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SliderBase.h"
#include "GUIHelper.h"

class ControlLever  : public SliderBase
{
public:
    ControlLever(juce::RangedAudioParameter& param, const juce::String labelText, juce::ValueTree lfoTree);
    ~ControlLever() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    std::unique_ptr<juce::Drawable> handleSVG;
    
    juce::Rectangle<int> mainArea;
    
    juce::Path markingPath, basePath;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ControlLever)
};
