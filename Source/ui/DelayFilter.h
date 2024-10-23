/*
  ==============================================================================

    DelayFilter.h
    Created: 26 Jul 2024 2:27:39pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ComponentIDs.h"
#include "GUIHelper.h"
#include "TextSlider.h"
#include "ValueTreeComponents.h"
#include "../dsp/DSPHelper.h"
#include "../service/ParamIDs.h"

class DelayFilter  : public juce::Component
                   , public juce::ValueTree::Listener
{
public:
    DelayFilter(juce::RangedAudioParameter& centerParam,
                juce::RangedAudioParameter& widthParam,
                juce::ValueTree delayTree,
                juce::ValueTree lfoTree,
                double sampRate);
    ~DelayFilter() override;
    
    void mouseDown (const juce::MouseEvent& event) override;
    void mouseDrag (const juce::MouseEvent& event) override;
    void mouseUp   (const juce::MouseEvent& event) override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;

private:
    void updateFilter();
    void updateResponseCurve();
    
    void updateCenter(float newCenter);
    void updateWidth(float newWidth);
    
    juce::ValueTree delayTree;
    
    std::unique_ptr<juce::Drawable> filterIcon;
    std::unique_ptr<juce::Drawable> offIcon;
    
    juce::RangedAudioParameter& centerParameter;
    juce::RangedAudioParameter& widthParameter;
    juce::ParameterAttachment centerAttachment;
    juce::ParameterAttachment widthAttachment;
    
    TextSlider centerSlider;
    TextSlider widthSlider;
    ValueTreeToolButton ioButton;
    
    double sampleRate;
    
    juce::Rectangle<int> mainArea;
    
    float currentCenter { 100.0f };
    float currentWidth { 5.0f };
    
    juce::dsp::IIR::Coefficients<float> highpassCoeffs, lowpassCoeffs;
    std::vector<double> magnitudes;
    
    bool filterOn;
    
    juce::Path responseCurve;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayFilter)
};
