/*
  ==============================================================================

    WaveshaperModule.cpp
    Created: 10 May 2024 12:19:28pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "WaveshaperModule.h"

/* ----- WAVESHAPER RADAR ----- */

WaveshaperRadar::WaveshaperRadar()
{
    
}

void WaveshaperRadar::paint(juce::Graphics &g)
{
    g.setColour(MyColors::viewBackground);
    g.fillRect(mainArea);
    
    g.setColour(MyColors::darkPrimary);
    
    float lineRadius = lineInterval;
    
    juce::Path radarCircles;
    
    while (lineRadius < mainArea.getWidth() * 0.8f)
    {
        radarCircles.addEllipse(center.x - lineRadius, center.y - lineRadius, lineRadius * 2.0f, lineRadius * 2.0f);
        lineRadius += lineInterval;
    }
    
    radarCircles.startNewSubPath(mainArea.getX(), center.y);
    radarCircles.lineTo(mainArea.getRight(), center.y);
    radarCircles.startNewSubPath(center.x, mainArea.getY());
    radarCircles.lineTo(center.x, mainArea.getBottom());
    
    g.strokePath(radarCircles, juce::PathStrokeType{MyWidths::standardWidth});
    
    GUIHelper::drawRectangleWithThickness(g, mainArea, MyWidths::standardWidth);
}

void WaveshaperRadar::resized()
{
    mainArea = getLocalBounds().toFloat();
    center = mainArea.getCentre();
}

/* ----- WAVESHAPER GRAPH ----- */

WaveshaperGraph::WaveshaperGraph(juce::RangedAudioParameter& t_xTransformParam,
                                 juce::RangedAudioParameter& t_yTransformParam,
                                 juce::ValueTree t_nodeTree,
                                 juce::ValueTree t_toolTree,
                                 juce::dsp::LookupTableTransform<float>& t_controlTable,
                                 juce::dsp::LookupTableTransform<float>& t_transformTable)
: xTransformParam(t_xTransformParam)
, yTransformParam(t_yTransformParam)
, xAttachment(xTransformParam, [&] (float i) { repaint(); }, nullptr)
, yAttachment(yTransformParam, [&] (float i) { repaint(); }, nullptr)
, nodeTree(t_nodeTree)
, toolTree(t_toolTree)
, controlTable(t_controlTable)
, transformTable(t_transformTable)
{
    nodeTree.addListener(this);
    toolTree.addListener(this);
}

WaveshaperGraph::~WaveshaperGraph()
{
    nodeTree.removeListener(this);
    toolTree.removeListener(this);
}

void WaveshaperGraph::paint (juce::Graphics& g)
{
    juce::Path funcGraph;
    funcGraph.startNewSubPath(mainArea.getX(), center.y - quadHeight * transformTable.processSample(-1.0f));
    
    for (float j = -1.0f; j < 1.0f; j+= 0.01f)
    {
        funcGraph.lineTo(center.x + quadWidth * j, center.y - quadHeight * transformTable.processSample(j));
    }
    
    g.setColour(MyColors::lightSecondary);
    g.strokePath(funcGraph, juce::PathStrokeType{MyWidths::standardWidth, juce::PathStrokeType::curved});
    
    funcGraph.clear();
    
    funcGraph.startNewSubPath(mainArea.getX(), center.y - quadHeight * controlTable.processSample(-1.0f));
    
    for (float j = -1.0f; j < 1.0f; j+= 0.01f)
    {
        funcGraph.lineTo(center.x + quadWidth * j, center.y - quadHeight * controlTable.processSample(j));
    }
    
    g.setColour(MyColors::white);
    g.strokePath(funcGraph, juce::PathStrokeType{MyWidths::standardWidth, juce::PathStrokeType::curved});
}

void WaveshaperGraph::resized()
{
    mainArea = getLocalBounds().toFloat();
    
    center = mainArea.getCentre();
    quadWidth = mainArea.getWidth() / 2.0f;
    quadHeight = mainArea.getHeight() / 2.0f;
}

void WaveshaperGraph::valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property)
{
    repaint();
}

/* ----- WAVESHAPER CONTROLS -----*/

// TODO TODO TODO unfortunately you need to consider that deleting valuetree nodes instead of moving them around and enabling/disabling them makes a lot more sense.

