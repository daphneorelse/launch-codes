/*
  ==============================================================================

    ComponentIDs.h
    Created: 10 Sep 2024 10:00:06am
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

namespace ComponentIDs
{
constexpr auto savePreset { "savePreset" };
constexpr auto previousPreset { "previousPreset" };
constexpr auto presetMenu { "presetMenu" };
constexpr auto nextPreset { "nextPreset" };
constexpr auto deletePreset { "deletePreset" };

constexpr auto oscSwitch { "oscSwitch" };
constexpr auto oscGain { "oscGain" };
constexpr auto oscPan { "oscPan" };
constexpr auto oscWavePicker { "oscWavePicker" };
constexpr auto oscAmpEnv { "oscAmpEnv" };
constexpr auto envToggler { "envToggler" };
constexpr auto multiEnvToggler { "multiEnvToggler" };
constexpr auto oscPitchEnv { "oscPitchEnv" };
constexpr auto oscOctave { "oscOctave" };
constexpr auto oscSemitone { "oscSemitone" };
constexpr auto oscFine { "oscFine" };
constexpr auto oscCoarse { "oscCoarse" };

constexpr auto noiseSwitch { "noiseSwitch" };
constexpr auto noiseGain { "noiseGain" };
constexpr auto noisePan { "noisePan" };
constexpr auto noiseFilterPicker { "noiseWavePicker" };
constexpr auto noiseFilterQ { "noiseFilterQ" };
constexpr auto noiseAmpEnv { "noiseAmpEnv" };
constexpr auto noiseFilterEnv { "noiseFilterEnv" };
constexpr auto noiseQ { "noiseQ" };
constexpr auto noiseCoarse { "noiseCoarse" };
constexpr auto noiseFilterKeyFollow { "noiseFilterKeyFollow" };

constexpr auto attack { "attack" };
constexpr auto decay { "decay" };
constexpr auto release { "release" };
constexpr auto initial { "initial" };
constexpr auto peak { "peak" };
constexpr auto sustain { "sustain" };
constexpr auto end { "end" };

constexpr auto effect1 { "effect1" };
constexpr auto effect2 { "effect2" };
constexpr auto effect3 { "effect3" };
constexpr auto effect4 { "effect4" };
constexpr auto effectCrosshair { "effectCrosshair" };

constexpr auto delayLeft { "delayLeft" };
constexpr auto delayRight { "delayRight" };
constexpr auto delayLink { "delayLink" };
constexpr auto delayOffsetLeft { "delayOffsetLeft" };
constexpr auto delayOffsetRight { "delayOffsetRight" };
constexpr auto delaySyncLeft { "delaySyncLeft" };
constexpr auto delaySyncRight { "delaySyncRight" };
constexpr auto delayFeedback { "delayFeedback" };
constexpr auto delayMix { "delayMix" };
constexpr auto delayFilter { "delayFilter" };
constexpr auto delayFilterCenter { "delayFilterCenter" };
constexpr auto delayFilterWidth { "delayFilterWidth" };

constexpr auto reverbPad { "reverbPad" };
constexpr auto reverbSize { "reverbSize" };
constexpr auto reverbDamping { "reverbDamping" };
constexpr auto reverbWidth { "reverbWidth" };
constexpr auto reverbMix { "reverbMix" };

constexpr auto filterVisualizer { "filterVisualizer" };
constexpr auto filterLowCutoff { "filterLowCutoff" };
constexpr auto filterLowQ { "filterLowQ" };
constexpr auto filterLowGain { "filterLowGain" };
constexpr auto filterLowMode { "filterLowMode" };
constexpr auto filterHighCutoff { "filterHighCutoff" };
constexpr auto filterHighQ { "filterHighQ" };
constexpr auto filterHighGain { "filterHighGain" };
constexpr auto filterHighMode { "filterHighMode" };
constexpr auto filterMix { "filterMix" };

constexpr auto waveshaperRadar { "waveshaperRadar" };
constexpr auto waveshaperOddEven { "waveshaperOddEven" };
constexpr auto waveshaperXTransform { "waveshaperXTransform" };
constexpr auto waveshaperYTransform { "waveshaperYTransform" };
constexpr auto waveshaperInputGain { "waveshaperInputGain" };
constexpr auto waveshaperOutputGain { "waveshaperOutputGain" };
constexpr auto waveshaperMix { "waveshaperMix" };

constexpr auto lfoWavePicker { "lfoWavePicker" };
constexpr auto lfoDragger { "lfoDragger" };
constexpr auto lfoVisualizer { "lfoVisualizer" };
constexpr auto lfoFrequency { "lfoFrequency" };
constexpr auto lfoRange { "lfoRange" };
constexpr auto lfoSync { "lfoSync" };

constexpr auto bigRedButton { "bigRedButton" };
constexpr auto midiNote { "midiNote" };

constexpr auto masterGain { "masterGain" };
constexpr auto levelMeter { "levelMeter" };

constexpr auto descriptionBox { "descriptionBox" };
}
