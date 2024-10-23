/*
  ==============================================================================

    DelayKnob.h
    Created: 22 Jul 2024 9:33:23am
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SliderBase.h"
#include "../service/ParamIDs.h"

class DelayKnob  : public SliderBase
{
public:
    DelayKnob (juce::RangedAudioParameter& param,
               const juce::String labelText,
               juce::ValueTree lfoTree,
               const int t_numParamChoices = -1);
    
    void paint (juce::Graphics& g) override;
    
    void resized() override;
    
private:
    const int numParamChoices;
    
    juce::Rectangle<float> mainArea;
    juce::Point<float> center;
    
    juce::Path spinnerPath;
    juce::Path dotsPath;
    juce::Path notchPath;
    
    const float startAngle { 3.0f * juce::MathConstants<float>::pi / 4.0f };
    const float endAngle { 9.0f * juce::MathConstants<float>::pi / 4.0f };
    
    float modRadius;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayKnob);
};
