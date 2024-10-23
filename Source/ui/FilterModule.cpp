/*
  ==============================================================================

    FilterModule.cpp
    Created: 16 Sep 2024 1:43:21pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "FilterModule.h"

FilterVisualizer::FilterVisualizer(PluginProcessor& p, juce::AudioProcessorValueTreeState& t_apvts, juce::ValueTree t_filterTree)
: processor(p)
, apvts(t_apvts)
, filterTree(t_filterTree)
, highpassCutoffParam(*apvts.getParameter(ParamIDs::filterHighpassCutoff))
, highpassQParam(*apvts.getParameter(ParamIDs::filterHighpassQ))
, highpassGainParam(*apvts.getParameter(ParamIDs::filterHighpassGain))
, lowpassCutoffParam(*apvts.getParameter(ParamIDs::filterLowpassCutoff))
, lowpassQParam(*apvts.getParameter(ParamIDs::filterLowpassQ))
, lowpassGainParam(*apvts.getParameter(ParamIDs::filterLowpassGain))
, highpassCutoffAttachment(highpassCutoffParam, [this] (float val) { highCutChanged(val); })
, highpassQAttachment(highpassQParam, [this] (float val) { highQChanged(val); })
, highpassGainAttachment(highpassGainParam, [this] (float val) { highGainChanged(val); })
, lowpassCutoffAttachment(lowpassCutoffParam, [this] (float val) { lowCutChanged(val); })
, lowpassQAttachment(lowpassQParam, [this] (float val) { lowQChanged(val); })
, lowpassGainAttachment(lowpassGainParam, [this] (float val) { lowGainChanged(val); })
{
    setOpaque(true);
    
    highFilterType = filterTree.getProperty(ParamIDs::filterHighpassMode);
    lowFilterType = filterTree.getProperty(ParamIDs::filterLowpassMode);
    
    nodes.add(new FilterNode(highpassCutoffParam,
                             highpassQParam,
                             highpassGainParam,
                             highpassCutoffAttachment,
                             highpassQAttachment,
                             highpassGainAttachment));
    nodes.add(new FilterNode(lowpassCutoffParam,
                             lowpassQParam,
                             lowpassGainParam,
                             lowpassCutoffAttachment,
                             lowpassQAttachment,
                             lowpassGainAttachment));
    
    magnitudes.resize(1);
    
    ParameterHelper::castParameter (apvts, ParamIDs::filterLowpassCutoff, lowpassCutoff);
    ParameterHelper::castParameter (apvts, ParamIDs::filterLowpassQ, lowpassQ);
    ParameterHelper::castParameter (apvts, ParamIDs::filterLowpassGain, lowpassGain);
    ParameterHelper::castParameter (apvts, ParamIDs::filterHighpassCutoff, highpassCutoff);
    ParameterHelper::castParameter (apvts, ParamIDs::filterHighpassQ, highpassQ);
    ParameterHelper::castParameter (apvts, ParamIDs::filterHighpassGain, highpassGain);
    
    populateFrequencies();
    updateFilter();
    
    filterTree.addListener(this);
    
    highpassCutoffAttachment.sendInitialUpdate();
    highpassQAttachment.sendInitialUpdate();
    highpassGainAttachment.sendInitialUpdate();
    lowpassCutoffAttachment.sendInitialUpdate();
    lowpassQAttachment.sendInitialUpdate();
    lowpassGainAttachment.sendInitialUpdate();
}

FilterVisualizer::~FilterVisualizer()
{
    filterTree.removeListener(this);
}

void FilterVisualizer::setParametersFromMouseInput (const juce::MouseEvent& event)
{
    const float normFreq = (event.position.x - mainArea.getX()) / mainArea.getWidth();
    const float normY = (mainArea.getBottom() - event.position.y) / mainArea.getHeight();
    currentNode->attachmentFreq.setValueAsPartOfGesture(normFreq);
    const float denormY = currentNode->getCurrentYParameter()->convertFrom0to1(normY);
    currentNode->getCurrentYAttachment()->setValueAsPartOfGesture(denormY);
}

void FilterVisualizer::mouseDown (const juce::MouseEvent& event)
{
    GUIHelper::selectClosestNode(event.position, &currentNode, nodes, MyWidths::highlightAreaRadius);
    
    if (!currentNode || !currentNode->visible)
    {
        currentNode = nullptr;
        return;
    }

    currentNode->attachmentFreq.beginGesture();
    currentNode->getCurrentYAttachment()->beginGesture();
    setParametersFromMouseInput(event);
}

void FilterVisualizer::mouseDrag (const juce::MouseEvent& event)
{
    if (currentNode)
    {
        setParametersFromMouseInput(event);
    }
}

void FilterVisualizer::mouseUp (const juce::MouseEvent& event)
{
    if (currentNode)
    {
        currentNode->attachmentFreq.endGesture();
        currentNode->getCurrentYAttachment()->endGesture();
    }
}

void FilterVisualizer::mouseMove (const juce::MouseEvent& event)
{
    GUIHelper::selectClosestNode(event.position, &currentNode, nodes, MyWidths::highlightAreaRadius);
    
    if (currentNode && !currentNode->visible)
        currentNode = nullptr;
    
    repaint();
}

void FilterVisualizer::mouseExit (const juce::MouseEvent& event)
{
    currentNode = nullptr;
    repaint();
}

void FilterVisualizer::paint(juce::Graphics& g)
{
    /* ----- DRAW BACKGROUND GRID ----- */
    
    g.setColour(MyColors::viewBackground);
    g.fillRect(mainArea);
    
    g.setColour(MyColors::darkPrimary);
    
    for (int& freq: frequencies)
    {
        const float normX = juce::mapFromLog10(static_cast<float>(freq),
                                               DSPHelper::minimumFrequency,
                                               DSPHelper::maximumFrequency);
        g.drawLine(normX * mainArea.getWidth() + mainArea.getX(),
                   mainArea.getY(),
                   normX * mainArea.getWidth() + mainArea.getX(),
                   mainArea.getBottom(),
                   MyWidths::thinWidth);
    }
    
    for (int i = 1; i < 8; i++)
    {
        g.drawLine(mainArea.getX(),
                   mainArea.getY() + static_cast<float>(i) / 8 * mainArea.getHeight(),
                   mainArea.getRight(),
                   mainArea.getY() + static_cast<float>(i) / 8 * mainArea.getHeight(),
                   MyWidths::thinWidth);
    }
    
    /* ----- DRAW RESPONSE CURVE ----- */
    
    g.setColour(MyColors::white);
    g.strokePath(responseCurve, juce::PathStrokeType(MyWidths::standardWidth, juce::PathStrokeType::curved));
    
    /* ----- DRAW CONTROLS ----- */
    
    if (currentNode)
    {
        g.setColour(MyColors::overlay);
        g.fillEllipse(currentNode->pos.x - MyWidths::highlightAreaRadius,
                      currentNode->pos.y - MyWidths::highlightAreaRadius,
                      MyWidths::highlightAreaRadius * 2,
                      MyWidths::highlightAreaRadius * 2);
    }
    
    g.setColour(MyColors::lightPrimary);
    for (auto* node: nodes)
    {
        if (node->visible)
            g.drawEllipse(node->pos.x - MyWidths::largeNodeRadius,
                          node->pos.y - MyWidths::largeNodeRadius,
                          MyWidths::largeNodeRadius * 2,
                          MyWidths::largeNodeRadius * 2,
                          MyWidths::standardWidth);
    }
    
    g.setColour(MyColors::darkPrimary);
    GUIHelper::drawRectangleWithThickness(g, mainArea.toFloat(), MyWidths::standardWidth);
}

