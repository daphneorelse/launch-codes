/*
  ==============================================================================

    ValueTreeComponents.cpp
    Created: 11 Jul 2024 11:11:22am
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ValueTreeComponents.h"

ValueTreeTogglerBase::ValueTreeTogglerBase(juce::ValueTree treeNode, const juce::Identifier& propertyName)
: tree(treeNode)
, propName(propertyName)
{
    state = tree.getProperty(propertyName, false);
    
    tree.addListener(this);
    
    setMouseCursor(juce::MouseCursor::PointingHandCursor);
}

ValueTreeTogglerBase::~ValueTreeTogglerBase()
{
    tree.removeListener(this);
}

void ValueTreeTogglerBase::mouseDown (const juce::MouseEvent& event)
{
    pressed = true;
    repaint();
}

void ValueTreeTogglerBase::mouseUp (const juce::MouseEvent& event)
{
    pressed = false;
    repaint();
    if (contains(event.position))
    {
        tree.setProperty(propName, !state, nullptr);
    }
}

void ValueTreeTogglerBase::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
{
    if (treeWhosePropertyHasChanged == tree && property == propName)
    {
        state = tree.getProperty(propName);
        repaint();
    }
}
