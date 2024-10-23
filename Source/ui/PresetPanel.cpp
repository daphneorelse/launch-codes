/*
  ==============================================================================

    PresetPanel.cpp
    Author:  Akash Murthy, edited by Daphne Wilkerson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PresetPanel.h"

PresetPanel::PresetPanel(PresetManager& pm)
: presetManager(pm)
, saveIcon(juce::Drawable::createFromImageData(BinaryData::save_svg, BinaryData::save_svgSize))
, deleteIcon(juce::Drawable::createFromImageData(BinaryData::delete_svg, BinaryData::delete_svgSize))
, previousIcon(juce::Drawable::createFromImageData(BinaryData::previous_svg, BinaryData::previous_svgSize))
, nextIcon(juce::Drawable::createFromImageData(BinaryData::next_svg, BinaryData::next_svgSize))
, saveButton(saveIcon)
, deleteButton(deleteIcon)
, previousPresetButton(previousIcon)
, nextPresetButton(nextIcon)
{
    setLookAndFeel(&lnf);
    
    addAndMakeVisible(saveButton);
    addAndMakeVisible(deleteButton);
    addAndMakeVisible(previousPresetButton);
    addAndMakeVisible(nextPresetButton);
    
    saveButton.setComponentID(ComponentIDs::savePreset);
    deleteButton.setComponentID(ComponentIDs::deletePreset);
    previousPresetButton.setComponentID(ComponentIDs::previousPreset);
    nextPresetButton.setComponentID(ComponentIDs::nextPreset);
    
    saveButton.callback = [&]
    {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Please enter the name of the preset to save",
            PresetManager::defaultDirectory,
            "*." + PresetManager::extension
        );
        fileChooser->launchAsync(juce::FileBrowserComponent::saveMode, [&](const juce::FileChooser& chooser)
            {
                const auto resultFile = chooser.getResult();
                presetManager.savePreset(resultFile.getFileNameWithoutExtension());
                loadPresetList();
            });
    };
    deleteButton.callback = [&]
    {
        presetManager.deletePreset(presetManager.getCurrentPreset());
        loadPresetList();
    };
    previousPresetButton.callback = [&]
    {
        const auto index = presetManager.loadPreviousPreset();
        presetList.setSelectedItemIndex(index, juce::dontSendNotification);
    };
    nextPresetButton.callback = [&]
    {
        const auto index = presetManager.loadNextPreset();
        presetList.setSelectedItemIndex(index, juce::dontSendNotification);
    };

    presetList.setTextWhenNothingSelected("No Preset Selected");
    presetList.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    addAndMakeVisible(presetList);
    presetList.setComponentID(ComponentIDs::presetMenu);
    presetList.addListener(this);

    loadPresetList();
}

PresetPanel::~PresetPanel()
{
    presetList.removeListener(this);
    
    setLookAndFeel(nullptr);
}

void PresetPanel::resized()
{
    const int margin = 4;
    const int squareButtonWidth = MyWidths::buttonDimension;
    const int listWidth = 250;
    
    const int totalWidth = squareButtonWidth * 4 + listWidth + margin * 4;
    
    juce::Rectangle<int> container (totalWidth, getHeight() - margin * 2);
    container.setCentre(getLocalBounds().getCentre());
    auto bounds = container;
    
    saveButton.setBounds(bounds.removeFromLeft(squareButtonWidth));
    bounds.removeFromLeft(margin);
    previousPresetButton.setBounds(bounds.removeFromLeft(squareButtonWidth));
    bounds.removeFromLeft(margin);
    presetList.setBounds(bounds.removeFromLeft(listWidth));
    bounds.removeFromLeft(margin);
    nextPresetButton.setBounds(bounds.removeFromLeft(squareButtonWidth));
    bounds.removeFromLeft(margin);
    deleteButton.setBounds(bounds.removeFromLeft(squareButtonWidth));
}

void PresetPanel::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    // TODO need ability to reselect selected preset
//    DBG(presetList.getItemText(presetList.getSelectedItemIndex()));
    
    if (comboBoxThatHasChanged == &presetList)
    {
        presetManager.loadPreset(presetList.getItemText(presetList.getSelectedItemIndex()));
    }
}

void PresetPanel::loadPresetList()
{
    presetList.clear(juce::dontSendNotification);
    const auto allPresets = presetManager.getAllPresets();
    const auto currentPreset = presetManager.getCurrentPreset();
    presetList.addItemList(allPresets, 1);
    presetList.setSelectedItemIndex(allPresets.indexOf(currentPreset), juce::dontSendNotification);
}
