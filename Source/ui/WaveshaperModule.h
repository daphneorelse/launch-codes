/*
  ==============================================================================

    WaveshaperModule.h
    Created: 10 May 2024 12:19:28pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../PluginProcessor.h"
#include "GUIHelper.h"
#include "ComponentIDs.h"
#include "WaveshaperNode.h"
#include "ValueTreeComponents.h"
#include "TextSlider.h"
#include "IconSlider.h"
#include "EffectsSlot.h"
#include "CrosshairDragger.h"
#include "../service/ParamIDs.h"

/* ----- WAVESHAPER RADAR ----- */

class WaveshaperRadar : public juce::Component
{
public:
    WaveshaperRadar();
    
    void paint (juce::Graphics& g) override;
    void resized() override;
    
private:
    juce::Rectangle<float> mainArea;
    juce::Point<float> center;
    
    const float lineInterval { 15.0f };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveshaperRadar);
};

/* ----- WAVESHAPER GRAPH ----- */

class WaveshaperGraph : public juce::Component,
                        public juce::ValueTree::Listener
{
public:
    WaveshaperGraph(juce::RangedAudioParameter& xTransformParam,
                    juce::RangedAudioParameter& yTransformParam,
                    juce::ValueTree nodeTree,
                    juce::ValueTree toolTree,
                    juce::dsp::LookupTableTransform<float>& controlTable,
                    juce::dsp::LookupTableTransform<float>& transformTable);
    ~WaveshaperGraph() override;
    
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property) override;

private:
    juce::RangedAudioParameter& xTransformParam;
    juce::RangedAudioParameter& yTransformParam;
    juce::ParameterAttachment xAttachment;
    juce::ParameterAttachment yAttachment;
    
    juce::ValueTree nodeTree;
    juce::ValueTree toolTree;
    juce::dsp::LookupTableTransform<float>& controlTable;
    juce::dsp::LookupTableTransform<float>& transformTable;
    
    juce::Rectangle<float> mainArea;
    juce::Point<float> center;
    float quadWidth;
    float quadHeight;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveshaperGraph)
};

/* ----- WAVESHAPER CONTROLS ----- */

class WaveshaperControls : public juce::Component, public juce::ValueTree::Listener
{
public:
    WaveshaperControls(juce::ValueTree nodeTree);
    ~WaveshaperControls() override;
    
    void mouseMove        (const juce::MouseEvent& event) override;
    void mouseDown        (const juce::MouseEvent& event) override;
    void mouseDrag        (const juce::MouseEvent& event) override;
    void mouseUp          (const juce::MouseEvent& event) override;
    void mouseDoubleClick (const juce::MouseEvent& event) override;
    void mouseExit        (const juce::MouseEvent& event) override;
    
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property) override;
    void valueTreeChildOrderChanged(juce::ValueTree &parentTreeWhoseChildrenHaveMoved, int oldIndex, int newIndex) override;

private:
    void positionNodes();
    float selectClosestNode (const juce::Point<float> eventPos);
    
    float getMidpointYFromSlope (const float slope);
    float getSlopeFromMidpointY(const float y);
    
    juce::ValueTree nodeTree;
    WaveshaperNode* currentNode;
    
    juce::Rectangle<float> mainArea;
    juce::Point<float> center;
    float quadWidth;
    float quadHeight;
    juce::Rectangle<float> highlightArea { MyWidths::highlightAreaRadius * 2.0f, MyWidths::highlightAreaRadius * 2.0f };
    bool showHighlight { false };
    bool nodeClicked { false };
    
    juce::OwnedArray<WaveshaperNode> nodeArray;
    juce::OwnedArray<WaveshaperNode> slopeArray;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveshaperControls)
};

/* ----- WAVESHAPER MODULE ----- */

class WaveshaperModule : public EffectsSlot
{
public:
    WaveshaperModule(PluginProcessor& processor,
                     juce::AudioProcessorValueTreeState& apvts,
                     juce::ValueTree nodeTree,
                     juce::ValueTree toolTree,
                     juce::ValueTree lfoTree,
                     juce::ValueTree effectsOrderTree);
    
    void paint (juce::Graphics&) override;
    void resized() override;
    
private:
    PluginProcessor& processor;
    juce::AudioProcessorValueTreeState& apvts;
    
    juce::ValueTree lfoTree;
    
    std::unique_ptr<juce::Drawable> header, oddIcon, evenIcon, inIcon, outIcon, xTransformIcon, yTransformIcon;
    
    CrosshairDragger crosshair;
    
    WaveshaperRadar radar;
    WaveshaperGraph graph;
    WaveshaperControls controls;
    
    ValueTreeToolButton oddEvenButton;
    IconSlider inputGainSlider, xTransformSlider, yTransformSlider, outputGainSlider;
    
    TextSlider mixSlider;
    
    juce::Rectangle<int> totalArea, mainArea, toolArea;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveshaperModule);
};
