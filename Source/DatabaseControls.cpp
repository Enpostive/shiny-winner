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

    previewButton.reset (new juce::TextButton ("Preview"));
    addAndMakeVisible (previewButton.get());


    //[UserPreSize]
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

    sampleList->setBounds (0, 0, getWidth() - 168, proportionOfHeight (0.9974f));
    importButton->setBounds (getWidth() - 158, 8, 150, 24);
    previewButton->setBounds (getWidth() - 158, 40, 150, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
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
            virtualName="juce::TableListBox" explicitFocusOrder="0" pos="0 0 168M 99.74%"
            rootVisible="1" openByDefault="0"/>
  <TEXTBUTTON name="Import" id="9be56120a7e53174" memberName="importButton"
              virtualName="" explicitFocusOrder="0" pos="158R 8 150 24" buttonText="Import..."
              connectedEdges="0" needsCallback="0" radioGroupId="0"/>
  <TEXTBUTTON name="Preview" id="7040713001c363e4" memberName="previewButton"
              virtualName="" explicitFocusOrder="0" pos="158R 40 150 24" buttonText="Preview"
              connectedEdges="0" needsCallback="0" radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

