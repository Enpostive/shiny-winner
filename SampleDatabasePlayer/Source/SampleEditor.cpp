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

#include "SampleEditor.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
SampleEditor::SampleEditor ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    sampleListBox.reset (new juce::TableListBox ("sampleListBox"));
    addAndMakeVisible (sampleListBox.get());

    sampleListBox->setBounds (0, 0, 600, 180);

    searchbar.reset (new juce::TextEditor ("searchbar"));
    addAndMakeVisible (searchbar.get());
    searchbar->setMultiLine (false);
    searchbar->setReturnKeyStartsNewLine (false);
    searchbar->setReadOnly (false);
    searchbar->setScrollbarsShown (true);
    searchbar->setCaretVisible (true);
    searchbar->setPopupMenuEnabled (true);
    searchbar->setText (juce::String());

    searchbar->setBounds (160, 180, 420, 24);

    categoryFilter.reset (new juce::ComboBox ("categoryFilter"));
    addAndMakeVisible (categoryFilter.get());
    categoryFilter->setEditableText (false);
    categoryFilter->setJustificationType (juce::Justification::centredLeft);
    categoryFilter->setTextWhenNothingSelected (TRANS ("Filter by...."));
    categoryFilter->setTextWhenNoChoicesAvailable (TRANS ("(no choices)"));
    categoryFilter->addListener (this);

    categoryFilter->setBounds (0, 180, 160, 24);

    adjuster.reset (new SampleAdjuster());
    addAndMakeVisible (adjuster.get());
    adjuster->setName ("Adjuster");

    adjuster->setBounds (8, 232, 584, 60);

    unloadButton.reset (new juce::TextButton ("Unload Button"));
    addAndMakeVisible (unloadButton.get());
    unloadButton->setButtonText (TRANS ("Unload"));
    unloadButton->addListener (this);

    unloadButton->setBounds (0, 208, 64, 20);

    selectSlot1Button.reset (new juce::TextButton ("Select Slot 1 Button"));
    addAndMakeVisible (selectSlot1Button.get());
    selectSlot1Button->setButtonText (TRANS ("Slot 1"));
    selectSlot1Button->addListener (this);

    selectSlot1Button->setBounds (144, 208, 150, 20);

    selectSlot2Button.reset (new juce::TextButton ("Select Slot 2 Button"));
    addAndMakeVisible (selectSlot2Button.get());
    selectSlot2Button->setButtonText (TRANS ("Slot 2"));
    selectSlot2Button->addListener (this);

    selectSlot2Button->setBounds (296, 208, 150, 20);

    selectSlot3Button.reset (new juce::TextButton ("Select Slot 3 Button"));
    addAndMakeVisible (selectSlot3Button.get());
    selectSlot3Button->setButtonText (TRANS ("Slot 3"));
    selectSlot3Button->addListener (this);

    selectSlot3Button->setBounds (448, 208, 150, 20);


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

 adjuster->setEnabled(false);
 selectSlot1Button->setEnabled(false);

 sampleListBox->setHeaderHeight(10);
    //[/UserPreSize]

    setSize (600, 292);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

SampleEditor::~SampleEditor()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    sampleListBox = nullptr;
    searchbar = nullptr;
    categoryFilter = nullptr;
    adjuster = nullptr;
    unloadButton = nullptr;
    selectSlot1Button = nullptr;
    selectSlot2Button = nullptr;
    selectSlot3Button = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void SampleEditor::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (juce::Colour (0xff323e44));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void SampleEditor::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void SampleEditor::comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged)
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

