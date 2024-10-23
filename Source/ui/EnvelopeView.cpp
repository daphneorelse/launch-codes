/*
  ==============================================================================

    EnvelopeView.cpp
    Created: 17 May 2024 9:15:34pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "EnvelopeView.h"

/* ----- ENVELOPE GRAPH ----- */

EnvelopeGraph::EnvelopeGraph(juce::RangedAudioParameter* param1Y,
                             juce::RangedAudioParameter* param2X,
                             juce::RangedAudioParameter* param2Y,
                             juce::RangedAudioParameter* param3X,
                             juce::RangedAudioParameter* param3Y,
                             juce::RangedAudioParameter* param4X,
                             juce::RangedAudioParameter* param4Y,
                             juce::OwnedArray<juce::dsp::LookupTableTransform<float>>& tables,
                             juce::UndoManager* um)
: audioParam1Y(param1Y)
, audioParam2X(param2X)
, audioParam2Y(param2Y)
, audioParam3X(param3X)
, audioParam3Y(param3Y)
, audioParam4X(param4X)
, audioParam4Y(param4Y)
, paramAttachment1Y(*param1Y, [&] (float val) {update1Y(val);}, um)
, paramAttachment2X(*param2X, [&] (float val) {update2X(val);}, um)
, paramAttachment2Y(*param2Y, [&] (float val) {update2Y(val);}, um)
, paramAttachment3X(*param3X, [&] (float val) {update3X(val);}, um)
, paramAttachment3Y(*param3Y, [&] (float val) {update3Y(val);}, um)
, paramAttachment4X(*param4X, [&] (float val) {update4X(val);}, um)
, paramAttachment4Y(audioParam4Y ? std::unique_ptr<juce::ParameterAttachment> (new juce::ParameterAttachment(*audioParam4Y, [&] (float val) {update4Y(val);} , um)) : nullptr)
, envTables(tables)
{
    for (int i = 0; i < 4; i++)
        points.emplace_back(0.0f, 0.0f);
    
    paramAttachment1Y.sendInitialUpdate();
    paramAttachment2X.sendInitialUpdate();
    paramAttachment2Y.sendInitialUpdate();
    paramAttachment3X.sendInitialUpdate();
    paramAttachment3Y.sendInitialUpdate();
    paramAttachment4X.sendInitialUpdate();
    if (paramAttachment4Y)
        paramAttachment4Y->sendInitialUpdate();
}

void EnvelopeGraph::paint(juce::Graphics & g)
{
    juce::Path envGraph;
    envGraph.startNewSubPath(mainArea.getX(), mainArea.getBottom() - points[0].y * mainArea.getHeight());
    
    for (int i = 0; i < points.size() - 1; i++)
    {
        float totalXNorm = 0;
        for (int j = 1; j <= i; j++)
            totalXNorm += points[j].x;

        const float startX = mainArea.getX() + totalXNorm * timeParameterWidth;
        const float startY = mainArea.getBottom() - points[i].y * mainArea.getHeight();
        float x = startX;
        const float deltaX = points[i + 1].x * timeParameterWidth / resolution;
        
        for (int k = 0; k < resolution; k++)
        {
            const float tableVal = envTables[i]->processSample(static_cast<float>(k) / resolution);
            const float y = startY - mainArea.getHeight() * (tableVal * (points[i + 1].y - points[i].y));
            envGraph.lineTo(x, y);
            x += deltaX;
        }
    }
    
    g.setColour(MyColors::white);
    g.strokePath(envGraph, juce::PathStrokeType(MyWidths::standardWidth, juce::PathStrokeType::curved));
}

void EnvelopeGraph::resized()
{
    mainArea = getLocalBounds();
    timeParameterWidth = mainArea.getWidth() * 0.3f;
}

void EnvelopeGraph::update1Y(float newVal)
{
    points[0].y = audioParam1Y->convertTo0to1(newVal);
    repaint();
}

void EnvelopeGraph::update2X(float newVal)
{
    points[1].x = audioParam2X->convertTo0to1(newVal);
    repaint();
}

void EnvelopeGraph::update2Y(float newVal)
{
    points[1].y = audioParam2Y->convertTo0to1(newVal);
    repaint();
}

void EnvelopeGraph::update3X(float newVal)
{
    points[2].x = audioParam3X->convertTo0to1(newVal);
    repaint();
}

