/*
  ==============================================================================

    DSPHelper.h
    Created: 17 Sep 2024 6:13:19pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

struct DSPHelper
{
    static constexpr float minimumFrequency = 30.0f, maximumFrequency = 20000.0f;
    static constexpr float minimumFilterDecibels = -24.0f, maximumFilterDecibels = 24.0f;
    static constexpr float minimumLevelDecibels = -72.0f, maximumLevelDecibels = 6.0f;
    static constexpr float minimumGainDecibels = -1000.0f, maximumGainDecibels = 18.0f;
    static constexpr float minimumEnvGainDecibels = -1000.0f, centerEnvGainDecibels = -12.0f;
    static constexpr float minimumAttack = 0.001, maximumAttack = 10.0f, centerAttack = 0.5f, minimumDecay = 0.005f, maximumDecay = 20.0f, centerDecay = 1.0f, minimumRelease = 0.005f, maximumRelease = 30.0f, centerRelease = 1.5f;
    static constexpr float defaultFilterQ = 0.77f;
    
    static float mapNormalizedToFrequency (float normValue)
    {
        return juce::mapToLog10(normValue, minimumFrequency, maximumFrequency);
    }
    
    static float mapFrequencyToNormalized (float denormValue)
    {
        return juce::mapFromLog10(denormValue, minimumFrequency, maximumFrequency);
    }
    
    static float getDelayLowpassCutoff (float centerFreq, float width)
    {
        return juce::jlimit(minimumFrequency, maximumFrequency, centerFreq * width);
    }
    
    static float getDelayHighpassCutoff (float centerFreq, float width)
    {
        return juce::jlimit(minimumFrequency, maximumFrequency, centerFreq / width);
    }
    
    static float getFrequencyFromMidi (const float noteNumber)
    {
        return 440.0f * std::pow(2.0f, ((noteNumber - 69.0f) / 12.0f));
    }
};
