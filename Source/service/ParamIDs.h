/*
  ==============================================================================

    ParamIDs.h
    Created: 10 May 2024 3:17:23pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

namespace ParamIDs
{
/* ----- APVTS PARAMETER IDS ----- */

constexpr auto oscGain { "Osc Gain" };
constexpr auto oscPan  { "Osc Pan" };

constexpr auto oscInitialAmp  { "Osc Inital Amp" };
constexpr auto oscPeakAmp     { "Osc Peak Amp" };
constexpr auto oscSustainAmp  { "Osc Sustain Amp" };
constexpr auto oscAttackAmp   { "Osc Attack Amp" };
constexpr auto oscDecayAmp    { "Osc Decay Amp" };
constexpr auto oscReleaseAmp  { "Osc Release Amp" };
constexpr auto oscSlope1Amp   { "Osc Slope 1 Amp" };
constexpr auto oscSlope2Amp   { "Osc Slope 2 Amp" };
constexpr auto oscSlope3Amp   { "Osc Slope 3 Amp" };

constexpr auto oscInitialPitch  { "Osc Inital Pitch" };
constexpr auto oscPeakPitch     { "Osc Peak Pitch" };
constexpr auto oscSustainPitch  { "Osc Sustain Pitch" };
constexpr auto oscEndPitch      { "Osc End Pitch" };
constexpr auto oscAttackPitch   { "Osc Attack Pitch" };
constexpr auto oscDecayPitch    { "Osc Decay Pitch" };
constexpr auto oscReleasePitch  { "Osc Release Pitch" };
constexpr auto oscSlope1Pitch   { "Osc Slope 1 Pitch" };
constexpr auto oscSlope2Pitch   { "Osc Slope 2 Pitch" };
constexpr auto oscSlope3Pitch   { "Osc Slope 3 Pitch" };

constexpr auto oscOctave   { "Osc Octave" };
constexpr auto oscSemitone { "Osc Semitone" };
constexpr auto oscFine     { "Osc Fine" };
constexpr auto oscCoarse   { "Osc Coarse" };

constexpr auto noiseGain { "Noise Gain" };
constexpr auto noisePan  { "Noise Pan" };
constexpr auto noiseQ    { "Noise Resonance" };

constexpr auto noiseInitialAmp  { "Noise Inital Amp" };
constexpr auto noisePeakAmp     { "Noise Peak Amp" };
constexpr auto noiseSustainAmp  { "Noise Sustain Amp" };
constexpr auto noiseAttackAmp   { "Noise Attack Amp" };
constexpr auto noiseDecayAmp    { "Noise Decay Amp" };
constexpr auto noiseReleaseAmp  { "Noise Release Amp" };
constexpr auto noiseSlope1Amp   { "Noise Slope 1 Amp" };
constexpr auto noiseSlope2Amp   { "Noise Slope 2 Amp" };
constexpr auto noiseSlope3Amp   { "Noise Slope 3 Amp" };

constexpr auto noiseInitialCutoff  { "Noise Inital Cutoff" };
constexpr auto noisePeakCutoff     { "Noise Peak Cutoff" };
constexpr auto noiseSustainCutoff  { "Noise Sustain Cutoff" };
constexpr auto noiseEndCutoff      { "Noise End Cutoff" };
constexpr auto noiseAttackCutoff   { "Noise Attack Cutoff" };
constexpr auto noiseDecayCutoff    { "Noise Decay Cutoff" };
constexpr auto noiseReleaseCutoff  { "Noise Release Cutoff" };
constexpr auto noiseSlope1Cutoff   { "Noise Slope 1 Cutoff" };
constexpr auto noiseSlope2Cutoff   { "Noise Slope 2 Cutoff" };
constexpr auto noiseSlope3Cutoff   { "Noise Slope 3 Cutoff" };
constexpr auto noiseFilterCoarse { "Noise Filter Coarse" };

constexpr auto leftDelayTime     { "Left Delay Time" };
constexpr auto rightDelayTime    { "Right Delay Time" };
constexpr auto leftDelaySync     { "Left Delay Sync" };
constexpr auto rightDelaySync    { "Right Delay Sync" };
constexpr auto leftDelayOffset   { "Left Delay Offset" };
constexpr auto rightDelayOffset  { "Right Delay Offset" };
constexpr auto delayFeedback     { "Delay Feedback" };
constexpr auto delayFilterCenter { "Delay Filter Center" };
constexpr auto delayFilterWidth  { "Delay Filter Width" };
constexpr auto delayMix          { "Delay Mix" };

