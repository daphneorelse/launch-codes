/*
  ==============================================================================

    LevelMeter.h
    Created: 20 Sep 2024 2:45:04pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../PluginProcessor.h"
#include "GUIHelper.h"
#include "../dsp/DSPHelper.h"

class PeakLevel
{
public:
    PeakLevel(const float initialValue)
    {
        smoothedValue.setCurrentAndTargetValue(initialValue);
    }
    
    void reset (const double sampleRate, const float decayTimeInSeconds, const float holdTimeInSeconds)
    {
        smoothedValue.reset(sampleRate, decayTimeInSeconds);
        holdTimeInSamples = sampleRate * holdTimeInSeconds;
    }
    
    void skip (const int numSamples)
    {
        if (sampleCounter > holdTimeInSamples)
        {
            smoothedValue.skip(numSamples);
        }
        else
            sampleCounter += numSamples;
    }
    
    void setValue (const float newValue)
    {
        if (newValue > smoothedValue.getCurrentValue())
        {
            smoothedValue.setCurrentAndTargetValue(newValue);
            sampleCounter = 0;
        }
        else
            smoothedValue.setTargetValue(newValue);
    }
    
    float getValue()
    {
        return smoothedValue.getCurrentValue();
    }
    
private:
    juce::SmoothedValue<float> smoothedValue;
    int holdTimeInSamples { 0 };
    int sampleCounter;
};

class MeterPair  : public juce::Component
                 , public juce::Timer
{
public:
    MeterPair(PluginProcessor& processor);
    ~MeterPair() override;
    
    void mouseDown (const juce::MouseEvent& event) override;

    void paintMeter (juce::Graphics&, juce::SmoothedValue<float>& level, PeakLevel& peak, juce::Rectangle<int>& area);
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void setNewLevelAndPeak (juce::SmoothedValue<float>& level, PeakLevel& peak, const int channel);
    
    void timerCallback() override;

private:
    PluginProcessor& processor;
    
    const int refreshRate { 45 };
    
    juce::SmoothedValue<float> leftLevel { -100.0f }, rightLevel { -100.0f };
    PeakLevel leftPeak { -100.0f }, rightPeak { -100.0f };
    
    juce::Label levelLabel;
    
    juce::Rectangle<int> leftMeterArea, rightMeterArea, labelArea, markingArea;
    
    juce::NormalisableRange<float> meterRange;
    float displayLevel { -100.0f };
    
    bool displayPeak { true };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MeterPair)
};
