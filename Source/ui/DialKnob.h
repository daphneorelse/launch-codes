/*
  ==============================================================================

    DialKnob.h
    Created: 16 May 2024 10:33:01am
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SliderBase.h"
#include "GUIHelper.h"
#include "TextBox.h"
#include "FocusBorder.h"
#include "../service/ParamIDs.h"

class DialKnob  : public SliderBase
{
public:
    DialKnob(juce::RangedAudioParameter& param,
             const juce::String labelText,
             juce::ValueTree lfoTree = juce::ValueTree(),
             juce::UndoManager* um = nullptr);
    
    void paint (juce::Graphics& g) override;
    void resized() override;
    
private:
    juce::Rectangle<float> mainArea;
    
    const int numTicks { 20 };
    const int numNeedleTicks { 15 };
    
    const float startAngle { 3.0f * juce::MathConstants<float>::pi / 4.0f };
    const float endAngle { 9.0f * juce::MathConstants<float>::pi / 4.0f };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DialKnob)
};
