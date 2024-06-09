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

#include "Controls.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
Controls::Controls ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    sampleListBox.reset (new juce::TableListBox ("sampleListBox"));
    addAndMakeVisible (sampleListBox.get());

    sampleListBox->setBounds (0, 0, 600, 180);

    juce__label.reset (new juce::Label ("new label",
                                        TRANS ("Playback KRMS")));
    addAndMakeVisible (juce__label.get());
    juce__label->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    juce__label->setJustificationType (juce::Justification::centredLeft);
    juce__label->setEditable (false, false, false);
    juce__label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    juce__label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    juce__label->setBounds (0, 368, 150, 24);

    playbacklevel.reset (new juce::Slider ("playbacklevel"));
    addAndMakeVisible (playbacklevel.get());
    playbacklevel->setRange (0, 10, 0);
    playbacklevel->setSliderStyle (juce::Slider::LinearHorizontal);
    playbacklevel->setTextBoxStyle (juce::Slider::TextBoxRight, false, 80, 20);
    playbacklevel->addListener (this);

    playbacklevel->setBounds (0, 392, 200, 24);

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

    samplelengthmode.reset (new juce::ComboBox ("samplelengthmode"));
    addAndMakeVisible (samplelengthmode.get());
    samplelengthmode->setEditableText (false);
    samplelengthmode->setJustificationType (juce::Justification::centredLeft);
    samplelengthmode->setTextWhenNothingSelected (juce::String());
    samplelengthmode->setTextWhenNoChoicesAvailable (TRANS ("(no choices)"));
    samplelengthmode->addItem (TRANS ("ms"), 1);
    samplelengthmode->addItem (TRANS ("% of 1/4 beat"), 2);
    samplelengthmode->addListener (this);

    samplelengthmode->setBounds (200, 368, 198, 24);

    mslength.reset (new juce::Slider ("mslength"));
    addAndMakeVisible (mslength.get());
    mslength->setRange (0, 10, 0);
    mslength->setSliderStyle (juce::Slider::LinearHorizontal);
    mslength->setTextBoxStyle (juce::Slider::TextBoxRight, false, 80, 20);
    mslength->addListener (this);

    mslength->setBounds (200, 392, 200, 24);

    fraclength.reset (new juce::Slider ("fraclength"));
    addAndMakeVisible (fraclength.get());
    fraclength->setRange (0, 10, 0);
    fraclength->setSliderStyle (juce::Slider::LinearHorizontal);
    fraclength->setTextBoxStyle (juce::Slider::TextBoxLeft, false, 80, 20);
    fraclength->addListener (this);

    fraclength->setBounds (200, 392, 200, 24);

    fadeout.reset (new juce::Slider ("fadeout"));
    addAndMakeVisible (fadeout.get());
    fadeout->setRange (0, 10, 0);
    fadeout->setSliderStyle (juce::Slider::LinearHorizontal);
    fadeout->setTextBoxStyle (juce::Slider::TextBoxRight, false, 80, 20);
    fadeout->addListener (this);

    fadeout->setBounds (400, 392, 200, 24);

    juce__label2.reset (new juce::Label ("new label",
                                         TRANS ("Fadeout")));
    addAndMakeVisible (juce__label2.get());
    juce__label2->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    juce__label2->setJustificationType (juce::Justification::centredLeft);
    juce__label2->setEditable (false, false, false);
    juce__label2->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    juce__label2->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    juce__label2->setBounds (400, 368, 150, 24);

    adjuster.reset (new SampleAdjuster());
    addAndMakeVisible (adjuster.get());
    adjuster->setName ("Adjuster");

    adjuster->setBounds (0, 232, 600, 48);

    unloadButton.reset (new juce::TextButton ("Unload Button"));
    addAndMakeVisible (unloadButton.get());
    unloadButton->setButtonText (TRANS ("Unload"));
    unloadButton->addListener (this);

    unloadButton->setBounds (0, 280, 64, 24);

    selectSlot1Button.reset (new juce::TextButton ("Select Slot 1 Button"));
    addAndMakeVisible (selectSlot1Button.get());
    selectSlot1Button->setButtonText (TRANS ("Slot 1"));
    selectSlot1Button->addListener (this);

    selectSlot1Button->setBounds (80, 280, 150, 24);

    selectSlot2Button.reset (new juce::TextButton ("Select Slot 2 Button"));
    addAndMakeVisible (selectSlot2Button.get());
    selectSlot2Button->setButtonText (TRANS ("Slot 2"));
    selectSlot2Button->addListener (this);

    selectSlot2Button->setBounds (248, 280, 150, 24);

    selectSlot3Button.reset (new juce::TextButton ("Select Slot 3 Button"));
    addAndMakeVisible (selectSlot3Button.get());
    selectSlot3Button->setButtonText (TRANS ("Slot 3"));
    selectSlot3Button->addListener (this);

    selectSlot3Button->setBounds (416, 280, 150, 24);


    //[UserPreSize]
    for (auto &child: getChildren())
 {
  child->setComponentID(child->getName());
 }

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
    //[/UserPreSize]

    setSize (600, 600);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

