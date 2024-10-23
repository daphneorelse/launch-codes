/*
  ==============================================================================

    GUIHelper.cpp
    Created: 22 Sep 2024 3:53:34pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#include "GUIHelper.h"

juce::Colour GUIHelper::intHSBToColor (const int h, const int s, const int b, const int a)
{
    return juce::Colour (static_cast<float>(h) / 360,
                         static_cast<float>(s) / 100,
                         static_cast<float>(b) / 100,
                         static_cast<float>(a) / 100);
}

float GUIHelper::cubicBezier (const float x)
{
    return x < 0.5 ? 2 * x * x : 1 - std::powf(-2 * x + 2, 2) / 2;
}

float GUIHelper::transformWithSlope (float input, float slope)
{
    return (expf(slope * input) - 1.0f) / (expf(slope) - 1.0f);
}

float GUIHelper::inverseTransformWithSlope (float input, float slope)
{
    if (slope == 0)
        return input;
    return log((expf(slope) - 1) * (input - 1 / (1 - expf(slope)))) / slope;
}

void GUIHelper::drawRectangleWithThickness (juce::Graphics& g, const juce::Rectangle<float> rectangle, const float thickness)
{
    juce::Path rectPath;
    rectPath.addRectangle(rectangle.reduced(thickness / 2.0f));
    g.strokePath(rectPath, juce::PathStrokeType(thickness));
}

void GUIHelper::paintOutline (juce::Graphics& g, const juce::Rectangle<int>& componentBounds)
{
    juce::Rectangle<float> bounds = componentBounds.toFloat().reduced(MyWidths::standardWidth / 2);
    float rotation = 0.0f;
    
    for (juce::Point<float> p: {bounds.getTopLeft(), bounds.getTopRight(), bounds.getBottomRight(), bounds.getBottomLeft()})
    {
        juce::Path corner;
        corner.startNewSubPath(p.x, p.y + 10.0f);
        corner.lineTo(p);
        corner.lineTo(p.x + 10.0f, p.y);
        corner.applyTransform(juce::AffineTransform::rotation(rotation, p.x, p.y));
        rotation += juce::MathConstants<float>::halfPi;
        g.strokePath(corner, juce::PathStrokeType{MyWidths::standardWidth});
    }
}

void GUIHelper::paintXBox (juce::Graphics& g, const juce::Rectangle<int>& bounds)
{
    drawRectangleWithThickness(g, bounds.toFloat(), MyWidths::thinWidth);
    
    juce::Rectangle<int> reducedBounds = bounds.reduced(1);
    
    g.drawLine(reducedBounds.getX(),
               reducedBounds.getY(),
               reducedBounds.getRight(),
               reducedBounds.getBottom(),
               MyWidths::thinWidth);
    g.drawLine(reducedBounds.getX(),
               reducedBounds.getBottom(),
               reducedBounds.getRight(),
               reducedBounds.getY(),
               MyWidths::thinWidth);
}

juce::Colour GUIHelper::mapValueToColor (const float lowerLimit, const float upperLimit, const float value, const float brightness)
{
    const float limitedValue = juce::jlimit(lowerLimit, upperLimit, value);
    return juce::Colour(juce::jmap(limitedValue, lowerLimit, upperLimit, 0.27f, 0.0f), 0.8f, brightness, 1.0f);
}

juce::Rectangle<int> GUIHelper::getCenterSquare (const juce::Rectangle<int> bounds)
{
    const int dimension = juce::jmin(bounds.getWidth(), bounds.getHeight());
    juce::Rectangle<int> square (dimension, dimension);
    return square.withCentre(bounds.getCentre());
}

juce::Rectangle<int> GUIHelper::getCenterRectWithWidth (const juce::Rectangle<int> bounds, const int width)
{
    juce::Rectangle<int> rect (width, bounds.getHeight());
    return rect.withCentre(bounds.getCentre());
}