void FilterVisualizer::resized()
{
    mainArea = getLocalBounds();
    
    for (auto* node: nodes)
        node->viewArea = mainArea;
    
    magnitudes.resize(mainArea.getWidth());
    updateResponseCurve();
}

void FilterVisualizer::populateFrequencies()
{
//    frequencies.resize(29);
    
//    int baseFreq = DSPHelper::minimumFrequency;
//    for (int i = 0; i < 29; i++)
//    {
//        const int reducedIndex = i % 9;
//        if (reducedIndex == 0)
//            baseFreq *= 10;
//
//        frequencies.push_back(baseFreq * (reducedIndex + 1));
//    }
    
    frequencies = { 30, 40, 50, 60, 70, 80, 90, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 20000 };
}

void FilterVisualizer::updateFilter()
{
    using CoeffArray = juce::dsp::IIR::ArrayCoefficients<float>;
    
    const double sampleRate = processor.getSampleRate();
    
    if (lowFilterType == ParameterHelper::pass)
       lowpassCoeffs = CoeffArray::makeLowPass(sampleRate,
                                               DSPHelper::mapNormalizedToFrequency(lowpassCutoff->get()),
                                               lowpassQ->get());
    else if (lowFilterType == ParameterHelper::shelf)
        lowpassCoeffs = CoeffArray::makeHighShelf(sampleRate,
                                                  DSPHelper::mapNormalizedToFrequency(lowpassCutoff->get()),
                                                  lowpassQ->get(),
                                                  juce::Decibels::decibelsToGain(lowpassGain->get()));
    
    if (highFilterType == ParameterHelper::pass)
        highpassCoeffs = CoeffArray::makeHighPass(sampleRate,
                                                  DSPHelper::mapNormalizedToFrequency(highpassCutoff->get()),
                                                  highpassQ->get());
    else if (highFilterType == ParameterHelper::shelf)
        highpassCoeffs = CoeffArray::makeLowShelf(sampleRate,
                                                  DSPHelper::mapNormalizedToFrequency(highpassCutoff->get()),
                                                  highpassQ->get(),
                                                  juce::Decibels::decibelsToGain(highpassGain->get()));
}

