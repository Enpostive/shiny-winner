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
#include "SampleEditor.h"
#include "FreezeEditor.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class Controls  : public juce::Component,
                  public juce::Slider::Listener,
                  public juce::ComboBox::Listener,
                  public juce::Button::Listener
{
public:
    //==============================================================================
    Controls ();
    ~Controls() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
 SampleEditor& getEditor()
 { return *sampleEditor; }

 FreezeEditor& getFreezeEditor()
 { return *freezeEditor; }
    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;
    void sliderValueChanged (juce::Slider* sliderThatWasMoved) override;
    void comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged) override;
    void buttonClicked (juce::Button* buttonThatWasClicked) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<juce::Label> juce__label;
    std::unique_ptr<juce::Slider> playbacklevel;
    std::unique_ptr<juce::ComboBox> samplelengthmode;
    std::unique_ptr<juce::Slider> mslength;
    std::unique_ptr<juce::Slider> fraclength;
    std::unique_ptr<juce::Slider> fadeout;
    std::unique_ptr<juce::Label> juce__label2;
    std::unique_ptr<SampleEditor> sampleEditor;
    std::unique_ptr<juce::ToggleButton> freeze;
    std::unique_ptr<FreezeEditor> freezeEditor;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Controls)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

