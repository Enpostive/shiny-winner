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

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "DatabaseControls.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
DatabaseControls::DatabaseControls ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    sampleList.reset (new juce::TableListBox ("Sample List"));
    addAndMakeVisible (sampleList.get());

    importButton.reset (new juce::TextButton ("Import"));
    addAndMakeVisible (importButton.get());
    importButton->setButtonText (TRANS ("Import..."));
    importButton->addListener (this);

    previewButton.reset (new juce::TextButton ("Preview"));
    addAndMakeVisible (previewButton.get());
    previewButton->addListener (this);

    searchbar.reset (new juce::TextEditor ("searchbar"));
    addAndMakeVisible (searchbar.get());
    searchbar->setMultiLine (false);
    searchbar->setReturnKeyStartsNewLine (false);
    searchbar->setReadOnly (false);
    searchbar->setScrollbarsShown (true);
    searchbar->setCaretVisible (true);
    searchbar->setPopupMenuEnabled (true);
    searchbar->setText (juce::String());

    categoryFilter.reset (new juce::ComboBox ("categoryFilter"));
    addAndMakeVisible (categoryFilter.get());
    categoryFilter->setEditableText (false);
    categoryFilter->setJustificationType (juce::Justification::centredLeft);
    categoryFilter->setTextWhenNothingSelected (TRANS ("Filter by...."));
    categoryFilter->setTextWhenNoChoicesAvailable (TRANS ("(no choices)"));
    categoryFilter->addListener (this);

    categoryFilter->setBounds (0, 0, 160, 24);


    //[UserPreSize]

 searchbar->onTextChange = [&]()
 {
  if (onSearchStringChanged) onSearchStringChanged(searchbar->getText());
 };
 searchbar->setTextToShowWhenEmpty("Search samples...", juce::Colours::white.withBrightness(0.5));
 searchbar->setSelectAllWhenFocused(true);

 categoryFilter->onChange = [&]()
 {
  if (onFilterChange) onFilterChange(categoryFilter->getSelectedId());
 };

    //[/UserPreSize]

    setSize (800, 400);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

DatabaseControls::~DatabaseControls()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    sampleList = nullptr;
    importButton = nullptr;
    previewButton = nullptr;
    searchbar = nullptr;
    categoryFilter = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void DatabaseControls::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (juce::Colour (0xff323e44));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void DatabaseControls::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    sampleList->setBounds (0, 24, getWidth() - 168, getHeight() - 25);
    importButton->setBounds (getWidth() - 158, 8, 150, 24);
    previewButton->setBounds (getWidth() - 158, 40, 150, 24);
    searchbar->setBounds (160, 0, getWidth() - 328, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void DatabaseControls::buttonClicked (juce::Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == importButton.get())
    {
        //[UserButtonCode_importButton] -- add your button handler code here..
     if (onImportClicked) onImportClicked();
        //[/UserButtonCode_importButton]
    }
    else if (buttonThatWasClicked == previewButton.get())
    {
        //[UserButtonCode_previewButton] -- add your button handler code here..
     if (onPreviewClicked) onPreviewClicked();
        //[/UserButtonCode_previewButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void DatabaseControls::comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == categoryFilter.get())
    {
        //[UserComboBoxCode_categoryFilter] -- add your combo box handling code here..
        //[/UserComboBoxCode_categoryFilter]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

void DatabaseControls::resetFilterItems()
{
 if (categoryFilter)
 {
  categoryFilter->clear(juce::dontSendNotification);
 }
}

void DatabaseControls::addFilterItem(const juce::String &item, int itemId)
{
 if (categoryFilter)
 {
  categoryFilter->addItem(item, itemId);
 }
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="DatabaseControls" componentName=""
                 parentClasses="public juce::Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="800" initialHeight="400">
  <BACKGROUND backgroundColour="ff323e44"/>
  <TREEVIEW name="Sample List" id="dde63c33743ef72e" memberName="sampleList"
            virtualName="juce::TableListBox" explicitFocusOrder="0" pos="0 24 168M 25M"
            rootVisible="1" openByDefault="0"/>
  <TEXTBUTTON name="Import" id="9be56120a7e53174" memberName="importButton"
              virtualName="" explicitFocusOrder="0" pos="158R 8 150 24" buttonText="Import..."
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="Preview" id="7040713001c363e4" memberName="previewButton"
              virtualName="" explicitFocusOrder="0" pos="158R 40 150 24" buttonText="Preview"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTEDITOR name="searchbar" id="f2be597637359fb5" memberName="searchbar"
              virtualName="" explicitFocusOrder="0" pos="160 0 328M 24" initialText=""
              multiline="0" retKeyStartsLine="0" readonly="0" scrollbars="1"
              caret="1" popupmenu="1"/>
  <COMBOBOX name="categoryFilter" id="ef06d654082523d0" memberName="categoryFilter"
            virtualName="" explicitFocusOrder="0" pos="0 0 160 24" editable="0"
            layout="33" items="" textWhenNonSelected="Filter by...." textWhenNoItems="(no choices)"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