void EnvelopeGraph::update3Y(float newVal)
{
    points[2].y = audioParam3Y->convertTo0to1(newVal);
    repaint();
}

void EnvelopeGraph::update4X(float newVal)
{
    points[3].x = audioParam4X->convertTo0to1(newVal);
    repaint();
}

void EnvelopeGraph::update4Y(float newVal)
{
    points[3].y = audioParam4Y->convertTo0to1(newVal);
    repaint();
}

/* ----- ENVELOPE CONTROLS ----- */

EnvelopeControls::EnvelopeControls(juce::RangedAudioParameter* param1Y,
                                 juce::RangedAudioParameter* param2X,
                                 juce::RangedAudioParameter* param2Y,
                                 juce::RangedAudioParameter* param3X,
                                 juce::RangedAudioParameter* param3Y,
                                 juce::RangedAudioParameter* param4X,
                                 juce::RangedAudioParameter* param4Y,
                                 juce::RangedAudioParameter* paramSlope1,
                                 juce::RangedAudioParameter* paramSlope2,
                                 juce::RangedAudioParameter* paramSlope3,
                                 juce::UndoManager* um)

: audioParam1Y(param1Y)
, audioParam2X(param2X)
, audioParam2Y(param2Y)
, audioParam3X(param3X)
, audioParam3Y(param3Y)
, audioParam4X(param4X)
, audioParam4Y(param4Y)
, audioParamSlope1(paramSlope1)
, audioParamSlope2(paramSlope2)
, audioParamSlope3(paramSlope3)
, paramAttachment1Y(*param1Y, [&] (float val) {update1Y(val);}, um)
, paramAttachment2X(*param2X, [&] (float val) {update2X(val);}, um)
, paramAttachment2Y(*param2Y, [&] (float val) {update2Y(val);}, um)
, paramAttachment3X(*param3X, [&] (float val) {update3X(val);}, um)
, paramAttachment3Y(*param3Y, [&] (float val) {update3Y(val);}, um)
, paramAttachment4X(*param4X, [&] (float val) {update4X(val);}, um)
, paramAttachment4Y(audioParam4Y ? std::unique_ptr<juce::ParameterAttachment> (new juce::ParameterAttachment(*audioParam4Y, [&] (float val) {update4Y(val);} , um)) : nullptr)
, paramAttachmentSlope1(*paramSlope1, [&] (float val) {updateSlope1(val);}, um)
, paramAttachmentSlope2(*paramSlope2, [&] (float val) {updateSlope2(val);}, um)
, paramAttachmentSlope3(*paramSlope3, [&] (float val) {updateSlope3(val);}, um)
{
    nodes.add(new EnvelopeNode (nullptr, audioParam1Y, nullptr, &paramAttachment1Y, false, 0));
    nodes.add(new EnvelopeNode (audioParam2X, audioParam2Y, &paramAttachment2X, &paramAttachment2Y, false, 1));
    nodes.add(new EnvelopeNode (audioParam3X, audioParam3Y, &paramAttachment3X, &paramAttachment3Y, false, 2));
    nodes.add(new EnvelopeNode (audioParam4X, audioParam4Y, &paramAttachment4X, paramAttachment4Y.get(), false, 3));
    
    slopes.add(new EnvelopeNode (nullptr, audioParamSlope1, nullptr, &paramAttachmentSlope1, true, 0));
    slopes.add(new EnvelopeNode (nullptr, audioParamSlope2, nullptr, &paramAttachmentSlope2, true, 1));
    slopes.add(new EnvelopeNode (nullptr, audioParamSlope3, nullptr, &paramAttachmentSlope3, true, 2));
    
    for (auto& slope: slopes)
        slope->posNorm.y = getMidpointYFromSlope(slope->audioParamY->convertFrom0to1(slope->audioParamY->getValue()));
    
    paramAttachment1Y.sendInitialUpdate();
    paramAttachment2X.sendInitialUpdate();
    paramAttachment2Y.sendInitialUpdate();
    paramAttachment3X.sendInitialUpdate();
    paramAttachment3Y.sendInitialUpdate();
    paramAttachment4X.sendInitialUpdate();
    if (paramAttachment4Y)
        paramAttachment4Y->sendInitialUpdate();
    paramAttachmentSlope1.sendInitialUpdate();
    paramAttachmentSlope2.sendInitialUpdate();
    paramAttachmentSlope3.sendInitialUpdate();
}

