#include "PluginProcessor.h"
#include "PluginEditor.h"

PluginProcessor::PluginProcessor()

: AudioProcessor (BusesProperties()
                  .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
, midiFifo(16)
, apvts(*this, &undoManager, juce::Identifier("Parameters"),
        ParameterHelper::createParameterLayout())
, nonParamStateTree(ParameterHelper::createNonParameterLayout())
, presetManager(apvts, nonParamStateTree)
, effectsProcessorGraph()
, lfoTree(nonParamStateTree.getChildWithName(ParamIDs::lfoTree))
, lfo1(*apvts.getParameter(ParamIDs::lfo1FrequencyTime),
        *apvts.getParameter(ParamIDs::lfo1FrequencySync),
        *apvts.getParameter(ParamIDs::lfo1Range),
        nonParamStateTree.getChildWithName(ParamIDs::lfoTree).getChildWithName(ParamIDs::lfo1),
        bpm)
, lfo2(*apvts.getParameter(ParamIDs::lfo2FrequencyTime),
        *apvts.getParameter(ParamIDs::lfo2FrequencySync),
        *apvts.getParameter(ParamIDs::lfo2Range),
        nonParamStateTree.getChildWithName(ParamIDs::lfoTree).getChildWithName(ParamIDs::lfo2),
        bpm)
{
    /* ----- INITIALIZE EFFECTS AUDIO GRAPH ----- */
    
    using IOProcessor = juce::AudioProcessorGraph::AudioGraphIOProcessor;
    
    inputNode = effectsProcessorGraph.addNode(std::make_unique<IOProcessor>(IOProcessor::audioInputNode));
    outputNode = effectsProcessorGraph.addNode(std::make_unique<IOProcessor>(IOProcessor::audioOutputNode));
    delayNode = effectsProcessorGraph.addNode(std::make_unique<DelayProcessor>(apvts, nonParamStateTree, lfo1, lfo2, bpm));
    reverbNode = effectsProcessorGraph.addNode(std::make_unique<ReverbProcessor>(apvts, nonParamStateTree, lfo1, lfo2));
    filterNode = effectsProcessorGraph.addNode(std::make_unique<FilterProcessor>(apvts, nonParamStateTree, lfo1, lfo2));
    waveshaperNode = effectsProcessorGraph.addNode(std::make_unique<WaveshaperProcessor>(apvts, nonParamStateTree, lfo1, lfo2, waveshaperControlTable, waveshaperTransformTable));
    
    effectsOrder.add(delayNode);
    effectsOrder.add(reverbNode);
    effectsOrder.add(filterNode);
    effectsOrder.add(waveshaperNode);
    
    effectsOrderTree = nonParamStateTree.getChildWithName(ParamIDs::effectsOrderTree);
    effectsOrderTree.addListener(this);
    
    /* ----- INITIALIZE SYNTH VOICES ----- */
    
    for (int i = 0; i < numOscVoices; i++)
    {
        synth.addVoice(new MyVoice(apvts,
                                    nonParamStateTree,
                                    lfo1,
                                    lfo2,
                                    oscAmpTables,
                                    oscPitchTables,
                                    noiseAmpTables,
                                    noiseFilterTables));
    }
            
    synth.addSound (new MySound());
    
    /* ----- HANDLE ENVELOPE TABLES ----- */
    
    apvts.addParameterListener(ParamIDs::oscSlope1Amp, this);
    apvts.addParameterListener(ParamIDs::oscSlope2Amp, this);
    apvts.addParameterListener(ParamIDs::oscSlope3Amp, this);
    apvts.addParameterListener(ParamIDs::oscSlope1Pitch, this);
    apvts.addParameterListener(ParamIDs::oscSlope2Pitch, this);
    apvts.addParameterListener(ParamIDs::oscSlope3Pitch, this);
    apvts.addParameterListener(ParamIDs::noiseSlope1Amp, this);
    apvts.addParameterListener(ParamIDs::noiseSlope2Amp, this);
    apvts.addParameterListener(ParamIDs::noiseSlope3Amp, this);
    apvts.addParameterListener(ParamIDs::noiseSlope1Cutoff, this);
    apvts.addParameterListener(ParamIDs::noiseSlope2Cutoff, this);
    apvts.addParameterListener(ParamIDs::noiseSlope3Cutoff, this);
    
    oscAmpTables.add(new juce::dsp::LookupTableTransform<float>());
    oscAmpTables.add(new juce::dsp::LookupTableTransform<float>());
    oscAmpTables.add(new juce::dsp::LookupTableTransform<float>());
    updateTableWithSlope(oscAmpTables[0], *apvts.getRawParameterValue(ParamIDs::oscSlope1Amp));
    updateTableWithSlope(oscAmpTables[1], *apvts.getRawParameterValue(ParamIDs::oscSlope2Amp));
    updateTableWithSlope(oscAmpTables[2], *apvts.getRawParameterValue(ParamIDs::oscSlope3Amp));
    
    oscPitchTables.add(new juce::dsp::LookupTableTransform<float>());
    oscPitchTables.add(new juce::dsp::LookupTableTransform<float>());
    oscPitchTables.add(new juce::dsp::LookupTableTransform<float>());
    updateTableWithSlope(oscPitchTables[0], *apvts.getRawParameterValue(ParamIDs::oscSlope1Pitch));
    updateTableWithSlope(oscPitchTables[1], *apvts.getRawParameterValue(ParamIDs::oscSlope2Pitch));
    updateTableWithSlope(oscPitchTables[2], *apvts.getRawParameterValue(ParamIDs::oscSlope3Pitch));
    
    noiseAmpTables.add(new juce::dsp::LookupTableTransform<float>());
    noiseAmpTables.add(new juce::dsp::LookupTableTransform<float>());
    noiseAmpTables.add(new juce::dsp::LookupTableTransform<float>());
    updateTableWithSlope(noiseAmpTables[0], *apvts.getRawParameterValue(ParamIDs::noiseSlope1Amp));
    updateTableWithSlope(noiseAmpTables[1], *apvts.getRawParameterValue(ParamIDs::noiseSlope2Amp));
    updateTableWithSlope(noiseAmpTables[2], *apvts.getRawParameterValue(ParamIDs::noiseSlope3Amp));
    
    noiseFilterTables.add(new juce::dsp::LookupTableTransform<float>());
    noiseFilterTables.add(new juce::dsp::LookupTableTransform<float>());
    noiseFilterTables.add(new juce::dsp::LookupTableTransform<float>());
    updateTableWithSlope(noiseFilterTables[0], *apvts.getRawParameterValue(ParamIDs::noiseSlope1Cutoff));
    updateTableWithSlope(noiseFilterTables[1], *apvts.getRawParameterValue(ParamIDs::noiseSlope2Cutoff));
    updateTableWithSlope(noiseFilterTables[2], *apvts.getRawParameterValue(ParamIDs::noiseSlope3Cutoff));
    
    /* ----- CAST PARAMETERS ----- */
    
    ParameterHelper::castParameter(apvts, ParamIDs::masterGain, masterGainParam);
}

PluginProcessor::~PluginProcessor()
{
    nonParamStateTree.removeListener(this);
}

//==============================================================================
const juce::String PluginProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PluginProcessor::getTailLengthSeconds() const { return 0.0; }
int PluginProcessor::getNumPrograms() { return 1; }
int PluginProcessor::getCurrentProgram() { return 0; }
void PluginProcessor::setCurrentProgram (int index) {}
const juce::String PluginProcessor::getProgramName (int index) { return {}; }
void PluginProcessor::changeProgramName (int index, const juce::String& newName) {}

//==============================================================================
void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);
    
    for (int i = 0; i < synth.getNumVoices(); i++)
    {
        if (auto voice = dynamic_cast<MyVoice*>(synth.getVoice(i)))
        {
            voice->prepareToPlay(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
        }
    }
    
    juce::dsp::ProcessSpec spec;

    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    
    effectsProcessorGraph.setPlayConfigDetails(2, 2, sampleRate, samplesPerBlock);
    effectsProcessorGraph.prepareToPlay(sampleRate, samplesPerBlock);
    
    for (auto node: effectsOrder)
        node->getProcessor()->setPlayConfigDetails(2, 2, sampleRate, samplesPerBlock);
    
    connectEffectsGraph();
    
    lfo1.prepare(spec);
    lfo2.prepare(spec);
    
    masterGain.prepare(spec);
    masterGain.setRampDurationSeconds(0.0005f);
}

void PluginProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PluginProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;\
    
    if (auto bpmFromHost = *getPlayHead()->getPosition()->getBpm())
        bpm.store(bpmFromHost);
    
    buffer.clear(0, buffer.getNumSamples());
    
    /* ----- LFO PROCESSING ----- */
    
    lfo1.processToOutput(buffer.getNumSamples());
    lfo2.processToOutput(buffer.getNumSamples());
    
    /* ----- SYNTH PROCESSING ----- */
    
    juce::MidiMessage message;
    while (midiFifo.pop(message))
        midiMessages.addEvent(message, 0);

    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    midiMessages.clear();
    
    /* ----- EFFECTS GRAPH PROCESSING ----- */
    
    bool expected = true;
    if (shouldUpdateEffectsGraph.compare_exchange_strong(expected, false))
        connectEffectsGraph();
    
    effectsProcessorGraph.processBlock(buffer, midiMessages);
    
    /* ----- APPLY MASTER GAIN ----- */
    
    const float gainVal = LFO::processLFOSample(masterGainParam,
                                                lfoTree.getChildWithName(ParamIDs::lfoMasterGain),
                                                masterGainParam->convertTo0to1(masterGainParam->get()),
                                                lfo1.buffer.getSample(0, 0),
                                                lfo2.buffer.getSample(0, 0));
    masterGain.setGainLinear(juce::Decibels::decibelsToGain(gainVal));
    juce::dsp::AudioBlock<float> block (buffer);
    masterGain.process(juce::dsp::ProcessContextReplacing<float>(block));
    
    /* ----- STORE LEVELS FOR METER ----- */
    
    leftRMS.store(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));
    rightRMS.store(buffer.getRMSLevel(1, 0, buffer.getNumSamples()));
    
    auto leftMinMax = buffer.findMinMax(0, 0, buffer.getNumSamples());
    leftPeak.store(juce::jmax(std::abs(leftMinMax.getStart()), std::abs(leftMinMax.getEnd())));
    auto rightMinMax = buffer.findMinMax(1, 0, buffer.getNumSamples());
    rightPeak.store(juce::jmax(std::abs(rightMinMax.getStart()), std::abs(rightMinMax.getEnd())));
}