WaveshaperControls::WaveshaperControls(juce::ValueTree t_nodeTree)
: nodeTree(t_nodeTree), currentNode()
{
    nodeTree.addListener(this);
    
    // initialize node and curve arrays
    for (int i = 0; i < nodeTree.getNumChildren(); i++)
    {
        nodeArray.add(new WaveshaperNode(nodeTree.getChild(i)));
        slopeArray.add(new WaveshaperNode(nodeTree.getChild(i)));
        slopeArray[i]->slopeY = getMidpointYFromSlope(nodeTree.getChild(i)[ParamIDs::nodeSlope]);
    }
}

WaveshaperControls::~WaveshaperControls()
{
    nodeTree.removeListener(this);
}

void WaveshaperControls::mouseMove (const juce::MouseEvent& event)
{
    const float minDistance = selectClosestNode(event.position);
    
    if (minDistance > MyWidths::highlightAreaRadius)
    {
        currentNode = nullptr;
        showHighlight = false;
    }
    else
    {
        highlightArea.setCentre(currentNode->pos);
        showHighlight = true;
    }
    
    repaint();
}

void WaveshaperControls::mouseDown (const juce::MouseEvent& event)
{
    const float minDistance = selectClosestNode(event.position);
    
    if (minDistance > MyWidths::highlightAreaRadius)
    {
        currentNode = nullptr;
    }
    else
    {
        nodeClicked = true;
    }
    
    repaint();
}

void WaveshaperControls::mouseDrag (const juce::MouseEvent& event)
{
    if (!currentNode)
        return;
    
    const int thisIndex = nodeTree.indexOf(currentNode->nodeTree);
    
    if (currentNode->slopeY < 0.0f)
    {
        const float valueUnlimitedX = (event.position.x - center.x) / quadWidth;
        const float valueUnlimitedY = (center.y - event.position.y) / quadHeight;
        
        float lowerLimitX = 0.0f;
        float upperLimitX = 1.0f;
        int upperIndex = thisIndex + 1;
        
        if (thisIndex > 1)
        {
            lowerLimitX = nodeTree.getChild(thisIndex - 1)[ParamIDs::nodeX];
        }
        
        // get next valid and enabled node
        while (nodeTree.getChild(upperIndex).isValid() && !nodeTree.getChild(upperIndex)[ParamIDs::nodeEnabled])
            upperIndex++;
        
        if (nodeTree.getChild(upperIndex).isValid())
        {
            upperLimitX = nodeTree.getChild(thisIndex + 1)[ParamIDs::nodeX];
        }
        
        const float valueX = juce::jlimit(lowerLimitX, upperLimitX, valueUnlimitedX);
        const float valueY = juce::jlimit(-1.0f, 1.0f, valueUnlimitedY);
        
        currentNode->nodeTree.setProperty(ParamIDs::nodeX, valueX, nullptr);
        currentNode->nodeTree.setProperty(ParamIDs::nodeY, valueY, nullptr);
    }
    else
    {
        float leftLimitY = nodeTree.getChild(thisIndex)[ParamIDs::nodeY];
        float rightLimitY = 1.0f;
        int rightIndex = thisIndex + 1;
        
        // get next valid and enabled node
        // TODO maybe delete because no disabled nodes are between enabled nodes
        while (nodeTree.getChild(rightIndex).isValid() && !nodeTree.getChild(rightIndex)[ParamIDs::nodeEnabled])
            rightIndex++;
        
        if (nodeTree.getChild(rightIndex).isValid())
        {
            rightLimitY = nodeTree.getChild(thisIndex + 1)[ParamIDs::nodeY];
        }
        
        if (leftLimitY == rightLimitY)
            return;
        
        float valueUnlimitedY = ((center.y - leftLimitY * quadHeight) - event.position.y) / ((rightLimitY - leftLimitY) * quadHeight);
        
        const float valueY = juce::jlimit(0.0f, 1.0f, valueUnlimitedY);
        currentNode->slopeY = valueY;
        currentNode->nodeTree.setProperty(ParamIDs::nodeSlope, getSlopeFromMidpointY(valueY), nullptr);
    }
    
    highlightArea.setCentre(currentNode->pos);
}

