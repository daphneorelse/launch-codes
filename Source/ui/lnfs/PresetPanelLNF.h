/*
  ==============================================================================

    PresetPanelLNF.h
    Created: 22 Aug 2024 9:47:13am
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../GUIHelper.h"

class PresetPanelLNF : public juce::LookAndFeel_V4
{
public:
    PresetPanelLNF() {
        setColour(juce::ComboBox::backgroundColourId, MyColors::background);
        setColour(juce::ComboBox::textColourId, MyColors::white);
        setColour(juce::ComboBox::outlineColourId, MyColors::mediumPrimary);
        
        setColour(juce::PopupMenu::backgroundColourId, MyColors::background);
        setColour(juce::PopupMenu::highlightedBackgroundColourId, MyColors::mediumPrimary);
        setColour(juce::PopupMenu::textColourId, MyColors::white);
    };
    
    void drawPopupMenuItem (juce::Graphics& g, const juce::Rectangle<int>& area,
                                            const bool isSeparator, const bool isActive,
                                            const bool isHighlighted, const bool isTicked,
                                            const bool hasSubMenu, const juce::String& text,
                                            const juce::String& shortcutKeyText,
                                            const juce::Drawable* icon, const juce::Colour* const textColourToUse) override
    {
        auto textColour = findColour (juce::PopupMenu::textColourId);

        auto r  = area.reduced (1);

        if (isHighlighted && isActive)
        {
            g.setColour (findColour (juce::PopupMenu::highlightedBackgroundColourId));
            g.fillRect (r);

            g.setColour (findColour (juce::PopupMenu::highlightedTextColourId));
        }
        else
        {
            g.setColour (textColour.withMultipliedAlpha (isActive ? 1.0f : 0.5f));
        }

        r.reduce (juce::jmin (5, area.getWidth() / 20), 0);

        auto font = getPopupMenuFont();

        auto maxFontHeight = (float) r.getHeight() / 1.3f;

        if (font.getHeight() > maxFontHeight)
            font.setHeight (maxFontHeight);

        g.setFont (font);

        auto iconArea = r.removeFromLeft (juce::roundToInt (maxFontHeight)).toFloat();

        if (isTicked)
        {
            juce::Path tick;
            tick.startNewSubPath(0, 5);
            tick.lineTo(10, 5);
            tick.startNewSubPath(5, 0);
            tick.lineTo(5, 10);
            tick.addEllipse(2.5f, 2.5f, 5.0f, 5.0f);
            tick.applyTransform(tick.getTransformToScaleToFit (iconArea.reduced (iconArea.getWidth() / 5, 0).toFloat(), true));
            g.strokePath (tick, juce::PathStrokeType(1.0f));
        }

        if (hasSubMenu)
        {
            auto arrowH = 0.6f * getPopupMenuFont().getAscent();

            auto x = static_cast<float> (r.removeFromRight ((int) arrowH).getX());
            auto halfH = static_cast<float> (r.getCentreY());

            juce::Path path;
            path.startNewSubPath (x, halfH - arrowH * 0.5f);
            path.lineTo (x + arrowH * 0.6f, halfH);
            path.lineTo (x, halfH + arrowH * 0.5f);

            g.strokePath (path, juce::PathStrokeType (2.0f));
        }

        r.removeFromRight (3);
        g.drawFittedText (text, r, juce::Justification::centredLeft, 1);

        if (shortcutKeyText.isNotEmpty())
        {
            auto f2 = font;
            f2.setHeight (f2.getHeight() * 0.75f);
            f2.setHorizontalScale (0.95f);
            g.setFont (f2);

            g.drawText (shortcutKeyText, r, juce::Justification::centredRight, true);
        }
        
    }
};