float PluginProcessor::getRMSLevel (const int channel)
{
    if (channel == 0)
        return juce::Decibels::gainToDecibels(leftRMS.load());
    if (channel == 1)
        return juce::Decibels::gainToDecibels(rightRMS.load());
    
    jassert(false);
    return 0.0f;
}

float PluginProcessor::getSimplePeak (const int channel)
{
    if (channel == 0)
        return juce::Decibels::gainToDecibels(leftPeak.load());
    if (channel == 1)
        return juce::Decibels::gainToDecibels(rightPeak.load());
    
    jassert(false);
    return 0.0f;
}

void PluginProcessor::parameterChanged (const juce::String& parameterID, float newValue)
{
    if (parameterID == ParamIDs::oscSlope1Amp)
        updateTableWithSlope(oscAmpTables[0], newValue);
    
    else if (parameterID == ParamIDs::oscSlope2Amp)
        updateTableWithSlope(oscAmpTables[1], newValue);
    
    else if (parameterID == ParamIDs::oscSlope3Amp)
        updateTableWithSlope(oscAmpTables[2], newValue);
    
    else if (parameterID == ParamIDs::oscSlope1Pitch)
        updateTableWithSlope(oscPitchTables[0], newValue);
    
    else if (parameterID == ParamIDs::oscSlope2Pitch)
        updateTableWithSlope(oscPitchTables[1], newValue);
    
    else if (parameterID == ParamIDs::oscSlope3Pitch)
        updateTableWithSlope(oscPitchTables[2], newValue);
    
    else if (parameterID == ParamIDs::noiseSlope1Amp)
        updateTableWithSlope(noiseAmpTables[0], newValue);
    
    else if (parameterID == ParamIDs::noiseSlope2Amp)
        updateTableWithSlope(noiseAmpTables[1], newValue);
    
    else if (parameterID == ParamIDs::noiseSlope3Amp)
        updateTableWithSlope(noiseAmpTables[2], newValue);
    
    else if (parameterID == ParamIDs::noiseSlope1Cutoff)
        updateTableWithSlope(noiseFilterTables[0], newValue);
    
    else if (parameterID == ParamIDs::noiseSlope2Cutoff)
        updateTableWithSlope(noiseFilterTables[1], newValue);
    
    else if (parameterID == ParamIDs::noiseSlope3Cutoff)
        updateTableWithSlope(noiseFilterTables[2], newValue);
}