void FilterVisualizer::updateResponseCurve()
{
    const int numFrequencies = mainArea.getWidth();
    
    for (int i = 0; i < numFrequencies; i++)
    {
        double mag = 1.0;
        double freq = DSPHelper::mapNormalizedToFrequency((float) i / numFrequencies);
        
        if (lowFilterType < 2)
            mag *= lowpassCoeffs.getMagnitudeForFrequency(freq, processor.getSampleRate());
        if (highFilterType < 2)
            mag *= highpassCoeffs.getMagnitudeForFrequency(freq, processor.getSampleRate());
        
        magnitudes[i] = juce::Decibels::gainToDecibels(mag);
    }
    
    responseCurve.clear();
    
    const double outputMin = mainArea.getBottom();
    const double outputMax = mainArea.getY();
    auto map = [outputMin, outputMax](double input)
    {
        return juce::jmap(juce::jlimit((double) DSPHelper::minimumFilterDecibels,
                                       (double) DSPHelper::maximumFilterDecibels,
                                       input),
                          (double) DSPHelper::minimumFilterDecibels,
                          (double) DSPHelper::maximumFilterDecibels,
                          outputMin,
                          outputMax);
    };
    
    responseCurve.startNewSubPath(mainArea.getX(), map(magnitudes.front()));
    
    for(int i = 1; i < magnitudes.size(); i++)
    {
        responseCurve.lineTo(mainArea.getX() + i, map(magnitudes[i]));
    }
    
    updateNodes();
}

void FilterVisualizer::updateNodes()
{
    nodes[0]->updatePos();
    nodes[1]->updatePos();
    
    repaint();
}

void FilterVisualizer::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
{
    if (property == ParamIDs::filterHighpassMode)
    {
        highFilterType = filterTree[property];
        nodes[0]->mode = highFilterType;
    }
    else if (property == ParamIDs::filterLowpassMode)
    {
        lowFilterType = filterTree[property];
        nodes[1]->mode = lowFilterType;
    }
    
    updateFilter();
    updateResponseCurve();
}

void FilterVisualizer::highCutChanged (float newVal)
{
    nodes[0]->cutoff = newVal;
    updateFilter();
    updateResponseCurve();
}

void FilterVisualizer::highQChanged (float newVal)
{
    nodes[0]->q = newVal;
    updateFilter();
    updateResponseCurve();
}

void FilterVisualizer::highGainChanged (float newVal)
{
    nodes[0]->gain = newVal;
    updateFilter();
    updateResponseCurve();
}

void FilterVisualizer::lowCutChanged (float newVal)
{
    nodes[1]->cutoff = newVal;
    updateFilter();
    updateResponseCurve();
}

void FilterVisualizer::lowQChanged (float newVal)
{
    nodes[1]->q = newVal;
    updateFilter();
    updateResponseCurve();
}

void FilterVisualizer::lowGainChanged (float newVal)
{
    nodes[1]->gain = newVal;
    updateFilter();
    updateResponseCurve();
}