void WaveshaperControls::mouseUp (const juce::MouseEvent& event)
{
    currentNode = nullptr;
    nodeClicked = false;
    showHighlight = false;
    
    repaint();
}

void WaveshaperControls::mouseDoubleClick (const juce::MouseEvent& event)
{
    const float minDistance = selectClosestNode(event.position);
    
    // if clicked on radar view, try to enable an available node
    if (minDistance > MyWidths::largeNodeRadius)
    {
        // initialize
        bool nodeAvailable = false;
        bool indexFound = false;
        const int treeEndIndex = nodeTree.getNumChildren() - 1;
        int newNodeIndex = 1;
        float newNodeX = (event.position.x - center.x) / quadWidth;
        float newNodeY = (center.y - event.position.y) / quadHeight;
        
        // check if out of bounds
        if (newNodeX < 0.0f || newNodeX > 1.0f || newNodeY > 1.0f || newNodeY < -1.0f)
            return;
        
        for (auto& node: nodeArray)
        {
            if (node->nodeTree[ParamIDs::nodeEnabled])
            {
                // case if clicked between enabled nodes
                if (node->nodeTree.hasProperty(ParamIDs::nodeX) &&
                    !indexFound &&
                    static_cast<float>(node->nodeTree[ParamIDs::nodeX]) > newNodeX)
                {
                    newNodeIndex = nodeTree.indexOf(node->nodeTree);
                    indexFound = true;
                }
            }
            else
            {
                nodeAvailable = true;
                if (!indexFound)
                {
                    newNodeIndex = nodeTree.indexOf(node->nodeTree);
                    indexFound = true;
                }
            }
        }
        
        if (!nodeAvailable)
            return;
        
        nodeTree.moveChild(treeEndIndex, newNodeIndex, nullptr);
        nodeTree.getChild(newNodeIndex).setProperty(ParamIDs::nodeEnabled, true, nullptr);
        nodeTree.getChild(newNodeIndex).setProperty(ParamIDs::nodeX, newNodeX, nullptr);
        nodeTree.getChild(newNodeIndex).setProperty(ParamIDs::nodeY, newNodeY, nullptr);
    }
    
    // if clicked on existing node, disable it
    else if (currentNode->slopeY < 0.0f)
    {
        const int nodeIndex = nodeTree.indexOf(currentNode->nodeTree);
        nodeTree.moveChild(nodeIndex, -1, nullptr);
        currentNode->nodeTree.setProperty(ParamIDs::nodeEnabled, false, nullptr);
    }
    
    // if clicked on existing slope, reset it to a straight line
    else
    {
        currentNode->slopeY = 0.5f;
        currentNode->nodeTree.setProperty(ParamIDs::nodeSlope, 0.0f, nullptr);
    }
}

void WaveshaperControls::mouseExit (const juce::MouseEvent& event)
{
    showHighlight = false;
    nodeClicked = false;
    currentNode = nullptr;
    repaint();
}

void WaveshaperControls::paint (juce::Graphics& g)
{
    if (showHighlight)
    {
        g.setColour(MyColors::overlay);
        g.fillEllipse(highlightArea);
    }
    
    for (int i = 1; i < nodeArray.size(); i++)
    {
        if (nodeArray[i]->nodeTree.getProperty(ParamIDs::nodeEnabled))
        {
            if (nodeArray[i] == currentNode)
            {
                g.setColour(MyColors::lightestSecondary);
                g.drawEllipse(nodeArray[i]->pos.x - MyWidths::largeNodeRadius,
                              nodeArray[i]->pos.y - MyWidths::largeNodeRadius,
                              MyWidths::largeNodeRadius * 2.0f,
                              MyWidths::largeNodeRadius * 2.0f,
                              nodeClicked ? MyWidths::thickWidth : MyWidths::standardWidth);
            }
            else
            {
                g.setColour(MyColors::lightPrimary);
                g.drawEllipse(nodeArray[i]->pos.x - MyWidths::largeNodeRadius,
                              nodeArray[i]->pos.y - MyWidths::largeNodeRadius,
                              MyWidths::largeNodeRadius * 2.0f,
                              MyWidths::largeNodeRadius * 2.0f,
                              MyWidths::standardWidth);
            }
        }
    }
    
    for (int i = 0; i < slopeArray.size(); i++)
    {
        if (slopeArray[i]->nodeTree.getProperty(ParamIDs::nodeEnabled))
        {
            if (slopeArray[i] == currentNode)
            {
                g.setColour(MyColors::lightestSecondary);
                g.drawEllipse(slopeArray[i]->pos.x - MyWidths::smallNodeRadius,
                              slopeArray[i]->pos.y - MyWidths::smallNodeRadius,
                              MyWidths::smallNodeRadius * 2.0f,
                              MyWidths::smallNodeRadius * 2.0f,
                              nodeClicked ? MyWidths::thickWidth : MyWidths::standardWidth);
            }
            else
            {
                g.setColour(MyColors::lightPrimary);
                g.drawEllipse(slopeArray[i]->pos.x - MyWidths::smallNodeRadius,
                              slopeArray[i]->pos.y - MyWidths::smallNodeRadius,
                              MyWidths::smallNodeRadius * 2.0f,
                              MyWidths::smallNodeRadius * 2.0f,
                              MyWidths::standardWidth);
            }
        }
    }
}

