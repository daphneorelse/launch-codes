/*
  ==============================================================================

    LevelMeter.cpp
    Created: 20 Sep 2024 2:45:04pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "LevelMeter.h"

MeterPair::MeterPair(PluginProcessor& t_processor)
: processor(t_processor)
{
    setOpaque(true);
    
    startTimerHz(refreshRate);
    
    leftLevel.reset(processor.getSampleRate(), 0.25f);
    rightLevel.reset(processor.getSampleRate(), 0.25f);
    leftPeak.reset(processor.getSampleRate(), 0.2f, 1.0f);
    rightPeak.reset(processor.getSampleRate(), 0.2f, 1.0f);
    
    addAndMakeVisible(levelLabel);
    
    levelLabel.setColour(juce::Label::textColourId, MyColors::white);
    levelLabel.setMinimumHorizontalScale(1.0f);
    
    for (auto* child: getChildren())
        child->addMouseListener(this, true);
    
    meterRange = ParameterHelper::normRangeWithMidpointSkew(DSPHelper::minimumLevelDecibels, DSPHelper::maximumLevelDecibels, -24.0f);
}

MeterPair::~MeterPair()
{
    stopTimer();
    
    for (auto* child: getChildren())
        child->removeMouseListener(this);
}

void MeterPair::mouseDown (const juce::MouseEvent& event)
{
    if (event.originalComponent == &levelLabel)
    {
        displayLevel = -100.0f;
    }
}

void MeterPair::paintMeter (juce::Graphics& g, juce::SmoothedValue<float>& level, PeakLevel& peak, juce::Rectangle<int>& area)
{
    const float currentLevelDecibels = juce::jlimit(DSPHelper::minimumLevelDecibels,
                                                    DSPHelper::maximumLevelDecibels,
                                                    level.getCurrentValue());
    const float currentPeakDecibels = juce::jlimit(DSPHelper::minimumLevelDecibels,
                                                   DSPHelper::maximumLevelDecibels,
                                                   peak.getValue());
    
    if (currentPeakDecibels < 0.0f)
        g.setColour(GUIHelper::mapValueToColor(-12.0f,
                                               12.0f,
                                               currentPeakDecibels));
    else
        g.setColour(MyColors::lightestSecondary);
    
    const float normLevel = meterRange.convertTo0to1(currentLevelDecibels);
    const float normPeak = meterRange.convertTo0to1(currentPeakDecibels);
    
    auto bounds = area.toFloat();
    g.fillRoundedRectangle(bounds.removeFromBottom(bounds.proportionOfHeight(normLevel)), 2.0f);
    g.drawLine(area.getX(),
               area.getBottom() - area.proportionOfHeight(normPeak),
               area.getRight(),
               area.getBottom() - area.proportionOfHeight(normPeak),
               MyWidths::standardWidth);
    
    const int numSamples = processor.getSampleRate() * 1.0f / refreshRate;
    level.skip(numSamples);
    peak.skip(numSamples);
}

void MeterPair::paint (juce::Graphics& g)
{
    g.fillAll(MyColors::background);
    
    /* ----- DRAW METERS ----- */
    
    paintMeter(g, leftLevel, leftPeak, leftMeterArea);
    paintMeter(g, rightLevel, rightPeak, rightMeterArea);
    
    /* ----- DRAW MARKINGS ----- */
    
    juce::Path levelMarks;
    
    for (int decibelValue = DSPHelper::minimumLevelDecibels;
         decibelValue < 6;
         decibelValue += 6)
    {
        const float thisY = markingArea.getBottom() - markingArea.getHeight() * meterRange.convertTo0to1(decibelValue);
        const float nextY = markingArea.getBottom() - markingArea.getHeight() * meterRange.convertTo0to1(decibelValue + 6);
        
        if (decibelValue == 0)
            levelMarks.startNewSubPath(leftMeterArea.getX(), thisY);
        else
            levelMarks.startNewSubPath(markingArea.getX(), thisY);
        levelMarks.lineTo(markingArea.getRight(), thisY);
        levelMarks.lineTo(markingArea.getRight(), nextY);
        levelMarks.lineTo(markingArea.getX(), nextY);
    }
    
    g.setColour(MyColors::white);
    g.strokePath(levelMarks, juce::PathStrokeType(MyWidths::thinWidth));
}

void MeterPair::resized()
{
    juce::Rectangle<int> bounds = getLocalBounds();
    const int margin = MyWidths::standardMarginInt;
    
    labelArea = bounds.removeFromBottom(10);
    levelLabel.setBounds(labelArea);
    levelLabel.setFont(labelArea.proportionOfHeight(0.9f));
    bounds.removeFromBottom(margin);
    
    const int meterWidth = bounds.getWidth() * 0.3f;
    
    bounds.reduce(2, 2);
    leftMeterArea = bounds.removeFromLeft(meterWidth);
    bounds.removeFromLeft(margin);
    rightMeterArea = bounds.removeFromLeft(meterWidth);
    bounds.removeFromLeft(margin);
    markingArea = bounds;
}

void MeterPair::setNewLevelAndPeak (juce::SmoothedValue<float>& level, PeakLevel& peak, const int channel)
{
    const float newLevel = processor.getRMSLevel(channel);
    
    if (newLevel > level.getCurrentValue())
        level.setCurrentAndTargetValue(newLevel);
    else
        level.setTargetValue(newLevel);
    
    const float newPeak = processor.getSimplePeak(channel);
    peak.setValue(newPeak);
    
    if (displayPeak)
    {
        if (displayPeak && newPeak > displayLevel)
            displayLevel = newPeak;
    }
    else
        displayLevel = level.getCurrentValue();
}

void MeterPair::timerCallback()
{
    setNewLevelAndPeak(leftLevel, leftPeak, 0);
    setNewLevelAndPeak(rightLevel, rightPeak, 1);
    
    const juce::String text = displayLevel < -96.0f ? "-inf db" : juce::String(displayLevel, 1) + " db";
    
    levelLabel.setText(text, juce::dontSendNotification);
    
    repaint();
}
