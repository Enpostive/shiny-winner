/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 8.0.0

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) - Raw Material Software Limited.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include <JuceHeader.h>
#include "ProgressSpinner.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class DatabaseControls  : public juce::Component,
                          public juce::Button::Listener,
                          public juce::ComboBox::Listener,
                          public juce::Slider::Listener
{
public:
    //==============================================================================
    DatabaseControls ();
    ~DatabaseControls() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    juce::TableListBox* listBox()
    { return sampleList.get(); }

 std::function<void ()> onImportClicked;
 std::function<void ()> onPreviewClicked;
 std::function<void (const juce::String&)> onSearchStringChanged;
 std::function<void (int)> onFilterChange;
 std::function<void ()> onSettingsClicked;

 void resetFilterItems();
 void addFilterItem(const juce::String &item, int itemId);

 float getReferenceLevel()
 { if (referenceLevel) return referenceLevel->getValue(); else return 0.; }

 bool isKSelected()
 { if (krmsSelect) return krmsSelect->getToggleState(); else return false; }

 void setProgressSpinnerVisible(bool shouldBeVisible)
 { if (progressSpinner) progressSpinner->setVisible(shouldBeVisible); }
    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;
    void buttonClicked (juce::Button* buttonThatWasClicked) override;
    void comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged) override;
    void sliderValueChanged (juce::Slider* sliderThatWasMoved) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<juce::TableListBox> sampleList;
    std::unique_ptr<juce::TextButton> importButton;
    std::unique_ptr<juce::TextButton> previewButton;
    std::unique_ptr<juce::TextEditor> searchbar;
    std::unique_ptr<juce::ComboBox> categoryFilter;
    std::unique_ptr<juce::ToggleButton> krmsSelect;
    std::unique_ptr<juce::Slider> referenceLevel;
    std::unique_ptr<juce::TextButton> audioSettings;
    std::unique_ptr<ProgressSpinner> progressSpinner;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DatabaseControls)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

