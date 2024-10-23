/*
  ==============================================================================

    KeyKnob.h
    Created: 10 May 2024 12:17:59pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SliderBase.h"
#include "GUIHelper.h"
#include "TextBox.h"
#include "FocusBorder.h"

class KeyKnob  : public SliderBase
{
public:
    KeyKnob (juce::RangedAudioParameter& param,
             const juce::String labelText,
             juce::ValueTree lfoTree = juce::ValueTree(),
             juce::UndoManager* um = nullptr);
    
    void paint (juce::Graphics& g) override;
    void resized() override;
    
private:
    juce::Rectangle<int> mainArea, textBoxBounds;
    
    juce::Path holePath;
    
    float baseRadius, innerRadius, outerRadius, gaugeRadius;
    juce::Point<float> center;

    const float gaugeWidth { 5.0f };
    const int numGaugeTicks { 15 };
    const float tickWidth { 2.0f };

    const float startAngle { 5.0f * juce::MathConstants<float>::pi / 4.0f };
    const float endAngle { 11.0f * juce::MathConstants<float>::pi / 4.0f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KeyKnob)
};
