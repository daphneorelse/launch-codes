/*
  ==============================================================================

    ValueTreeComponents.h
    Created: 11 Jul 2024 11:11:22am
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GUIHelper.h"
#include "FocusBorder.h"

//==============================================================================
/*
*/
class ValueTreeTogglerBase  : public juce::Component
                            , public juce::ValueTree::Listener

{
public:
    ValueTreeTogglerBase(juce::ValueTree treeNode, const juce::Identifier& propertyName);
    ~ValueTreeTogglerBase() override;
    
    void mouseDown (const juce::MouseEvent& event) override;
    void mouseUp   (const juce::MouseEvent& event) override;
    
    void handlePaintFocusBorder (juce::Graphics& g);
    
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;

protected:
    juce::ValueTree tree;
    const juce::Identifier& propName;
    
    bool state;
    bool pressed {false};
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ValueTreeTogglerBase)
};


class ValueTreeToggleSwitch : public ValueTreeTogglerBase
{
public:
    ValueTreeToggleSwitch(juce::ValueTree treeNode, const juce::Identifier& propertyName) : ValueTreeTogglerBase(treeNode, propertyName)
    {
        setOpaque(true);
    }
    
    void paint (juce::Graphics& g) override
    {
        g.fillAll(MyColors::background);
        
        const juce::Rectangle<float> area = getLocalBounds().toFloat();
        const juce::Point<float> center = area.getCentre();
        
        const float outerRadius = 15.0f;
        const float innerRadius = outerRadius * 0.7f;
        
        g.setColour(MyColors::mediumNeutral);
        g.fillEllipse(center.x - outerRadius, center.y - outerRadius, outerRadius * 2, outerRadius * 2);
        g.setColour(MyColors::background);
        g.fillEllipse(center.x - innerRadius, center.y - innerRadius, innerRadius * 2, innerRadius * 2);
        
        juce::Path switchPath;
        switchPath.startNewSubPath(center.x - 2.5f, center.y + 2.0f);
        switchPath.lineTo(center.x - 4.0f, area.getY() + 4.0f);
        switchPath.lineTo(center.x + 4.0f, area.getY() + 4.0f);
        switchPath.lineTo(center.x + 2.5f, center.y + 2.0f);
        switchPath.closeSubPath();
        switchPath.addEllipse(center.x - 3.85f, area.getY() + 0.9f, 7.7f, 7.7f);
        
        g.setColour(pressed ? MyColors::mediumNeutral : MyColors::lightNeutral);
        
        if (state)
        {
            g.fillPath(switchPath);
        }
        else
        {
            g.fillPath(switchPath, juce::AffineTransform::rotation(juce::MathConstants<float>::pi, center.x, center.y));
        }
    };
};


class ValueTreeToolButton : public ValueTreeTogglerBase
{
public:
    ValueTreeToolButton(juce::ValueTree treeNode,
                        const juce::Identifier& propertyName,
                        std::unique_ptr<juce::Drawable>& t_defaultIcon,
                        std::unique_ptr<juce::Drawable>& t_toggledIcon,
                        int t_styleFlags = 0)
    : ValueTreeTogglerBase(treeNode, propertyName)
    , defaultIcon(t_defaultIcon)
    , toggledIcon(t_toggledIcon)
    , styleFlags(t_styleFlags)
    {
        setOpaque(true);
    }
    
    enum StyleFlags
    {
        showBackgroundToggled = 1,
        showColoredBackgrounds = 2,
        fitBorderToBounds = 4
    };
    
    void paint (juce::Graphics& g) override
    {
        g.fillAll(MyColors::background);
        
        g.setColour(MyColors::lightNeutral);
        GUIHelper::drawRectangleWithThickness(g, mainArea.toFloat(), MyWidths::thinWidth);
        
        if (state && styleFlags & showBackgroundToggled)
        {
            g.setColour(MyColors::mediumPrimary);
            g.fillRect(mainArea.reduced(2));
        }
        else if (styleFlags & showColoredBackgrounds)
        {
            if (state)
                g.setColour(MyColors::lightSecondary);
            else
                g.setColour(MyColors::darkPrimary);
            
            g.fillRect(mainArea.reduced(2));
        }
            
        if (pressed)
        {
            g.setColour(MyColors::overlay);
            g.fillRect(mainArea.reduced(2));
        }
        
        if (state)
            toggledIcon->drawWithin(g, iconArea.toFloat(), 0, 1.0f);
        else
            defaultIcon->drawWithin(g, iconArea.toFloat(), 0, 1.0f);
    }

