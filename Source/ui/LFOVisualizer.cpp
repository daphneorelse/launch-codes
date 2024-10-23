/*
  ==============================================================================

    LFOVisualizer.cpp
    Created: 30 Aug 2024 10:19:22am
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "LFOVisualizer.h"

LFOVisualizer::LFOVisualizer()
: fifo(64)
{
    setOpaque (true);
    
    values.fill(0.0f);
    setRepaintRate (60);
}

LFOVisualizer::~LFOVisualizer()
{
    if (isTimerRunning())
        stopTimer();
}

void LFOVisualizer::setBufferSize(int newBufferSize)
{
    bufferSize = newBufferSize;
    values.resize(bufferSize);
    values.fill(0.0f);
}

void LFOVisualizer::setSamplesPerBlock(int newNumInputSamplesPerBlock)  noexcept
{
    inputSamplesPerBlock.store(newNumInputSamplesPerBlock);
}

void LFOVisualizer::pushBuffer (const juce::AudioBuffer<float>& bufferToPush)
{
//    if (!viewOn)
//        return;
    
    // TODO check for bugs when closing computer
    
    const int numSamples = bufferToPush.getNumSamples();
    
    const float* buff = bufferToPush.getReadPointer(0);
    while (sampleIndex < numSamples)
    {
        float val = buff[sampleIndex];
        fifo.push(std::move(val));
        sampleIndex += inputSamplesPerBlock.load();
    }
    
    sampleIndex -= numSamples;
}

void LFOVisualizer::setRepaintRate (int frequencyInHz)
{
    repaintRate = frequencyInHz;
    startTimerHz(frequencyInHz);
}

void LFOVisualizer::mouseDown (const juce::MouseEvent& event)
{
    viewOn = !viewOn;
    
    if (viewOn)
    {
        startTimerHz(repaintRate);
        values.fill(0.0f);
        float dummy;
        while (fifo.pop(dummy))
            ;
    }
    else
        stopTimer();
}

void LFOVisualizer::paint (juce::Graphics& g)
{
    /* ----- DRAW VISUALIZER BACKGROUND ----- */
    
    g.fillAll(MyColors::viewBackground);
    
    juce::Path gridLines;
    const auto bounds = getLocalBounds();
    
    for (int i = 1; i < numXGridLines - 1; i++)
    {
        const float x = bounds.getX() + (static_cast<float>(i) / (numXGridLines - 1)) * bounds.getWidth();
        
        gridLines.startNewSubPath(x, bounds.getY());
        gridLines.lineTo         (x, bounds.getBottom());
    }
    
    for (int i = 1; i < numYGridLines - 1; i++)
    {
        const float y = bounds.getY() + (static_cast<float>(i) / (numYGridLines - 1)) * bounds.getHeight();
        
        gridLines.startNewSubPath(bounds.getX(), y);
        gridLines.lineTo         (bounds.getRight(), y);
    }
    
    g.setColour(MyColors::darkPrimary);
    g.strokePath(gridLines, juce::PathStrokeType(MyWidths::standardWidth));
    GUIHelper::drawRectangleWithThickness(g, bounds.toFloat(), MyWidths::standardWidth);
    
    /* ----- DRAW LFO GRAPH ----- */
    
    const int width = mainArea.getWidth();
    const int halfHeight = mainArea.getHeight() / 2;

    juce::Path path;
    path.startNewSubPath(mainArea.getX(), mainArea.getCentre().y - values[arrayIndex % bufferSize] * halfHeight);
    
    for (int i = 1; i < bufferSize; i++)
    {
        float arrayValue = values[(arrayIndex + i) % bufferSize];
        path.lineTo(mainArea.getX() + static_cast<float>(i) / bufferSize * width, mainArea.getCentre().y - arrayValue * halfHeight);
    }
    
    g.setColour (MyColors::lightPrimary);
    g.strokePath (path, juce::PathStrokeType(MyWidths::standardWidth, juce::PathStrokeType::curved));
}

void LFOVisualizer::resized()
{
    mainArea = getLocalBounds().reduced(2);
}

void LFOVisualizer::timerCallback()
{
    float newSample;
    while (fifo.pop(newSample))
    {
        if (arrayIndex >= bufferSize)
            arrayIndex = 0;

        values.set(arrayIndex, newSample);
        arrayIndex++;
    }
    
    repaint();
}