void EnvelopeControls::mouseMove(const juce::MouseEvent &event)
{
    const float minDistance = selectClosestNode(event.position);
    
    if (minDistance > MyWidths::highlightAreaRadius)
    {
        currentNode = nullptr;
    }
    else
    {
        highlightArea.setCentre(currentNode->pos);
    }
    
    repaint();
}

void EnvelopeControls::mouseDown(const juce::MouseEvent &event)
{
    const float minDistance = selectClosestNode(event.position);
    
    if (minDistance > MyWidths::highlightAreaRadius)
    {
        currentNode = nullptr;
    }
    else
    {
        nodeClicked = true;
        if (currentNode->paramAttachmentX)
            currentNode->paramAttachmentX->beginGesture();
        if (currentNode->paramAttachmentY)
            currentNode->paramAttachmentY->beginGesture();
        
        currentNode->lastDragPos = event.position;
    }
    
    repaint();
}

void EnvelopeControls::mouseDrag(const juce::MouseEvent &event)
{
    if (!currentNode)
        return;
    
    if (!currentNode->isSlope)
    {
        if (currentNode->paramAttachmentX)
        {
            const float normValueUnlimitedX = currentNode->posNorm.x + (event.position.x - currentNode->lastDragPos.x) / timeParameterWidth;
            const float normValueX = juce::jlimit(0.0f, 1.0f, normValueUnlimitedX);
            currentNode->posNorm.x = normValueX;
            const float denormValueX = currentNode->audioParamX->convertFrom0to1(normValueX);
            currentNode->paramAttachmentX->setValueAsPartOfGesture(denormValueX);
        }
        if (currentNode->paramAttachmentY)
        {
            const float normValueUnlimitedY = currentNode->posNorm.y + (currentNode->lastDragPos.y - event.position.y) / mainArea.getHeight();
            const float normValueY = juce::jlimit(0.0f, 1.0f, normValueUnlimitedY);
            currentNode->posNorm.y = normValueY;
            const float denormValueY = currentNode->audioParamY->convertFrom0to1(normValueY);
            currentNode->paramAttachmentY->setValueAsPartOfGesture(denormValueY);
        }
    }
    else
    {
        const float segmentHeight = nodes[currentNode->index + 1]->posNorm.y - nodes[currentNode->index]->posNorm.y;
        
        const float normValueUnlimitedY = currentNode->posNorm.y + (currentNode->lastDragPos.y - event.position.y) / (segmentHeight * mainArea.getHeight());
        const float normValueY = juce::jlimit(0.0f, 1.0f, normValueUnlimitedY);
        currentNode->posNorm.y = normValueY;
        currentNode->paramAttachmentY->setValueAsPartOfGesture(getSlopeFromMidpointY(normValueY));
    }
    
    currentNode->lastDragPos = event.position;
}

void EnvelopeControls::mouseUp(const juce::MouseEvent &event)
{
    if (!currentNode)
        return;
    
    if (!currentNode->isSlope)
    {
        if (currentNode->paramAttachmentX)
            currentNode->paramAttachmentX->endGesture();
        if (currentNode->paramAttachmentY)
            currentNode->paramAttachmentY->endGesture();
    }
    else
    {
        currentNode->paramAttachmentY->endGesture();
    }
    
    currentNode = nullptr;
    nodeClicked = false;
    
    repaint();
}

void EnvelopeControls::mouseDoubleClick (const juce::MouseEvent& event)
{
    const float minDistance = selectClosestNode(event.position);
    
    if (minDistance > MyWidths::highlightAreaRadius)
    {
        currentNode = nullptr;
    }
    else
    {
        if (currentNode->isSlope)
        {
            currentNode->paramAttachmentY->setValueAsCompleteGesture(0.0f);
        }
    }
}

void EnvelopeControls::mouseExit (const juce::MouseEvent& event)
{
    currentNode = nullptr;
    nodeClicked = false;
    repaint();
}

void EnvelopeControls::textSliderMouseoverCallback (const int nodeNum)
{
    if (nodeNum == -1)
    {
        currentNode = nullptr;
    }
    else
    {
        currentNode = nodes[nodeNum];
        highlightArea.setCentre(currentNode->pos);
    }
    
    repaint();
}

