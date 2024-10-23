/*
  ==============================================================================

    IconSlider.h
    Created: 8 Sep 2024 3:01:11pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SliderBase.h"

class IconSlider  : public SliderBase
{
public:
    enum StyleFlags
    {
        dragHorizontal = 1,
        showColoredBackgrounds = 2
    };
    
    IconSlider(juce::RangedAudioParameter& param,
               std::unique_ptr<juce::Drawable>& t_icon,
               int t_styleFlags = 0,
               juce::ValueTree lfoTree = juce::ValueTree(),
               juce::UndoManager* um = nullptr)
    : SliderBase(param, "", lfoTree, (t_styleFlags & dragHorizontal) ? SliderBase::dragHorizontal : 0, um)
    , icon(t_icon)
    , styleFlags(t_styleFlags)
    {
        setOpaque(true);
        
        textBox.setVisible(false);
    }

    ~IconSlider() override
    {
    }
    
    void mouseEnter (const juce::MouseEvent& event) override
    {
        shouldDrawIcon = false;
        textBox.setVisible(true);
        repaint();
    }
    
    void mouseExit (const juce::MouseEvent& event) override
    {
        shouldDrawIcon = true;
        textBox.setVisible(false);
        repaint();
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll(MyColors::background);
        
        const float paramValue = currentValue.load();
        
        g.setColour(MyColors::lightNeutral);
        GUIHelper::drawRectangleWithThickness(g, mainArea.toFloat(), MyWidths::thinWidth);
        
        if (lfoTree.getChild(currentLFONum).getProperty(ParamIDs::lfoOn))
        {
            const float centerX = lfoPositionRange.convertFrom0to1(paramValue);
            const float leftX = lfoPositionRange.convertFrom0to1((lfoTree.getChild(currentLFONum)[ParamIDs::lfoDirection] == ParamIDs::lfoBidirectional
                                                                  ? juce::jlimit(0.0f, 1.0f, paramValue - currentLFORange)
                                                                  : paramValue));
            const float rightX = lfoPositionRange.convertFrom0to1(juce::jlimit(0.0f, 1.0f, paramValue + currentLFORange));
            
            g.setColour(MyColors::lightSecondary);
            g.drawLine(leftX, 3.0f, rightX, 3.0f, MyWidths::standardWidth);
            
            g.setColour(MyColors::white);
            g.drawLine(centerX - 1, 3.0f, centerX + 1, 3.0f, MyWidths::standardWidth);
        }
        else if (styleFlags & showColoredBackgrounds)
        {
            if (shouldDrawIcon)
                g.setColour(MyColors::darkPrimary);
            else
                g.setColour(MyColors::lightSecondary);
            
            g.fillRect(mainArea.reduced(2));
        }
        
        if (lfoTree.getChild((currentLFONum + 1) % 2)[ParamIDs::lfoOn])
        {
            g.setColour(MyColors::lightSecondary);
            g.fillEllipse(getWidth() - 6, 2, 4, 4);
        }
        
        if (shouldDrawIcon)
        {
            icon->drawWithin(g, iconArea.toFloat(), 0, 1.0f);
        }
    }

    void resized() override
    {
        focusBorder.setBounds(getLocalBounds());
        
        const int dimension = juce::jmin(getWidth(), getHeight()) - 4 * 2;
        iconArea = juce::Rectangle<int> (dimension, dimension);
        iconArea.setCentre(getLocalBounds().getCentre());
        
        mainArea = getLocalBounds();
        textBox.setBounds(mainArea.reduced(0, mainArea.proportionOfHeight(0.22f)));
        textBox.setFont(textBox.getHeight() * 0.7f);
        
        lfoPositionRange = juce::NormalisableRange<float> (4, getWidth() - 4);
    }

private:
    std::unique_ptr<juce::Drawable>& icon;
    
    juce::Rectangle<int> iconArea, mainArea;
    
    int styleFlags;
    bool shouldDrawIcon { true };
    
    juce::NormalisableRange<float> lfoPositionRange;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IconSlider)
};
