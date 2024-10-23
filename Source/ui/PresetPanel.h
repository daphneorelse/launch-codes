/*
  ==============================================================================

    PresetPanel.h
    Author:  Akash Murthy, edited by Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ComponentIDs.h"
#include "FunctionalButton.h"
#include "lnfs/PresetPanelLNF.h"
#include "../service/PresetManager.h"

//==============================================================================
/*
*/
class PresetPanel  : public juce::Component
                   , public juce::ComboBox::Listener
{
public:
    PresetPanel(PresetManager& pm);
    ~PresetPanel() override;

    void resized() override;
    
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
    
    void loadPresetList();

private:
    PresetManager& presetManager;
    std::unique_ptr<juce::Drawable> saveIcon;
    std::unique_ptr<juce::Drawable> deleteIcon;
    std::unique_ptr<juce::Drawable> previousIcon;
    std::unique_ptr<juce::Drawable> nextIcon;
    FunctionalButton saveButton, deleteButton, previousPresetButton, nextPresetButton;
    juce::ComboBox presetList;
    std::unique_ptr<juce::FileChooser> fileChooser;
    
    PresetPanelLNF lnf;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetPanel)
};
