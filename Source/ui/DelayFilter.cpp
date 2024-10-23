/*
  ==============================================================================

    DelayFilter.cpp
    Created: 26 Jul 2024 2:27:39pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "DelayFilter.h"

DelayFilter::DelayFilter(juce::RangedAudioParameter& centerParam,
                         juce::RangedAudioParameter& widthParam,
                         juce::ValueTree t_delayTree,
                         juce::ValueTree lfoTree,
                         double sampRate)
: delayTree(t_delayTree)
, filterIcon(juce::Drawable::createFromImageData(BinaryData::bandpass_svg, BinaryData::bandpass_svgSize))
, offIcon(juce::Drawable::createFromImageData(BinaryData::cancel_svg, BinaryData::cancel_svgSize))
, centerParameter(centerParam)
, widthParameter(widthParam)
, centerAttachment(centerParameter, [&] (float value) { updateCenter(value); }, nullptr)
, widthAttachment(widthParameter, [&] (float value) { updateWidth(value); }, nullptr)
, centerSlider(centerParameter, "Center", 0, lfoTree.getChildWithName(ParamIDs::lfoDelayFilterCenter))
, widthSlider(widthParameter, "Width", 0, lfoTree.getChildWithName(ParamIDs::lfoDelayFilterWidth))
, ioButton(delayTree, ParamIDs::delayFilterIO, offIcon, filterIcon)
, sampleRate(sampRate)
{
    setOpaque(true);
    
    delayTree.addListener(this);
    
    filterOn = delayTree[ParamIDs::delayFilterIO];
    magnitudes.resize(1);
    
    addAndMakeVisible(ioButton);
    addAndMakeVisible(centerSlider);
    addAndMakeVisible(widthSlider);
    
    centerSlider.setComponentID(ComponentIDs::delayFilterCenter);
    widthSlider.setComponentID(ComponentIDs::delayFilterWidth);
    
    centerAttachment.sendInitialUpdate();
    widthAttachment.sendInitialUpdate();
}

DelayFilter::~DelayFilter()
{
    delayTree.removeListener(this);
}

void DelayFilter::mouseDown (const juce::MouseEvent& event)
{
    if (mainArea.contains(event.getPosition()))
    {
        const float centerVal = (event.position.x - mainArea.getX()) / mainArea.getWidth();
        centerAttachment.beginGesture();
        centerAttachment.setValueAsPartOfGesture(centerVal);
        
        const float widthVal = (mainArea.getBottom() - event.position.y) / mainArea.getHeight();
        widthAttachment.beginGesture();
        widthAttachment.setValueAsPartOfGesture(widthParameter.convertFrom0to1(widthVal));
    }
}

void DelayFilter::mouseDrag (const juce::MouseEvent& event)
{
    if (mainArea.contains(event.mouseDownPosition.toInt()))
    {
        const float normFreq = (event.position.x - mainArea.getX()) / mainArea.getWidth();
        const float normWidth = (mainArea.getBottom() - event.position.y) / mainArea.getHeight();
        centerAttachment.setValueAsPartOfGesture(normFreq);
        const float denormWidth = widthParameter.convertFrom0to1(normWidth);
        widthAttachment.setValueAsPartOfGesture(denormWidth);

    }
}

void DelayFilter::mouseUp (const juce::MouseEvent& event)
{
    if (mainArea.contains(event.mouseDownPosition.toInt()))
    {
        centerAttachment.endGesture();
        widthAttachment.endGesture();
    }
}

void DelayFilter::paint (juce::Graphics& g)
{
    g.fillAll(MyColors::background);
    
    g.setColour(MyColors::viewBackground);
    g.fillRect(mainArea);
    
    g.setColour(filterOn ? MyColors::white : MyColors::mediumNeutral);
    g.strokePath(responseCurve, juce::PathStrokeType(MyWidths::standardWidth));
    
    const float x = mainArea.getX() + DSPHelper::mapFrequencyToNormalized(currentCenter) * mainArea.getWidth();
    const float y = mainArea.getBottom() - widthParameter.convertTo0to1(currentWidth) * mainArea.getHeight();
    
    g.setColour(MyColors::lightPrimary);
    g.drawEllipse(x - MyWidths::largeNodeRadius,
                  y - MyWidths::largeNodeRadius,
                  MyWidths::largeNodeRadius * 2,
                  MyWidths::largeNodeRadius * 2,
                  MyWidths::standardWidth);
    
    g.setColour(MyColors::darkPrimary);
    GUIHelper::drawRectangleWithThickness(g, mainArea.expanded(MyWidths::standardWidth / 2).toFloat(), MyWidths::standardWidth);
}

void DelayFilter::resized()
{
    const int margin = MyWidths::standardMarginInt;
    
    auto bounds = getLocalBounds().reduced(3);
    mainArea = bounds.removeFromTop(bounds.getHeight() - MyWidths::buttonDimension - margin);
    bounds.removeFromTop(margin);
    
    magnitudes.resize(mainArea.getWidth());
    updateResponseCurve();
    
    ioButton.setBounds(bounds.removeFromRight(MyWidths::buttonDimension));
    centerSlider.setBounds(bounds.removeFromLeft(bounds.proportionOfWidth(0.5f)));
    widthSlider.setBounds(bounds);
}

void DelayFilter::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
{
    if (property == ParamIDs::delayFilterIO)
    {
        filterOn = delayTree.getProperty(ParamIDs::delayFilterIO);
        updateFilter();
        updateResponseCurve();
    }
}

void DelayFilter::updateFilter()
{
    const float currentLowpassCutoff = DSPHelper::getDelayLowpassCutoff(currentCenter, currentWidth);
    const float currentHighpassCutoff = DSPHelper::getDelayHighpassCutoff(currentCenter, currentWidth);
    
    using CoeffArray = juce::dsp::IIR::ArrayCoefficients<float>;
    
    lowpassCoeffs = CoeffArray::makeLowPass(sampleRate,
                                            currentLowpassCutoff,
                                            DSPHelper::defaultFilterQ);
    
    highpassCoeffs = CoeffArray::makeHighPass(sampleRate,
                                              currentHighpassCutoff,
                                              DSPHelper::defaultFilterQ);
}

void DelayFilter::updateResponseCurve()
{
    const int numFrequencies = mainArea.getWidth();
    
    for (int i = 0; i < numFrequencies; i++)
    {
        double mag = 1.0;
        double freq = DSPHelper::mapNormalizedToFrequency((float) i / numFrequencies);
        
        if (filterOn)
        {
            mag *= lowpassCoeffs.getMagnitudeForFrequency(freq, sampleRate);
            mag *= highpassCoeffs.getMagnitudeForFrequency(freq, sampleRate);
        }
        
        magnitudes[i] = juce::Decibels::gainToDecibels(mag);
    }
    
    responseCurve.clear();
    
    const double outputMin = mainArea.getBottom();
    const double outputMax = mainArea.getY();
    auto map = [outputMin, outputMax](double input)
    {
        return juce::jmap(juce::jlimit(-12.0,
                                       6.0,
                                       input),
                          -12.0,
                          6.0,
                          outputMin,
                          outputMax);
    };
    
    responseCurve.startNewSubPath(mainArea.getX(), map(magnitudes.front()));
    
    for(int i = 1; i < magnitudes.size(); i++)
    {
        responseCurve.lineTo(mainArea.getX() + i, map(magnitudes[i]));
    }
    
    repaint();
}

void DelayFilter::updateCenter(float newCenter)
{
    currentCenter = DSPHelper::mapNormalizedToFrequency(newCenter);
    updateFilter();
    updateResponseCurve();
}

void DelayFilter::updateWidth(float newWidth)
{
    currentWidth = newWidth;
    updateFilter();
    updateResponseCurve();
}
