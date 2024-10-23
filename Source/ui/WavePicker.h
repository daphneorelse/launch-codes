/*
  ==============================================================================

    WavePicker.h
    Created: 17 Jun 2024 3:49:45pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GUIHelper.h"
#include "FunctionalButton.h"
#include "../service/ParamIDs.h"

// ORDER OF WAVEFORMS: sine, triangle, sawtooth, square

class WavePicker  : public juce::Component
                  , public juce::ValueTree::Listener
{
public:
    WavePicker(juce::ValueTree waveTree, const juce::Identifier& waveProperty);
    ~WavePicker() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property) override;

private:
    juce::ValueTree waveTree;
    const juce::Identifier& waveProperty;
    
    int wave { 0 };
    
    std::unique_ptr<juce::Drawable> sineSVG, triangleSVG, sawtoothSVG, squareSVG, previousSVG, nextSVG;
    
    juce::Rectangle<float> waveArea, leftArrowArea, rightArrowArea;
    
    FunctionalButton previousButton, nextButton;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WavePicker)
};
