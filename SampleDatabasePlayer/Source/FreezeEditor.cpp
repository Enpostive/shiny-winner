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

#include "FreezeEditor.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
FreezeEditor::FreezeEditor ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    leftWaveformScope.reset (new ColouredScope());
    addAndMakeVisible (leftWaveformScope.get());
    leftWaveformScope->setName ("leftWaveformScope");

    leftWaveformScope->setBounds (8, 0, 584, 64);

    rightWaveformScope.reset (new ColouredScope());
    addAndMakeVisible (rightWaveformScope.get());
    rightWaveformScope->setName ("rightWaveformScope");

    rightWaveformScope->setBounds (8, 64, 584, 64);

    leftEnvelopeScope.reset (new ColouredScope());
    addAndMakeVisible (leftEnvelopeScope.get());
    leftEnvelopeScope->setName ("leftEnvelopeScope");

    leftEnvelopeScope->setBounds (8, 0, 584, 64);

    rightEnvelopeScope.reset (new ColouredScope());
    addAndMakeVisible (rightEnvelopeScope.get());
    rightEnvelopeScope->setName ("rightEnvelopeScope");

    rightEnvelopeScope->setBounds (8, 64, 584, 64);

    juce__label.reset (new juce::Label ("new label",
                                        TRANS ("Clumping Frequency")));
    addAndMakeVisible (juce__label.get());
    juce__label->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    juce__label->setJustificationType (juce::Justification::centredLeft);
    juce__label->setEditable (false, false, false);
    juce__label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    juce__label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    juce__label->setBounds (0, 128, 150, 24);

    clumpingFrequency.reset (new juce::Slider ("clumpingFrequency"));
    addAndMakeVisible (clumpingFrequency.get());
    clumpingFrequency->setRange (10, 1000, 1);
    clumpingFrequency->setSliderStyle (juce::Slider::LinearHorizontal);
    clumpingFrequency->setTextBoxStyle (juce::Slider::TextBoxLeft, false, 80, 20);
    clumpingFrequency->addListener (this);

    clumpingFrequency->setBounds (136, 128, 238, 24);

    juce__label2.reset (new juce::Label ("new label",
                                         TRANS ("Remove Threshold")));
    addAndMakeVisible (juce__label2.get());
    juce__label2->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    juce__label2->setJustificationType (juce::Justification::centredLeft);
    juce__label2->setEditable (false, false, false);
    juce__label2->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    juce__label2->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    juce__label2->setBounds (0, 152, 150, 24);

    removeThreshold.reset (new juce::Slider ("removeThreshold"));
    addAndMakeVisible (removeThreshold.get());
    removeThreshold->setRange (0, 20, 0.1);
    removeThreshold->setSliderStyle (juce::Slider::LinearHorizontal);
    removeThreshold->setTextBoxStyle (juce::Slider::TextBoxLeft, false, 80, 20);
    removeThreshold->addListener (this);

    removeThreshold->setBounds (136, 152, 238, 24);

    juce__label3.reset (new juce::Label ("new label",
                                         TRANS ("Reshape Amount")));
    addAndMakeVisible (juce__label3.get());
    juce__label3->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    juce__label3->setJustificationType (juce::Justification::centredLeft);
    juce__label3->setEditable (false, false, false);
    juce__label3->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    juce__label3->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    juce__label3->setBounds (0, 240, 150, 24);

    reshapeAmount.reset (new juce::Slider ("reshapeAmount"));
    addAndMakeVisible (reshapeAmount.get());
    reshapeAmount->setRange (0, 100, 1);
    reshapeAmount->setSliderStyle (juce::Slider::LinearHorizontal);
    reshapeAmount->setTextBoxStyle (juce::Slider::TextBoxLeft, false, 80, 20);
    reshapeAmount->addListener (this);

    reshapeAmount->setBounds (0, 264, 238, 24);

    analysisFeedback.reset (new juce::Label ("analysisFeedback",
                                             juce::String()));
    addAndMakeVisible (analysisFeedback.get());
    analysisFeedback->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    analysisFeedback->setJustificationType (juce::Justification::centredRight);
    analysisFeedback->setEditable (false, false, false);
    analysisFeedback->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    analysisFeedback->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    analysisFeedback->setBounds (368, 128, 230, 24);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 292);


    //[Constructor] You can add your own custom stuff here..
 leftWaveformScope->source = &leftWaveformSource;
 rightWaveformScope->source = &rightWaveformSource;
 leftEnvelopeScope->source = &leftEnvelopeSource;
 rightEnvelopeScope->source = &rightEnvelopeSource;
 leftWaveformScope->drawBackground = false;
 rightWaveformScope->drawBackground = false;
 leftWaveformScope->reverse = false;
 rightWaveformScope->reverse = false;
 leftWaveformScope->centreEnable = true;
 rightWaveformScope->centreEnable = true;
 leftWaveformScope->setVerticalScale(0.4);
 rightWaveformScope->setVerticalScale(0.4);
 leftWaveformSource.defaultColour = juce::Colours::lime;
 rightWaveformSource.defaultColour = juce::Colours::magenta;

 leftEnvelopeScope->drawBackground = false;
 rightEnvelopeScope->drawBackground = false;
 leftEnvelopeScope->reverse = false;
 rightEnvelopeScope->reverse = false;
 leftEnvelopeScope->centreEnable = false;
 rightEnvelopeScope->centreEnable = false;
 leftEnvelopeScope->setVerticalScale(0.4);
 rightEnvelopeScope->setVerticalScale(0.4);
 leftEnvelopeScope->fillEnable = false;
 rightEnvelopeScope->fillEnable = false;
 leftEnvelopeScope->strokeEnable = true;
 rightEnvelopeScope->strokeEnable = true;

 clumpingFrequency->setDoubleClickReturnValue(true, 200.);
 removeThreshold->setDoubleClickReturnValue(true, 5.);
 reshapeAmount->setDoubleClickReturnValue(true, 0.);
    //[/Constructor]
}

