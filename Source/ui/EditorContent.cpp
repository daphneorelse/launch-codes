/*
  ==============================================================================

    EditorContent.cpp
    Created: 23 Aug 2024 2:57:21pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "EditorContent.h"

EditorContent::EditorContent(PluginProcessor& p, juce::UndoManager& um)
: audioProcessor(p)
, apvts(p.getPluginAPVST())
, nonParamTree(p.getPluginNonParamTree())
, lfoTree(nonParamTree.getChildWithName(ParamIDs::lfoTree))
, effectsOrderTree(nonParamTree.getChildWithName(ParamIDs::effectsOrderTree))
, logoGroup(juce::Drawable::createFromImageData(BinaryData::logo_group_svg, BinaryData::logo_group_svgSize))
, timeIcon(juce::Drawable::createFromImageData(BinaryData::frequencyTime_svg, BinaryData::frequencyTime_svgSize))
, syncIcon(juce::Drawable::createFromImageData(BinaryData::frequencySync_svg, BinaryData::frequencySync_svgSize))
, unlinkedIcon(juce::Drawable::createFromImageData(BinaryData::unlinked_svg, BinaryData::unlinked_svgSize))
, linkedIcon(juce::Drawable::createFromImageData(BinaryData::linked_svg, BinaryData::linked_svgSize))
, oneshotIcon(juce::Drawable::createFromImageData(BinaryData::oneshot_svg, BinaryData::oneshot_svgSize))
, loopIcon(juce::Drawable::createFromImageData(BinaryData::loop_svg, BinaryData::loop_svgSize))
, offIcon(juce::Drawable::createFromImageData(BinaryData::cancel_svg, BinaryData::cancel_svgSize))
, pianoIcon(juce::Drawable::createFromImageData(BinaryData::piano_svg, BinaryData::piano_svgSize))
, oscillatorHeader(juce::Drawable::createFromImageData(BinaryData::oscillator_header_svg, BinaryData::oscillator_header_svgSize))
, noiseHeader(juce::Drawable::createFromImageData(BinaryData::noise_header_svg, BinaryData::noise_header_svgSize))
, lfoHeader(juce::Drawable::createFromImageData(BinaryData::lfo_header_svg, BinaryData::lfo_header_svgSize))
, buttonHeader(juce::Drawable::createFromImageData(BinaryData::button_header_svg, BinaryData::button_header_svgSize))
, meterHeader(juce::Drawable::createFromImageData(BinaryData::meter_header_svg, BinaryData::meter_header_svgSize))
, bottomRightMarkings(juce::Drawable::createFromImageData(BinaryData::bottomright_markings_svg, BinaryData::bottomright_markings_svgSize))
, presetPanel(p.getPresetManager())
, oscSwitch      (nonParamTree.getChildWithName(ParamIDs::synthTree), ParamIDs::oscIO)
, oscGainKnob    (*apvts.getParameter(ParamIDs::oscGain), "Gain", lfoTree.getChildWithName(ParamIDs::lfoOscGain), &um)
, oscPanWheel(*apvts.getParameter(ParamIDs::oscPan), "Pan", lfoTree.getChildWithName(ParamIDs::lfoOscPan))
, oscWavePicker(nonParamTree.getChildWithName(ParamIDs::synthTree), ParamIDs::oscWaveType)
, oscAmpEnv(apvts.getParameter(ParamIDs::oscInitialAmp), apvts.getParameter(ParamIDs::oscAttackAmp), apvts.getParameter(ParamIDs::oscPeakAmp), apvts.getParameter(ParamIDs::oscDecayAmp), apvts.getParameter(ParamIDs::oscSustainAmp), apvts.getParameter(ParamIDs::oscReleaseAmp), nullptr, apvts.getParameter(ParamIDs::oscSlope1Amp), apvts.getParameter(ParamIDs::oscSlope2Amp), apvts.getParameter(ParamIDs::oscSlope3Amp), audioProcessor.oscAmpTables, &um)
, oscPitchEnv(apvts.getParameter(ParamIDs::oscInitialPitch), apvts.getParameter(ParamIDs::oscAttackPitch), apvts.getParameter(ParamIDs::oscPeakPitch), apvts.getParameter(ParamIDs::oscDecayPitch), apvts.getParameter(ParamIDs::oscSustainPitch), apvts.getParameter(ParamIDs::oscReleasePitch), apvts.getParameter(ParamIDs::oscEndPitch), apvts.getParameter(ParamIDs::oscSlope1Pitch), apvts.getParameter(ParamIDs::oscSlope2Pitch), apvts.getParameter(ParamIDs::oscSlope3Pitch), audioProcessor.oscPitchTables, &um)
, oscOctaveSlider(*apvts.getParameter(ParamIDs::oscOctave), "Oct", TextSlider::displayHorizonal, lfoTree.getChildWithName(ParamIDs::lfoOscOctave))
, oscSemitoneSlider(*apvts.getParameter(ParamIDs::oscSemitone), "Semi", TextSlider::displayHorizonal, lfoTree.getChildWithName(ParamIDs::lfoOscSemitone))
, oscFineSlider(*apvts.getParameter(ParamIDs::oscFine), "Fine", TextSlider::displayHorizonal, lfoTree.getChildWithName(ParamIDs::lfoOscFine))
, oscCoarseSlider(*apvts.getParameter(ParamIDs::oscCoarse), "Crs", TextSlider::displayHorizonal, lfoTree.getChildWithName(ParamIDs::lfoOscCoarse))
, oscAmpEnvToggler(nonParamTree.getChildWithName(ParamIDs::synthTree), ParamIDs::oscAmpEnvMode, oneshotIcon, loopIcon)
, pitchEnvToggler(nonParamTree.getChildWithName(ParamIDs::synthTree), ParamIDs::pitchEnvMode, oneshotIcon, loopIcon, offIcon)
, noiseSwitch    (nonParamTree.getChildWithName(ParamIDs::synthTree), ParamIDs::noiseIO)
, noiseGainKnob  (*apvts.getParameter(ParamIDs::noiseGain), "Gain", lfoTree.getChildWithName(ParamIDs::lfoNoiseGain), &um)
, noisePanWheel(*apvts.getParameter(ParamIDs::noisePan), "Pan", lfoTree.getChildWithName(ParamIDs::lfoNoisePan))
, noiseFilterPicker(nonParamTree.getChildWithName(ParamIDs::synthTree), *apvts.getParameter(ParamIDs::noiseQ))
, filterQSlider(*apvts.getParameter(ParamIDs::noiseQ), "Q", TextSlider::displayHorizonal, lfoTree.getChildWithName(ParamIDs::lfoNoiseQ))
, noiseAmpEnv(apvts.getParameter(ParamIDs::noiseInitialAmp), apvts.getParameter(ParamIDs::noiseAttackAmp), apvts.getParameter(ParamIDs::noisePeakAmp), apvts.getParameter(ParamIDs::noiseDecayAmp), apvts.getParameter(ParamIDs::noiseSustainAmp), apvts.getParameter(ParamIDs::noiseReleaseAmp), nullptr, apvts.getParameter(ParamIDs::noiseSlope1Amp), apvts.getParameter(ParamIDs::noiseSlope2Amp), apvts.getParameter(ParamIDs::noiseSlope3Amp), audioProcessor.noiseAmpTables, &um)
, noiseFilterEnv(apvts.getParameter(ParamIDs::noiseInitialCutoff), apvts.getParameter(ParamIDs::noiseAttackCutoff), apvts.getParameter(ParamIDs::noisePeakCutoff), apvts.getParameter(ParamIDs::noiseDecayCutoff), apvts.getParameter(ParamIDs::noiseSustainCutoff), apvts.getParameter(ParamIDs::noiseReleaseCutoff), apvts.getParameter(ParamIDs::noiseEndCutoff), apvts.getParameter(ParamIDs::noiseSlope1Cutoff), apvts.getParameter(ParamIDs::noiseSlope2Cutoff), apvts.getParameter(ParamIDs::noiseSlope3Cutoff), audioProcessor.noiseFilterTables, &um)
, noiseCoarseSlider(*apvts.getParameter(ParamIDs::noiseFilterCoarse), "Crs", TextSlider::displayHorizonal, lfoTree.getChildWithName(ParamIDs::lfoNoiseCoarse))
, noiseAmpEnvToggler(nonParamTree.getChildWithName(ParamIDs::synthTree), ParamIDs::noiseAmpEnvMode, oneshotIcon, loopIcon)
, filterEnvToggler(nonParamTree.getChildWithName(ParamIDs::synthTree), ParamIDs::filterEnvMode, oneshotIcon, loopIcon, offIcon)
, filterKeyFollowButton(nonParamTree.getChildWithName(ParamIDs::synthTree), ParamIDs::filterKeyFollowIO, pianoIcon, pianoIcon, ValueTreeToolButton::showBackgroundToggled)
, delayModule(audioProcessor, apvts, nonParamTree.getChildWithName(ParamIDs::delayTree), lfoTree, effectsOrderTree)
, reverbModule(audioProcessor, apvts, lfoTree, effectsOrderTree)
, filterModule(audioProcessor, apvts, nonParamTree.getChildWithName(ParamIDs::filterTree), lfoTree, effectsOrderTree)
, waveshaperModule(audioProcessor, apvts, nonParamTree.getChildWithName(ParamIDs::waveshaperNodeTree), nonParamTree.getChildWithName(ParamIDs::waveshaperToolTree), lfoTree, effectsOrderTree)
, lfo1Module(lfoTree.getChildWithName(ParamIDs::lfo1), *apvts.getParameter(ParamIDs::lfo1FrequencyTime), *apvts.getParameter(ParamIDs::lfo1FrequencySync), *apvts.getParameter(ParamIDs::lfo1Range), audioProcessor.getLFOVisualizer(0))
, lfo2Module(lfoTree.getChildWithName(ParamIDs::lfo2), *apvts.getParameter(ParamIDs::lfo2FrequencyTime), *apvts.getParameter(ParamIDs::lfo2FrequencySync), *apvts.getParameter(ParamIDs::lfo2Range), audioProcessor.getLFOVisualizer(1))
, bigRedButton(*apvts.getParameter(ParamIDs::midiNote), audioProcessor.midiFifo)
, midiNoteSlider(*apvts.getParameter(ParamIDs::midiNote), "Note")
, levelMeters(audioProcessor)
, volumeLever(*apvts.getParameter(ParamIDs::masterGain), "Master Gain", lfoTree.getChildWithName(ParamIDs::lfoMasterGain))
{
    addAndMakeVisible(presetPanel);
    
    addAndMakeVisible(oscSwitch);
    addAndMakeVisible(oscGainKnob);
    addAndMakeVisible(oscPanWheel);
    addAndMakeVisible(oscWavePicker);
    addAndMakeVisible(oscAmpEnv);
    addAndMakeVisible(oscAmpEnvToggler);
    addAndMakeVisible(oscPitchEnv);
    addAndMakeVisible(pitchEnvToggler);
    addAndMakeVisible(oscOctaveSlider);
    addAndMakeVisible(oscSemitoneSlider);
    addAndMakeVisible(oscFineSlider);
    addAndMakeVisible(oscCoarseSlider);
    
    addAndMakeVisible(noiseSwitch);
    addAndMakeVisible(noiseGainKnob);
    addAndMakeVisible(noisePanWheel);
    addAndMakeVisible(noiseFilterPicker);
    addAndMakeVisible(filterQSlider);
    addAndMakeVisible(noiseAmpEnv);
    addAndMakeVisible(noiseAmpEnvToggler);
    addAndMakeVisible(noiseFilterEnv);
    addAndMakeVisible(filterEnvToggler);
    addAndMakeVisible(noiseCoarseSlider);
    addAndMakeVisible(filterKeyFollowButton);
    
    addAndMakeVisible(delayModule);
    addAndMakeVisible(reverbModule);
    addAndMakeVisible(waveshaperModule);
    addAndMakeVisible(filterModule);
    
    addAndMakeVisible(lfo1Module);
    addAndMakeVisible(lfo2Module);
    
    addAndMakeVisible(bigRedButton);
    addAndMakeVisible(midiNoteSlider);
    
    addAndMakeVisible(levelMeters);
    addAndMakeVisible(volumeLever);
    
    addAndMakeVisible(descriptionBox);
    
    oscSwitch.setComponentID(ComponentIDs::oscSwitch);
    oscGainKnob.setComponentID(ComponentIDs::oscGain);
    oscPanWheel.setComponentID(ComponentIDs::oscPan);
    oscWavePicker.setComponentID(ComponentIDs::oscWavePicker);
    oscAmpEnv.setComponentID(ComponentIDs::oscAmpEnv);
    oscAmpEnvToggler.setComponentID(ComponentIDs::envToggler);
    oscPitchEnv.setComponentID(ComponentIDs::oscPitchEnv);
    pitchEnvToggler.setComponentID(ComponentIDs::multiEnvToggler);
    oscOctaveSlider.setComponentID(ComponentIDs::oscOctave);
    oscSemitoneSlider.setComponentID(ComponentIDs::oscSemitone);
    oscFineSlider.setComponentID(ComponentIDs::oscFine);
    oscCoarseSlider.setComponentID(ComponentIDs::oscCoarse);
    noiseSwitch.setComponentID(ComponentIDs::noiseSwitch);
    noiseGainKnob.setComponentID(ComponentIDs::noiseGain);
    noisePanWheel.setComponentID(ComponentIDs::noisePan);
    noiseFilterPicker.setComponentID(ComponentIDs::noiseFilterPicker);
    noiseAmpEnv.setComponentID(ComponentIDs::noiseAmpEnv);
    noiseAmpEnvToggler.setComponentID(ComponentIDs::envToggler);
    noiseFilterEnv.setComponentID(ComponentIDs::noiseFilterEnv);
    filterEnvToggler.setComponentID(ComponentIDs::multiEnvToggler);
    filterQSlider.setComponentID(ComponentIDs::noiseQ);
    noiseCoarseSlider.setComponentID(ComponentIDs::noiseCoarse);
    filterKeyFollowButton.setComponentID(ComponentIDs::noiseFilterKeyFollow);
    bigRedButton.setComponentID(ComponentIDs::bigRedButton);
    midiNoteSlider.setComponentID(ComponentIDs::midiNote);
    volumeLever.setComponentID(ComponentIDs::masterGain);
    levelMeters.setComponentID(ComponentIDs::levelMeter);
    
    for (auto& child: getChildren())
        child->addMouseListener(&descriptionBox, true);
    
    effectsOrderTree.addListener(this);
}

EditorContent::~EditorContent()
{
    for (auto& child: getChildren())
        child->removeMouseListener(&descriptionBox);
    
    effectsOrderTree.removeListener(this);
}

void EditorContent::paint (juce::Graphics& g)
{
    g.fillAll (MyColors::background);
    
    logoGroup->drawWithin(g, logoArea.reduced(2).toFloat(), juce::RectanglePlacement::xLeft, 1.0f);
    oscillatorHeader->drawWithin(g, oscillatorHeaderArea.toFloat(), juce::RectanglePlacement::xRight, 1.0f);
    noiseHeader->drawWithin(g, noiseHeaderArea.toFloat(), juce::RectanglePlacement::xRight, 1.0f);
    lfoHeader->drawWithin(g, lfoHeaderArea.toFloat(), juce::RectanglePlacement::xRight | juce::RectanglePlacement::yTop, 1.0f);
    buttonHeader->drawWithin(g, buttonHeaderArea.toFloat(), juce::RectanglePlacement::xLeft, 1.0f);
    meterHeader->drawWithin(g, meterHeaderArea.toFloat(), juce::RectanglePlacement::xLeft, 1.0f);
    bottomRightMarkings->drawWithin(g, bottomRightMarkingsArea.toFloat(), juce::RectanglePlacement::xLeft | juce::RectanglePlacement::yBottom, 1.0f);
}

void EditorContent::resized()
{
    juce::Rectangle<int> totalArea = getLocalBounds();
    
    static const int margin = MyWidths::standardMarginInt;
    
    static const int headerHeight = MyWidths::buttonDimension + margin * 2;
    juce::Rectangle<int> headerArea = totalArea.removeFromTop(headerHeight);
    static const int presetPanelWidth = headerArea.proportionOfWidth(0.5f);
    juce::Rectangle<int> presetPanelArea (presetPanelWidth, headerHeight);
    presetPanel.setBounds(presetPanelArea.withCentre(headerArea.getCentre()));
    logoArea = juce::Rectangle<int> (200, headerHeight * 0.82f);
    logoArea.setCentre(headerArea.getCentre());
    logoArea.setX(headerArea.getX() + 38);
    
    bottomRightMarkingsArea = totalArea;
    totalArea.removeFromBottom(14);
    totalArea.removeFromRight(9);
    
    static const int synthsWidth = 450;
    static const int envelopeWidth = (synthsWidth - margin * 3) / 2;
    juce::Rectangle<int> synthsArea = totalArea.removeFromLeft(synthsWidth + additionalHeaderWidth);
    
    const int synthAreaHeight = moduleHeight;
    oscillatorHeaderArea = synthsArea.removeFromTop(synthAreaHeight + additionalHeaderHeight);
    juce::Rectangle<int> oscArea = oscillatorHeaderArea.withTrimmedLeft(additionalHeaderWidth).withTrimmedTop(additionalHeaderHeight);
    noiseHeaderArea = synthsArea.removeFromTop(synthAreaHeight + additionalHeaderHeight);
    juce::Rectangle<int> noiseArea = noiseHeaderArea.withTrimmedLeft(additionalHeaderWidth).withTrimmedTop(additionalHeaderHeight);
    
    static const int synthTopBarHeight = 70;
    juce::Rectangle<int> oscTopBar = oscArea.removeFromTop(synthTopBarHeight);
    juce::Rectangle<int> oscTopBarInner = oscTopBar.reduced(margin);
    juce::Rectangle<int> noiseTopBar = noiseArea.removeFromTop(synthTopBarHeight);
    juce::Rectangle<int> noiseTopBarInner = noiseTopBar.reduced(margin);
    
    static const int switchWidth = 40;
    juce::Rectangle<int> oscSwitchArea = oscTopBarInner.removeFromLeft(switchWidth);
    oscSwitch.setBounds(oscSwitchArea);
    oscTopBarInner.removeFromLeft(margin);
    juce::Rectangle<int> noiseSwitchArea = noiseTopBarInner.removeFromLeft(switchWidth);
    noiseSwitch.setBounds(noiseSwitchArea);
    noiseTopBarInner.removeFromLeft(margin);
    
    juce::Rectangle<int> oscKnobArea = oscTopBarInner.removeFromLeft(MyWidths::knobDimension);
    oscGainKnob.setBounds(oscKnobArea);
    oscTopBarInner.removeFromLeft(margin);
    juce::Rectangle<int> noiseKnobArea = noiseTopBarInner.removeFromLeft(MyWidths::knobDimension);
    noiseGainKnob.setBounds(noiseKnobArea);
    noiseTopBarInner.removeFromLeft(margin);
    
    static const int wheelWidth = 100;
    static const int wheelHeight = 35;
    juce::Rectangle<int> wheelArea (wheelWidth, wheelHeight);
    oscPanWheel.setBounds(wheelArea.withCentre(oscTopBarInner.removeFromLeft(wheelWidth).getCentre()));
    noisePanWheel.setBounds(wheelArea.withCentre(noiseTopBarInner.removeFromLeft(wheelWidth).getCentre()));
    
    static const int pickerWidth = 120;
    static const int pickerHeight = 32;
    juce::Rectangle<int> pickerArea (pickerWidth, pickerHeight);
    
    juce::Rectangle<int> oscTopbarPitchArea = oscTopBarInner.removeFromRight(envelopeWidth);
    oscTopbarPitchArea.removeFromRight(margin);
    juce::Rectangle<int> oscPitchControlsArea = oscTopbarPitchArea.removeFromBottom(MyWidths::horizontalTextSliderBounds.getHeight());
    static const int horizontalSliderWidth = MyWidths::horizontalTextSliderBounds.getWidth();
    oscOctaveSlider.setBounds(oscPitchControlsArea.removeFromLeft(horizontalSliderWidth));
    oscSemitoneSlider.setBounds(oscPitchControlsArea.removeFromLeft(horizontalSliderWidth));
    oscFineSlider.setBounds(oscPitchControlsArea.removeFromLeft(horizontalSliderWidth));
    oscCoarseSlider.setBounds(oscPitchControlsArea.removeFromLeft(horizontalSliderWidth));
    oscTopbarPitchArea.removeFromBottom(margin);
    
    oscWavePicker.setBounds(pickerArea.withCentre(oscTopbarPitchArea.getCentre()));
    
    juce::Rectangle<int> noiseTopbarFilterArea = noiseTopBarInner.removeFromRight(envelopeWidth);
    noiseTopbarFilterArea.removeFromRight(margin);
    juce::Rectangle<int> noiseFilterControlsArea = noiseTopbarFilterArea.removeFromBottom(MyWidths::horizontalTextSliderBounds.getHeight());
    noiseFilterControlsArea.reduce(envelopeWidth / 4, 0);
    filterQSlider.setBounds(noiseFilterControlsArea.removeFromLeft(horizontalSliderWidth));
    noiseCoarseSlider.setBounds(noiseFilterControlsArea.removeFromLeft(horizontalSliderWidth));
    noiseTopbarFilterArea.removeFromBottom(margin);
    
    noiseFilterPicker.setBounds(pickerArea.withCentre(noiseTopbarFilterArea.getCentre()));
    filterKeyFollowButton.setBounds(MyWidths::buttonBounds.withY(noiseTopbarFilterArea.getY()).withRightX(noiseTopbarFilterArea.getRight()));
    noiseTopBarInner.removeFromLeft(margin);
    
    juce::Rectangle<int> oscEnvelopesAreaInner = oscArea.reduced(margin);
    juce::Rectangle<int> oscAmpEnvArea = oscEnvelopesAreaInner.removeFromLeft(envelopeWidth);
    oscAmpEnv.setBounds(oscAmpEnvArea);
    oscAmpEnvToggler.setBounds(oscAmpEnv.getEnvModeButtonBounds());
    juce::Rectangle<int> pitchEnvArea = oscEnvelopesAreaInner.removeFromRight(envelopeWidth);
    oscPitchEnv.setBounds(pitchEnvArea);
    pitchEnvToggler.setBounds(oscPitchEnv.getEnvModeButtonBounds());
    
    juce::Rectangle<int> noiseEnvelopesAreaInner = noiseArea.reduced(margin);
    juce::Rectangle<int> noiseAmpEnvArea = noiseEnvelopesAreaInner.removeFromLeft(envelopeWidth);
    noiseAmpEnv.setBounds(noiseAmpEnvArea);
    noiseAmpEnvToggler.setBounds(noiseAmpEnv.getEnvModeButtonBounds());
    juce::Rectangle<int> filterEnvArea = noiseEnvelopesAreaInner.removeFromRight(envelopeWidth);
    noiseFilterEnv.setBounds(filterEnvArea);
    filterEnvToggler.setBounds(noiseFilterEnv.getEnvModeButtonBounds());
    
    effectsBounds = totalArea.removeFromLeft((effectWidth + additionalHeaderWidth) * 2);
    setEffectsBounds();
    
    static const int utilitiesWidth = 300;
    juce::Rectangle<int> utilitiesArea = totalArea.removeFromLeft(utilitiesWidth);
    
    static const int lfoPanelHeight = 145;
    lfoHeaderArea = utilitiesArea.removeFromTop(lfoPanelHeight + additionalHeaderHeight);
    juce::Rectangle<int> lfoPanelArea = lfoHeaderArea.withTrimmedLeft(additionalHeaderWidth).withTrimmedTop(additionalHeaderHeight);
    lfo1Module.setBounds(lfoPanelArea.removeFromLeft((lfoPanelArea.getWidth() - margin) / 2));
    lfoPanelArea.removeFromLeft(margin);
    lfo2Module.setBounds(lfoPanelArea);
    
    static const int bigButtonDimension = 250;
    juce::Rectangle<int> buttonArea = utilitiesArea.removeFromTop(bigButtonDimension + additionalHeaderHeight);
    buttonHeaderArea = buttonArea;
    buttonArea.removeFromLeft(18);
    buttonArea.removeFromTop(additionalHeaderHeight);
    bigRedButton.setBounds(GUIHelper::getCenterSquare(buttonArea));
    midiNoteSlider.setBounds(MyWidths::smallTextSliderBounds.withY(buttonArea.getY()).withRightX(bigRedButton.getRight()));
    
    meterHeaderArea = utilitiesArea;
    utilitiesArea.removeFromLeft(18);
    utilitiesArea.removeFromTop(additionalHeaderHeight);
    descriptionBox.setBounds(utilitiesArea.removeFromRight(160));
    volumeLever.setBounds(utilitiesArea.removeFromLeft(60));
    utilitiesArea.removeFromLeft(margin);
    levelMeters.setBounds(GUIHelper::getCenterRectWithWidth(utilitiesArea, 40));
    
}

void EditorContent::setEffectsBounds()
{
    juce::Rectangle<int> effectsBoundsCopy = effectsBounds;
    juce::Rectangle<int> leftColumn  = effectsBoundsCopy.removeFromLeft(effectWidth + additionalHeaderWidth);
    juce::Rectangle<int> rightColumn = effectsBoundsCopy.removeFromLeft(effectWidth + additionalHeaderWidth);
    
    const int effectHeight = moduleHeight;
    
    juce::OwnedArray<juce::Rectangle<int>> effectBoundsArray;
    effectBoundsArray.add(new juce::Rectangle<int> (leftColumn.removeFromTop(effectHeight + additionalHeaderHeight)));
    effectBoundsArray.add(new juce::Rectangle<int> (leftColumn.removeFromTop(effectHeight + additionalHeaderHeight)));
    effectBoundsArray.add(new juce::Rectangle<int> (rightColumn.removeFromTop(effectHeight + additionalHeaderHeight)));
    effectBoundsArray.add(new juce::Rectangle<int> (rightColumn.removeFromTop(effectHeight + additionalHeaderHeight)));
    
    for (int i = 0; i < effectsOrderTree.getNumChildren(); i++)
    {
        const juce::Identifier& type = effectsOrderTree.getChild(i).getType();
        
        if (type == ParamIDs::delayNode)
        {
            delayModule.setBounds(*effectBoundsArray[i]);
            delayModule.setComponentID("effect" + juce::String{i + 1});
        }
        else if (type == ParamIDs::reverbNode)
        {
            reverbModule.setBounds(*effectBoundsArray[i]);
            reverbModule.setComponentID("effect" + juce::String{i + 1});
        }
        else if (type == ParamIDs::filterNode)
        {
            filterModule.setBounds(*effectBoundsArray[i]);
            filterModule.setComponentID("effect" + juce::String{i + 1});
        }
        else if (type == ParamIDs::waveshaperNode)
        {
            waveshaperModule.setBounds(*effectBoundsArray[i]);
            waveshaperModule.setComponentID("effect" + juce::String{i + 1});
        }
    }
    
    repaint();
}

void EditorContent::valueTreeChildOrderChanged (juce::ValueTree &parentTreeWhoseChildrenHaveMoved, int oldIndex, int newIndex)
{
    setEffectsBounds();
}
