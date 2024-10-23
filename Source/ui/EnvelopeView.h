/*
  ==============================================================================

    EnvelopeView.h
    Created: 17 May 2024 9:15:34pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ComponentIDs.h"
#include "GUIHelper.h"
#include "EnvelopeNode.h"
#include "TextSlider.h"
#include "ValueTreeComponents.h"
#include "lnfs/EditorLnf.h"
#include "../dsp/MyADSR.h"

/* ----- ENVELOPE GRAPH ----- */

class EnvelopeGraph : public juce::Component
{
public:
    EnvelopeGraph(juce::RangedAudioParameter* param1Y,
                  juce::RangedAudioParameter* param2X,
                  juce::RangedAudioParameter* param2Y,
                  juce::RangedAudioParameter* param3X,
                  juce::RangedAudioParameter* param3Y,
                  juce::RangedAudioParameter* param4X,
                  juce::RangedAudioParameter* param4Y,
                  juce::OwnedArray<juce::dsp::LookupTableTransform<float>>& tables,
                  juce::UndoManager* um = nullptr);
    
    void paint (juce::Graphics&) override;
    void resized() override;
    
private:
    void update1Y(float newVal);
    void update2X(float newVal);
    void update2Y(float newVal);
    void update3X(float newVal);
    void update3Y(float newVal);
    void update4X(float newVal);
    void update4Y(float newVal);
    
    juce::RangedAudioParameter* audioParam1Y;
    juce::RangedAudioParameter* audioParam2X;
    juce::RangedAudioParameter* audioParam2Y;
    juce::RangedAudioParameter* audioParam3X;
    juce::RangedAudioParameter* audioParam3Y;
    juce::RangedAudioParameter* audioParam4X;
    juce::RangedAudioParameter* audioParam4Y;
    
    juce::ParameterAttachment paramAttachment1Y;
    juce::ParameterAttachment paramAttachment2X;
    juce::ParameterAttachment paramAttachment2Y;
    juce::ParameterAttachment paramAttachment3X;
    juce::ParameterAttachment paramAttachment3Y;
    juce::ParameterAttachment paramAttachment4X;
    std::unique_ptr<juce::ParameterAttachment> paramAttachment4Y;
    
    juce::OwnedArray<juce::dsp::LookupTableTransform<float>>& envTables;
    
    juce::Rectangle<int> mainArea;
    float timeParameterWidth;
    const int resolution { 128 };
    
    std::vector<juce::Point<float>> points;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EnvelopeGraph);
};

/* ----- ENVELOPE CONTROLS ----- */

class EnvelopeControls : public juce::Component
{
public:
    EnvelopeControls(juce::RangedAudioParameter* param1Y,
                     juce::RangedAudioParameter* param2X,
                     juce::RangedAudioParameter* param2Y,
                     juce::RangedAudioParameter* param3X,
                     juce::RangedAudioParameter* param3Y,
                     juce::RangedAudioParameter* param4X,
                     juce::RangedAudioParameter* param4Y,
                     juce::RangedAudioParameter* paramSlope1,
                     juce::RangedAudioParameter* paramSlope2,
                     juce::RangedAudioParameter* paramSlope3,
                     juce::UndoManager* um = nullptr);
    
    void mouseMove        (const juce::MouseEvent& event) override;
    void mouseDown        (const juce::MouseEvent& event) override;
    void mouseDrag        (const juce::MouseEvent& event) override;
    void mouseUp          (const juce::MouseEvent& event) override;
    void mouseDoubleClick (const juce::MouseEvent& event) override;
    void mouseExit        (const juce::MouseEvent& event) override;
    
    void textSliderMouseoverCallback (const int nodeNum);
    
    void paint (juce::Graphics&) override;
    void resized() override;
    
private:
    void positionNodes();
    float selectClosestNode (const juce::Point<float> eventPos);
    
    void update1Y(float newVal);
    void update2X(float newVal);
    void update2Y(float newVal);
    void update3X(float newVal);
    void update3Y(float newVal);
    void update4X(float newVal);
    void update4Y(float newVal);
    void updateSlope1(float newSlope);
    void updateSlope2(float newSlope);
    void updateSlope3(float newSlope);
    
    float getMidpointYFromSlope (const float slope);
    float getSlopeFromMidpointY (const float y);
    
    juce::RangedAudioParameter* audioParam1Y;
    juce::RangedAudioParameter* audioParam2X;
    juce::RangedAudioParameter* audioParam2Y;
    juce::RangedAudioParameter* audioParam3X;
    juce::RangedAudioParameter* audioParam3Y;
    juce::RangedAudioParameter* audioParam4X;
    juce::RangedAudioParameter* audioParam4Y;
    
    juce::RangedAudioParameter* audioParamSlope1;
    juce::RangedAudioParameter* audioParamSlope2;
    juce::RangedAudioParameter* audioParamSlope3;
    
    juce::ParameterAttachment paramAttachment1Y;
    juce::ParameterAttachment paramAttachment2X;
    juce::ParameterAttachment paramAttachment2Y;
    juce::ParameterAttachment paramAttachment3X;
    juce::ParameterAttachment paramAttachment3Y;
    juce::ParameterAttachment paramAttachment4X;
    std::unique_ptr<juce::ParameterAttachment> paramAttachment4Y;
    
    juce::ParameterAttachment paramAttachmentSlope1;
    juce::ParameterAttachment paramAttachmentSlope2;
    juce::ParameterAttachment paramAttachmentSlope3;
    
    juce::OwnedArray<EnvelopeNode> nodes;
    juce::OwnedArray<EnvelopeNode> slopes;
    
    EnvelopeNode* currentNode { nullptr };
    
    juce::Rectangle<float> highlightArea { MyWidths::highlightAreaRadius * 2.0f, MyWidths::highlightAreaRadius * 2.0f };
    bool nodeClicked { false };
    
    juce::Rectangle<int> mainArea;
    float timeParameterWidth;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EnvelopeControls);
};

/* ----- ENVELOPE VIEW ----- */

class EnvelopeView : public juce::Component
{
public:
    EnvelopeView(juce::RangedAudioParameter* param1Y,
                 juce::RangedAudioParameter* param2X,
                 juce::RangedAudioParameter* param2Y,
                 juce::RangedAudioParameter* param3X,
                 juce::RangedAudioParameter* param3Y,
                 juce::RangedAudioParameter* param4X,
                 juce::RangedAudioParameter* param4Y,
                 juce::RangedAudioParameter* paramSlope1,
                 juce::RangedAudioParameter* paramSlope2,
                 juce::RangedAudioParameter* paramSlope3,
                 juce::OwnedArray<juce::dsp::LookupTableTransform<float>>& tables,
                 juce::UndoManager* um = nullptr);
    ~EnvelopeView() override;
    
    void mouseEnter (const juce::MouseEvent& event) override;
    void mouseExit  (const juce::MouseEvent& event) override;
    
    void paint (juce::Graphics&) override;
    void resized() override;
    
    juce::Rectangle<int> getEnvModeButtonBounds();
    
private:
    EnvelopeGraph envelopeGraph;
    EnvelopeControls envelopeControls;
    
    TextSlider attackSlider;
    TextSlider decaySlider;
    TextSlider releaseSlider;
    TextSlider initialSlider;
    TextSlider peakSlider;
    TextSlider sustainSlider;
    std::unique_ptr<TextSlider> endSlider;
    
    juce::Rectangle<int> viewBounds, mainArea, emptySliderArea, endSliderArea;
    
    const int numXGridLines { 11 };
    const int numYGridLines { 5 };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EnvelopeView);
};
