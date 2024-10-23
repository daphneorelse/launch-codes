/*
  ==============================================================================

    PresetManager.cpp
    Author:  Akash Murthy & Daphne Wilkerson

  ==============================================================================
*/

#include "PresetManager.h"

const juce::File PresetManager::defaultDirectory {
    juce::File::getSpecialLocation(juce::File::SpecialLocationType::commonDocumentsDirectory)
        .getChildFile(ProjectInfo::companyName)
        .getChildFile(ProjectInfo::projectName)
};
const juce::String PresetManager::extension { "preset" };
//const juce::String PresetManager::presetNameProperty { "presetName" };

PresetManager::PresetManager(juce::AudioProcessorValueTreeState& t_apvts, juce::ValueTree t_nonParamTree)
: apvts(t_apvts)
, nonParamTree(t_nonParamTree)
, settingsTree(nonParamTree.getChildWithName(ParamIDs::settingsTree))
{
    // Create a default Preset Directory, if it doesn't exist
    if (!defaultDirectory.exists())
    {
        const auto result = defaultDirectory.createDirectory();
        if (result.failed())
        {
            DBG("Could not create preset directory: " + result.getErrorMessage());
            jassertfalse;
        }
    }

//    settingsTree.addListener(this);
    currentPreset.referTo(settingsTree.getPropertyAsValue(ParamIDs::currentPreset, nullptr));
}

void PresetManager::savePreset(const juce::String& presetName)
{
    if (presetName.isEmpty())
        return;

    currentPreset.setValue(presetName);
    const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
    
    std::unique_ptr<juce::XmlElement> xml = getStateXml();
    
    if (!xml->writeTo(presetFile))
    {
        DBG("Could not create preset file: " + presetFile.getFullPathName());
        jassertfalse;
    }
}

void PresetManager::deletePreset(const juce::String& presetName)
{
    if (presetName.isEmpty())
        return;

    const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
    if (!presetFile.existsAsFile())
    {
        DBG("Preset file " + presetFile.getFullPathName() + " does not exist");
        jassertfalse;
        return;
    }
    if (!presetFile.deleteFile())
    {
        DBG("Preset file " + presetFile.getFullPathName() + " could not be deleted");
        jassertfalse;
        return;
    }
    currentPreset.setValue("");
}

void PresetManager::loadPreset(const juce::String& presetName)
{
    if (presetName.isEmpty())
        return;

    const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
    if (!presetFile.existsAsFile())
    {
        DBG("Preset file " + presetFile.getFullPathName() + " does not exist");
        jassertfalse;
        return;
    }
    
    juce::XmlDocument xmlDocument { presetFile };
    
    std::unique_ptr<juce::XmlElement> xmlState = xmlDocument.getDocumentElement();
    loadStateFromXml(std::move(xmlState));

    currentPreset.setValue(presetName);
}

int PresetManager::loadNextPreset()
{
    const auto allPresets = getAllPresets();
    if (allPresets.isEmpty())
        return -1;
    const auto currentIndex = allPresets.indexOf(currentPreset.toString());
    const auto nextIndex = currentIndex + 1 > (allPresets.size() - 1) ? 0 : currentIndex + 1;
    loadPreset(allPresets.getReference(nextIndex));
    return nextIndex;
}

int PresetManager::loadPreviousPreset()
{
    const auto allPresets = getAllPresets();
    if (allPresets.isEmpty())
        return -1;
    const auto currentIndex = allPresets.indexOf(currentPreset.toString());
    const auto previousIndex = currentIndex - 1 < 0 ? allPresets.size() - 1 : currentIndex - 1;
    loadPreset(allPresets.getReference(previousIndex));
    return previousIndex;
}

juce::StringArray PresetManager::getAllPresets() const
{
    juce::StringArray presets;
    const auto fileArray = defaultDirectory.findChildFiles(juce::File::TypesOfFileToFind::findFiles, false, "*." + extension);
    for (const auto& file : fileArray)
    {
        presets.add(file.getFileNameWithoutExtension());
    }
    return presets;
}

juce::String PresetManager::getCurrentPreset() const
{
    return currentPreset.toString();
}

std::unique_ptr<juce::XmlElement> PresetManager::getStateXml()
{
    std::unique_ptr<juce::XmlElement> xmlParent (new juce::XmlElement("parent"));

    std::unique_ptr<juce::XmlElement> xmlAPVTS (apvts.copyState().createXml());
    xmlParent->addChildElement(xmlAPVTS.release());

    std::unique_ptr<juce::XmlElement> xmlNonAPVTS (nonParamTree.createXml());
    xmlParent->addChildElement(xmlNonAPVTS.release());
    
    return xmlParent;
}

void PresetManager::loadStateFromXml(std::unique_ptr<juce::XmlElement> xml)
{
    juce::XmlElement* xmlAPVTS = xml->getChildByName(apvts.state.getType());
    juce::XmlElement* xmlNonAPVTS = xml->getChildByName(nonParamTree.getType());

    if (xml.get() != nullptr)
    {
        if (xmlAPVTS != nullptr)
            if (xmlAPVTS->hasTagName(apvts.state.getType()))
                apvts.replaceState(juce::ValueTree::fromXml(*xmlAPVTS));

        if (xmlNonAPVTS != nullptr)
            if (xmlNonAPVTS->hasTagName(nonParamTree.getType()))
                overwriteAllValueTreeProperties(juce::ValueTree::fromXml(*xmlNonAPVTS), nonParamTree);
    }
}

void PresetManager::overwriteAllValueTreeProperties(juce::ValueTree treeToCopy, juce::ValueTree destTree)
{
    jassert(destTree.hasType(treeToCopy.getType()));
    
//    DBG(treeToCopy.getType().toString() + ", " + juce::String{treeToCopy.getNumProperties()});
//    DBG(destTree.getType().toString() + ", " + juce::String{destTree.getNumProperties()});
    
    for (int i = 0; i < treeToCopy.getNumProperties(); i++)
    {
        auto propertyName = treeToCopy.getPropertyName(i);
        jassert(destTree.hasProperty(propertyName));
        
        auto val = treeToCopy.getProperty(propertyName);
        destTree.setProperty(propertyName, val, nullptr);
    }
    
    for (int i = 0; i < treeToCopy.getNumChildren(); i++)
    {
        juce::ValueTree copyChild = treeToCopy.getChild(i);
        juce::ValueTree destChild = destTree.getChildWithName(copyChild.getType());
        jassert(destChild.isValid());
        
//        DBG(copyChild.getType().toString() + " has index: " + juce::String(treeToCopy.indexOf(copyChild)) + ", " + destChild.getType().toString() + " has index: " + juce::String(destTree.indexOf(destChild)));
        
        destTree.moveChild(destTree.indexOf(destChild), treeToCopy.indexOf(copyChild), nullptr);
        
        overwriteAllValueTreeProperties(copyChild, destChild);
    }
}
