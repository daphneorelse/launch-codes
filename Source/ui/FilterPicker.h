/*
  ==============================================================================

    FilterPicker.h
    Created: 9 Jul 2024 9:15:57pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../service/ParamIDs.h"
#include "GUIHelper.h"
#include "FunctionalButton.h"

class FilterPicker : public juce::Component
                   , public juce::ValueTree::Listener
{
public:
    FilterPicker(juce::ValueTree tree, juce::RangedAudioParameter& qParam);
    ~FilterPicker() override;
    
    void updateFilterGraph ();

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property) override;

private:
    void qChanged (const float newQ);
    
    void populateFrequencyArray ();
    double getMaxMagnitude ();
    
    juce::ValueTree paramTree;
    juce::RangedAudioParameter& qParameter;
    juce::ParameterAttachment qAttachment;
    
    float currentQ { 1.0f };
    int currentFilterNum { 0 };
    
    const double sampleRate { 48000.0 };
    const size_t numFrequencySamples { 40 };
    const size_t numMidlineDashes { 4 };
    double * frequencies;
    double * magnitudes;
    
    juce::dsp::IIR::Coefficients<float>::Ptr filterGraph;
    
    juce::Rectangle<float> filterArea;
    juce::Rectangle<int> leftArrowArea;
    juce::Rectangle<int> rightArrowArea;
    
    std::unique_ptr<juce::Drawable> previousIcon, nextIcon;
    FunctionalButton previousButton, nextButton;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterPicker)
};
