/*
  ==============================================================================

    ParameterHelper.h
    Created: 23 Aug 2024 10:14:23am
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../dsp/DSPHelper.h"
#include "ParamIDs.h"

class ParameterHelper
{
public:
    ParameterHelper() = delete;
    
    template<typename T>
    static void castParameter (juce::AudioProcessorValueTreeState& apvts, juce::StringRef paramID, T& destination)
    {
        destination = dynamic_cast<T> (apvts.getParameter (paramID));
        jassert (destination != nullptr);
    }
    
    static const inline juce::StringArray syncOptions { "1", "2", "3", "4", "6", "8", "12", "16", "32", "64" };
    
    enum filterType { pass, shelf, off };
    
    static juce::NormalisableRange<float> normRangeWithMidpointSkew (const float lowerBound, const float upperBound,
//                                                                     const float interval,
                                                                     const float midPoint)
    {
        juce::NormalisableRange<float> range (lowerBound, upperBound);
        range.setSkewForCentre(midPoint);
        return range;
    }
    
    static void swapValueTreeNodes (juce::ValueTree tree, const int sourceIndex, const int targetIndex)
    {
        tree.moveChild(sourceIndex, targetIndex, nullptr);
        tree.moveChild(targetIndex + 1, sourceIndex, nullptr);
    }

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;
        
        auto normalizedToPercentFormat = [] (float value, int /*maximumStringLength*/)
        {
            return juce::String { juce::roundToInt(value * 100) } + "%";
        };
        
        auto percentToNormalizedFormat = [] (const juce::String& text)
        {
            const float value = text.getFloatValue();
            return value / 100.0f;
        };
        
        auto toFloatPercentFormat = [] (float value, int /*maximumStringLength*/)
        {
            return juce::String { value, 1 } + "%";
        };
        
        auto gainToDecibelFormat = [] (float value, int /*maximumStringLength*/)
        {
            if (value <= 0.01f)
                return juce::String {"-inf db"};
            
            const auto decibels = juce::Decibels::gainToDecibels(value);
            if (decibels <= -10.0)
                return juce::String { juce::roundToInt(decibels) } + " db";
            
            return juce::String { decibels, 1 } + " db";
        };
        
        auto decibelToGainFormat = [] (const juce::String& text)
        {
            const float db = text.getFloatValue(); // TODO error checking
            return juce::Decibels::decibelsToGain(db);
        };
        
        auto toDecibelFormat = [] (float value, int)
        {
            if (value < -96.0f)
                return juce::String("-inf db");
            return juce::String(value, 1) + " db";
        };
        
        auto toPitchFormat = [] (float value, int /*maximumStringLength*/)
        {
            if (value < 0.0f)
                return juce::String { value , 1 } + " st";
            return "+" + juce::String { value , 1 } + " st";
        };
        
        auto toDurationFormat = [] (float value, int /*maximumStringLength*/)
        {
            if (value < 0.9995f)
                return juce::String { juce::roundToInt(value * 1000) } + " ms";
            return juce::String { value, 1 } + " s";
        };
        
        auto fromDurationFormat = [] (const juce::String& text)
        {
            if (text.endsWithIgnoreCase("ms"))
            {
                auto milliseconds = text.trimCharactersAtEnd("msMS ").getFloatValue();
                return milliseconds / 1000.0f;
            }
            else if (text.endsWithIgnoreCase("s"))
            {
                auto seconds = text.trimCharactersAtEnd("sS ").getFloatValue();
                return seconds;
            }
            else
            {
                auto milliseconds = text.getFloatValue();
                return milliseconds / 1000.0f;
            }
        };
        
        auto toHertzFormat = [] (float value, int /*maximumStringLength*/)
        {
            if (value > 1000.0f || juce::approximatelyEqual(value, 1000.0f))
                return juce::String { value / 1000.0f , 1 } + " kHz";
            return juce::String { value, 1 } + " hz";
        };
        
        auto toPanFormat = [] (float value, int /*maximumStringLength*/)
        {
            const int roundedVal = juce::roundToInt(value * 50);
            
            if (roundedVal < 0)
                return juce::String { -roundedVal } + "L";
            else if (roundedVal > 0)
                return juce::String { roundedVal } + "R";
            return juce::String("C");
        };
        
        auto fromPanFormat = [] (const juce::String& text)
        {
            if (text.endsWithIgnoreCase("L"))
                return -text.trimCharactersAtEnd("lL ").getFloatValue() / 50;
            else if (text.endsWithIgnoreCase("R"))
                return text.trimCharactersAtEnd("rR ").getFloatValue() / 50;
            return 0.0f;
        };
        
        auto toMidiFormat = [] (int value, int /*maximumStringLength*/)
        {
            juce::String noteLetter;
            const int noteValue = value % 12;
            
            switch (noteValue) {
                case 0:
                    noteLetter = "C";
                    break;
                    
                case 1:
                    noteLetter = "C#";
                    break;
                    
                case 2:
                    noteLetter = "D";
                    break;
                    
                case 3:
                    noteLetter = "D#";
                    break;
                    
                case 4:
                    noteLetter = "E";
                    break;
                    
                case 5:
                    noteLetter = "F";
                    break;
                    
                case 6:
                    noteLetter = "F#";
                    break;
                    
                case 7:
                    noteLetter = "G";
                    break;
                    
                case 8:
                    noteLetter = "G#";
                    break;
                    
                case 9:
                    noteLetter = "A";
                    break;
                    
                case 10:
                    noteLetter = "A#";
                    break;
                    
                case 11:
                    noteLetter = "B";
                    break;
                    
                default:
                    break;
            }
            
            const int octaveValue = std::floor(static_cast<float>(value - 21) / 12.0f);
            
            return noteLetter + juce::String{octaveValue};
        };
        
        auto fromMidiFormat = [] (const juce::String& text)
        {
            const juce::String noteName = text.initialSectionContainingOnly("ABCDEFGabcdefg#");
            const juce::String lowerName = noteName.toLowerCase();
            const int octave = text.trimCharactersAtStart("ABCDEFG#b").getIntValue();
            
            int noteValue;
            
            if (lowerName == "a")
                noteValue = 0;
            else if (lowerName == "a#" || noteName == "Bb")
                noteValue = 1;
            else if (lowerName == "b")
                noteValue = 2;
            else if (lowerName == "c")
                noteValue = 3;
            else if (lowerName == "c#" || noteName == "Db")
                noteValue = 4;
            else if (lowerName == "d")
                noteValue = 5;
            else if (lowerName == "d#" || noteName == "Eb")
                noteValue = 6;
            else if (lowerName == "e")
                noteValue = 7;
            else if (lowerName == "f")
                noteValue = 8;
            else if (lowerName == "f#" || noteName == "Gb")
                noteValue = 9;
            else if (lowerName == "g")
                noteValue = 10;
            else if (lowerName == "g#" || noteName == "Ab")
                noteValue = 11;
            else
                return -1;
            
            return (noteValue + 21) + octave * 12;
        };
        
        auto normalizedToFrequency = [] (float value, int)
        {
            const float freq = DSPHelper::mapNormalizedToFrequency(value);
            
            if (freq >= 1000.0f)
                return juce::String(freq / 1000, 1) + " kHz";
            else
                return juce::String(freq, 1) + " hz";
        };
        
        auto frequencyToNormalized = [] (const juce::String& text)
        {
            const float normValue = text.getFloatValue();
            
            return DSPHelper::mapFrequencyToNormalized(normValue);
        };
        
        auto toRoundedFloatFormat = [] (float value, int /*maximumStringLength*/)
        {
            return juce::String { value, 1 };
        };
        
        // TODO handle typing duration in ms format
        
        /* ----- OSC PARAMETERS ----- */
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::oscGain, 1 },
                                                                 ParamIDs::oscGain,
                                                                 normRangeWithMidpointSkew(DSPHelper::minimumGainDecibels, DSPHelper::maximumGainDecibels, 0.0f),
                                                                 0.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toDecibelFormat,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::oscPan, 1 },
                                                                 ParamIDs::oscPan,
                                                                 juce::NormalisableRange { -1.0f, 1.0f, 0.01f, 1.0f },
                                                                 0.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toPanFormat,
                                                                 fromPanFormat));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::oscInitialAmp, 1 },
                                                                 ParamIDs::oscInitialAmp,
                                                                 normRangeWithMidpointSkew(DSPHelper::minimumEnvGainDecibels,
                                                                                           0.0f,
                                                                                           DSPHelper::centerEnvGainDecibels),
                                                                 -1000.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toDecibelFormat,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::oscPeakAmp, 1 },
                                                                 ParamIDs::oscPeakAmp,
                                                                 normRangeWithMidpointSkew(DSPHelper::minimumEnvGainDecibels,
                                                                                           0.0f,
                                                                                           DSPHelper::centerEnvGainDecibels),
                                                                 0.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toDecibelFormat,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::oscSustainAmp, 1 },
                                                                 ParamIDs::oscSustainAmp,
                                                                 normRangeWithMidpointSkew(DSPHelper::minimumEnvGainDecibels,
                                                                                           0.0f,
                                                                                           DSPHelper::centerEnvGainDecibels),
                                                                 -12.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toDecibelFormat,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::oscAttackAmp, 1 },
                                                                 ParamIDs::oscAttackAmp,
                                                                 normRangeWithMidpointSkew(DSPHelper::minimumAttack,
                                                                                           DSPHelper::maximumAttack,
                                                                                           DSPHelper::centerAttack),
                                                                 DSPHelper::centerAttack,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toDurationFormat,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::oscDecayAmp, 1 },
                                                                 ParamIDs::oscDecayAmp,
                                                                 normRangeWithMidpointSkew(DSPHelper::minimumDecay,
                                                                                           DSPHelper::maximumDecay,
                                                                                           DSPHelper::centerDecay),
                                                                 DSPHelper::centerDecay,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toDurationFormat,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::oscReleaseAmp, 1 },
                                                                 ParamIDs::oscReleaseAmp,
                                                                 normRangeWithMidpointSkew(DSPHelper::minimumRelease,
                                                                                           DSPHelper::maximumRelease,
                                                                                           DSPHelper::centerRelease),
                                                                 DSPHelper::centerRelease,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toDurationFormat,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::oscSlope1Amp, 1 },
                                                                 ParamIDs::oscSlope1Amp,
                                                                 juce::NormalisableRange { -10.0f, 10.0f },
                                                                 -2.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 nullptr,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::oscSlope2Amp, 1 },
                                                                 ParamIDs::oscSlope2Amp,
                                                                 juce::NormalisableRange { -10.0f, 10.0f },
                                                                 -2.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 nullptr,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::oscSlope3Amp, 1 },
                                                                 ParamIDs::oscSlope3Amp,
                                                                 juce::NormalisableRange { -10.0f, 10.0f },
                                                                 -2.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 nullptr,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::oscInitialPitch, 1 },
                                                                 ParamIDs::oscInitialPitch,
                                                                 juce::NormalisableRange { -24.0f, 72.0f },
                                                                 0.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toPitchFormat,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::oscPeakPitch, 1 },
                                                                 ParamIDs::oscPeakPitch,
                                                                 juce::NormalisableRange { -24.0f, 72.0f },
                                                                 24.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toPitchFormat,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::oscSustainPitch, 1 },
                                                                 ParamIDs::oscSustainPitch,
                                                                 juce::NormalisableRange { -24.0f, 72.0f },
                                                                 0.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toPitchFormat,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::oscEndPitch, 1 },
                                                                 ParamIDs::oscEndPitch,
                                                                 juce::NormalisableRange { -24.0f, 72.0f },
                                                                 -12.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toPitchFormat,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::oscAttackPitch, 1 },
                                                                 ParamIDs::oscAttackPitch,
                                                                 normRangeWithMidpointSkew(DSPHelper::minimumAttack,
                                                                                           DSPHelper::maximumAttack,
                                                                                           DSPHelper::centerAttack),
                                                                 DSPHelper::centerAttack,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toDurationFormat,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::oscDecayPitch, 1 },
                                                                 ParamIDs::oscDecayPitch,
                                                                 normRangeWithMidpointSkew(DSPHelper::minimumDecay,
                                                                                           DSPHelper::maximumDecay,
                                                                                           DSPHelper::centerDecay),
                                                                 DSPHelper::centerDecay,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toDurationFormat,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::oscReleasePitch, 1 },
                                                                 ParamIDs::oscReleasePitch,
                                                                 normRangeWithMidpointSkew(DSPHelper::minimumRelease,
                                                                                           DSPHelper::maximumRelease,
                                                                                           DSPHelper::centerRelease),
                                                                 DSPHelper::centerRelease,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toDurationFormat,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::oscSlope1Pitch, 1 },
                                                                 ParamIDs::oscSlope1Pitch,
                                                                 juce::NormalisableRange { -10.0f, 10.0f },
                                                                 -2.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 nullptr,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::oscSlope2Pitch, 1 },
                                                                 ParamIDs::oscSlope2Pitch,
                                                                 juce::NormalisableRange { -10.0f, 10.0f },
                                                                 -2.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 nullptr,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::oscSlope3Pitch, 1 },
                                                                 ParamIDs::oscSlope3Pitch,
                                                                 juce::NormalisableRange { -10.0f, 10.0f },
                                                                 -2.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 nullptr,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterInt> (juce::ParameterID { ParamIDs::oscOctave, 1 },
                                                                 ParamIDs::oscOctave,
                                                                 -4,
                                                                 4,
                                                                 0));
        
        layout.add (std::make_unique<juce::AudioParameterInt> (juce::ParameterID { ParamIDs::oscSemitone, 1 },
                                                                 ParamIDs::oscSemitone,
                                                                 -12,
                                                                 12,
                                                                 0,
                                                                 "st"));
        
        layout.add (std::make_unique<juce::AudioParameterInt> (juce::ParameterID { ParamIDs::oscFine, 1 },
                                                                 ParamIDs::oscFine,
                                                                 -100,
                                                                 100,
                                                                 0,
                                                                 "cts"));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::oscCoarse, 1 },
                                                                 ParamIDs::oscCoarse,
                                                                 juce::NormalisableRange<float>(-24.0f, 24.0f),
                                                                 0.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toRoundedFloatFormat,
                                                                 nullptr));
        
        /* ----- NOISE PARAMETERS ----- */
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::noiseGain, 1 },
                                                                 ParamIDs::noiseGain,
                                                                 normRangeWithMidpointSkew(DSPHelper::minimumGainDecibels, DSPHelper::maximumGainDecibels, 0.0f),
                                                                 0.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toDecibelFormat,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::noisePan, 1 },
                                                                 ParamIDs::noisePan,
                                                                 juce::NormalisableRange { -1.0f, 1.0f, 0.01f, 1.0f },
                                                                 0.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toPanFormat,
                                                                 fromPanFormat));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::noiseQ, 1 },
                                                                 ParamIDs::noiseQ,
                                                                 juce::NormalisableRange { 0.1f, 20.0f, 0.01f, 1.0f },
                                                                 1.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toRoundedFloatFormat,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::noiseInitialAmp, 1 },
                                                                 ParamIDs::noiseInitialAmp,
                                                                 normRangeWithMidpointSkew(DSPHelper::minimumEnvGainDecibels,
                                                                                           0.0f,
                                                                                           DSPHelper::centerEnvGainDecibels),
                                                                 -1000.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toDecibelFormat,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::noisePeakAmp, 1 },
                                                                 ParamIDs::noisePeakAmp,
                                                                 normRangeWithMidpointSkew(DSPHelper::minimumEnvGainDecibels,
                                                                                           0.0f,
                                                                                           DSPHelper::centerEnvGainDecibels),
                                                                 0.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toDecibelFormat,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::noiseSustainAmp, 1 },
                                                                 ParamIDs::noiseSustainAmp,
                                                                 normRangeWithMidpointSkew(DSPHelper::minimumEnvGainDecibels,
                                                                                           0.0f,
                                                                                           DSPHelper::centerEnvGainDecibels),
                                                                 -12.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toDecibelFormat,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::noiseAttackAmp, 1 },
                                                                 ParamIDs::noiseAttackAmp,
                                                                 normRangeWithMidpointSkew(DSPHelper::minimumAttack,
                                                                                           DSPHelper::maximumAttack,
                                                                                           DSPHelper::centerAttack),
                                                                 DSPHelper::centerAttack,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toDurationFormat,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::noiseDecayAmp, 1 },
                                                                 ParamIDs::noiseDecayAmp,
                                                                 normRangeWithMidpointSkew(DSPHelper::minimumDecay,
                                                                                           DSPHelper::maximumDecay,
                                                                                           DSPHelper::centerDecay),
                                                                 DSPHelper::centerDecay,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toDurationFormat,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::noiseReleaseAmp, 1 },
                                                                 ParamIDs::noiseReleaseAmp,
                                                                 normRangeWithMidpointSkew(DSPHelper::minimumRelease,
                                                                                           DSPHelper::maximumRelease,
                                                                                           DSPHelper::centerRelease),
                                                                 DSPHelper::centerRelease,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toDurationFormat,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::noiseSlope1Amp, 1 },
                                                                 ParamIDs::noiseSlope1Amp,
                                                                 juce::NormalisableRange { -10.0f, 10.0f },
                                                                 -2.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 nullptr,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::noiseSlope2Amp, 1 },
                                                                 ParamIDs::noiseSlope2Amp,
                                                                 juce::NormalisableRange { -10.0f, 10.0f },
                                                                 -2.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 nullptr,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::noiseSlope3Amp, 1 },
                                                                 ParamIDs::noiseSlope3Amp,
                                                                 juce::NormalisableRange { -10.0f, 10.0f },
                                                                 -2.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 nullptr,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::noiseInitialCutoff, 1 },
                                                                 ParamIDs::noiseInitialCutoff,
                                                                 normRangeWithMidpointSkew(30.0f, 20000.0f, 1000.0f),
                                                                 1000.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toHertzFormat,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::noisePeakCutoff, 1 },
                                                                 ParamIDs::noisePeakCutoff,
                                                                 normRangeWithMidpointSkew(30.0f, 20000.0f, 1000.0f),
                                                                 5000.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toHertzFormat,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::noiseSustainCutoff, 1 },
                                                                 ParamIDs::noiseSustainCutoff,
                                                                 normRangeWithMidpointSkew(30.0f, 20000.0f, 1000.0f),
                                                                 1000.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toHertzFormat,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::noiseEndCutoff, 1 },
                                                                 ParamIDs::noiseEndCutoff,
                                                                 normRangeWithMidpointSkew(30.0f, 20000.0f, 1000.0f),
                                                                 50.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toHertzFormat,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::noiseAttackCutoff, 1 },
                                                                 ParamIDs::noiseAttackCutoff,
                                                                 normRangeWithMidpointSkew(DSPHelper::minimumAttack,
                                                                                           DSPHelper::maximumAttack,
                                                                                           DSPHelper::centerAttack),
                                                                 DSPHelper::centerAttack,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toDurationFormat,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::noiseDecayCutoff, 1 },
                                                                 ParamIDs::noiseDecayCutoff,
                                                                 normRangeWithMidpointSkew(DSPHelper::minimumDecay,
                                                                                           DSPHelper::maximumDecay,
                                                                                           DSPHelper::centerDecay),
                                                                 DSPHelper::centerDecay,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toDurationFormat,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::noiseReleaseCutoff, 1 },
                                                                 ParamIDs::noiseReleaseCutoff,
                                                                 normRangeWithMidpointSkew(DSPHelper::minimumRelease,
                                                                                           DSPHelper::maximumRelease,
                                                                                           DSPHelper::centerRelease),
                                                                 DSPHelper::centerRelease,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toDurationFormat,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::noiseSlope1Cutoff, 1 },
                                                                 ParamIDs::noiseSlope1Cutoff,
                                                                 juce::NormalisableRange { -10.0f, 10.0f },
                                                                 -2.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 nullptr,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::noiseSlope2Cutoff, 1 },
                                                                 ParamIDs::noiseSlope2Cutoff,
                                                                 juce::NormalisableRange { -10.0f, 10.0f },
                                                                 -2.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 nullptr,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::noiseSlope3Cutoff, 1 },
                                                                 ParamIDs::noiseSlope3Cutoff,
                                                                 juce::NormalisableRange { -10.0f, 10.0f },
                                                                 -2.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 nullptr,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::noiseFilterCoarse, 1 },
                                                                 ParamIDs::noiseFilterCoarse,
                                                                 juce::NormalisableRange<float> { -4.0f, 4.0f },
                                                                 -0.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toRoundedFloatFormat,
                                                                 nullptr));
        
        /* ----- DELAY PARAMETERS ----- */
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::leftDelayTime, 1 },
                                                                 ParamIDs::leftDelayTime,
                                                                 normRangeWithMidpointSkew(0.001f, 3.0f, 0.75f),
                                                                 0.75f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toDurationFormat,
                                                                 fromDurationFormat));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::rightDelayTime, 1 },
                                                                 ParamIDs::rightDelayTime,
                                                                 normRangeWithMidpointSkew(0.001f, 3.0f, 0.75f),
                                                                 0.75f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toDurationFormat,
                                                                 fromDurationFormat));
        
        layout.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID { ParamIDs::leftDelaySync, 1 },
                                                                  ParamIDs::leftDelaySync,
                                                                  syncOptions,
                                                                  3));
        
        layout.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID { ParamIDs::rightDelaySync, 1 },
                                                                  ParamIDs::rightDelaySync,
                                                                  syncOptions,
                                                                  3));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::leftDelayOffset, 1 },
                                                                 ParamIDs::leftDelayOffset,
                                                                 juce::NormalisableRange { -50.0f, 50.0f, 0.1f, 1.0f },
                                                                 0.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toFloatPercentFormat,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::rightDelayOffset, 1 },
                                                                 ParamIDs::rightDelayOffset,
                                                                 juce::NormalisableRange { -50.0f, 50.0f, 0.1f, 1.0f },
                                                                 0.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toFloatPercentFormat,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::delayFeedback, 1 },
                                                                 ParamIDs::delayFeedback,
                                                                 juce::NormalisableRange { 0.0f, 1.0f },
                                                                 0.5f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 normalizedToPercentFormat,
                                                                 percentToNormalizedFormat));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::delayMix, 1 },
                                                                 ParamIDs::delayMix,
                                                                 juce::NormalisableRange { 0.0f, 1.0f },
                                                                 0.25f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 normalizedToPercentFormat,
                                                                 percentToNormalizedFormat));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::delayFilterCenter, 1 },
                                                                 ParamIDs::delayFilterCenter,
                                                                 juce::NormalisableRange { 0.0f, 1.0f },
                                                                 0.5f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 normalizedToFrequency,
                                                                 frequencyToNormalized));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::delayFilterWidth, 1 },
                                                                 ParamIDs::delayFilterWidth,
                                                                 juce::NormalisableRange { 1.0f, 10.0f, 0.01f, 1.0f },
                                                                 5.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 nullptr,
                                                                 nullptr));
        
        /* ----- REVERB PARAMETERS ----- */

        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::reverbSize, 1 },
                                                                 ParamIDs::reverbSize,
                                                                 juce::NormalisableRange { 0.0f, 1.0f },
                                                                 0.5f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 normalizedToPercentFormat,
                                                                 percentToNormalizedFormat));

        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::reverbDamping, 1 },
                                                                 ParamIDs::reverbDamping,
                                                                 juce::NormalisableRange { 0.0f, 1.0f },
                                                                 0.5f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 normalizedToPercentFormat,
                                                                 percentToNormalizedFormat));

        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::reverbWidth, 1 },
                                                                 ParamIDs::reverbWidth,
                                                                 juce::NormalisableRange { 0.0f, 1.0f },
                                                                 0.5f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 normalizedToPercentFormat,
                                                                 percentToNormalizedFormat));

        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::reverbMix, 1 },
                                                                 ParamIDs::reverbMix,
                                                                 juce::NormalisableRange { 0.0f, 1.0f },
                                                                 0.25f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 normalizedToPercentFormat,
                                                                 percentToNormalizedFormat));
        
        /* ----- WAVESHAPER PARAMETERS ----- */
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::waveshaperXTransform, 1 },
                                                                 ParamIDs::waveshaperXTransform,
                                                                 juce::NormalisableRange { -10.0f, 10.0f, 0.01f },
                                                                 0.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toRoundedFloatFormat,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::waveshaperYTransform, 1 },
                                                                 ParamIDs::waveshaperYTransform,
                                                                 juce::NormalisableRange { -10.0f, 10.0f, 0.01f },
                                                                 0.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toRoundedFloatFormat,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::waveshaperInputGain, 1 },
                                                                 ParamIDs::waveshaperInputGain,
                                                                 juce::NormalisableRange { 0.0f, 2.0f, 0.01f, 1.0f },
                                                                 1.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 gainToDecibelFormat,
                                                                 decibelToGainFormat));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::waveshaperOutputGain, 1 },
                                                                 ParamIDs::waveshaperOutputGain,
                                                                 juce::NormalisableRange { 0.0f, 2.0f, 0.01f, 1.0f },
                                                                 1.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 gainToDecibelFormat,
                                                                 decibelToGainFormat));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::waveshaperMix, 1 },
                                                                 ParamIDs::waveshaperMix,
                                                                 juce::NormalisableRange { 0.0f, 1.0f },
                                                                 0.5f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 normalizedToPercentFormat,
                                                                 percentToNormalizedFormat));
        
        /* ----- FILTER PARAMETERS ----- */
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::filterHighpassCutoff, 1 },
                                                                 ParamIDs::filterHighpassCutoff,
                                                                 juce::NormalisableRange { 0.0f, 1.0f },
                                                                 DSPHelper::mapFrequencyToNormalized(100.0f),
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 normalizedToFrequency,
                                                                 frequencyToNormalized));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::filterHighpassQ, 1 },
                                                                 ParamIDs::filterHighpassQ,
                                                                 normRangeWithMidpointSkew(0.1f, 15.0f, 1.25f),
                                                                 DSPHelper::defaultFilterQ,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toRoundedFloatFormat,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::filterHighpassGain, 1 },
                                                                 ParamIDs::filterHighpassGain,
                                                                 juce::NormalisableRange { DSPHelper::minimumFilterDecibels, DSPHelper::maximumFilterDecibels },
                                                                 -6.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toDecibelFormat,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::filterLowpassCutoff, 1 },
                                                                 ParamIDs::filterLowpassCutoff,
                                                                 juce::NormalisableRange { 0.0f, 1.0f },
                                                                 DSPHelper::mapFrequencyToNormalized(5000.0f),
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 normalizedToFrequency,
                                                                 frequencyToNormalized));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::filterLowpassQ, 1 },
                                                                 ParamIDs::filterLowpassQ,
                                                                 normRangeWithMidpointSkew(0.1f, 15.0f, 1.25f),
                                                                 DSPHelper::defaultFilterQ,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toRoundedFloatFormat,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::filterLowpassGain, 1 },
                                                                 ParamIDs::filterLowpassGain,
                                                                 juce::NormalisableRange { DSPHelper::minimumFilterDecibels, DSPHelper::maximumFilterDecibels },
                                                                 -6.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toDecibelFormat,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::filterMix, 1 },
                                                                 ParamIDs::filterMix,
                                                                 juce::NormalisableRange { 0.0f, 1.0f },
                                                                 1.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 normalizedToPercentFormat,
                                                                 percentToNormalizedFormat));
        
        /* ----- LFO PARAMETERS ----- */
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::lfo1FrequencyTime, 1 },
                                                                 ParamIDs::lfo1FrequencyTime,
                                                                 normRangeWithMidpointSkew(0.01f, 40.0f, 5.0f),
                                                                 1.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toHertzFormat,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::lfo2FrequencyTime, 1 },
                                                                 ParamIDs::lfo2FrequencyTime,
                                                                 normRangeWithMidpointSkew(0.01f, 40.0f, 5.0f),
                                                                 2.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toHertzFormat,
                                                                 nullptr));
        
        layout.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID { ParamIDs::lfo1FrequencySync, 1 },
                                                                  ParamIDs::lfo1FrequencySync,
                                                                  syncOptions,
                                                                  3));
        
        layout.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID { ParamIDs::lfo2FrequencySync, 1 },
                                                                  ParamIDs::lfo2FrequencySync,
                                                                  syncOptions,
                                                                  3));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::lfo1Range, 1 },
                                                                 ParamIDs::lfo1Range,
                                                                 0.0f,
                                                                 1.0f,
                                                                 1.0f));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::lfo2Range, 1 },
                                                                 ParamIDs::lfo2Range,
                                                                 0.0f,
                                                                 1.0f,
                                                                 1.0f));
        
        /* ----- UTILITY PARAMETERS ----- */
        
        layout.add (std::make_unique<juce::AudioParameterInt> (juce::ParameterID { ParamIDs::midiNote, 1 },
                                                               ParamIDs::midiNote,
                                                               0,
                                                               127,
                                                               72,
                                                               juce::String {},
                                                               toMidiFormat,
                                                               fromMidiFormat));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::masterGain, 1 },
                                                                 ParamIDs::masterGain,
                                                                 normRangeWithMidpointSkew(DSPHelper::minimumGainDecibels, DSPHelper::maximumGainDecibels, 0.0f),
                                                                 0.0f,
                                                                 juce::String {},
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 toDecibelFormat,
                                                                 nullptr));
            
        return layout;
    }
    
    static void copyChildToNewSibling (juce::ValueTree parent, juce::ValueTree treeToCopy, juce::Identifier typeName)
    {
        juce::ValueTree copyTree (typeName);
        copyTree.copyPropertiesAndChildrenFrom(treeToCopy, nullptr);
        parent.appendChild(copyTree, nullptr);
    }

    static juce::ValueTree createNonParameterLayout()
    {
        juce::ValueTree tree (ParamIDs::nonParamTree);
        
        /* ----- WAVESHAPER TREES ----- */
        
        juce::ValueTree waveshaperNodes (ParamIDs::waveshaperNodeTree);
        tree.addChild(waveshaperNodes, -1, nullptr);
        
        juce::ValueTree node0 (ParamIDs::shaperNode0);
        node0.setProperty(ParamIDs::nodeSlope, 0.0f, nullptr);
        node0.setProperty(ParamIDs::nodeEnabled, true, nullptr);
        waveshaperNodes.addChild(node0, 0, nullptr);
        
        juce::ValueTree node1 (ParamIDs::shaperNode1);
        node1.setProperty(ParamIDs::nodeX, 0.5f, nullptr);
        node1.setProperty(ParamIDs::nodeY, 0.45f, nullptr);
        node1.setProperty(ParamIDs::nodeSlope, 5.0f, nullptr);
        node1.setProperty(ParamIDs::nodeEnabled, false, nullptr);
        waveshaperNodes.addChild(node1, 1, nullptr);
        
        juce::ValueTree node2 (ParamIDs::shaperNode2);
        node2.copyPropertiesFrom(node1, nullptr);
        waveshaperNodes.addChild(node2, 2, nullptr);
        
        juce::ValueTree node3 (ParamIDs::shaperNode3);
        node3.copyPropertiesFrom(node1, nullptr);
        waveshaperNodes.addChild(node3, 3, nullptr);
        
        juce::ValueTree node4 (ParamIDs::shaperNode4);
        node4.copyPropertiesFrom(node1, nullptr);
        waveshaperNodes.addChild(node4, 4, nullptr);
        
        juce::ValueTree waveshaperTools (ParamIDs::waveshaperToolTree);
        waveshaperTools.setProperty(ParamIDs::oddEven, true, nullptr);
        tree.appendChild(waveshaperTools, nullptr);
        
        /* ----- SYNTH TREE ----- */
        
        juce::ValueTree synthTree (ParamIDs::synthTree);
        synthTree.setProperty(ParamIDs::oscIO, true, nullptr);
        synthTree.setProperty(ParamIDs::oscWaveType, 0, nullptr);
        synthTree.setProperty(ParamIDs::oscAmpEnvMode, false, nullptr);
        synthTree.setProperty(ParamIDs::pitchEnvMode, 0, nullptr);
        synthTree.setProperty(ParamIDs::noiseIO, true, nullptr);
        synthTree.setProperty(ParamIDs::noiseAmpEnvMode, false, nullptr);
        synthTree.setProperty(ParamIDs::filterType, 0, nullptr);
        synthTree.setProperty(ParamIDs::filterEnvMode, 0, nullptr);
        synthTree.setProperty(ParamIDs::filterKeyFollowIO, false, nullptr);
        tree.appendChild(synthTree, nullptr);
        
        /* ----- DELAY TREE ----- */
        
        juce::ValueTree delayTree (ParamIDs::delayTree);
        delayTree.setProperty(ParamIDs::delayIO, true, nullptr);
        delayTree.setProperty(ParamIDs::delaySyncLeft, false, nullptr);
        delayTree.setProperty(ParamIDs::delaySyncRight, false, nullptr);
        delayTree.setProperty(ParamIDs::delayLink, false, nullptr);
        delayTree.setProperty(ParamIDs::delayLowpassCutoff, 0.0f, nullptr);
        delayTree.setProperty(ParamIDs::delayHighpassCutoff, 0.0f, nullptr);
        delayTree.setProperty(ParamIDs::delayFilterIO, true, nullptr);
        tree.appendChild(delayTree, nullptr);
        
        /* ----- FILTER TREE ----- */
        
        juce::ValueTree filterTree (ParamIDs::filterTree);
        filterTree.setProperty(ParamIDs::filterLowpassMode, 0, nullptr);
        filterTree.setProperty(ParamIDs::filterHighpassMode, 0, nullptr);
        tree.appendChild(filterTree, nullptr);
        
        /* ----- LFO TREES ----- */
        
        juce::ValueTree lfoTree (ParamIDs::lfoTree);
        
        // Stores which LFO has been selected in the GUI for modulation editing
        juce::ValueTree lfoSelectionTree (ParamIDs::lfoSelectionTree);
        lfoSelectionTree.setProperty(ParamIDs::lfoSelected, 0, nullptr);
        lfoTree.appendChild(lfoSelectionTree, nullptr);
        
        // High level LFO properties
        juce::ValueTree lfo1Tree (ParamIDs::lfo1);
        lfo1Tree.setProperty(ParamIDs::lfoSync, false, nullptr);
        lfo1Tree.setProperty(ParamIDs::lfoWaveType, 0, nullptr);
        lfoTree.appendChild(lfo1Tree, nullptr);
        
        juce::ValueTree lfo2Tree (ParamIDs::lfo2);
        lfo2Tree.copyPropertiesFrom(lfo1Tree, nullptr);
        lfoTree.appendChild(lfo2Tree, nullptr);
        
        // Modulation target-specific properties
        juce::ValueTree lfo1ModulationTree (ParamIDs::lfo1);
        lfo1ModulationTree.setProperty(ParamIDs::lfoOn, false, nullptr);
        lfo1ModulationTree.setProperty(ParamIDs::lfoRange, 0.25f, nullptr);
        lfo1ModulationTree.setProperty(ParamIDs::lfoDirection, ParamIDs::lfoBidirectional, nullptr);
        
        juce::ValueTree lfo2ModulationTree (ParamIDs::lfo2);
        lfo2ModulationTree.copyPropertiesFrom(lfo1ModulationTree, nullptr);
        
        juce::ValueTree lfoOscCoarseTree (ParamIDs::lfoOscCoarse);
        lfoOscCoarseTree.appendChild(lfo1ModulationTree, nullptr);
        lfoOscCoarseTree.appendChild(lfo2ModulationTree, nullptr);
        lfoTree.appendChild(lfoOscCoarseTree, nullptr);
        
        copyChildToNewSibling(lfoTree, lfoOscCoarseTree, ParamIDs::lfoOscOctave);
        copyChildToNewSibling(lfoTree, lfoOscCoarseTree, ParamIDs::lfoOscSemitone);
        copyChildToNewSibling(lfoTree, lfoOscCoarseTree, ParamIDs::lfoOscFine);
        copyChildToNewSibling(lfoTree, lfoOscCoarseTree, ParamIDs::lfoOscCoarse);
        copyChildToNewSibling(lfoTree, lfoOscCoarseTree, ParamIDs::lfoOscGain);
        copyChildToNewSibling(lfoTree, lfoOscCoarseTree, ParamIDs::lfoOscPan);
        copyChildToNewSibling(lfoTree, lfoOscCoarseTree, ParamIDs::lfoNoiseCoarse);
        copyChildToNewSibling(lfoTree, lfoOscCoarseTree, ParamIDs::lfoNoiseGain);
        copyChildToNewSibling(lfoTree, lfoOscCoarseTree, ParamIDs::lfoNoisePan);
        copyChildToNewSibling(lfoTree, lfoOscCoarseTree, ParamIDs::lfoNoiseCutoff);
        copyChildToNewSibling(lfoTree, lfoOscCoarseTree, ParamIDs::lfoNoiseQ);
        copyChildToNewSibling(lfoTree, lfoOscCoarseTree, ParamIDs::lfoLeftDelayTime);
        copyChildToNewSibling(lfoTree, lfoOscCoarseTree, ParamIDs::lfoRightDelayTime);
        copyChildToNewSibling(lfoTree, lfoOscCoarseTree, ParamIDs::lfoLeftDelaySync);
        copyChildToNewSibling(lfoTree, lfoOscCoarseTree, ParamIDs::lfoRightDelaySync);
        copyChildToNewSibling(lfoTree, lfoOscCoarseTree, ParamIDs::lfoLeftDelayOffset);
        copyChildToNewSibling(lfoTree, lfoOscCoarseTree, ParamIDs::lfoRightDelayOffset);
        copyChildToNewSibling(lfoTree, lfoOscCoarseTree, ParamIDs::lfoDelayFilterCenter);
        copyChildToNewSibling(lfoTree, lfoOscCoarseTree, ParamIDs::lfoDelayFilterWidth);
        copyChildToNewSibling(lfoTree, lfoOscCoarseTree, ParamIDs::lfoDelayFeedback);
        copyChildToNewSibling(lfoTree, lfoOscCoarseTree, ParamIDs::lfoDelayMix);
        copyChildToNewSibling(lfoTree, lfoOscCoarseTree, ParamIDs::lfoReverbSize);
        copyChildToNewSibling(lfoTree, lfoOscCoarseTree, ParamIDs::lfoReverbDamping);
        copyChildToNewSibling(lfoTree, lfoOscCoarseTree, ParamIDs::lfoReverbWidth);
        copyChildToNewSibling(lfoTree, lfoOscCoarseTree, ParamIDs::lfoReverbMix);
        copyChildToNewSibling(lfoTree, lfoOscCoarseTree, ParamIDs::lfoFilterHighpassCutoff);
        copyChildToNewSibling(lfoTree, lfoOscCoarseTree, ParamIDs::lfoFilterHighpassQ);
        copyChildToNewSibling(lfoTree, lfoOscCoarseTree, ParamIDs::lfoFilterHighpassGain);
        copyChildToNewSibling(lfoTree, lfoOscCoarseTree, ParamIDs::lfoFilterLowpassCutoff);
        copyChildToNewSibling(lfoTree, lfoOscCoarseTree, ParamIDs::lfoFilterLowpassQ);
        copyChildToNewSibling(lfoTree, lfoOscCoarseTree, ParamIDs::lfoFilterLowpassGain);
        copyChildToNewSibling(lfoTree, lfoOscCoarseTree, ParamIDs::lfoFilterMix);
        copyChildToNewSibling(lfoTree, lfoOscCoarseTree, ParamIDs::lfoWaveshaperInputGain);
        copyChildToNewSibling(lfoTree, lfoOscCoarseTree, ParamIDs::lfoWaveshaperOutputGain);
        copyChildToNewSibling(lfoTree, lfoOscCoarseTree, ParamIDs::lfoWaveshaperMix);
        copyChildToNewSibling(lfoTree, lfoOscCoarseTree, ParamIDs::lfoMasterGain);
        
        tree.appendChild(lfoTree, nullptr);
        
        /* ----- SETTINGS TREE ----- */
        
        juce::ValueTree settingsTree (ParamIDs::settingsTree);
        settingsTree.setProperty(ParamIDs::currentPreset, "", nullptr);
        tree.appendChild(settingsTree, nullptr);
        
        /* ----- EFFECTS ORDER TREE ----- */
        
        juce::ValueTree effectsOrderTree (ParamIDs::effectsOrderTree);
        juce::ValueTree delayNode (ParamIDs::delayNode);
        effectsOrderTree.appendChild(delayNode, nullptr);
        juce::ValueTree reverbNode (ParamIDs::reverbNode);
        effectsOrderTree.appendChild(reverbNode, nullptr);
        juce::ValueTree filterNode (ParamIDs::filterNode);
        effectsOrderTree.appendChild(filterNode, nullptr);
        juce::ValueTree waveshaperNode (ParamIDs::waveshaperNode);
        effectsOrderTree.appendChild(waveshaperNode, nullptr);
        tree.appendChild(effectsOrderTree, nullptr);
        
        return tree;
    }
};
