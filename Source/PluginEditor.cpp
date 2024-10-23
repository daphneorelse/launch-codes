#include "PluginProcessor.h"
#include "PluginEditor.h"

PluginEditor::PluginEditor (PluginProcessor& p, juce::UndoManager& um)
: AudioProcessorEditor (&p), audioProcessor (p)
, apvts(p.getPluginAPVST())
, nonParamTree(p.getPluginNonParamTree())
, editorContent(p, um)
{
    setSize (1264, 618);
    
    addAndMakeVisible(editorContent);
}

PluginEditor::~PluginEditor()
{
    
}

void PluginEditor::paint (juce::Graphics& g)
{
    
}

void PluginEditor::resized()
{
    editorContent.setBounds(getBounds());
}
