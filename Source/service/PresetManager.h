/*
  ==============================================================================

    PresetManager.h
    Author:  Akash Murthy & Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ParamIDs.h"

class PresetManager
{
public:
    static const juce::File defaultDirectory;
    static const juce::String extension;
//    static const juce::String presetNameProperty;

    PresetManager(juce::AudioProcessorValueTreeState&, juce::ValueTree);

    void savePreset(const juce::String& presetName);
    void deletePreset(const juce::String& presetName);
    void loadPreset(const juce::String& presetName);
    int loadNextPreset();
    int loadPreviousPreset();
    juce::StringArray getAllPresets() const;
    juce::String getCurrentPreset() const;
    
    std::unique_ptr<juce::XmlElement> getStateXml();
    void loadStateFromXml(std::unique_ptr<juce::XmlElement>);
    static void overwriteAllValueTreeProperties(juce::ValueTree treeToCopy, juce::ValueTree destTree);
    
private:
    juce::AudioProcessorValueTreeState& apvts;
    juce::ValueTree nonParamTree;
    juce::ValueTree settingsTree;
    juce::Value currentPreset;
};