void EnvelopeControls::paint (juce::Graphics& g)
{
    if (currentNode)
    {
        g.setColour(MyColors::overlay);
        g.fillEllipse(highlightArea);
    }
    
    for (auto& node: nodes)
    {
        if (node == currentNode)
        {
            g.setColour(MyColors::lightestSecondary);
            g.drawEllipse(node->pos.x - MyWidths::largeNodeRadius,
                          node->pos.y - MyWidths::largeNodeRadius,
                          MyWidths::largeNodeRadius * 2.0f,
                          MyWidths::largeNodeRadius * 2.0f,
                          nodeClicked ? MyWidths::thickWidth : MyWidths::standardWidth);
        }
        else
        {
            g.setColour(MyColors::lightPrimary);
            g.drawEllipse(node->pos.x - MyWidths::largeNodeRadius,
                          node->pos.y - MyWidths::largeNodeRadius,
                          MyWidths::largeNodeRadius * 2.0f,
                          MyWidths::largeNodeRadius * 2.0f,
                          MyWidths::standardWidth);
        }
    }
    
    for (auto& slope: slopes)
    {
        if (slope == currentNode)
        {
            g.setColour(MyColors::lightestSecondary);
            g.drawEllipse(slope->pos.x - MyWidths::smallNodeRadius,
                          slope->pos.y - MyWidths::smallNodeRadius,
                          MyWidths::smallNodeRadius * 2.0f,
                          MyWidths::smallNodeRadius * 2.0f,
                          nodeClicked ? MyWidths::thickWidth : MyWidths::standardWidth);
        }
        else
        {
            g.setColour(MyColors::lightPrimary);
            g.drawEllipse(slope->pos.x - MyWidths::smallNodeRadius,
                          slope->pos.y - MyWidths::smallNodeRadius,
                          MyWidths::smallNodeRadius * 2.0f,
                          MyWidths::smallNodeRadius * 2.0f,
                          MyWidths::standardWidth);
        }
    }
}

void EnvelopeControls::resized()
{
    mainArea = getLocalBounds().reduced(7);
    timeParameterWidth = mainArea.getWidth() * 0.3f;
    positionNodes();
}

void EnvelopeControls::positionNodes()
{
    for (int i = 0; i < nodes.size(); i++)
    {
        float totalXNorm = 0;
        for (int j = 1; j <= i; j++)
            totalXNorm += nodes[j]->posNorm.x;
        
        nodes[i]->pos.x = mainArea.getX() + totalXNorm * timeParameterWidth;
        nodes[i]->pos.y = mainArea.getBottom() - nodes[i]->posNorm.y * mainArea.getHeight();
    }
    
    for (int i = 0; i < slopes.size(); i++)
    {
        float totalXNorm = 0;
        for (int j = 1; j <= i; j++)
            totalXNorm += nodes[j]->posNorm.x;
        totalXNorm += nodes[i + 1]->posNorm.x / 2.0f;
        
        slopes[i]->pos.x = mainArea.getX() + totalXNorm * timeParameterWidth;
        slopes[i]->pos.y = mainArea.getBottom() - mainArea.getHeight() * (nodes[i]->posNorm.y + slopes[i]->posNorm.y * (nodes[i + 1]->posNorm.y - nodes[i]->posNorm.y));
    }
    
    if (currentNode)
        highlightArea.setCentre(currentNode->pos);
    
    repaint();
}

float EnvelopeControls::selectClosestNode (const juce::Point<float> eventPos)
{
    float minDistance = getWidth();
    
    for (auto& node: nodes)
    {
        const float distance = std::hypot(node->pos.x - eventPos.x, node->pos.y - eventPos.y);
        if (distance < minDistance)
        {
            minDistance = distance;
            currentNode = node;
        }
    }
    
    for (auto& slope: slopes)
    {
        const float distance = std::hypot(slope->pos.x - eventPos.x, slope->pos.y - eventPos.y);
        if (distance < minDistance)
        {
            minDistance = distance;
            currentNode = slope;
        }
    }
    
    return minDistance;
}

void EnvelopeControls::update1Y(float newVal)
{
    nodes[0]->posNorm.y = (*audioParam1Y).convertTo0to1(newVal);
    positionNodes();
}

void EnvelopeControls::update2X(float newVal)
{
    nodes[1]->posNorm.x = (*audioParam2X).convertTo0to1(newVal);
    positionNodes();
}