FilterModule::FilterModule(PluginProcessor& p, juce::AudioProcessorValueTreeState& t_apvts, juce::ValueTree t_filterTree, juce::ValueTree lfoTree, juce::ValueTree effectsOrderTree)
: EffectsSlot(ParamIDs::filterNode, effectsOrderTree)
, apvts(t_apvts)
, filterTree(t_filterTree)
, header(juce::Drawable::createFromImageData(BinaryData::filter_header_svg, BinaryData::filter_header_svgSize))
, lowpassIcon(juce::Drawable::createFromImageData(BinaryData::lowpass_svg, BinaryData::lowpass_svgSize))
, highpassIcon(juce::Drawable::createFromImageData(BinaryData::highpass_svg, BinaryData::highpass_svgSize))
, lowshelfIcon(juce::Drawable::createFromImageData(BinaryData::lowshelf_svg, BinaryData::lowshelf_svgSize))
, highshelfIcon(juce::Drawable::createFromImageData(BinaryData::highshelf_svg, BinaryData::highshelf_svgSize))
, offIcon(juce::Drawable::createFromImageData(BinaryData::cancel_svg, BinaryData::cancel_svgSize))
, crosshair(ParamIDs::filterNode.toString())
, visualizer(p, apvts, filterTree)
, highpassCutoffSlider(*apvts.getParameter(ParamIDs::filterHighpassCutoff), "Cutoff", 0, lfoTree.getChildWithName(ParamIDs::lfoFilterHighpassCutoff))
, highpassQSlider(*apvts.getParameter(ParamIDs::filterHighpassQ), "Q", 0, lfoTree.getChildWithName(ParamIDs::lfoFilterHighpassQ))
, highpassGainSlider(*apvts.getParameter(ParamIDs::filterHighpassGain), "Gain", 0, lfoTree.getChildWithName(ParamIDs::lfoFilterHighpassGain))
, lowpassCutoffSlider(*apvts.getParameter(ParamIDs::filterLowpassCutoff), "Cutoff", 0, lfoTree.getChildWithName(ParamIDs::lfoFilterLowpassCutoff))
, lowpassQSlider(*apvts.getParameter(ParamIDs::filterLowpassQ), "Q", 0, lfoTree.getChildWithName(ParamIDs::lfoFilterLowpassQ))
, lowpassGainSlider(*apvts.getParameter(ParamIDs::filterLowpassGain), "Gain", 0, lfoTree.getChildWithName(ParamIDs::lfoFilterLowpassGain))
, highFilterToggler(filterTree, ParamIDs::filterHighpassMode, highpassIcon, lowshelfIcon, offIcon)
, lowFilterToggler(filterTree, ParamIDs::filterLowpassMode, lowpassIcon, highshelfIcon, offIcon)
, mixSlider(*apvts.getParameter(ParamIDs::filterMix), "Mix", TextSlider::displayColorRange, lfoTree.getChildWithName(ParamIDs::lfoFilterMix))
{
    setOpaque(true);
    
    filterTree.addListener(this);
    
    highFilterType = filterTree.getProperty(ParamIDs::filterHighpassMode);
    lowFilterType = filterTree.getProperty(ParamIDs::filterLowpassMode);
    
    addAndMakeVisible(crosshair);
    addAndMakeVisible(visualizer);
    addAndMakeVisible(highpassCutoffSlider);
    addAndMakeVisible(highpassQSlider);
    addChildComponent(highpassGainSlider);
    addAndMakeVisible(lowpassCutoffSlider);
    addAndMakeVisible(lowpassQSlider);
    addChildComponent(lowpassGainSlider);
    addAndMakeVisible(highFilterToggler);
    addAndMakeVisible(lowFilterToggler);
    addAndMakeVisible(mixSlider);
    
    highpassGainSlider.setVisible(filterTree[ParamIDs::filterHighpassMode]);
    lowpassGainSlider.setVisible(filterTree[ParamIDs::filterLowpassMode]);
    
    crosshair.setComponentID(ComponentIDs::effectCrosshair);
    visualizer.setComponentID(ComponentIDs::filterVisualizer);
    highpassCutoffSlider.setComponentID(ComponentIDs::filterLowCutoff);
    highpassQSlider.setComponentID(ComponentIDs::filterLowQ);
    highpassGainSlider.setComponentID(ComponentIDs::filterLowGain);
    highFilterToggler.setComponentID(ComponentIDs::filterLowMode);
    lowpassCutoffSlider.setComponentID(ComponentIDs::filterHighCutoff);
    lowpassQSlider.setComponentID(ComponentIDs::filterHighQ);
    lowpassGainSlider.setComponentID(ComponentIDs::filterHighGain);
    lowFilterToggler.setComponentID(ComponentIDs::filterHighMode);
}

