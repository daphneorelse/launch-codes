/*
  ==============================================================================

    MidiButton.cpp
    Created: 15 Sep 2024 6:26:39pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MidiButton.h"

MidiButton::MidiButton(juce::RangedAudioParameter& midiNoteParam, farbot::fifo<juce::MidiMessage, farbot::fifo_options::concurrency::single, farbot::fifo_options::concurrency::single, farbot::fifo_options::full_empty_failure_mode::return_false_on_full_or_empty, farbot::fifo_options::full_empty_failure_mode::return_false_on_full_or_empty>& fifo)
: midiNoteAttachment(midiNoteParam, [&] (float val) { midiNoteChanged(static_cast<int>(val)); })
, midiFifo(fifo)
, buttonStandby(juce::Drawable::createFromImageData(BinaryData::big_red_button_standby_svg, BinaryData::big_red_button_standby_svgSize))
, buttonPressed(juce::Drawable::createFromImageData(BinaryData::big_red_button_pressed_svg, BinaryData::big_red_button_pressed_svgSize))
{
    setOpaque(true);
    
    midiNoteAttachment.sendInitialUpdate();
}

void MidiButton::mouseDown (const juce::MouseEvent& event)
{
    midiFifo.push(juce::MidiMessage::noteOn(1, noteNumber.load(), 1.0f));
    
    pressed = true;
    repaint();
}

void MidiButton::mouseUp (const juce::MouseEvent& event)
{
    midiFifo.push(juce::MidiMessage::noteOff(1, noteNumber.load(), 1.0f));
    
    pressed = false;
    repaint();
}

void MidiButton::paint (juce::Graphics& g)
{
    g.fillAll(MyColors::background);
    
    if (pressed)
        buttonPressed->drawWithin(g, mainArea.toFloat(), 0, 1.0f);
    else
        buttonStandby->drawWithin(g, mainArea.toFloat(), 0, 1.0f);
}

void MidiButton::resized()
{
    const auto bounds = getLocalBounds();
    
    const int dimension = juce::jmin(bounds.getWidth(), bounds.getHeight());
    mainArea = juce::Rectangle<int> (dimension, dimension);
    mainArea.setCentre(bounds.getCentre());
}

void MidiButton::midiNoteChanged (int newNum)
{
    noteNumber.store(newNum);
}