Controls::~Controls()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    sampleListBox = nullptr;
    juce__label = nullptr;
    playbacklevel = nullptr;
    searchbar = nullptr;
    categoryFilter = nullptr;
    samplelengthmode = nullptr;
    mslength = nullptr;
    fraclength = nullptr;
    fadeout = nullptr;
    juce__label2 = nullptr;
    adjuster = nullptr;
    unloadButton = nullptr;
    selectSlot1Button = nullptr;
    selectSlot2Button = nullptr;
    selectSlot3Button = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void Controls::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (juce::Colour (0xff323e44));

    {
        int x = 0, y = 420, width = 600, height = 180;
        juce::Colour fillColour = juce::Colours::black;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void Controls::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void Controls::sliderValueChanged (juce::Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == playbacklevel.get())
    {
        //[UserSliderCode_playbacklevel] -- add your slider handling code here..
        //[/UserSliderCode_playbacklevel]
    }
    else if (sliderThatWasMoved == mslength.get())
    {
        //[UserSliderCode_mslength] -- add your slider handling code here..
        //[/UserSliderCode_mslength]
    }
    else if (sliderThatWasMoved == fraclength.get())
    {
        //[UserSliderCode_fraclength] -- add your slider handling code here..
        //[/UserSliderCode_fraclength]
    }
    else if (sliderThatWasMoved == fadeout.get())
    {
        //[UserSliderCode_fadeout] -- add your slider handling code here..
        //[/UserSliderCode_fadeout]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

void Controls::comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == categoryFilter.get())
    {
        //[UserComboBoxCode_categoryFilter] -- add your combo box handling code here..
        //[/UserComboBoxCode_categoryFilter]
    }
    else if (comboBoxThatHasChanged == samplelengthmode.get())
    {
        //[UserComboBoxCode_samplelengthmode] -- add your combo box handling code here..
     int mode = samplelengthmode->getSelectedId();
     mslength->setVisible(mode == 1);
     fraclength->setVisible(mode == 2);
        //[/UserComboBoxCode_samplelengthmode]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}

void Controls::buttonClicked (juce::Button* buttonThatWasClicked)
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
void Controls::resetFilterItems()
{
 if (categoryFilter)
 {
  categoryFilter->clear(juce::dontSendNotification);
 }
}

void Controls::addFilterItem(const juce::String &item, int itemId)
{
 if (categoryFilter)
 {
  categoryFilter->addItem(item, itemId);
 }
}

void Controls::connect(std::vector<float> &l,
                       std::vector<float> &r,
                       SampleParameters &p,
                       int length)
{
 adjuster->connect(l, r, p);
 adjuster->updateWaveform(length);
 adjuster->setEnabled(true);
 adjuster->repaint();
}

void Controls::disconnect(int length)
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

<JUCER_COMPONENT documentType="Component" className="Controls" componentName=""
                 parentClasses="public juce::Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="600" initialHeight="600">
  <BACKGROUND backgroundColour="ff323e44">
    <RECT pos="0 420 600 180" fill="solid: ff000000" hasStroke="0"/>
  </BACKGROUND>
  <TREEVIEW name="sampleListBox" id="5cc1b467e1895d2f" memberName="sampleListBox"
            virtualName="juce::TableListBox" explicitFocusOrder="0" pos="0 0 600 180"
            rootVisible="1" openByDefault="0"/>
  <LABEL name="new label" id="266264f56ea77339" memberName="juce__label"
         virtualName="" explicitFocusOrder="0" pos="0 368 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Playback KRMS" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <SLIDER name="playbacklevel" id="10a32d0bcb0543cb" memberName="playbacklevel"
          virtualName="" explicitFocusOrder="0" pos="0 392 200 24" min="0.0"
          max="10.0" int="0.0" style="LinearHorizontal" textBoxPos="TextBoxRight"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <TEXTEDITOR name="searchbar" id="f2be597637359fb5" memberName="searchbar"
              virtualName="" explicitFocusOrder="0" pos="160 180 420 24" initialText=""
              multiline="0" retKeyStartsLine="0" readonly="0" scrollbars="1"
              caret="1" popupmenu="1"/>
  <COMBOBOX name="categoryFilter" id="ef06d654082523d0" memberName="categoryFilter"
            virtualName="" explicitFocusOrder="0" pos="0 180 160 24" editable="0"
            layout="33" items="" textWhenNonSelected="Filter by...." textWhenNoItems="(no choices)"/>
  <COMBOBOX name="samplelengthmode" id="2b3c6c910cd48ee3" memberName="samplelengthmode"
            virtualName="" explicitFocusOrder="0" pos="200 368 198 24" editable="0"
            layout="33" items="ms&#10;% of 1/4 beat" textWhenNonSelected=""
            textWhenNoItems="(no choices)"/>
  <SLIDER name="mslength" id="50172c56e791a43e" memberName="mslength" virtualName=""
          explicitFocusOrder="0" pos="200 392 200 24" min="0.0" max="10.0"
          int="0.0" style="LinearHorizontal" textBoxPos="TextBoxRight"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="fraclength" id="5f3ef3ceeac045c7" memberName="fraclength"
          virtualName="" explicitFocusOrder="0" pos="200 392 200 24" min="0.0"
          max="10.0" int="0.0" style="LinearHorizontal" textBoxPos="TextBoxLeft"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="fadeout" id="b4450cfb3455f294" memberName="fadeout" virtualName=""
          explicitFocusOrder="0" pos="400 392 200 24" min="0.0" max="10.0"
          int="0.0" style="LinearHorizontal" textBoxPos="TextBoxRight"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="new label" id="56a0f024194ef1b8" memberName="juce__label2"
         virtualName="" explicitFocusOrder="0" pos="400 368 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Fadeout" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
  <GENERICCOMPONENT name="Adjuster" id="6d6e626654413295" memberName="adjuster" virtualName="SampleAdjuster"
                    explicitFocusOrder="0" pos="0 232 600 48" class="juce::Component"
                    params=""/>
  <TEXTBUTTON name="Unload Button" id="b03b05dadc196a03" memberName="unloadButton"
              virtualName="" explicitFocusOrder="0" pos="0 280 64 24" buttonText="Unload"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="Select Slot 1 Button" id="86fc0b5cfb9e817b" memberName="selectSlot1Button"
              virtualName="" explicitFocusOrder="0" pos="80 280 150 24" buttonText="Slot 1"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="Select Slot 2 Button" id="57d257772f49f25f" memberName="selectSlot2Button"
              virtualName="" explicitFocusOrder="0" pos="248 280 150 24" buttonText="Slot 2"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="Select Slot 3 Button" id="f71ddfa36d4b9d52" memberName="selectSlot3Button"
              virtualName="" explicitFocusOrder="0" pos="416 280 150 24" buttonText="Slot 3"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

