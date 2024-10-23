/*
  ==============================================================================

    TextSlider.h
    Created: 17 May 2024 9:18:13pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SliderBase.h"
#include "GUIHelper.h"
#include "TextBox.h"
#include "FocusBorder.h"
#include "../service/ParamIDs.h"

class TextSlider  : public SliderBase
{
public:
    enum StyleFlags
    {
        displayHorizonal = 1,
        displayColorRange = 2
    };
    
    TextSlider (juce::RangedAudioParameter& param,
                const juce::String& labelText,
                int t_styleFlags = 0,
                juce::ValueTree t_lfoTree = juce::ValueTree(),
                juce::UndoManager* um = nullptr)
    : SliderBase(param, labelText, t_lfoTree, SliderBase::hideLabelsWhenModifyingLFORange, um)
    , styleFlags(t_styleFlags)
    {
        setOpaque(true);
    }
    
    void paint (juce::Graphics& g) override
    {
        g.fillAll(MyColors::background);
        
        const float paramValue = currentValue.load();
        
        if (styleFlags & displayColorRange)
        {
            g.setColour(GUIHelper::mapValueToColor(0.0f, 1.0f, paramValue));
            GUIHelper::drawRectangleWithThickness(g, getLocalBounds().toFloat(), MyWidths::thinWidth);
        }
        
        if (lfoTree.getChild(currentLFONum).getProperty(ParamIDs::lfoOn))
        {
            const float centerX = lfoPositionRange.convertFrom0to1(paramValue);
            const float leftX = lfoPositionRange.convertFrom0to1((lfoTree.getChild(currentLFONum)[ParamIDs::lfoDirection] == ParamIDs::lfoBidirectional
                                                                  ? juce::jlimit(0.0f, 1.0f, paramValue - currentLFORange)
                                                                  : paramValue));
            const float rightX = lfoPositionRange.convertFrom0to1(juce::jlimit(0.0f, 1.0f, paramValue + currentLFORange));
            
            juce::Path lfoFillPath;
            
            if (isModifyingLFORange)
            {
                g.setColour(MyColors::lightSecondary);
                lfoFillPath.startNewSubPath(leftX, 2.0f);
                lfoFillPath.lineTo(rightX, 2.0f);
                lfoFillPath.lineTo(rightX, getHeight() - 2.0f);
                lfoFillPath.lineTo(leftX, getHeight() - 2.0f);
                lfoFillPath.closeSubPath();
                g.fillPath(lfoFillPath);
                
                g.setColour(MyColors::lightNeutral);
                g.drawLine(centerX, 2.0f, centerX, getHeight() - 2.0f, MyWidths::standardWidth);
            }
            else
            {
                g.setColour(MyColors::darkSecondary);
                lfoFillPath.startNewSubPath(leftX, 2.0f);
                lfoFillPath.lineTo(rightX, 2.0f);
                lfoFillPath.lineTo(rightX, getHeight() - 2.0f);
                lfoFillPath.lineTo(leftX, getHeight() - 2.0f);
                lfoFillPath.closeSubPath();
                g.fillPath(lfoFillPath);
                
                g.setColour(MyColors::mediumNeutral);
                g.drawLine(centerX, 2.0f, centerX, getHeight() - 2.0f, MyWidths::standardWidth);
            }
        }
        
        if (lfoTree.getChild((currentLFONum + 1) % 2)[ParamIDs::lfoOn])
        {
            g.setColour(MyColors::lightSecondary);
            g.fillEllipse(getWidth() - 4, 0, 4, 4);
        }
    };
    
    void resized() override
    {
        juce::Rectangle<int> bounds = getLocalBounds();
        
        focusBorder.setBounds(bounds);
        
        if (styleFlags & displayHorizonal)
        {
            label.setBounds(bounds.removeFromLeft(bounds.proportionOfWidth(0.5f)));
            label.setFont(static_cast<float> (label.getHeight()) * 0.62f);
            
            textBox.setBounds(bounds);
            textBox.setFont(static_cast<float> (textBox.getHeight()) * 0.58f);
        }
        else
        {
            label.setBounds(bounds.removeFromTop(bounds.proportionOfHeight(0.5f)));
            label.setFont(static_cast<float> (label.getHeight()) * 0.75f);
            
            textBox.setBounds(bounds.toNearestInt());
            textBox.setFont(static_cast<float> (textBox.getHeight()) * 0.75f);
        }
        
        lfoPositionRange = juce::NormalisableRange<float> (2, getWidth() - 2);
    };
    
private:
    int styleFlags;
    
    juce::NormalisableRange<float> lfoPositionRange;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TextSlider);
};


class TextSliderSwitcher : public juce::Component
                         , public juce::ValueTree::Listener
{
public:
    TextSliderSwitcher (juce::RangedAudioParameter& timeParam,
                       juce::RangedAudioParameter& syncParam,
                       juce::ValueTree t_lfoTree,
                       juce::UndoManager* um = nullptr)
    : timeSlider(timeParam, "Freq")
    , syncSlider(syncParam, "Beat")
    , lfoTree(t_lfoTree)
    , syncState(lfoTree[ParamIDs::lfoSync])
    {
        lfoTree.addListener(this);
        
        addChildComponent(timeSlider);
        addChildComponent(syncSlider);
        
        syncState ? syncSlider.setVisible(true) : timeSlider.setVisible(true);
    };
    
    ~TextSliderSwitcher() override
    {
        lfoTree.removeListener(this);
    };
    
    void resized() override
    {
        auto bounds = getLocalBounds();
        
        timeSlider.setBounds(bounds);
        syncSlider.setBounds(bounds);
    };
    
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override
    {
        if (property == ParamIDs::lfoSync)
        {
            syncState = lfoTree[ParamIDs::lfoSync];
            
            if (syncState)
            {
                syncSlider.setVisible(true);
                timeSlider.setVisible(false);
            }
            else
            {
                timeSlider.setVisible(true);
                syncSlider.setVisible(false);
            }
        }
    };
    
private:
    TextSlider timeSlider;
    TextSlider syncSlider;
    
    juce::ValueTree lfoTree;
    
    bool syncState;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TextSliderSwitcher);
};
