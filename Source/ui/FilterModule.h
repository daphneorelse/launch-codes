/*
  ==============================================================================

    FilterModule.h
    Created: 16 Sep 2024 1:43:21pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../PluginProcessor.h"
#include "TextSlider.h"
#include "ValueTreeComponents.h"
#include "GUIHelper.h"
#include "EffectsSlot.h"
#include "CrosshairDragger.h"
#include "ComponentIDs.h"
#include "../dsp/DSPHelper.h"
#include "../service/ParamIDs.h"
#include "../service/ParameterHelper.h"


class FilterNode
{
public:
    FilterNode(juce::RangedAudioParameter& t_parameterFreq,
               juce::RangedAudioParameter& t_parameterQ,
               juce::RangedAudioParameter& t_parameterGain,
               juce::ParameterAttachment& t_attachmentFreq,
               juce::ParameterAttachment& t_attachmentQ,
               juce::ParameterAttachment& t_attachmentGain)
    : parameterFreq(t_parameterFreq)
    , attachmentFreq(t_attachmentFreq)
    , parameterQ(t_parameterQ)
    , parameterGain(t_parameterGain)
    , attachmentQ(t_attachmentQ)
    , attachmentGain(t_attachmentGain)
    {
        
    }
    
    void updatePos()
    {
        if (mode == 0)
        {
            pos.x = cutoff * viewArea.getWidth() + viewArea.getX();
            pos.y = -parameterQ.convertTo0to1(q) * viewArea.getHeight() + viewArea.getBottom();
            visible = true;
        }
        else if (mode == 1)
        {
            pos.x = cutoff * viewArea.getWidth() + viewArea.getX();
            pos.y = -parameterGain.convertTo0to1(gain) * viewArea.getHeight() + viewArea.getBottom();
            visible = true;
        }
        else
        {
            visible = false;
        }
    }
    
    juce::RangedAudioParameter* getCurrentYParameter()
    {
        return mode == 0 ? &parameterQ : &parameterGain;
    }
    
    juce::ParameterAttachment* getCurrentYAttachment()
    {
        return mode == 0 ? &attachmentQ : &attachmentGain;
    }
    
    juce::Point<float> pos { 0.0f, 0.0f };
    bool visible = true;
    int mode = 0;
    float cutoff, q, gain;
    juce::Rectangle<int> viewArea;
    
    juce::RangedAudioParameter& parameterFreq;
    
    juce::ParameterAttachment& attachmentFreq;
    
    
private:
    juce::RangedAudioParameter& parameterQ;
    juce::RangedAudioParameter& parameterGain;
    juce::ParameterAttachment& attachmentQ;
    juce::ParameterAttachment& attachmentGain;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterNode);
};

class FilterVisualizer : public juce::Component
                       , public juce::ValueTree::Listener
{
public:
    FilterVisualizer(PluginProcessor& p, juce::AudioProcessorValueTreeState& apvts, juce::ValueTree filterTree);
    ~FilterVisualizer() override;
    
    void setParametersFromMouseInput (const juce::MouseEvent& event);
    
    void mouseDown (const juce::MouseEvent& event) override;
    void mouseDrag (const juce::MouseEvent& event) override;
    void mouseUp   (const juce::MouseEvent& event) override;
    void mouseMove (const juce::MouseEvent& event) override;
    void mouseExit (const juce::MouseEvent& event) override;
    
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void populateFrequencies();
    void updateFilter();
    void updateResponseCurve();
    void updateNodes();
    
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;
    
private:
    void highCutChanged (float newVal);
    void highQChanged (float newVal);
    void highGainChanged (float newVal);
    void lowCutChanged (float newVal);
    void lowQChanged (float newVal);
    void lowGainChanged (float newVal);
    
    PluginProcessor& processor;
    juce::AudioProcessorValueTreeState& apvts;
    juce::ValueTree filterTree;
    
    juce::RangedAudioParameter& highpassCutoffParam;
    juce::RangedAudioParameter& highpassQParam;
    juce::RangedAudioParameter& highpassGainParam;
    juce::RangedAudioParameter& lowpassCutoffParam;
    juce::RangedAudioParameter& lowpassQParam;
    juce::RangedAudioParameter& lowpassGainParam;
    juce::ParameterAttachment highpassCutoffAttachment, highpassQAttachment, highpassGainAttachment, lowpassCutoffAttachment, lowpassQAttachment, lowpassGainAttachment;
    
    juce::AudioParameterFloat* lowpassCutoff { nullptr };
    juce::AudioParameterFloat* lowpassQ { nullptr };
    juce::AudioParameterFloat* lowpassGain { nullptr };
    juce::AudioParameterFloat* highpassCutoff { nullptr };
    juce::AudioParameterFloat* highpassQ { nullptr };
    juce::AudioParameterFloat* highpassGain { nullptr };
    
    juce::dsp::IIR::Coefficients<float> highpassCoeffs, lowpassCoeffs;
    std::vector<double> magnitudes;
    
    int lowFilterType, highFilterType;
    
    juce::Path responseCurve;
    
    juce::OwnedArray<FilterNode> nodes;
    FilterNode* currentNode { nullptr };
    
    juce::Rectangle<int> mainArea;
    std::vector<int> frequencies;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterVisualizer);
};


class FilterModule  : public EffectsSlot
                    , public juce::ValueTree::Listener
{
public:
    FilterModule(PluginProcessor& p, juce::AudioProcessorValueTreeState& apvts, juce::ValueTree filterTree, juce::ValueTree lfoTree, juce::ValueTree effectsOrderTree);
    ~FilterModule() override;

    void paint (juce::Graphics&) override;
    void resized() override;
   
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;
private:
    juce::AudioProcessorValueTreeState& apvts;
    juce::ValueTree filterTree;
    
    std::unique_ptr<juce::Drawable> header, lowpassIcon, highpassIcon, lowshelfIcon, highshelfIcon, offIcon;
    
    CrosshairDragger crosshair;
    
    int lowFilterType, highFilterType;
    
    FilterVisualizer visualizer;
    TextSlider highpassCutoffSlider, highpassQSlider, highpassGainSlider, lowpassCutoffSlider, lowpassQSlider, lowpassGainSlider;
    ValueTreeToolMultiToggler highFilterToggler, lowFilterToggler;
    
    TextSlider mixSlider;
    
    juce::Rectangle<int> mainArea, filterArea;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterModule)
};