void WaveshaperControls::resized()
{
    mainArea = getLocalBounds().toFloat();
    center = mainArea.getCentre();
    quadWidth = mainArea.getWidth() / 2.0f;
    quadHeight = mainArea.getHeight() / 2.0f;
    
    positionNodes();
}

void WaveshaperControls::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
{
    positionNodes(); // TODO be more specific, positionNodes for node changes and repaint for table changes?
}

void WaveshaperControls::valueTreeChildOrderChanged(juce::ValueTree &parentTreeWhoseChildrenHaveMoved, int oldIndex, int newIndex)
{
    nodeArray.move(oldIndex, newIndex);
    slopeArray.move(oldIndex, newIndex);
}

void WaveshaperControls::positionNodes()
{
    for (int i = 1; i < nodeArray.size(); i++)
    {
        if (nodeArray[i]->nodeTree[ParamIDs::nodeEnabled])
        {
            nodeArray[i]->pos.x = center.x + static_cast<float>(nodeArray[i]->nodeTree[ParamIDs::nodeX]) * quadWidth;
            nodeArray[i]->pos.y = center.y - static_cast<float>(nodeArray[i]->nodeTree[ParamIDs::nodeY]) * quadHeight;
        }
    }
          
    for (int i = 0; i < slopeArray.size(); i++)
    {
        if (slopeArray[i]->nodeTree[ParamIDs::nodeEnabled])
        {
            float segmentWidth, segmentHeight;

            const float segmentOffsetX = static_cast<float>(slopeArray[i]->nodeTree.getProperty(ParamIDs::nodeX, 0.0f));
            const float segmentOffsetY = static_cast<float>(slopeArray[i]->nodeTree.getProperty(ParamIDs::nodeY, 0.0f));
            
            // if i is last enabled index
            if (i + 1 == nodeArray.size() || !nodeArray[i + 1]->nodeTree[ParamIDs::nodeEnabled])
            {
                segmentWidth  = 1.0f - static_cast<float>(slopeArray[i]->nodeTree.getProperty(ParamIDs::nodeX, 0.0f));
                segmentHeight = 1.0f - static_cast<float>(slopeArray[i]->nodeTree.getProperty(ParamIDs::nodeY, 0.0f));
            }
            else
            {
                segmentWidth  = static_cast<float>(slopeArray[i + 1]->nodeTree[ParamIDs::nodeX]) - static_cast<float>(slopeArray[i]->nodeTree.getProperty(ParamIDs::nodeX, 0.0f));
                segmentHeight = static_cast<float>(slopeArray[i + 1]->nodeTree[ParamIDs::nodeY]) - static_cast<float>(slopeArray[i]->nodeTree.getProperty(ParamIDs::nodeY, 0.0f));
            }
            
            slopeArray[i]->slopeY = getMidpointYFromSlope(slopeArray[i]->nodeTree[ParamIDs::nodeSlope]);
            
            slopeArray[i]->pos.x = center.x + quadWidth * (segmentOffsetX + segmentWidth / 2.0f);
            slopeArray[i]->pos.y = center.y - quadHeight * (segmentOffsetY + segmentHeight * slopeArray[i]->slopeY);
        }
    }
    
    repaint();
}

