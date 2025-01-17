/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 8.0.0

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) - Raw Material Software Limited.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include <JuceHeader.h>
#include "Zoom.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class ControlLayout  : public juce::Component,
                       public juce::Slider::Listener
{
public:
    //==============================================================================
    ControlLayout ();
    ~ControlLayout() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
 void resetParameters(float clump, float remove, int refine);

 std::function<void (float)> onClumpingFrequencyChange;
 std::function<void (float)> onRemoveThresholdChange;
 std::function<void (int)> onRefineParamChange;
 std::function<void (float)> onGuideLevelChange;
    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;
    void sliderValueChanged (juce::Slider* sliderThatWasMoved) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<juce::Label> juce__label;
    std::unique_ptr<juce::Slider> clumpingFrequency;
    std::unique_ptr<juce::Label> juce__label2;
    std::unique_ptr<juce::Slider> removeThreshold;
    std::unique_ptr<juce::Label> juce__label3;
    std::unique_ptr<juce::Slider> refineParam;
    std::unique_ptr<juce::Label> juce__label4;
    std::unique_ptr<juce::Slider> guideLevel;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ControlLayout)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

