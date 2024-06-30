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

#include "ControlLayout.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
ControlLayout::ControlLayout ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    juce__label.reset (new juce::Label ("new label",
                                        TRANS ("Clumping Frequency")));
    addAndMakeVisible (juce__label.get());
    juce__label->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    juce__label->setJustificationType (juce::Justification::centredLeft);
    juce__label->setEditable (false, false, false);
    juce__label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    juce__label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    juce__label->setBounds (0, 0, 150, 24);

    clumpingFrequency.reset (new juce::Slider ("clumpingFrequency"));
    addAndMakeVisible (clumpingFrequency.get());
    clumpingFrequency->setRange (10, 1000, 1);
    clumpingFrequency->setSliderStyle (juce::Slider::LinearHorizontal);
    clumpingFrequency->setTextBoxStyle (juce::Slider::TextBoxLeft, false, 80, 20);
    clumpingFrequency->addListener (this);

    clumpingFrequency->setBounds (136, 0, 238, 24);

    juce__label2.reset (new juce::Label ("new label",
                                         TRANS ("Remove Threshold")));
    addAndMakeVisible (juce__label2.get());
    juce__label2->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    juce__label2->setJustificationType (juce::Justification::centredLeft);
    juce__label2->setEditable (false, false, false);
    juce__label2->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    juce__label2->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    juce__label2->setBounds (0, 24, 150, 24);

    removeThreshold.reset (new juce::Slider ("removeThreshold"));
    addAndMakeVisible (removeThreshold.get());
    removeThreshold->setRange (0, 20, 0.1);
    removeThreshold->setSliderStyle (juce::Slider::LinearHorizontal);
    removeThreshold->setTextBoxStyle (juce::Slider::TextBoxLeft, false, 80, 20);
    removeThreshold->addListener (this);

    removeThreshold->setBounds (136, 24, 238, 24);

    juce__label3.reset (new juce::Label ("new label",
                                         TRANS ("Refine Stage")));
    addAndMakeVisible (juce__label3.get());
    juce__label3->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    juce__label3->setJustificationType (juce::Justification::centredLeft);
    juce__label3->setEditable (false, false, false);
    juce__label3->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    juce__label3->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    juce__label3->setBounds (0, 48, 150, 24);

    refineParam.reset (new juce::Slider ("refineParam"));
    addAndMakeVisible (refineParam.get());
    refineParam->setRange (1, 12, 1);
    refineParam->setSliderStyle (juce::Slider::LinearHorizontal);
    refineParam->setTextBoxStyle (juce::Slider::TextBoxLeft, false, 80, 20);
    refineParam->addListener (this);

    refineParam->setBounds (136, 48, 238, 24);

    juce__label4.reset (new juce::Label ("new label",
                                         TRANS ("Guide")));
    addAndMakeVisible (juce__label4.get());
    juce__label4->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    juce__label4->setJustificationType (juce::Justification::centredLeft);
    juce__label4->setEditable (false, false, false);
    juce__label4->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    juce__label4->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    juce__label4->setBounds (368, 0, 150, 24);

    guideLevel.reset (new juce::Slider ("guideLevel"));
    addAndMakeVisible (guideLevel.get());
    guideLevel->setRange (-12, 0, 0.1);
    guideLevel->setSliderStyle (juce::Slider::LinearHorizontal);
    guideLevel->setTextBoxStyle (juce::Slider::TextBoxLeft, false, 80, 20);
    guideLevel->addListener (this);

    guideLevel->setBounds (416, 0, 238, 24);


    //[UserPreSize]
 guideLevel->setValue(0.);
    //[/UserPreSize]

    setSize (600, 150);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

ControlLayout::~ControlLayout()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    juce__label = nullptr;
    clumpingFrequency = nullptr;
    juce__label2 = nullptr;
    removeThreshold = nullptr;
    juce__label3 = nullptr;
    refineParam = nullptr;
    juce__label4 = nullptr;
    guideLevel = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void ControlLayout::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (juce::Colour (0xff323e44));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void ControlLayout::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void ControlLayout::sliderValueChanged (juce::Slider* sliderThatWasMoved)
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
    else if (sliderThatWasMoved == refineParam.get())
    {
        //[UserSliderCode_refineParam] -- add your slider handling code here..
     if (onRefineParamChange) onRefineParamChange(refineParam->getValue());
        //[/UserSliderCode_refineParam]
    }
    else if (sliderThatWasMoved == guideLevel.get())
    {
        //[UserSliderCode_guideLevel] -- add your slider handling code here..
     if (onGuideLevelChange) onGuideLevelChange(guideLevel->getValue());
        //[/UserSliderCode_guideLevel]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void ControlLayout::resetParameters(float clump, float remove, int refine)
{
 clumpingFrequency->setValue(clump, juce::dontSendNotification);
 removeThreshold->setValue(remove*100., juce::dontSendNotification);
 refineParam->setValue(refine, juce::dontSendNotification);
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ControlLayout" componentName=""
                 parentClasses="public juce::Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="600" initialHeight="150">
  <BACKGROUND backgroundColour="ff323e44"/>
  <LABEL name="new label" id="9373f0f1bbb140ce" memberName="juce__label"
         virtualName="" explicitFocusOrder="0" pos="0 0 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Clumping Frequency" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <SLIDER name="clumpingFrequency" id="30c041eab425c64a" memberName="clumpingFrequency"
          virtualName="" explicitFocusOrder="0" pos="136 0 238 24" min="10.0"
          max="1000.0" int="1.0" style="LinearHorizontal" textBoxPos="TextBoxLeft"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="new label" id="cd402b46bbf80d5c" memberName="juce__label2"
         virtualName="" explicitFocusOrder="0" pos="0 24 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Remove Threshold" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <SLIDER name="removeThreshold" id="26d7d820943adbf1" memberName="removeThreshold"
          virtualName="" explicitFocusOrder="0" pos="136 24 238 24" min="0.0"
          max="20.0" int="0.1" style="LinearHorizontal" textBoxPos="TextBoxLeft"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="new label" id="9e10e3fffe74e786" memberName="juce__label3"
         virtualName="" explicitFocusOrder="0" pos="0 48 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Refine Stage" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <SLIDER name="refineParam" id="ecbfa1bf49fc24c7" memberName="refineParam"
          virtualName="" explicitFocusOrder="0" pos="136 48 238 24" min="1.0"
          max="12.0" int="1.0" style="LinearHorizontal" textBoxPos="TextBoxLeft"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="new label" id="c42a6f51b4abf89f" memberName="juce__label4"
         virtualName="" explicitFocusOrder="0" pos="368 0 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Guide" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
  <SLIDER name="guideLevel" id="4de4aab7c7d9b764" memberName="guideLevel"
          virtualName="" explicitFocusOrder="0" pos="416 0 238 24" min="-12.0"
          max="0.0" int="0.1" style="LinearHorizontal" textBoxPos="TextBoxLeft"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

