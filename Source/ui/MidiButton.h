/*
  ==============================================================================

    MidiButton.h
    Created: 15 Sep 2024 6:26:39pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../service/farbot/fifo.hpp"
#include "GUIHelper.h"

class MidiButton  : public juce::Component
{
public:
    MidiButton(juce::RangedAudioParameter& midiNoteParam, farbot::fifo<juce::MidiMessage, farbot::fifo_options::concurrency::single, farbot::fifo_options::concurrency::single, farbot::fifo_options::full_empty_failure_mode::return_false_on_full_or_empty, farbot::fifo_options::full_empty_failure_mode::return_false_on_full_or_empty>& fifo);
    
    void mouseDown (const juce::MouseEvent& event) override;
    void mouseUp   (const juce::MouseEvent& event) override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void midiNoteChanged (int newNum);
    
    juce::ParameterAttachment midiNoteAttachment;
    
    farbot::fifo<juce::MidiMessage, farbot::fifo_options::concurrency::single, farbot::fifo_options::concurrency::single, farbot::fifo_options::full_empty_failure_mode::return_false_on_full_or_empty, farbot::fifo_options::full_empty_failure_mode::return_false_on_full_or_empty>& midiFifo;
    
    std::atomic<int> noteNumber;
    
    std::unique_ptr<juce::Drawable> buttonStandby;
    std::unique_ptr<juce::Drawable> buttonPressed;
    
    juce::Rectangle<int> mainArea;
    
    bool pressed { false };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiButton)
};
