/*
  ==============================================================================

    DescriptionBox.h
    Created: 10 Sep 2024 9:49:59am
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GUIHelper.h"

class DescriptionBox  : public juce::Component
{
public:
    DescriptionBox()
    {
        setOpaque(true);
        
        setComponentID(ComponentIDs::descriptionBox);
        
        addAndMakeVisible(componentTitle);
        addAndMakeVisible(descriptionText);
        
        componentTitle.setColour(juce::Label::textColourId, MyColors::white);
        descriptionText.setColour(juce::Label::textColourId, MyColors::white);
        descriptionText.setJustificationType(juce::Justification::topLeft);
        descriptionText.setMinimumHorizontalScale(1.0f);
    }
    
    void mouseDown (const juce::MouseEvent& event) override
    {
        juce::Component* comp = event.originalComponent;
        
        while (comp && comp->getComponentID().isEmpty())
            comp = comp->getParentComponent();
        
        if (comp && comp == this)
        {
            disabled = !disabled;
            if (disabled)
                setEmptyContents();
            else
                setTextForComponent(comp);
        }
    }
    
    void mouseEnter (const juce::MouseEvent& event) override
    {
        if (disabled)
            return;
        
        juce::Component* comp = event.originalComponent;
        
        while (comp && comp->getComponentID().isEmpty())
            comp = comp->getParentComponent();
        
        setTextForComponent(comp);
    }
    
    void mouseExit (const juce::MouseEvent& event) override
    {
        setEmptyContents();
    }
    
    void setTextForComponent (const juce::Component* comp)
    {
        if (!comp)
        {
            setEmptyContents();
            return;
        }
        
        juce::String compID = comp->getComponentID();
        drawEmptyContents = false;
        
        if (compID == ComponentIDs::descriptionBox)
        {
            componentTitle.setText("Description Box", juce::dontSendNotification);
            descriptionText.setText("Hover over a control to read more about its function and how to use it. Click box to disable/enable", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::savePreset)
        {
            componentTitle.setText("Save Preset", juce::dontSendNotification);
            descriptionText.setText("Save a snapshot of all settings that can be restored from any instance of LaunchCodes", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::previousPreset)
        {
            componentTitle.setText("Previous Preset", juce::dontSendNotification);
            descriptionText.setText("Load the previous preset from the preset menu", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::presetMenu)
        {
            componentTitle.setText("Preset Menu", juce::dontSendNotification);
            descriptionText.setText("Click to expand the full selectable list of presets saved on your device", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::nextPreset)
        {
            componentTitle.setText("Next Preset", juce::dontSendNotification);
            descriptionText.setText("Load the next preset from the preset menu", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::deletePreset)
        {
            componentTitle.setText("Delete Preset", juce::dontSendNotification);
            descriptionText.setText("Delete the currently selected preset for your device", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::oscSwitch)
        {
            componentTitle.setText("Oscillator Switch", juce::dontSendNotification);
            descriptionText.setText("Click to toggle the oscillator unit on/off", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::oscGain)
        {
            componentTitle.setText("Oscillator Gain", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the gain of the oscillator", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::oscPan)
        {
            componentTitle.setText("Oscillator Pan", juce::dontSendNotification);
            descriptionText.setText("Drag left/right to adjust the stereo panning of the oscillator", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::oscWavePicker)
        {
            componentTitle.setText("Oscillator Waveform", juce::dontSendNotification);
            descriptionText.setText("Use the arrows to select a sine, triangle, sawtooth, or square wave for the oscillator", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::oscAmpEnv)
        {
            componentTitle.setText("Oscillator Amplitude Envelope", juce::dontSendNotification);
            descriptionText.setText("Each draggable node controls a stage of the envelope that controls the oscillator's amplitude", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::envToggler)
        {
            componentTitle.setText("Envelope Mode", juce::dontSendNotification);
            descriptionText.setText("Click to set the envelope mode to one-shot or loop", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::multiEnvToggler)
        {
            componentTitle.setText("Envelope Mode", juce::dontSendNotification);
            descriptionText.setText("Click to set the envelope mode to one-shot, loop, or off", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::oscPitchEnv)
        {
            componentTitle.setText("Oscillator Pitch Envelope", juce::dontSendNotification);
            descriptionText.setText("Each draggable node controls a stage of the envelope that controls the oscillator's pitch", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::oscOctave)
        {
            componentTitle.setText("Oscillator Octave", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the octave the oscillator will play relative to midi input", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::oscSemitone)
        {
            componentTitle.setText("Oscillator Semitone", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the semitone the oscillator will play relative to midi input", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::oscFine)
        {
            componentTitle.setText("Oscillator Fine", juce::dontSendNotification);
            descriptionText.setText("Drag to tune the note the oscillator will play in cents relative to midi input", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::oscCoarse)
        {
            componentTitle.setText("Oscillator Coarse", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the note the oscillator will play by semitones relative to midi input. This is an ideal LFO target for smooth frequency modulation", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::noiseSwitch)
        {
            componentTitle.setText("Noise Switch", juce::dontSendNotification);
            descriptionText.setText("Click to toggle the noise unit on/off", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::noiseGain)
        {
            componentTitle.setText("Noise Gain", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the gain of the noise unit", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::noisePan)
        {
            componentTitle.setText("Noise Pan", juce::dontSendNotification);
            descriptionText.setText("Drag left/right to adjust the stereo panning of the noise unit", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::noiseFilterPicker)
        {
            componentTitle.setText("Noise Filter Type", juce::dontSendNotification);
            descriptionText.setText("Use the arrows to select a lowpass, highpass, or bandpass filter for the noise unit", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::noiseFilterQ)
        {
            componentTitle.setText("Noise Filter Resonance", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the noise filter's resonance", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::noiseAmpEnv)
        {
            componentTitle.setText("Noise Amplitude Envelope", juce::dontSendNotification);
            descriptionText.setText("Each draggable node controls a stage of the envelope that controls the noise unit's amplitude", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::noiseFilterEnv)
        {
            componentTitle.setText("Noise Filter Envelope", juce::dontSendNotification);
            descriptionText.setText("Each draggable node controls a stage of the envelope that controls the noise unit's filter cutoff", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::noiseQ)
        {
            componentTitle.setText("Noise Filter Resonance", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the noise filter resonance", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::noiseCoarse)
        {
            componentTitle.setText("Noise Filter Coarse", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the noise filter cutoff frequency by powers of 2", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::noiseFilterKeyFollow)
        {
            componentTitle.setText("Noise Filter Key-Follow", juce::dontSendNotification);
            descriptionText.setText("Toggle key-follow mode, which tracks midi input to adjust the noise filter's cutoff", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::attack)
        {
            componentTitle.setText("Attack", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the attack time of the envelope", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::decay)
        {
            componentTitle.setText("Decay", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the decay time of the envelope", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::release)
        {
            componentTitle.setText("Release", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the release time of the envelope", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::initial)
        {
            componentTitle.setText("Initial", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the initial value of the envelope", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::peak)
        {
            componentTitle.setText("Peak", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the peak value of the envelope", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::sustain)
        {
            componentTitle.setText("Sustain", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the sustain value of the envelope", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::end)
        {
            componentTitle.setText("End", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the end value of the envelope", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::effect1)
        {
            componentTitle.setText("Effect 1", juce::dontSendNotification);
            descriptionText.setText("The effect in this slot is the first effect in the processing chain", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::effect2)
        {
            componentTitle.setText("Effect 2", juce::dontSendNotification);
            descriptionText.setText("The effect in this slot is the second effect in the processing chain", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::effect3)
        {
            componentTitle.setText("Effect 3", juce::dontSendNotification);
            descriptionText.setText("The effect in this slot is the third effect in the processing chain", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::effect4)
        {
            componentTitle.setText("Effect 4", juce::dontSendNotification);
            descriptionText.setText("The effect in this slot is the fourth effect in the processing chain", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::effectCrosshair)
        {
            componentTitle.setText("Effect Crosshair", juce::dontSendNotification);
            descriptionText.setText("Drag the crosshair over another effect slot to move it there. The audio processing chain will reorder itself to reflect this change", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::delayLeft)
        {
            componentTitle.setText("Left Delay", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the time of the left delay", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::delayRight)
        {
            componentTitle.setText("Right Delay", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the time of the right delay", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::delayLink)
        {
            componentTitle.setText("Delay Link Mode", juce::dontSendNotification);
            descriptionText.setText("Toggle between modes, where unlinked mode allows independent control of left and right delay, and linked mode ties both delays to the same time and sync mode", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::delayOffsetLeft)
        {
            componentTitle.setText("Left Delay Offset", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the percent offset from the displayed time that the left delay will be set", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::delayOffsetRight)
        {
            componentTitle.setText("Right Delay Offset", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the percent offset from the displayed time that the right delay will be set", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::delaySyncLeft)
        {
            componentTitle.setText("Left Delay Sync Mode", juce::dontSendNotification);
            descriptionText.setText("Toggle between modes, where time mode sets delay in seconds and milliseconds, and sync mode sets delay in sixteenth-note values quantized to host BPM", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::delaySyncRight)
        {
            componentTitle.setText("Right Delay Sync Mode", juce::dontSendNotification);
            descriptionText.setText("Toggle between modes, where time mode sets delay in seconds and milliseconds, and sync mode sets delay in sixteenth-note values quantized to host BPM", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::delayFeedback)
        {
            componentTitle.setText("Delay Feedback", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust both delays' feedback (the amount of the delay output that is fed back into itself)", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::delayMix)
        {
            componentTitle.setText("Delay Dry/Wet", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the mix of the delay unit, where 0% outputs no delay signal and 100% outputs only delay signal", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::delayFilter)
        {
            componentTitle.setText("Delay Filter", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the filter applied to the delay unit", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::delayFilterCenter)
        {
            componentTitle.setText("Delay Filter Center", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the center frequency of the delay unit's filter", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::delayFilterWidth)
        {
            componentTitle.setText("Delay Filter Center", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the bandwidth of the delay unit's filter", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::reverbPad)
        {
            componentTitle.setText("Reverb XY Pad", juce::dontSendNotification);
            descriptionText.setText("Visualizes the size, width, and damping of the reverb unit. Click and drag horizontally to adjust the reverb width, and vertically to adjust the reverb size", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::reverbSize)
        {
            componentTitle.setText("Reverb Size", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the size of the reverb unit", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::reverbDamping)
        {
            componentTitle.setText("Reverb Damping", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the damping of the reverb unit", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::reverbWidth)
        {
            componentTitle.setText("Reverb Width", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the stereo width of the reverb unit", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::reverbMix)
        {
            componentTitle.setText("Reverb Dry/Wet", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the mix of the reverb unit, where 0% outputs no reverb signal and 100% outputs only reverb signal", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::filterVisualizer)
        {
            componentTitle.setText("Filter Visualizer", juce::dontSendNotification);
            descriptionText.setText("Drag each node to adjust the cutoff frequency, resonance, and gain of the filters", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::filterLowCutoff)
        {
            componentTitle.setText("Low Filter Cutoff", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the cutoff frequency of the low filter", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::filterLowQ)
        {
            componentTitle.setText("Low Filter Resonance", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the resonance of the low filter", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::filterLowGain)
        {
            componentTitle.setText("Low Filter Gain", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the gain of the low shelf filter", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::filterLowMode)
        {
            componentTitle.setText("Low Filter Mode", juce::dontSendNotification);
            descriptionText.setText("Click to toggle between highpass, low shelf, and off", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::filterHighCutoff)
        {
            componentTitle.setText("High Filter Cutoff", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the cutoff frequency of the high filter", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::filterHighQ)
        {
            componentTitle.setText("High Filter Resonance", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the resonance of the high filter", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::filterHighGain)
        {
            componentTitle.setText("High Filter Gain", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the gain of the high shelf filter", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::filterHighMode)
        {
            componentTitle.setText("High Filter Mode", juce::dontSendNotification);
            descriptionText.setText("Click to toggle between lowpass, high shelf, and off", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::waveshaperRadar)
        {
            componentTitle.setText("Waveshaper Editor", juce::dontSendNotification);
            descriptionText.setText("Each draggable node controls the waveshaper, which takes the input signal and transforms it according to the graph of its output. Large nodes control breakpoint positions and small nodes control the slope between points", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::waveshaperOddEven)
        {
            componentTitle.setText("Odd/Even", juce::dontSendNotification);
            descriptionText.setText("Toggle between modes, where odd mode rotates the user-defined graph about the origin, and even mode reflects the graph across the x-axis", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::waveshaperXTransform)
        {
            componentTitle.setText("X Transform", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the waveshaper output graph horizontally, towards and away from the y-axis", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::waveshaperYTransform)
        {
            componentTitle.setText("Y Transform", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the waveshaper output graph vertically, towards and away from the x-axis", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::waveshaperInputGain)
        {
            componentTitle.setText("Waveshaper Pre-Gain", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the gain of the waveshaper input", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::waveshaperOutputGain)
        {
            componentTitle.setText("Waveshaper Post-Gain", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the gain of the waveshaper output", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::waveshaperMix)
        {
            componentTitle.setText("Waveshaper Dry-Wet", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the mix of the waveshaper unit, where 0% outputs no waveshaper signal and 100% outputs only waveshaper signal", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::lfoWavePicker)
        {
            componentTitle.setText("LFO Waveform", juce::dontSendNotification);
            descriptionText.setText("Use the arrows to select a sine, triangle, sawtooth, or square wave for the LFO", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::lfoDragger)
        {
            componentTitle.setText("LFO Crosshair", juce::dontSendNotification);
            descriptionText.setText("Click and drag the crosshair to a valid target to enable modulation by this LFO. On the modulation target: Adjust the range of the modulation by holding Command and dragging, and switch directional modes in the right-click menu", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::lfoVisualizer)
        {
            componentTitle.setText("LFO Visualizer", juce::dontSendNotification);
            descriptionText.setText("Displays the output of this LFO. Click to toggle the display on/off", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::lfoRange)
        {
            componentTitle.setText("LFO Range", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the range of the LFO output", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::lfoSync)
        {
            componentTitle.setText("LFO Sync Mode", juce::dontSendNotification);
            descriptionText.setText("Toggle between modes, where time mode sets frequency in hertz, and sync mode sets wavelength in sixteenth-note values quantized to host BPM ", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::lfoFrequency)
        {
            componentTitle.setText("LFO Frequency", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the frequency of the LFO", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::bigRedButton)
        {
            componentTitle.setText("Big Red Button", juce::dontSendNotification);
            descriptionText.setText("Do not touch", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::midiNote)
        {
            componentTitle.setText("Midi Note", juce::dontSendNotification);
            descriptionText.setText("Drag to set the note triggered by the big red button", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::masterGain)
        {
            componentTitle.setText("Master Gain", juce::dontSendNotification);
            descriptionText.setText("Drag to adjust the gain of the total output in decibels", juce::dontSendNotification);
        }
        else if (compID == ComponentIDs::levelMeter)
        {
            componentTitle.setText("Level Meter", juce::dontSendNotification);
            descriptionText.setText("The solid meters indicate the RMS levels of the output, while the thin lines indicate peak levels. The bottom value keeps track of the maximum output, and can be clicked to reset", juce::dontSendNotification);
        }
        else
        {
            setEmptyContents();
        }
        
        repaint();
    }
    
    void setEmptyContents()
    {
        componentTitle.setText("", juce::dontSendNotification);
        descriptionText.setText("", juce::dontSendNotification);
        drawEmptyContents = true;
        repaint();
    }

    void paint (juce::Graphics& g) override
    {
        g.setColour(MyColors::viewBackground);
        g.fillRect(totalArea.toFloat());
        
        g.setColour(MyColors::darkNeutral);
        GUIHelper::drawRectangleWithThickness(g, totalArea.toFloat(), MyWidths::standardWidth);
        g.drawLine(titleArea.getX(), titleArea.getBottom(), titleArea.getRight(), titleArea.getBottom(), MyWidths::standardWidth);
        
        if (drawEmptyContents)
        {
            g.setColour(MyColors::darkNeutral);
            GUIHelper::paintXBox(g, descriptionArea);
        }
    }

    void resized() override
    {
        totalArea = getLocalBounds();
        juce::Rectangle<int> bounds = totalArea;
        
        titleArea = bounds.removeFromTop(bounds.proportionOfHeight(0.2f));
        descriptionArea = bounds;
        
        componentTitle.setBounds(titleArea.reduced(2));
        descriptionText.setBounds(descriptionArea.reduced(2));
        descriptionText.setFont(static_cast<float>(descriptionText.getHeight()) / 10.0f);
    }

private:
    juce::Label componentTitle, descriptionText;
    
    juce::Rectangle<int> totalArea, titleArea, descriptionArea;
    
    bool drawEmptyContents { true };
    bool disabled { false };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DescriptionBox)
};