void EnvelopeControls::update2Y(float newVal)
{
    nodes[1]->posNorm.y = (*audioParam2Y).convertTo0to1(newVal);
    positionNodes();
}

void EnvelopeControls::update3X(float newVal)
{
    nodes[2]->posNorm.x = (*audioParam3X).convertTo0to1(newVal);
    positionNodes();
}

void EnvelopeControls::update3Y(float newVal)
{
    nodes[2]->posNorm.y = (*audioParam3Y).convertTo0to1(newVal);
    positionNodes();
}

void EnvelopeControls::update4X(float newVal)
{
    nodes[3]->posNorm.x = (*audioParam4X).convertTo0to1(newVal);
    positionNodes();
}

void EnvelopeControls::update4Y(float newVal)
{
    nodes[3]->posNorm.y = (*audioParam4Y).convertTo0to1(newVal);
    positionNodes();
}

void EnvelopeControls::updateSlope1(float newSlope)
{
    slopes[0]->posNorm.y = getMidpointYFromSlope(newSlope);
    positionNodes();
}

void EnvelopeControls::updateSlope2(float newSlope)
{
    slopes[1]->posNorm.y = getMidpointYFromSlope(newSlope);
    positionNodes();
}

void EnvelopeControls::updateSlope3(float newSlope)
{
    slopes[2]->posNorm.y = getMidpointYFromSlope(newSlope);
    positionNodes();
}

float EnvelopeControls::getMidpointYFromSlope(const float slope)
{
    return slope == 0.0f ? 0.5f : (expf(slope / 2.0f) - 1.0f) / (expf(slope) - 1.0f);
}

float EnvelopeControls::getSlopeFromMidpointY(const float y)
{
    if (y == 0.5f)
        return 0.0f;
    else
        return logf((y * y - 2 * y + 1) / (y * y));
}

/* ----- ENVELOPE VIEW ----- */

EnvelopeView::EnvelopeView(juce::RangedAudioParameter* param1Y,
                           juce::RangedAudioParameter* param2X,
                           juce::RangedAudioParameter* param2Y,
                           juce::RangedAudioParameter* param3X,
                           juce::RangedAudioParameter* param3Y,
                           juce::RangedAudioParameter* param4X,
                           juce::RangedAudioParameter* param4Y,
                           juce::RangedAudioParameter* paramSlope1,
                           juce::RangedAudioParameter* paramSlope2,
                           juce::RangedAudioParameter* paramSlope3,
                           juce::OwnedArray<juce::dsp::LookupTableTransform<float>>& tables,
                           juce::UndoManager* um)
: envelopeGraph(param1Y, param2X, param2Y, param3X, param3Y, param4X, param4Y, tables, um)
, envelopeControls(param1Y, param2X, param2Y, param3X, param3Y, param4X, param4Y, paramSlope1, paramSlope2, paramSlope3, um)
, attackSlider(*param2X, "Attack")
, decaySlider(*param3X, "Decay")
, releaseSlider(*param4X, "Release")
, initialSlider(*param1Y, "Initial")
, peakSlider(*param2Y, "Peak")
, sustainSlider(*param3Y, "Sustain")
, endSlider(param4Y ? std::unique_ptr<TextSlider> (new TextSlider(*param4Y, "End")) : nullptr)
{
    setOpaque(true);
    
    addAndMakeVisible(envelopeGraph);
    addAndMakeVisible(envelopeControls);
    
    addAndMakeVisible(initialSlider);
    addAndMakeVisible(peakSlider);
    addAndMakeVisible(sustainSlider);
    addAndMakeVisible(attackSlider);
    addAndMakeVisible(decaySlider);
    addAndMakeVisible(releaseSlider);
    if (endSlider)
        addAndMakeVisible(*endSlider);
    
    for (auto* child: getChildren())
        child->addMouseListener(this, true);
    
    attackSlider.setComponentID(ComponentIDs::attack);
    decaySlider.setComponentID(ComponentIDs::decay);
    releaseSlider.setComponentID(ComponentIDs::release);
    initialSlider.setComponentID(ComponentIDs::initial);
    peakSlider.setComponentID(ComponentIDs::peak);
    sustainSlider.setComponentID(ComponentIDs::sustain);
    if (endSlider)
        endSlider->setComponentID(ComponentIDs::end);
}

