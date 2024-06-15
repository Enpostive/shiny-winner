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

    sampleEditor.reset (new SampleEditor());
    addAndMakeVisible (sampleEditor.get());
    sampleEditor->setName ("Sample Editor");

    sampleEditor->setBounds (0, 0, 600, 292);

    freeze.reset (new juce::ToggleButton ("freeze"));
    addAndMakeVisible (freeze.get());
    freeze->setButtonText (TRANS ("Freeze"));
    freeze->addListener (this);

    freeze->setBounds (0, 296, 200, 24);

    freezeEditor.reset (new FreezeEditor());
    addAndMakeVisible (freezeEditor.get());
    freezeEditor->setName ("Freeze Editor");

    freezeEditor->setBounds (0, 0, 600, 292);


    //[UserPreSize]
    for (auto &child: getChildren())
 {
  child->setComponentID(child->getName());
 }

 freezeEditor->setVisible(false);
    //[/UserPreSize]

    setSize (600, 600);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

Controls::~Controls()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    juce__label = nullptr;
    playbacklevel = nullptr;
    samplelengthmode = nullptr;
    mslength = nullptr;
    fraclength = nullptr;
    fadeout = nullptr;
    juce__label2 = nullptr;
    sampleEditor = nullptr;
    freeze = nullptr;
    freezeEditor = nullptr;


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
        int x = 8, y = 420, width = 580, height = 180;
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

    if (comboBoxThatHasChanged == samplelengthmode.get())
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

    if (buttonThatWasClicked == freeze.get())
    {
        //[UserButtonCode_freeze] -- add your button handler code here..
     bool freezeStatus = freeze->getToggleState();
     sampleEditor->setVisible(!freezeStatus);
     freezeEditor->setVisible(freezeStatus);
     mslength->setEnabled(!freezeStatus);
     fraclength->setEnabled(!freezeStatus);
     samplelengthmode->setEnabled(!freezeStatus);
        //[/UserButtonCode_freeze]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
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
    <RECT pos="8 420 580 180" fill="solid: ff000000" hasStroke="0"/>
  </BACKGROUND>
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
  <GENERICCOMPONENT name="Sample Editor" id="51abe6d13b528605" memberName="sampleEditor"
                    virtualName="" explicitFocusOrder="0" pos="0 0 600 292" class="SampleEditor"
                    params=""/>
  <TOGGLEBUTTON name="freeze" id="104c3ffe8e1d100e" memberName="freeze" virtualName=""
                explicitFocusOrder="0" pos="0 296 200 24" buttonText="Freeze"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <GENERICCOMPONENT name="Freeze Editor" id="d29b91544b28f1d" memberName="freezeEditor"
                    virtualName="" explicitFocusOrder="0" pos="0 0 600 292" class="FreezeEditor"
                    params=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

