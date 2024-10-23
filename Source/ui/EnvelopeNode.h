/*
  ==============================================================================

    EnvelopeNode.h
    Created: 17 May 2024 9:16:49pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GUIHelper.h"

class EnvelopeNode
{
public:
    EnvelopeNode(juce::RangedAudioParameter* paramX,
                 juce::RangedAudioParameter* paramY,
                 juce::ParameterAttachment* attachX,
                 juce::ParameterAttachment* attachY,
                 const bool t_isSlope,
                 const int t_index)
    : audioParamX(paramX)
    , audioParamY(paramY)
    , paramAttachmentX(attachX)
    , paramAttachmentY(attachY)
    , isSlope(t_isSlope)
    , index(t_index)
    {
        
    }
    
    juce::Point<float> pos { 0.0f, 0.0f };
    juce::Point<float> posNorm { 0.0f, 0.0f };
    juce::Point<float> lastDragPos { 0.0f, 0.0f };

    juce::RangedAudioParameter* audioParamX;
    juce::RangedAudioParameter* audioParamY;
    juce::ParameterAttachment* paramAttachmentX;
    juce::ParameterAttachment* paramAttachmentY;
    
    const bool isSlope;
    const int index;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EnvelopeNode);
};
