/*
  ==============================================================================

    EffectsSlot.h
    Created: 19 Sep 2024 9:12:29am
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../service/ParamIDs.h"
#include "../service/ParameterHelper.h"

class EffectsSlot  : public juce::Component
                   , public juce::DragAndDropTarget
{
public:
    EffectsSlot(const juce::Identifier& nodeID, juce::ValueTree effectsOrderTree);
    
    bool isInterestedInDragSource (const juce::DragAndDropTarget::SourceDetails &dragSourceDetails) override;
    void itemDragEnter (const juce::DragAndDropTarget::SourceDetails &dragSourceDetails) override;
    void itemDragExit (const juce::DragAndDropTarget::SourceDetails &dragSourceDetails) override;
    void itemDropped (const juce::DragAndDropTarget::SourceDetails &dragSourceDetails) override;

protected:
    const juce::Identifier& nodeID;
    juce::ValueTree effectsOrderTree;
    
    bool paintDragBorder = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EffectsSlot)
};
