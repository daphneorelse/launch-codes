/*
  ==============================================================================

    LFOVisualizer.h
    Created: 30 Aug 2024 10:19:22am
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GUIHelper.h"
#include "../service/farbot/fifo.hpp"

//==============================================================================
/*
*/
class LFOVisualizer  : public juce::Component,
                       private juce::Timer
{
public:
    LFOVisualizer ();

    ~LFOVisualizer() override;

    void setBufferSize (int bufferSize);
    void setSamplesPerBlock (int newNumInputSamplesPerBlock) noexcept;

    void pushBuffer (const juce::AudioBuffer<float>& bufferToPush);

    void setRepaintRate (int frequencyInHz);
    
    void mouseDown (const juce::MouseEvent& event) override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::Array<float> values;
    int bufferSize = 100, arrayIndex = 0;
    std::atomic<int> inputSamplesPerBlock = 512;
    int sampleIndex = 0;
    int repaintRate = 60;
    
    farbot::fifo<float, farbot::fifo_options::concurrency::single, farbot::fifo_options::concurrency::single, farbot::fifo_options::full_empty_failure_mode::return_false_on_full_or_empty, farbot::fifo_options::full_empty_failure_mode::return_false_on_full_or_empty> fifo;
    
    juce::Rectangle<int> mainArea;
    
    const int numXGridLines { 6 }, numYGridLines { 5 };
    
    bool viewOn = true;

    void timerCallback() override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LFOVisualizer)
};
