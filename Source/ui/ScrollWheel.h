/*
  ==============================================================================

    ScrollWheel.h
    Created: 12 Jul 2024 11:13:52am
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SliderBase.h"
#include "GUIHelper.h"
#include "TextBox.h"
#include "FocusBorder.h"

class ScrollWheel  : public SliderBase
{
public:
    ScrollWheel (juce::RangedAudioParameter& t_param,
                 const juce::String labelText,
                 juce::ValueTree t_lfoTree = juce::ValueTree());
    
    void paint (juce::Graphics& g) override;
    void resized() override;
    
private:
    juce::Rectangle<int> wheelArea;
    juce::Rectangle<int> textArea;
    
    const int numTicks { 26 };
    
    juce::NormalisableRange<float> lfoPositionRange;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScrollWheel);
};
