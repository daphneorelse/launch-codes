#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ui/EditorContent.h"

//==============================================================================
/**
*/
class PluginEditor  : public juce::AudioProcessorEditor
{
public:
    PluginEditor (PluginProcessor&, juce::UndoManager&);
    ~PluginEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    PluginProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& apvts;
    juce::ValueTree nonParamTree;
    
    EditorContent editorContent;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
