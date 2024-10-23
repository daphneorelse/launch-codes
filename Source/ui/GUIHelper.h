/*
  ==============================================================================

    GUIHelper.h
    Created: 11 May 2024 11:28:43am
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct GUIHelper
{
    static juce::Colour intHSBToColor (const int h,
                                       const int s,
                                       const int b,
                                       const int a);

    static float cubicBezier (const float x);
    
    static float transformWithSlope (float input, float slope);
    
    static float inverseTransformWithSlope (float input, float slope);

    static void drawRectangleWithThickness (juce::Graphics& g,
                                            const juce::Rectangle<float> rectangle,
                                            const float thickness);

    template <typename NodeType>
    static void selectClosestNode (const juce::Point<float> eventPosition,
                                       NodeType** currentNodePtr,
                                       juce::OwnedArray<NodeType>& nodes,
                                       const float distance)
    {
        float minDistance = distance;
        
        for (auto& node: nodes)
        {
            const float distance = std::hypot(node->pos.x - eventPosition.x, node->pos.y - eventPosition.y);
            if (distance < minDistance)
            {
                minDistance = distance;
                *currentNodePtr = node;
            }
        }
        
        if (minDistance >= distance)
            *currentNodePtr = nullptr;
    }

    static void paintOutline (juce::Graphics& g,
                              const juce::Rectangle<int>& componentBounds);

    static void paintXBox (juce::Graphics& g,
                           const juce::Rectangle<int>& bounds);

    static juce::Colour mapValueToColor (const float lowerLimit,
                                         const float upperLimit,
                                         const float value,
                                         const float brightness = 0.95f);
    
    static juce::Rectangle<int> getCenterSquare (const juce::Rectangle<int> bounds);
    
    static juce::Rectangle<int> getCenterRectWithWidth (const juce::Rectangle<int> bounds, const int width);
};

namespace MyWidths
{
constexpr float thinWidth     { 1.0f };
constexpr float standardWidth { 2.0f };
constexpr float mediumWidth   { 3.0f };
constexpr float thickWidth    { 4.0f };
constexpr float largeNodeRadius { 6.0f };
constexpr float smallNodeRadius { 4.0f };
constexpr float highlightAreaRadius { 25.0f };
constexpr int   narrowMarginInt { 2 };
constexpr float standardMarginFloat { 4.0f };
constexpr int   standardMarginInt { 4 };
constexpr int   buttonDimension { 28 };
const juce::Rectangle<int> buttonBounds (buttonDimension, buttonDimension);
constexpr int   knobDimension { 66 };
const juce::Rectangle<int> knobBounds (knobDimension, knobDimension);
const juce::Rectangle<int> standardTextSliderBounds (55, buttonDimension);
const juce::Rectangle<int> horizontalTextSliderBounds (54, 15);
const juce::Rectangle<int> smallTextSliderBounds (34, buttonDimension);
constexpr int additionalHeaderWidth { 35 };
constexpr int additionalHeaderHeight { 9 };
// TODO font size constants
}

namespace MyColors
{
const juce::Colour background        { GUIHelper::intHSBToColor(227, 29, 18,  100) }; // 2a3f2f
const juce::Colour viewBackground    { GUIHelper::intHSBToColor(227, 29, 16,  100) };
const juce::Colour white             { GUIHelper::intHSBToColor(0,   0,  100, 100) };
const juce::Colour overlay           { GUIHelper::intHSBToColor(0,   0,  100, 40 ) };
const juce::Colour transparent       { GUIHelper::intHSBToColor(0,   0,  0,   0  ) };
const juce::Colour lightestPrimary   { GUIHelper::intHSBToColor(133, 60, 90,  100) }; // bce5c5
const juce::Colour lightPrimary      { GUIHelper::intHSBToColor(133, 60, 76,  100) }; // 8ec29a
const juce::Colour mediumPrimary     { GUIHelper::intHSBToColor(133, 60, 56,  100) }; // 638e6d
const juce::Colour darkPrimary       { GUIHelper::intHSBToColor(133, 60, 36,  100) }; // 415c47
const juce::Colour darkestPrimary    { GUIHelper::intHSBToColor(133, 60, 20,  100) };
const juce::Colour lightestSecondary { GUIHelper::intHSBToColor(0,   85, 100, 100) };
const juce::Colour lightSecondary    { GUIHelper::intHSBToColor(0,   85, 95,  100) };
const juce::Colour mediumSecondary   { GUIHelper::intHSBToColor(0,   85, 78,  100) };
const juce::Colour darkSecondary     { GUIHelper::intHSBToColor(0,   85, 49,  100) };
const juce::Colour lightestNeutral   { GUIHelper::intHSBToColor(133, 2,  92,  100) };
const juce::Colour lightNeutral      { GUIHelper::intHSBToColor(133, 2,  72,  100) };
const juce::Colour mediumNeutral     { GUIHelper::intHSBToColor(133, 2,  43,  100) };
const juce::Colour darkNeutral       { GUIHelper::intHSBToColor(133, 2,  30,  100) };
}
