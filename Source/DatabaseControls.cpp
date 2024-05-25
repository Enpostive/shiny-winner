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

    refineParameter.reset (new juce::Slider ("Refine Parameter"));
    addAndMakeVisible (refineParameter.get());
    refineParameter->setTooltip (TRANS ("Refine Parameter"));
    refineParameter->setRange (0, 10, 1);
    refineParameter->setSliderStyle (juce::Slider::LinearHorizontal);
    refineParameter->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    refineParameter->addListener (this);

    juce__label.reset (new juce::Label ("new label",
                                        TRANS ("Refine")));
    addAndMakeVisible (juce__label.get());
    juce__label->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    juce__label->setJustificationType (juce::Justification::centredLeft);
    juce__label->setEditable (false, false, false);
    juce__label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    juce__label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    clumpingParam.reset (new juce::Slider ("Clumping Frequency"));
    addAndMakeVisible (clumpingParam.get());
    clumpingParam->setTooltip (TRANS ("Clumping Frequency"));
    clumpingParam->setRange (20, 1000, 1);
    clumpingParam->setSliderStyle (juce::Slider::LinearHorizontal);
    clumpingParam->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    clumpingParam->addListener (this);
    clumpingParam->setSkewFactor (0.33333);

    juce__label2.reset (new juce::Label ("new label",
                                         TRANS ("Clumping Freq")));
    addAndMakeVisible (juce__label2.get());
    juce__label2->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    juce__label2->setJustificationType (juce::Justification::centredLeft);
    juce__label2->setEditable (false, false, false);
    juce__label2->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    juce__label2->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    removeParam.reset (new juce::Slider ("Remove Amplitude"));
    addAndMakeVisible (removeParam.get());
    removeParam->setTooltip (TRANS ("Removes maxima that are below a percentage of the highest maxima"));
    removeParam->setRange (0, 1, 0.01);
    removeParam->setSliderStyle (juce::Slider::LinearHorizontal);
    removeParam->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    removeParam->addListener (this);
    removeParam->setSkewFactor (0.33333);

    juce__label3.reset (new juce::Label ("new label",
                                         TRANS ("Remove Amplitude")));
    addAndMakeVisible (juce__label3.get());
    juce__label3->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    juce__label3->setJustificationType (juce::Justification::centredLeft);
    juce__label3->setEditable (false, false, false);
    juce__label3->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    juce__label3->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));


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
    refineParameter = nullptr;
    juce__label = nullptr;
    clumpingParam = nullptr;
    juce__label2 = nullptr;
    removeParam = nullptr;
    juce__label3 = nullptr;


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
    refineParameter->setBounds (getWidth() - 158, 88, 150, 48);
    juce__label->setBounds (getWidth() - 158, 72, 150, 24);
    clumpingParam->setBounds (getWidth() - 158, 152, 150, 48);
    juce__label2->setBounds (getWidth() - 158, 136, 150, 24);
    removeParam->setBounds (getWidth() - 158, 216, 150, 48);
    juce__label3->setBounds (getWidth() - 158, 200, 150, 24);
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

void DatabaseControls::sliderValueChanged (juce::Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == refineParameter.get())
    {
        //[UserSliderCode_refineParameter] -- add your slider handling code here..
        if (onRefineChange) onRefineChange(refineParameter->getValue());
        //[/UserSliderCode_refineParameter]
    }
    else if (sliderThatWasMoved == clumpingParam.get())
    {
        //[UserSliderCode_clumpingParam] -- add your slider handling code here..
     if (onClumpingChange) onClumpingChange(clumpingParam->getValue());
        //[/UserSliderCode_clumpingParam]
    }
    else if (sliderThatWasMoved == removeParam.get())
    {
        //[UserSliderCode_removeParam] -- add your slider handling code here..
     if (onDeleteThreshChange) onDeleteThreshChange(removeParam->getValue());
        //[/UserSliderCode_removeParam]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
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
  <SLIDER name="Refine Parameter" id="5bec8f4e7443f2f7" memberName="refineParameter"
          virtualName="" explicitFocusOrder="0" pos="158R 88 150 48" tooltip="Refine Parameter"
          min="0.0" max="10.0" int="1.0" style="LinearHorizontal" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="new label" id="85ace112fba68362" memberName="juce__label"
         virtualName="" explicitFocusOrder="0" pos="158R 72 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Refine" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
  <SLIDER name="Clumping Frequency" id="510109d5eadfe0b3" memberName="clumpingParam"
          virtualName="" explicitFocusOrder="0" pos="158R 152 150 48" tooltip="Clumping Frequency"
          min="20.0" max="1000.0" int="1.0" style="LinearHorizontal" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="0.33333"
          needsCallback="1"/>
  <LABEL name="new label" id="b198a443c14bd587" memberName="juce__label2"
         virtualName="" explicitFocusOrder="0" pos="158R 136 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Clumping Freq" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <SLIDER name="Remove Amplitude" id="3a67948d337266f6" memberName="removeParam"
          virtualName="" explicitFocusOrder="0" pos="158R 216 150 48" tooltip="Removes maxima that are below a percentage of the highest maxima"
          min="0.0" max="1.0" int="0.01" style="LinearHorizontal" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="0.33333"
          needsCallback="1"/>
  <LABEL name="new label" id="9bee57412bed140c" memberName="juce__label3"
         virtualName="" explicitFocusOrder="0" pos="158R 200 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Remove Amplitude" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