float WaveshaperControls::selectClosestNode (const juce::Point<float> eventPos)
{
    float minDistance = getWidth();
    
    for (int i = 1; i < nodeArray.size(); i++)
    {
        if (!nodeArray[i]->nodeTree[ParamIDs::nodeEnabled])
            break;
        
        const float distance = std::hypot(nodeArray[i]->pos.x - eventPos.x, nodeArray[i]->pos.y - eventPos.y);
        if (distance < minDistance)
        {
            minDistance = distance;
            currentNode = nodeArray[i];
        }
    }
    
    for (int i = 0; i < slopeArray.size(); i++)
    {
        if (!nodeArray[i]->nodeTree[ParamIDs::nodeEnabled])
            break;
        
        const float distance = std::hypot(slopeArray[i]->pos.x - eventPos.x, slopeArray[i]->pos.y - eventPos.y);
        if (distance < minDistance)
        {
            minDistance = distance;
            currentNode = slopeArray[i];
        }
    }
    
    return minDistance;
}

float WaveshaperControls::getMidpointYFromSlope (const float slope)
{
    return slope == 0.0f ? 0.5f : (expf(slope / 2.0f) - 1.0f) / (expf(slope) - 1.0f);
}

float WaveshaperControls::getSlopeFromMidpointY(const float y)
{
    if (y == 0.5f)
        return 0.0f;
    else
        return juce::jlimit(-10.1f, 10.1f, logf((y * y - 2 * y + 1) / (y * y)));
}

/* ----- WAVESHAPER MODULE ----- */

WaveshaperModule::WaveshaperModule(PluginProcessor& t_processor,
                                   juce::AudioProcessorValueTreeState& t_apvts,
                                   juce::ValueTree nodeTree,
                                   juce::ValueTree toolTree,
                                   juce::ValueTree t_lfoTree,
                                   juce::ValueTree effectsOrderTree)
: EffectsSlot(ParamIDs::waveshaperNode, effectsOrderTree)
, processor(t_processor)
, apvts(t_apvts)
, lfoTree(t_lfoTree)
, header(juce::Drawable::createFromImageData(BinaryData::waveshaper_header_svg, BinaryData::waveshaper_header_svgSize))
, oddIcon(juce::Drawable::createFromImageData(BinaryData::odd_svg, BinaryData::odd_svgSize))
, evenIcon(juce::Drawable::createFromImageData(BinaryData::even_svg, BinaryData::even_svgSize))
, inIcon(juce::Drawable::createFromImageData(BinaryData::in_svg, BinaryData::in_svgSize))
, outIcon(juce::Drawable::createFromImageData(BinaryData::out_svg, BinaryData::out_svgSize))
, xTransformIcon(juce::Drawable::createFromImageData(BinaryData::x_transform_svg, BinaryData::x_transform_svgSize))
, yTransformIcon(juce::Drawable::createFromImageData(BinaryData::y_transform_svg, BinaryData::y_transform_svgSize))
, crosshair(ParamIDs::waveshaperNode.toString())
, radar()
, graph(*apvts.getParameter(ParamIDs::waveshaperXTransform), *apvts.getParameter(ParamIDs::waveshaperYTransform), nodeTree, toolTree, processor.waveshaperControlTable, processor.waveshaperTransformTable)
, controls(nodeTree)
, oddEvenButton(toolTree, ParamIDs::oddEven, evenIcon, oddIcon, ValueTreeToolButton::showColoredBackgrounds | ValueTreeToolButton::fitBorderToBounds)
, inputGainSlider(*apvts.getParameter(ParamIDs::waveshaperInputGain), inIcon, IconSlider::showColoredBackgrounds, lfoTree.getChildWithName(ParamIDs::lfoWaveshaperInputGain))
, xTransformSlider(*apvts.getParameter(ParamIDs::waveshaperXTransform), xTransformIcon, IconSlider::dragHorizontal | IconSlider::showColoredBackgrounds)
, yTransformSlider(*apvts.getParameter(ParamIDs::waveshaperYTransform), yTransformIcon, IconSlider::showColoredBackgrounds)
, outputGainSlider(*apvts.getParameter(ParamIDs::waveshaperOutputGain), outIcon, IconSlider::showColoredBackgrounds, lfoTree.getChildWithName(ParamIDs::lfoWaveshaperOutputGain))
, mixSlider(*apvts.getParameter(ParamIDs::waveshaperMix), "Mix", TextSlider::displayColorRange, lfoTree.getChildWithName(ParamIDs::lfoWaveshaperMix))
{
    setOpaque(true);
    
    addAndMakeVisible(crosshair);
    addAndMakeVisible(radar);
    addAndMakeVisible(graph);
    addAndMakeVisible(controls);
    addAndMakeVisible(oddEvenButton);
    addAndMakeVisible(xTransformSlider);
    addAndMakeVisible(yTransformSlider);
    addAndMakeVisible(inputGainSlider);
    addAndMakeVisible(outputGainSlider);
    addAndMakeVisible(mixSlider);
    
    crosshair.setComponentID(ComponentIDs::effectCrosshair);
    controls.setComponentID(ComponentIDs::waveshaperRadar);
    oddEvenButton.setComponentID(ComponentIDs::waveshaperOddEven);
    xTransformSlider.setComponentID(ComponentIDs::waveshaperXTransform);
    yTransformSlider.setComponentID(ComponentIDs::waveshaperYTransform);
    inputGainSlider.setComponentID(ComponentIDs::waveshaperInputGain);
    outputGainSlider.setComponentID(ComponentIDs::waveshaperOutputGain);
    mixSlider.setComponentID(ComponentIDs::waveshaperMix);
}

