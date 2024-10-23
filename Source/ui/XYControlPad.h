/*
  ==============================================================================

    XYControlPad.h
    Created: 16 Jul 2024 4:24:10pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "FocusBorder.h"
#include "GUIHelper.h"
#include "../service/ParamIDs.h"

//==============================================================================
/*
*/
class XYControlPad  : public juce::Component
{
public:
    XYControlPad(juce::RangedAudioParameter& widthParameter,
                 juce::RangedAudioParameter& sizeParameter,
                 juce::RangedAudioParameter& dampParameter);
    
    void mouseDown (const juce::MouseEvent& event) override;
    void mouseDrag (const juce::MouseEvent& event) override;
    void mouseUp (const juce::MouseEvent& event) override;
    void mouseDoubleClick (const juce::MouseEvent& event) override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void updateWidth (const float newWidth);
    void updateSize (const float newSize);
    void updateDamp (const float newDamp);
    
    juce::RangedAudioParameter& widthParameter;
    juce::RangedAudioParameter& sizeParameter;
    juce::RangedAudioParameter& dampParameter;
    juce::ParameterAttachment widthAttachment;
    juce::ParameterAttachment sizeAttachment;
    juce::ParameterAttachment dampAttachment;
    
    juce::Point<float> lastMousePosition;
    const float dragSensitivity { 0.01f };
    
    float currentWidth { 0.5f }, currentSize { 0.5f }, currentDamp { 0.5f };
    
    juce::Rectangle<float> mainArea;
    juce::Rectangle<float> padInnerArea;
    juce::Point<float> center;
    
    const int numSquares { 10 };
    const float innerGraphMargin { 15.0f };
    const float graphLineWidth { MyWidths::thinWidth };
    
    juce::NormalisableRange<float> normToOutputRange { 0.33f, 0.9f }, widthToSlope { -10.0, 10.0 }, brightnessRange { MyColors::viewBackground.getBrightness(), MyColors::lightPrimary.getBrightness() };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (XYControlPad)
};
