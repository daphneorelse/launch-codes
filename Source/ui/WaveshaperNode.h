/*
  ==============================================================================

    WaveshaperNode.h
    Created: 23 May 2024 10:33:31am
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GUIHelper.h"
#include "../service/ParamIDs.h"

class WaveshaperNode
{
public:
    WaveshaperNode(juce::ValueTree tree)
    : nodeTree(tree)
    {
    }
    
    juce::Point<float> pos { 0.0f, 0.0f };
    juce::Point<float> lastDragPosition { 0.0f, 0.0f };
    float slopeY { -1.0f }; // TODO delete, absorb into pos, add bool to separate nodes and slopes
    juce::ValueTree nodeTree;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveshaperNode)
};
