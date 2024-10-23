/*
  ==============================================================================

    TextBox.h
    Created: 17 May 2024 2:49:28pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GUIHelper.h"

struct TextBox final : public juce::Label
{
    juce::String valueShownWithEditor {};

    TextBox()
    {
        setJustificationType (juce::Justification::centred);
        setInterceptsMouseClicks (false, false);
        setColour (juce::Label::outlineWhenEditingColourId, juce::Colours::transparentWhite);
    }

    juce::TextEditor* createEditorComponent() override
    {
        auto* ed = juce::Label::createEditorComponent();

        ed->setJustification (juce::Justification::centred);
        ed->setColour (juce::TextEditor::backgroundColourId, juce::Colours::transparentWhite);
        ed->setColour (juce::CaretComponent::caretColourId, MyColors::lightestSecondary);
        ed->setInputRestrictions (5, "0123456789 -.msABCDEFG#blLrRc");
        ed->setIndents (4, 1);
        ed->onTextChange = [] { juce::Desktop::getInstance().getMainMouseSource().hideCursor(); };

        return ed;
    }

    void editorShown (juce::TextEditor* ed) override
    {
        ed->clear();
        ed->setText (valueShownWithEditor);
    }
};