void SampleEditor::buttonClicked (juce::Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == unloadButton.get())
    {
        //[UserButtonCode_unloadButton] -- add your button handler code here..
  if (onUnload) onUnload();
        //[/UserButtonCode_unloadButton]
    }
    else if (buttonThatWasClicked == selectSlot1Button.get())
    {
        //[UserButtonCode_selectSlot1Button] -- add your button handler code here..
  if (onSlotSelected) onSlotSelected(0);
  selectSlot1Button->setEnabled(false);
  selectSlot2Button->setEnabled(true);
  selectSlot3Button->setEnabled(true);
        //[/UserButtonCode_selectSlot1Button]
    }
    else if (buttonThatWasClicked == selectSlot2Button.get())
    {
        //[UserButtonCode_selectSlot2Button] -- add your button handler code here..
  if (onSlotSelected) onSlotSelected(1);
  selectSlot1Button->setEnabled(true);
  selectSlot2Button->setEnabled(false);
  selectSlot3Button->setEnabled(true);
        //[/UserButtonCode_selectSlot2Button]
    }
    else if (buttonThatWasClicked == selectSlot3Button.get())
    {
        //[UserButtonCode_selectSlot3Button] -- add your button handler code here..
  if (onSlotSelected) onSlotSelected(2);
  selectSlot1Button->setEnabled(true);
  selectSlot2Button->setEnabled(true);
  selectSlot3Button->setEnabled(false);
        //[/UserButtonCode_selectSlot3Button]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void SampleEditor::resetFilterItems()
{
 if (categoryFilter)
 {
  categoryFilter->clear(juce::dontSendNotification);
 }
}

void SampleEditor::addFilterItem(const juce::String &item, int itemId)
{
 if (categoryFilter)
 {
  categoryFilter->addItem(item, itemId);
 }
}

void SampleEditor::connect(std::vector<float> &l,
                       std::vector<float> &r,
                       SampleParameters &p,
                       int length)
{
 adjuster->connect(l, r, p);
 adjuster->updateWaveform(length);
 adjuster->setEnabled(true);
 adjuster->repaint();
}

void SampleEditor::disconnect(int length)
{
 adjuster->disconnect();
 adjuster->updateWaveform(length);
 adjuster->setEnabled(false);
 adjuster->repaint();
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="SampleEditor" componentName=""
                 parentClasses="public juce::Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="600" initialHeight="292">
  <BACKGROUND backgroundColour="ff323e44"/>
  <TREEVIEW name="sampleListBox" id="5cc1b467e1895d2f" memberName="sampleListBox"
            virtualName="juce::TableListBox" explicitFocusOrder="0" pos="0 0 600 180"
            rootVisible="1" openByDefault="0"/>
  <TEXTEDITOR name="searchbar" id="f2be597637359fb5" memberName="searchbar"
              virtualName="" explicitFocusOrder="0" pos="160 180 420 24" initialText=""
              multiline="0" retKeyStartsLine="0" readonly="0" scrollbars="1"
              caret="1" popupmenu="1"/>
  <COMBOBOX name="categoryFilter" id="ef06d654082523d0" memberName="categoryFilter"
            virtualName="" explicitFocusOrder="0" pos="0 180 160 24" editable="0"
            layout="33" items="" textWhenNonSelected="Filter by...." textWhenNoItems="(no choices)"/>
  <GENERICCOMPONENT name="Adjuster" id="6d6e626654413295" memberName="adjuster" virtualName="SampleAdjuster"
                    explicitFocusOrder="0" pos="8 232 584 60" class="juce::Component"
                    params=""/>
  <TEXTBUTTON name="Unload Button" id="b03b05dadc196a03" memberName="unloadButton"
              virtualName="" explicitFocusOrder="0" pos="0 208 64 20" buttonText="Unload"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="Select Slot 1 Button" id="86fc0b5cfb9e817b" memberName="selectSlot1Button"
              virtualName="" explicitFocusOrder="0" pos="144 208 150 20" buttonText="Slot 1"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="Select Slot 2 Button" id="57d257772f49f25f" memberName="selectSlot2Button"
              virtualName="" explicitFocusOrder="0" pos="296 208 150 20" buttonText="Slot 2"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="Select Slot 3 Button" id="f71ddfa36d4b9d52" memberName="selectSlot3Button"
              virtualName="" explicitFocusOrder="0" pos="448 208 150 20" buttonText="Slot 3"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