    void resized() override
    {
        if (styleFlags & fitBorderToBounds)
        {
            mainArea = getLocalBounds();
            const int dimension = juce::jmin(getWidth(), getHeight()) - 4 * 2;
            iconArea = juce::Rectangle<int> (dimension, dimension);
            iconArea.setCentre(mainArea.getCentre());
        }
        else
        {
            const int dimension = juce::jmin(getWidth(), getHeight());
            mainArea = juce::Rectangle<int> (dimension, dimension);
            mainArea.setCentre(getLocalBounds().getCentre());
            iconArea = mainArea.reduced(4);
        }
        
    }
    
private:
    std::unique_ptr<juce::Drawable>& defaultIcon;
    std::unique_ptr<juce::Drawable>& toggledIcon;
    
    juce::Rectangle<int> iconArea, mainArea;
    
    int styleFlags;
};


class ValueTreeToolMultiToggler : public juce::Component
                                , public juce::ValueTree::Listener
{
public:
    ValueTreeToolMultiToggler(juce::ValueTree t_tree,
                              const juce::Identifier& t_propertyName,
                              std::unique_ptr<juce::Drawable>& t_icon1,
                              std::unique_ptr<juce::Drawable>& t_icon2,
                              std::unique_ptr<juce::Drawable>& t_icon3)
    : tree(t_tree)
    , propertyName(t_propertyName)
    , icon1(t_icon1)
    , icon2(t_icon2)
    , icon3(t_icon3)
    {
        setOpaque(true);
        
        tree.addListener(this);
        
        selection = static_cast<int>(tree[propertyName]);
    }
    
    void mouseDown (const juce::MouseEvent& event) override
    {
        pressed = true;
        repaint();
    }
    
    void mouseUp (const juce::MouseEvent& event) override
    {
        pressed = false;
        repaint();
        if (contains(event.position))
            tree.setProperty(propertyName, (selection + 1) % 3, nullptr);
    }
    
    void paint (juce::Graphics& g) override
    {
        g.fillAll(MyColors::background);
        
        g.setColour(MyColors::lightNeutral);
        GUIHelper::drawRectangleWithThickness(g, mainArea.toFloat(), MyWidths::thinWidth);
        
        if (pressed)
        {
            g.setColour(MyColors::overlay);
            g.fillRect(mainArea.reduced(2));
        }
        
        switch (selection) {
            case 0:
                icon1->drawWithin(g, iconArea.toFloat(), 0, 1.0f);
                break;

            case 1:
                icon2->drawWithin(g, iconArea.toFloat(), 0, 1.0f);
                break;

            case 2:
                icon3->drawWithin(g, iconArea.toFloat(), 0, 1.0f);
                break;

            default:
                break;
        }
            
    }

    void resized() override
    {
        const int dimension = juce::jmin(getWidth(), getHeight());
        mainArea = juce::Rectangle<int> (dimension, dimension);
        mainArea.setCentre(getLocalBounds().getCentre());
        iconArea = mainArea.reduced(4);
    }
    
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override
    {
        if (property == propertyName)
        {
            selection = static_cast<int>(tree[propertyName]);
        }
    }
    
private:
    juce::ValueTree tree;
    const juce::Identifier& propertyName;
    
    std::unique_ptr<juce::Drawable>& icon1;
    std::unique_ptr<juce::Drawable>& icon2;
    std::unique_ptr<juce::Drawable>& icon3;
    
    juce::Rectangle<int> iconArea, mainArea;
    
    int selection { 0 };
    bool pressed { false };
};

template <class T>
class ComponentSwitcher : public juce::Component
                        , public juce::ValueTree::Listener
{
public:
    ComponentSwitcher (T& t_comp1,
                       T& t_comp2,
                       juce::ValueTree t_tree,
                       const juce::Identifier& t_property,
                       juce::UndoManager* um = nullptr)
    : comp1(t_comp1)
    , comp2(t_comp2)
    , tree(t_tree)
    , property(t_property)
    {
        tree.addListener(this);
        
        state = tree[property];
        
        addAndMakeVisible(comp1);
        addAndMakeVisible(comp2);
        
        updateKnobVisible();
    }
    
    ~ComponentSwitcher() override
    {
        tree.removeListener(this);
    }
    
    void updateKnobVisible()
    {
        if (state)
        {
            comp2.setVisible(true);
            comp1.setVisible(false);
        }
        else
        {
            comp1.setVisible(true);
            comp2.setVisible(false);
        }
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds();
        comp1.setBounds(bounds);
        comp2.setBounds(bounds);
    }
    
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& changedProperty) override
    {
        if (changedProperty == property)
        {
            state = tree[property];
            updateKnobVisible();
        }
    }
    
private:
    T& comp1;
    T& comp2;
    
    juce::ValueTree tree;
    const juce::Identifier& property;
    
    bool state;
};