void PluginProcessor::connectEffectsGraph()
{
    for (int i = 0; i < effectsOrderTree.getNumChildren(); i++)
    {
        const juce::Identifier& type = effectsOrderTree.getChild(i).getType();
        
        if (type == ParamIDs::delayNode)
            effectsOrder.set(i, delayNode);

        else if (type == ParamIDs::reverbNode)
            effectsOrder.set(i, reverbNode);
        
        else if (type == ParamIDs::filterNode)
            effectsOrder.set(i, filterNode);
        
        else if (type == ParamIDs::waveshaperNode)
            effectsOrder.set(i, waveshaperNode);
    }
    
    for (auto& connection: effectsProcessorGraph.getConnections())
    {
        effectsProcessorGraph.removeConnection(connection);
    }
    
    for (int channel = 0; channel < 2; channel++)
    {
        effectsProcessorGraph.addConnection({{ inputNode->nodeID, channel }, { effectsOrder.getFirst()->nodeID, channel }});
        
        for (int i = 0; i < effectsOrder.size() - 1; i++)
        {
            effectsProcessorGraph.addConnection({{ effectsOrder[i]->nodeID, channel }, { effectsOrder[i + 1]->nodeID, channel }});
        }
        
        effectsProcessorGraph.addConnection({{ effectsOrder.getLast()->nodeID, channel }, { outputNode->nodeID, channel }});
    }
    
    // TODO handle bypassing, or maybe handle in a different callback
}

void PluginProcessor::updateTableWithSlope (juce::dsp::LookupTableTransform<float>* table, const float newSlope)
{
    (*table).initialise([&] (float i) {
        if (newSlope < -10.0f)
            return 1.0f;
        else if (newSlope > 10.0f)
            return 0.0f;
        else
            return newSlope == 0.0f ? i : (expf(newSlope * i) - 1.0f) / (expf(newSlope) - 1.0f);
    }, 0.0f, 1.0f, 64);
}

void PluginProcessor::valueTreeChildOrderChanged (juce::ValueTree &parentTreeWhoseChildrenHaveMoved, int oldIndex, int newIndex)
{
    shouldUpdateEffectsGraph.store(true);
}

//==============================================================================
bool PluginProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor (*this, undoManager);
}

void PluginProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::XmlElement xmlParent ("parent");

    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xmlAPVTS (state.createXml());
    xmlParent.addChildElement(xmlAPVTS.release());

    std::unique_ptr<juce::XmlElement> xmlNonAPVTS (nonParamStateTree.createXml());
    xmlParent.addChildElement(xmlNonAPVTS.release());
    
    std::unique_ptr<juce::XmlElement> xml = presetManager.getStateXml();
    copyXmlToBinary(*xml, destData);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary(data, sizeInBytes));
    presetManager.loadStateFromXml(std::move(xmlState));
}

juce::AudioProcessorValueTreeState& PluginProcessor::getPluginAPVST() { return apvts; }

juce::ValueTree PluginProcessor::getPluginNonParamTree() { return nonParamStateTree; }

PresetManager& PluginProcessor::getPresetManager() { return presetManager; }

LFOVisualizer& PluginProcessor::getLFOVisualizer (const int lfoNum)
{
    jassert(lfoNum == 0 || lfoNum == 1);
    
    return lfoNum == 0 ? lfo1.visualizer : lfo2.visualizer;
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}
