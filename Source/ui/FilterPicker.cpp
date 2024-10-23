/*
  ==============================================================================

    FilterPicker.cpp
    Created: 9 Jul 2024 9:15:57pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "FilterPicker.h"

//==============================================================================
FilterPicker::FilterPicker(juce::ValueTree tree, juce::RangedAudioParameter& qParam)
: paramTree(tree)
, qParameter(qParam)
, qAttachment(qParam, [&] (float val) { qChanged(val); })
, previousIcon(juce::Drawable::createFromImageData(BinaryData::previous_svg, BinaryData::previous_svgSize))
, nextIcon(juce::Drawable::createFromImageData(BinaryData::next_svg, BinaryData::next_svgSize))
, previousButton(previousIcon)
, nextButton(nextIcon)
{
    setOpaque(true);
    
    paramTree.addListener(this);
    
    addAndMakeVisible(previousButton);
    addAndMakeVisible(nextButton);
    
    previousButton.callback = [&]
    {
        currentFilterNum--;
        if (currentFilterNum == -1)
            currentFilterNum = 2;
        paramTree.setProperty(ParamIDs::filterType, currentFilterNum, nullptr);
    };
    nextButton.callback = [&]
    {
        currentFilterNum = (currentFilterNum + 1) % 3;
        paramTree.setProperty(ParamIDs::filterType, currentFilterNum, nullptr);
    };
    
    frequencies = new double[numFrequencySamples];
    magnitudes  = new double[numFrequencySamples];
    
    populateFrequencyArray();
    
    currentFilterNum = static_cast<int>(paramTree.getProperty(ParamIDs::filterType));
    qAttachment.sendInitialUpdate();
}

FilterPicker::~FilterPicker()
{
    paramTree.removeListener(this);
}

void FilterPicker::updateFilterGraph ()
{
    switch (currentFilterNum) {
        case 0:
            filterGraph = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 750.0f, currentQ);
            break;
            
        case 1:
            filterGraph = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 750.0f, currentQ);
            break;
            
        case 2:
            filterGraph = juce::dsp::IIR::Coefficients<float>::makeBandPass(sampleRate, 750.0f, currentQ);
            break;
            
        default:
            jassertfalse;
            break;
    }
    
    filterGraph->getMagnitudeForFrequencyArray(frequencies, magnitudes, numFrequencySamples, sampleRate);
    repaint();
}

void FilterPicker::paint (juce::Graphics& g)
{
    g.fillAll(MyColors::background);
    
    const float graphWidth = filterArea.getWidth();
    const float graphHeight = filterArea.getHeight();
    const double maxMag = juce::jmax(getMaxMagnitude() + 0.2, 1.1);
    
    juce::Path graphPath;
    graphPath.startNewSubPath(filterArea.getX(), filterArea.getBottom() - magnitudes[0] * graphHeight / maxMag);
    
    for (size_t i = 0; i < numFrequencySamples; i++)
    {
        // TODO graph with decibels instead of gain
        graphPath.lineTo(filterArea.getX() + graphWidth * i / numFrequencySamples, filterArea.getBottom() - magnitudes[i] * graphHeight / maxMag);
    }
    
    juce::Path midlinePath;
    
    for (size_t i = 0; i < numMidlineDashes; i++)
    {
        midlinePath.startNewSubPath(filterArea.getX() + graphWidth * i / numMidlineDashes, filterArea.getBottom() - 1.0 / maxMag * graphHeight);
        midlinePath.lineTo(filterArea.getX() + graphWidth * (i + 0.8) / numMidlineDashes, filterArea.getBottom() - 1.0 / maxMag * graphHeight);
    }
    
    g.setColour(MyColors::white);
    g.strokePath(graphPath, juce::PathStrokeType(MyWidths::standardWidth, juce::PathStrokeType::curved));
    g.strokePath(midlinePath, juce::PathStrokeType(MyWidths::standardWidth));
}

void FilterPicker::resized()
{
    juce::Rectangle<int> bounds = getLocalBounds();
    
    const float areaWidth = juce::jmin((float) getHeight(), getWidth() * 0.3f);
    
    filterArea = juce::Rectangle<float> (areaWidth, areaWidth);
    filterArea.setCentre(getLocalBounds().getCentre().toFloat());
    filterArea.reduce(2.0f, 2.0f);
    
    juce::Rectangle<int> arrowArea (areaWidth * 0.9f, areaWidth * 0.9f);
    
    previousButton.setBounds(arrowArea.withCentre(bounds.removeFromLeft(areaWidth).getCentre()));
    nextButton.setBounds(arrowArea.withCentre(bounds.removeFromRight(areaWidth).getCentre()));
}

void FilterPicker::valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property)
{
    if (treeWhosePropertyHasChanged == paramTree && property == ParamIDs::filterType)
    {
        currentFilterNum = paramTree.getProperty(ParamIDs::filterType);
        updateFilterGraph();
    }
}

void FilterPicker::qChanged (const float newQ)
{
    currentQ = newQ;
    updateFilterGraph();
}

void FilterPicker::populateFrequencyArray ()
{
    for (size_t i = 0; i < numFrequencySamples; i++)
    {
        frequencies[i] = 20.0 * pow(2.0, i / 4.0);
    }
}

double FilterPicker::getMaxMagnitude ()
{
    double max = 0.0;
    
    for (size_t i = 0; i < numFrequencySamples; i++)
    {
        if (magnitudes[i] > max)
            max = magnitudes[i];
    }
    
    return max;
}