constexpr auto reverbSize    { "Reverb Size" };
constexpr auto reverbDamping { "Reverb Damping" };
constexpr auto reverbWidth   { "Reverb Width" };
constexpr auto reverbMix     { "Reverb Mix" };

constexpr auto waveshaperXTransform { "Waveshaper X Transform"};
constexpr auto waveshaperYTransform { "Waveshaper Y Transform"};
constexpr auto waveshaperInputGain  { "Waveshaper Input Gain" };
constexpr auto waveshaperOutputGain { "Waveshaper Output Gain" };
constexpr auto waveshaperMix        { "Waveshaper Mix" };

constexpr auto filterLowpassCutoff { "Filter Lowpass Cutoff" };
constexpr auto filterLowpassQ { "Filter Lowpass Q" };
constexpr auto filterLowpassGain { "Filter Lowpass Gain" };
constexpr auto filterHighpassCutoff { "Filter Highpass Cutoff" };
constexpr auto filterHighpassQ { "Filter Highpass Q" };
constexpr auto filterHighpassGain { "Filter Highpass Gain" };
constexpr auto filterMix { "Filter Mix" };

constexpr auto lfo1FrequencyTime { "LFO 1 Frequency Time" };
constexpr auto lfo2FrequencyTime { "LFO 2 Frequency Time" };
constexpr auto lfo1FrequencySync { "LFO 1 Frequency Sync" };
constexpr auto lfo2FrequencySync { "LFO 2 Frequency Sync" };
constexpr auto lfo1Range { "LFO 1 Range" };
constexpr auto lfo2Range { "LFO 2 Range" };

constexpr auto midiNote { "Midi Note" };
constexpr auto masterGain { "Master Gain" };

/* ----- NON-APVTS VALUE TREE/PROPERTY IDS ----- */

const juce::Identifier nonParamTree { "nonParamTree" };

const juce::Identifier synthTree { "synthTree" };
const juce::Identifier oscIO { "oscIO" };
const juce::Identifier oscWaveType { "oscWaveType" };
const juce::Identifier oscAmpEnvMode { "oscAmpEnvMode" };
const juce::Identifier pitchEnvMode { "pitchEnvMode" };
const juce::Identifier noiseIO { "noiseIO" };
const juce::Identifier noiseAmpEnvMode { "noiseAmpEnvMode" };
const juce::Identifier filterType { "filterType" };
const juce::Identifier filterEnvMode { "filterEnvMode" };
const juce::Identifier filterKeyFollowIO { "filterKeyFollowIO" };

const juce::Identifier delayTree { "delayTree" };
const juce::Identifier delayIO { "delayIO" };
const juce::Identifier delaySyncLeft { "delaySyncLeftIO" };
const juce::Identifier delaySyncRight { "delaySyncRightIO" };
const juce::Identifier delayLink { "delayLink" };
const juce::Identifier delayLowpassCutoff { "delayLowpassCutoff" };
const juce::Identifier delayHighpassCutoff { "delayHighpassCutoff" };
const juce::Identifier delayFilterIO { "delayFilterIO" };

const juce::Identifier waveshaperNodeTree { "waveshaperNodeTree" };
const juce::Identifier shaperNode0 { "shaperNode0" };
const juce::Identifier shaperNode1 { "shaperNode1" };
const juce::Identifier shaperNode2 { "shaperNode2" };
const juce::Identifier shaperNode3 { "shaperNode3" };
const juce::Identifier shaperNode4 { "shaperNode4" };
const juce::Identifier nodeX { "node1X" };
const juce::Identifier nodeY { "node1Y" };
const juce::Identifier nodeSlope { "node1Slope" };
const juce::Identifier nodeEnabled { "node1Enabled" };

const juce::Identifier waveshaperToolTree { "waveshaperToolTree" };
const juce::Identifier oddEven { "oddEven" };

const juce::Identifier filterTree { "filterTree" };
const juce::Identifier filterLowpassMode { "filterLowpassMode" };
const juce::Identifier filterHighpassMode { "filterHighpassMode" };

