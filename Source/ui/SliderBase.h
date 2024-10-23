/*
  ==============================================================================

    SliderBase.h
    Created: 8 Sep 2024 9:55:16am
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../service/ParamIDs.h"
#include "FocusBorder.h"
#include "TextBox.h"

class SliderBase  : public juce::Component
                  , public juce::ValueTree::Listener
                  , public juce::DragAndDropTarget
{
public:
    enum StyleFlags
    {
        dragHorizontal = 1,
        hideFocusBorder = 2,
        hideLabelsWhenModifyingLFORange = 4
    };
    
    SliderBase(juce::RangedAudioParameter& t_param,
               const juce::String labelText,
               juce::ValueTree t_lfoTree = juce::ValueTree(),
               int t_styleFlags = 0,
               juce::UndoManager* um = nullptr)
    : param(t_param)
    , attachment(param, [&] (float val) { parameterChangedCallback(val); })
    , lfoTree(t_lfoTree)
    , selectionTree(lfoTree.getParent().getChildWithName(ParamIDs::lfoSelectionTree))
    , styleFlags(t_styleFlags)
    {
        if (lfoTree.isValid())
        {
            lfoTree.addListener(this);
            selectionTree.addListener(this);
        }
        
        setWantsKeyboardFocus(true);
        
        addAndMakeVisible(label);
        label.setText(labelText, juce::dontSendNotification);
        label.setJustificationType (juce::Justification::centred);
        label.setInterceptsMouseClicks (false, false);
        label.setColour(juce::Label::textColourId, MyColors::white);
        label.setMinimumHorizontalScale(1.0f);
        
        addAndMakeVisible(textBox);
        textBox.setColour(juce::Label::textColourId, MyColors::white);
        textBox.setColour(juce::Label::textWhenEditingColourId, MyColors::white);
        textBox.setMinimumHorizontalScale(1.0f);
        
        textBox.onTextChange = [&]
        {
            const float normVal = param.getValueForText(textBox.getText());
            const float denormVal = param.convertFrom0to1(normVal);
            attachment.setValueAsCompleteGesture(denormVal);
            textBox.setText(param.getCurrentValueAsText(), juce::dontSendNotification);
        };
        
        addAndMakeVisible(focusBorder);
        focusBorder.addMouseListener(this, false);
        
        attachment.sendInitialUpdate();
        
        currentLFORange = lfoTree.getChild(currentLFONum)[ParamIDs::lfoRange];
    }

    ~SliderBase() override
    {
        if (lfoTree.isValid())
        {
            lfoTree.removeListener(this);
            selectionTree.addListener(this);
        }
        
        focusBorder.removeMouseListener(this);
    }
    
    void mouseDown (const juce::MouseEvent& event) override
    {
        if (event.mods.isRightButtonDown())
        {
            if (!lfoTree.isValid())
                return;
            
            juce::PopupMenu menu;
            
            if (lfoTree.getChildWithName(ParamIDs::lfo1).getProperty(ParamIDs::lfoOn))
                menu.addItem("Disable LFO 1", [&] () { lfoTree.getChildWithName(ParamIDs::lfo1).setProperty(ParamIDs::lfoOn, false, nullptr); } );
            else
                menu.addItem("Enable LFO 1", [&] () { lfoTree.getChildWithName(ParamIDs::lfo1).setProperty(ParamIDs::lfoOn, true, nullptr);
                    lfoTree.getParent().getChildWithName(ParamIDs::lfoSelectionTree).setProperty(ParamIDs::lfoSelected, 0, nullptr);
                } );
            
            if (lfoTree.getChildWithName(ParamIDs::lfo2).getProperty(ParamIDs::lfoOn))
                menu.addItem("Disable LFO 2", [&] () { lfoTree.getChildWithName(ParamIDs::lfo2).setProperty(ParamIDs::lfoOn, false, nullptr); } );
            else
                menu.addItem("Enable LFO 2", [&] () { lfoTree.getChildWithName(ParamIDs::lfo2).setProperty(ParamIDs::lfoOn, true, nullptr);
                    lfoTree.getParent().getChildWithName(ParamIDs::lfoSelectionTree).setProperty(ParamIDs::lfoSelected, 1, nullptr);
                } );
            
            menu.addSeparator();
            
            if (lfoTree.getChildWithName(ParamIDs::lfo1).getProperty(ParamIDs::lfoOn))
            {
                if (lfoTree.getChildWithName(ParamIDs::lfo1).getProperty(ParamIDs::lfoDirection) == ParamIDs::lfoBidirectional)
                    menu.addItem("Make LFO 1 Unidirectional", [&] () { lfoTree.getChildWithName(ParamIDs::lfo1).setProperty(ParamIDs::lfoDirection, ParamIDs::lfoUnidirectional, nullptr); } );
                else
                    menu.addItem("Make LFO 1 Bidirectional", [&] () { lfoTree.getChildWithName(ParamIDs::lfo1).setProperty(ParamIDs::lfoDirection, ParamIDs::lfoBidirectional, nullptr); } );
            }
            
            if (lfoTree.getChildWithName(ParamIDs::lfo2).getProperty(ParamIDs::lfoOn))
            {
                if (lfoTree.getChildWithName(ParamIDs::lfo2).getProperty(ParamIDs::lfoDirection) == ParamIDs::lfoBidirectional)
                    menu.addItem("Make LFO 2 Unidirectional", [&] () { lfoTree.getChildWithName(ParamIDs::lfo2).setProperty(ParamIDs::lfoDirection, ParamIDs::lfoUnidirectional, nullptr); } );
                else
                    menu.addItem("Make LFO 2 Bidirectional", [&] () { lfoTree.getChildWithName(ParamIDs::lfo2).setProperty(ParamIDs::lfoDirection, ParamIDs::lfoBidirectional, nullptr); } );
            }
            
            menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(this));
        }
        else if (event.mods.isCommandDown() && lfoTree.isValid())
        {
            previousValue = lfoTree.getChild(currentLFONum).getProperty(ParamIDs::lfoRange);
            isModifyingLFORange = true;
            
            if (styleFlags & hideLabelsWhenModifyingLFORange)
            {
                label.setVisible(false);
                textBox.setVisible(false);
            }
        }
        else
        {
            previousValue = currentValue.load();
            attachment.beginGesture();
            isModifyingParameter = true;
        }
        
        event.source.enableUnboundedMouseMovement(true);
        lastMousePosition = event.position;
    }
    
    void mouseDrag (const juce::MouseEvent& event) override
    {
        float delta;
        if (styleFlags & dragHorizontal)
            delta = event.position.x - lastMousePosition.x;
        else
            delta = lastMousePosition.y - event.position.y;
        
        delta *= event.mods.isShiftDown() ? fineDragSensitivity : dragSensitivity;
        // TODO this has weird behavior when using shift mid-drag
        
        if (isModifyingLFORange)
        {
            currentLFORange = juce::jlimit(-1.0f, 1.0f, previousValue + delta);
            if (currentLFORange == 0.0f || currentLFORange == 1.0f)
            {
                lastMousePosition = event.position;
                previousValue = currentLFORange;
            }
            lfoTree.getChild(currentLFONum).setProperty(ParamIDs::lfoRange, currentLFORange, nullptr);
        }
        else if (isModifyingParameter)
        {
            currentValue = juce::jlimit(0.0f, 1.0f, previousValue + delta);
            if (currentValue == 0.0f || currentValue == 1.0f)
            {
                lastMousePosition = event.position;
                previousValue = currentValue.load();
            }
            const float denormValue = param.convertFrom0to1(currentValue);
            attachment.setValueAsPartOfGesture(denormValue);
        }
    }
    
    void mouseUp (const juce::MouseEvent& event) override
    {
        event.source.enableUnboundedMouseMovement(false);
        
        if (isModifyingParameter)
            attachment.endGesture();
        if (isModifyingParameter || isModifyingLFORange)
            juce::Desktop::getInstance().getMainMouseSource().setScreenPosition(event.source.getLastMouseDownPosition());
        
        if (isModifyingLFORange && (styleFlags & hideLabelsWhenModifyingLFORange))
        {
            label.setVisible(true);
            textBox.setVisible(true);
        }
        
        isModifyingLFORange = false;
        isModifyingParameter = false;
    }
    
    void mouseDoubleClick (const juce::MouseEvent& event) override
    {
        const float defaultVal = param.getDefaultValue();
        attachment.setValueAsCompleteGesture(param.convertFrom0to1(defaultVal));;
    }
    
    bool keyPressed (const juce::KeyPress& key) override
    {
        if (('0' <= key.getKeyCode() && '9' >= key.getKeyCode()) || '.' == key.getKeyCode() || '-' == key.getKeyCode() || ('A' <= key.getTextCharacter() && 'G' >= key.getTextCharacter()))
        {
            textBox.valueShownWithEditor = juce::String::charToString(key.getKeyCode());
            textBox.showEditor();
            return true;
        }
        
        return false;
    }
    
    bool isInterestedInDragSource (const juce::DragAndDropTarget::SourceDetails &dragSourceDetails) override
    {
        if (!lfoTree.isValid())
            return false;
        
        const auto description = dragSourceDetails.description.toString();
        
        if (description == ParamIDs::lfo1.toString() || description == ParamIDs::lfo2.toString())
            return true;
        
        return false;
    }
    
    void itemDragEnter (const juce::DragAndDropTarget::SourceDetails &dragSourceDetails) override
    {
        focusBorder.setPaintDragBorder(true);
    }

    void itemDragExit (const juce::DragAndDropTarget::SourceDetails &dragSourceDetails) override
    {
        focusBorder.setPaintDragBorder(false);
    }
    
    void itemDropped (const juce::DragAndDropTarget::SourceDetails &dragSourceDetails) override
    {
        focusBorder.setPaintDragBorder(false);
        
        const auto description = dragSourceDetails.description.toString();
        
        if (description == ParamIDs::lfo1.toString())
        {
            lfoTree.getChildWithName(ParamIDs::lfo1).setProperty(ParamIDs::lfoOn, true, nullptr);
        }
        else if (description == ParamIDs::lfo2.toString())
        {
            lfoTree.getChildWithName(ParamIDs::lfo2).setProperty(ParamIDs::lfoOn, true, nullptr);
        }
    }
    
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override
    {
        if (treeWhosePropertyHasChanged == lfoTree.getChild(currentLFONum))
        {
            if (property == ParamIDs::lfoRange)
            {
                currentLFORange = lfoTree.getChild(currentLFONum)[ParamIDs::lfoRange];
            }
        }
        else if (property == ParamIDs::lfoSelected)
        {
            currentLFONum = lfoTree.getParent().getChildWithName(ParamIDs::lfoSelectionTree)[ParamIDs::lfoSelected];
            currentLFORange = lfoTree.getChild(currentLFONum)[ParamIDs::lfoRange];
        }
        
        repaint();
    }

protected:
    void parameterChangedCallback (float newDenormValue)
    {
        const float newNormValue = param.convertTo0to1(newDenormValue);
        currentValue.store(newNormValue);
        textBox.setText(param.getText(newNormValue, 8), juce::dontSendNotification);
        repaint();
    }
    
    juce::RangedAudioParameter& param;
    juce::ParameterAttachment attachment;
    
    juce::ValueTree lfoTree;
    juce::ValueTree selectionTree;
    
    std::atomic<float> currentValue;
    float previousValue;
    int currentLFONum { 0 };
    float currentLFORange;
    
    juce::Point<float> lastMousePosition;
    const float dragSensitivity { 0.01f };
    const float fineDragSensitivity { 0.001f };
    bool isModifyingLFORange { false };
    bool isModifyingParameter { false };
    
    int styleFlags;
    
    FocusBorder focusBorder;
    
    juce::Label label;
    TextBox textBox;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SliderBase)
};
