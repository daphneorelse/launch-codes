/*
  ==============================================================================

    EffectsSlot.cpp
    Created: 19 Sep 2024 9:12:29am
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "EffectsSlot.h"

//==============================================================================
EffectsSlot::EffectsSlot(const juce::Identifier& t_nodeID, juce::ValueTree t_effectsOrderTree)
: nodeID(t_nodeID)
, effectsOrderTree(t_effectsOrderTree)
{
    
}

bool EffectsSlot::isInterestedInDragSource (const juce::DragAndDropTarget::SourceDetails &dragSourceDetails)
{
    const auto description = dragSourceDetails.description.toString();
    
    return ((description == ParamIDs::delayNode.toString() ||
        description == ParamIDs::reverbNode.toString() ||
        description == ParamIDs::filterNode.toString() ||
        description == ParamIDs::waveshaperNode.toString()) &&
        description != nodeID.toString());
}

void EffectsSlot::itemDragEnter (const juce::DragAndDropTarget::SourceDetails &dragSourceDetails)
{
    paintDragBorder = true;
    repaint();
}

void EffectsSlot::itemDragExit (const juce::DragAndDropTarget::SourceDetails &dragSourceDetails)
{
    paintDragBorder = false;
    repaint();
}

void EffectsSlot::itemDropped (const juce::DragAndDropTarget::SourceDetails &dragSourceDetails)
{
    paintDragBorder = false;
    repaint();
    
    const auto description = dragSourceDetails.description.toString();
    const juce::Identifier sourceID (description);
    const int sourceIndex = effectsOrderTree.indexOf(effectsOrderTree.getChildWithName(sourceID));
    const int targetIndex = effectsOrderTree.indexOf(effectsOrderTree.getChildWithName(nodeID));
    
    jassert(sourceIndex != -1 && targetIndex != -1);
    effectsOrderTree.moveChild(sourceIndex, targetIndex, nullptr);
}