EnvelopeView::~EnvelopeView()
{
    for (auto* child: getChildren())
        child->removeMouseListener(this);
}

void EnvelopeView::mouseEnter (const juce::MouseEvent& event)
{
    auto* component = event.eventComponent->getParentComponent();
    
    if (component == &initialSlider)
        envelopeControls.textSliderMouseoverCallback(0);
    
    else if (component == &peakSlider || component == &attackSlider)
        envelopeControls.textSliderMouseoverCallback(1);
    
    else if (component == &sustainSlider || component == &decaySlider)
        envelopeControls.textSliderMouseoverCallback(2);
    
    else if ((endSlider && component == endSlider.get()) || component == &releaseSlider)
        envelopeControls.textSliderMouseoverCallback(3);
}

void EnvelopeView::mouseExit (const juce::MouseEvent& event)
{
//    auto* component = event.eventComponent->getParentComponent();
    
    // TODO can maybe delete conditional
//    if (component == &initialSlider ||
//        component == &peakSlider ||
//        component == &attackSlider ||
//        component == &sustainSlider ||
//        component == &decaySlider ||
//        (endSlider && component == endSlider.get()) ||
//        component == &releaseSlider)
        envelopeControls.textSliderMouseoverCallback(-1);
}

void EnvelopeView::paint(juce::Graphics & g)
{
    g.fillAll(MyColors::background);
    
    g.setColour(MyColors::viewBackground);
    g.fillRect(viewBounds);
    
    juce::Path gridLines;
    
    for (int i = 0; i < numXGridLines; i++)
    {
        const float x = mainArea.getX() + (static_cast<float>(i) / (numXGridLines - 1)) * mainArea.getWidth();
        
        gridLines.startNewSubPath(x, mainArea.getY());
        gridLines.lineTo         (x, mainArea.getBottom());
    }
    
    for (int i = 0; i < numYGridLines; i++)
    {
        const float y = mainArea.getY() + (static_cast<float>(i) / (numYGridLines - 1)) * mainArea.getHeight();
        
        gridLines.startNewSubPath(mainArea.getX(), y);
        gridLines.lineTo         (mainArea.getRight(), y);
    }
    
    g.setColour(MyColors::darkPrimary);
    g.strokePath(gridLines, juce::PathStrokeType(MyWidths::standardWidth));
    
    GUIHelper::drawRectangleWithThickness(g, viewBounds.toFloat(), MyWidths::thinWidth);
    
    if (! endSlider)
    {
        g.setColour(MyColors::darkNeutral);
        GUIHelper::paintXBox(g, endSliderArea);
    }
}

void EnvelopeView::resized()
{
    juce::Rectangle<int> bounds = getLocalBounds();
    
    viewBounds = bounds.removeFromTop(120);
    mainArea = viewBounds.reduced(7);
    
    envelopeGraph.setBounds(mainArea);
    envelopeControls.setBounds(viewBounds);
    
    bounds.removeFromTop(5);
    
    const int sliderWidth = bounds.getWidth() / 4;
    const int sliderHeight = MyWidths::standardTextSliderBounds.getHeight();
    
    auto topRow = bounds.removeFromTop(sliderHeight);
    emptySliderArea = topRow.removeFromLeft(sliderWidth);
    attackSlider.setBounds(topRow.removeFromLeft(sliderWidth));
    decaySlider.setBounds(topRow.removeFromLeft(sliderWidth));
    releaseSlider.setBounds(topRow.removeFromLeft(sliderWidth));
    bounds.removeFromTop(5);
    
    auto bottomRow = bounds.removeFromTop(sliderHeight);
    initialSlider.setBounds(bottomRow.removeFromLeft(sliderWidth));
    peakSlider.setBounds(bottomRow.removeFromLeft(sliderWidth));
    sustainSlider.setBounds(bottomRow.removeFromLeft(sliderWidth));
    if (endSlider)
        endSlider->setBounds(bottomRow.removeFromLeft(sliderWidth));
    else
        endSliderArea = bottomRow.removeFromLeft(sliderWidth);
}

juce::Rectangle<int> EnvelopeView::getEnvModeButtonBounds()
{
    const juce::Rectangle<int> bounds = MyWidths::buttonBounds.withCentre(emptySliderArea.getCentre());
    return bounds.withX(bounds.getX() + getPosition().x).withY(bounds.getY() + getPosition().y);
}
