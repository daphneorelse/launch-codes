/*
  ==============================================================================

    LFODragger.cpp
    Created: 23 Aug 2024 3:55:43pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CrosshairDragger.h"

CrosshairDragger::CrosshairDragger(const juce::String t_description)
: crosshair(juce::Drawable::createFromImageData(BinaryData::crosshair_svg, BinaryData::crosshair_svgSize))
, description(t_description)
{
    setMouseCursor(juce::MouseCursor::CrosshairCursor);
}

void CrosshairDragger::mouseDown (const juce::MouseEvent& event)
{
    juce::DragAndDropContainer::findParentDragContainerFor(this)->startDragging(description, this);
}

void CrosshairDragger::paint (juce::Graphics& g)
{
    crosshair->drawWithin(g, getLocalBounds().toFloat(), 0, 1.0f);
}

void CrosshairDragger::resized()
{
    
}