void WaveshaperModule::paint (juce::Graphics& g)
{
    g.fillAll(MyColors::background);
    
    header->drawWithin(g, getLocalBounds().toFloat(), juce::RectanglePlacement::xRight, 1.0f);
    
    g.setColour(MyColors::darkPrimary);
    GUIHelper::drawRectangleWithThickness(g, mainArea.toFloat(), MyWidths::standardWidth);
    GUIHelper::drawRectangleWithThickness(g, toolArea.toFloat(), MyWidths::standardWidth);
    
    if (paintDragBorder)
    {
        g.setColour(MyColors::lightSecondary);
        GUIHelper::paintOutline(g, totalArea);
    }
}

void WaveshaperModule::resized()
{
    totalArea = getLocalBounds().withTrimmedLeft(MyWidths::additionalHeaderWidth).withTrimmedTop(MyWidths::additionalHeaderHeight);
    juce::Rectangle<int> bounds = totalArea;
    
    crosshair.setBounds(MyWidths::buttonBounds.withX(4).withY(9));
    mixSlider.setBounds(MyWidths::smallTextSliderBounds.withX(1).withBottomY(getHeight() - 16));
    
    const int margin = MyWidths::standardMarginInt;
    mainArea = bounds.removeFromTop(bounds.getWidth()).reduced(margin);
    toolArea = bounds.removeFromTop(MyWidths::buttonDimension * 2 + margin * 3).reduced(20, 0);
    juce::Rectangle<int> buttonArea = toolArea.reduced(margin);
    juce::Rectangle<int> buttonsTopRow = buttonArea.removeFromTop(MyWidths::buttonDimension);
    juce::Rectangle<int> buttonsBottomRow = buttonArea.removeFromBottom(MyWidths::buttonDimension);
    
    radar.setBounds(mainArea);
    graph.setBounds(mainArea);
    controls.setBounds(mainArea);
    
    const int buttonWidth = (buttonArea.getWidth() - margin * 2) / 3;
    
    inputGainSlider.setBounds(buttonsTopRow.removeFromLeft(buttonWidth));
    buttonsTopRow.removeFromLeft(margin);
    outputGainSlider.setBounds(buttonsTopRow.removeFromLeft(buttonWidth));
    buttonsTopRow.removeFromLeft(margin);
    oddEvenButton.setBounds(buttonsBottomRow.removeFromLeft(buttonWidth));
    buttonsBottomRow.removeFromLeft(margin);
    xTransformSlider.setBounds(buttonsBottomRow.removeFromLeft(buttonWidth));
    buttonsBottomRow.removeFromLeft(margin);
    yTransformSlider.setBounds(buttonsBottomRow.removeFromLeft(buttonWidth));
}