const juce::Identifier lfoTree { "lfoTree" };

const juce::Identifier lfoSelectionTree { "lfoSelectionTree" };
const juce::Identifier lfoSelected { "lfoSelected" };

const juce::Identifier lfo1 { "lfo1" };
const juce::Identifier lfo2 { "lfo2" };

const juce::Identifier lfoSync { "lfoSync" };
const juce::Identifier lfoWaveType { "lfoWaveType" };

const juce::Identifier lfoOscOctave { "lfoOscOctave" };
const juce::Identifier lfoOscSemitone { "lfoOscSemitone" };
const juce::Identifier lfoOscFine { "lfoOscFine" };
const juce::Identifier lfoOscCoarse { "lfoOscCoarse" };
const juce::Identifier lfoOscGain { "lfoOscGain" };
const juce::Identifier lfoOscPan { "lfoOscPan" };
const juce::Identifier lfoNoiseCoarse { "lfoNoiseCoarse" };
const juce::Identifier lfoNoiseGain { "lfoNoiseGain" };
const juce::Identifier lfoNoiseCutoff { "lfoNoiseCutoff" };
const juce::Identifier lfoNoiseQ { "lfoNoiseQ" };
const juce::Identifier lfoNoisePan { "lfoNoisePan" };
const juce::Identifier lfoLeftDelayTime { "lfoLeftDelayTime" };
const juce::Identifier lfoRightDelayTime { "lfoRightDelayTime" };
const juce::Identifier lfoLeftDelaySync { "lfoLeftDelaySync" };
const juce::Identifier lfoRightDelaySync { "lfoRightDelaySync" };
const juce::Identifier lfoLeftDelayOffset { "lfoLeftDelayOffset" };
const juce::Identifier lfoRightDelayOffset { "lfoRightDelayOffset" };
const juce::Identifier lfoDelayFilterCenter { "lfoDelayFilterCenter" };
const juce::Identifier lfoDelayFilterWidth { "lfoDelayFilterWidth" };
const juce::Identifier lfoDelayFeedback { "lfoDelayFeedback" };
const juce::Identifier lfoDelayMix { "lfoDelayMix" };
const juce::Identifier lfoReverbSize { "lfoReverbSize" };
const juce::Identifier lfoReverbDamping { "lfoReverbDamping" };
const juce::Identifier lfoReverbWidth { "lfoReverbWidth" };
const juce::Identifier lfoReverbMix { "lfoReverbMix" };
const juce::Identifier lfoFilterHighpassCutoff { "lfoFilterHighpassCutoff" };
const juce::Identifier lfoFilterHighpassQ { "lfoFilterHighpassQ" };
const juce::Identifier lfoFilterHighpassGain { "lfoFilterHighpassGain" };
const juce::Identifier lfoFilterLowpassCutoff { "lfoFilterLowpassCutoff" };
const juce::Identifier lfoFilterLowpassQ { "lfoFilterLowpassQ" };
const juce::Identifier lfoFilterLowpassGain { "lfoFilterLowpassGain" };
const juce::Identifier lfoFilterMix { "lfoFilterMix" };
const juce::Identifier lfoWaveshaperInputGain { "lfoWaveshaperInputGain" };
const juce::Identifier lfoWaveshaperOutputGain { "lfoWaveshaperOutputGain" };
const juce::Identifier lfoWaveshaperMix { "lfoWaveshaperMix" };
const juce::Identifier lfoMasterGain { "lfoMasterGain" };

const juce::Identifier lfoOn { "lfoOn" };
const juce::Identifier lfoRange { "lfoRange" };
const juce::Identifier lfoDirection { "lfoDirection" };
const juce::String lfoBidirectional { "bidirectional" };
const juce::String lfoUnidirectional { "unidirectional" };

const juce::Identifier settingsTree { "settingsTree" };
const juce::Identifier currentPreset { "currentPreset" };

const juce::Identifier effectsOrderTree { "effectsOrderTree" };
const juce::Identifier delayNode { "delayNode" };
const juce::Identifier reverbNode { "reverbNode" };
const juce::Identifier filterNode { "filterNode" };
const juce::Identifier waveshaperNode { "waveshaperNode" };
} // namespace ParamIDs
