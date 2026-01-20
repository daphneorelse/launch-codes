/*
  ==============================================================================

    EditorLnf.h
    Created: 27 May 2024 12:04:26pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class DefaultLNF final : public juce::LookAndFeel_V4
{
public:
    DefaultLNF ()
    {
        inter = juce::Typeface::createSystemTypefaceFor (BinaryData::Inter_ttf, BinaryData::Inter_ttfSize);

        setColour(juce::PopupMenu::backgroundColourId, MyColors::background);
        setColour(juce::PopupMenu::highlightedBackgroundColourId, MyColors::mediumPrimary);
        setColour(juce::PopupMenu::textColourId, MyColors::white);

        juce::LookAndFeel::setDefaultLookAndFeel (this);
    }
    
    juce::Typeface::Ptr getTypefaceForFont (const juce::Font& font) override
    {
        return inter;
    };
    
private:
    juce::Typeface::Ptr inter;
};