FreezeEditor::~FreezeEditor()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    leftWaveformScope = nullptr;
    rightWaveformScope = nullptr;
    leftEnvelopeScope = nullptr;
    rightEnvelopeScope = nullptr;
    juce__label = nullptr;
    clumpingFrequency = nullptr;
    juce__label2 = nullptr;
    removeThreshold = nullptr;
    juce__label3 = nullptr;
    reshapeAmount = nullptr;
    analysisFeedback = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void FreezeEditor::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (juce::Colour (0xff323e44));

    {
        int x = 8, y = 0, width = 584, height = 128;
        juce::Colour fillColour = juce::Colours::black;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void FreezeEditor::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void FreezeEditor::sliderValueChanged (juce::Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == clumpingFrequency.get())
    {
        //[UserSliderCode_clumpingFrequency] -- add your slider handling code here..
     if (onClumpingFrequencyChange) onClumpingFrequencyChange(clumpingFrequency->getValue());
        //[/UserSliderCode_clumpingFrequency]
    }
    else if (sliderThatWasMoved == removeThreshold.get())
    {
        //[UserSliderCode_removeThreshold] -- add your slider handling code here..
     if (onRemoveThresholdChange) onRemoveThresholdChange(removeThreshold->getValue()*0.01);
        //[/UserSliderCode_removeThreshold]
    }
    else if (sliderThatWasMoved == reshapeAmount.get())
    {
        //[UserSliderCode_reshapeAmount] -- add your slider handling code here..
     if (onReshapeAmountChange) onReshapeAmountChange(reshapeAmount->getValue()*0.01);
        //[/UserSliderCode_reshapeAmount]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void FreezeEditor::updateScopes()
{
 leftWaveformScope->update();
 rightWaveformScope->update();
 leftEnvelopeScope->update();
 rightEnvelopeScope->update();

 leftWaveformScope->repaint();
 rightWaveformScope->repaint();
 leftEnvelopeScope->repaint();
 rightEnvelopeScope->repaint();
}

void FreezeEditor::resetParameters(float clump, float remove, float reshape)
{
 clumpingFrequency->setValue(clump, juce::dontSendNotification);
 removeThreshold->setValue(remove*100., juce::dontSendNotification);
 reshapeAmount->setValue(reshape*100., juce::dontSendNotification);
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="FreezeEditor" componentName=""
                 parentClasses="public juce::Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="600" initialHeight="292">
  <BACKGROUND backgroundColour="ff323e44">
    <RECT pos="8 0 584 128" fill="solid: ff000000" hasStroke="0"/>
  </BACKGROUND>
  <GENERICCOMPONENT name="leftWaveformScope" id="e61fe2f06bd4f870" memberName="leftWaveformScope"
                    virtualName="" explicitFocusOrder="0" pos="8 0 584 64" class="ColouredScope"
                    params=""/>
  <GENERICCOMPONENT name="rightWaveformScope" id="1fd3105919cd4eef" memberName="rightWaveformScope"
                    virtualName="" explicitFocusOrder="0" pos="8 64 584 64" class="ColouredScope"
                    params=""/>
  <GENERICCOMPONENT name="leftEnvelopeScope" id="237360d9ee9fbe98" memberName="leftEnvelopeScope"
                    virtualName="" explicitFocusOrder="0" pos="8 0 584 64" class="ColouredScope"
                    params=""/>
  <GENERICCOMPONENT name="rightEnvelopeScope" id="f49fcc143f56c533" memberName="rightEnvelopeScope"
                    virtualName="" explicitFocusOrder="0" pos="8 64 584 64" class="ColouredScope"
                    params=""/>
  <LABEL name="new label" id="9373f0f1bbb140ce" memberName="juce__label"
         virtualName="" explicitFocusOrder="0" pos="0 128 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Clumping Frequency" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <SLIDER name="clumpingFrequency" id="30c041eab425c64a" memberName="clumpingFrequency"
          virtualName="" explicitFocusOrder="0" pos="136 128 238 24" min="10.0"
          max="1000.0" int="1.0" style="LinearHorizontal" textBoxPos="TextBoxLeft"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="new label" id="cd402b46bbf80d5c" memberName="juce__label2"
         virtualName="" explicitFocusOrder="0" pos="0 152 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Remove Threshold" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <SLIDER name="removeThreshold" id="26d7d820943adbf1" memberName="removeThreshold"
          virtualName="" explicitFocusOrder="0" pos="136 152 238 24" min="0.0"
          max="20.0" int="0.1" style="LinearHorizontal" textBoxPos="TextBoxLeft"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="new label" id="607795e9953e6471" memberName="juce__label3"
         virtualName="" explicitFocusOrder="0" pos="0 240 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Reshape Amount" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <SLIDER name="reshapeAmount" id="5f4369e25c800c6b" memberName="reshapeAmount"
          virtualName="" explicitFocusOrder="0" pos="0 264 238 24" min="0.0"
          max="100.0" int="1.0" style="LinearHorizontal" textBoxPos="TextBoxLeft"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="analysisFeedback" id="1b1fa3cf673d21fe" memberName="analysisFeedback"
         virtualName="" explicitFocusOrder="0" pos="368 128 230 24" edTextCol="ff000000"
         edBkgCol="0" labelText="" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="34"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

