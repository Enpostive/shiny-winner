/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 7.0.12

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2020 - Raw Material Software Limited.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include <JuceHeader.h>
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
                          public juce::ComboBox::Listener
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

 void resetFilterItems();
 void addFilterItem(const juce::String &item, int itemId);
    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;
    void buttonClicked (juce::Button* buttonThatWasClicked) override;
    void comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<juce::TableListBox> sampleList;
    std::unique_ptr<juce::TextButton> importButton;
    std::unique_ptr<juce::TextButton> previewButton;
    std::unique_ptr<juce::TextEditor> searchbar;
    std::unique_ptr<juce::ComboBox> categoryFilter;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DatabaseControls)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