FilterModule::~FilterModule()
{
    filterTree.removeListener(this);
}

void FilterModule::paint (juce::Graphics& g)
{
    g.fillAll(MyColors::background);
    
    header->drawWithin(g, getLocalBounds().toFloat(), juce::RectanglePlacement::xRight, 1.0f);
    
    g.setColour(MyColors::darkNeutral);
    
    if (! highpassGainSlider.isVisible())
        GUIHelper::paintXBox(g, highpassGainSlider.getBounds());
    
    if (! lowpassGainSlider.isVisible())
        GUIHelper::paintXBox(g, lowpassGainSlider.getBounds());
    
    if (paintDragBorder)
    {
        g.setColour(MyColors::lightSecondary);
        GUIHelper::paintOutline(g, mainArea);
    }
}

void FilterModule::resized()
{
    mainArea = getLocalBounds().withTrimmedLeft(MyWidths::additionalHeaderWidth).withTrimmedTop(MyWidths::additionalHeaderHeight);
    juce::Rectangle<int> bounds = mainArea;
    
    crosshair.setBounds(MyWidths::buttonBounds.withX(4).withY(9));
    mixSlider.setBounds(MyWidths::smallTextSliderBounds.withX(1).withBottomY(getHeight() - 16));
    
    const int margin = MyWidths::standardMarginInt;
    
    filterArea = bounds.removeFromTop(135).reduced(margin);
    visualizer.setBounds(filterArea);
    
    juce::Rectangle<int> leftColumn = bounds.removeFromLeft(bounds.proportionOfWidth(0.5f));
    juce::Rectangle<int> rightColumn = bounds;
    
    const int sliderHeight = MyWidths::standardTextSliderBounds.getHeight();
    
    highpassCutoffSlider.setBounds(MyWidths::standardTextSliderBounds.withCentre(leftColumn.removeFromTop(sliderHeight).getCentre()));
    leftColumn.removeFromTop(margin);
    highpassQSlider.setBounds(MyWidths::standardTextSliderBounds.withCentre(leftColumn.removeFromTop(sliderHeight).getCentre()));
    leftColumn.removeFromTop(margin);
    highpassGainSlider.setBounds(MyWidths::standardTextSliderBounds.withCentre(leftColumn.removeFromTop(sliderHeight).getCentre()));
    leftColumn.removeFromTop(margin);
    highFilterToggler.setBounds(MyWidths::buttonBounds.withCentre(leftColumn.removeFromTop(sliderHeight).getCentre()));
    
    lowpassCutoffSlider.setBounds(MyWidths::standardTextSliderBounds.withCentre(rightColumn.removeFromTop(sliderHeight).getCentre()));
    rightColumn.removeFromTop(margin);
    lowpassQSlider.setBounds(MyWidths::standardTextSliderBounds.withCentre(rightColumn.removeFromTop(sliderHeight).getCentre()));
    rightColumn.removeFromTop(margin);
    lowpassGainSlider.setBounds(MyWidths::standardTextSliderBounds.withCentre(rightColumn.removeFromTop(sliderHeight).getCentre()));
    rightColumn.removeFromTop(margin);
    lowFilterToggler.setBounds(MyWidths::buttonBounds.withCentre(rightColumn.removeFromTop(sliderHeight).getCentre()));
}

void FilterModule::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
{
    if (property == ParamIDs::filterHighpassMode)
    {
        highFilterType = filterTree.getProperty(ParamIDs::filterHighpassMode);
        highpassGainSlider.setVisible(highFilterType);
    }
    else if (property == ParamIDs::filterLowpassMode)
    {
        lowFilterType = filterTree.getProperty(ParamIDs::filterLowpassMode);
        lowpassGainSlider.setVisible(lowFilterType);
    }
}


