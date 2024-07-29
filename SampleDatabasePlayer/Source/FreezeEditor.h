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
#include "PiecewiseEnvelopeEditor.h"
#include "../../Source/ColouredScope.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class FreezeEditor  : public juce::Component,
                      public juce::Slider::Listener
{
public:
    //==============================================================================
    FreezeEditor ();
    ~FreezeEditor() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
 SampleBufferSource leftWaveformSource;
 SampleBufferSource rightWaveformSource;
 AnalysisWaveformSource leftEnvelopeSource;
 AnalysisWaveformSource rightEnvelopeSource;

 void updateScopes();
 void resetParameters(float clump, float remove, float reshape);

 std::function<void (float)> onClumpingFrequencyChange;
 std::function<void (float)> onRemoveThresholdChange;
 std::function<void (float)> onReshapeAmountChange;

 juce::Label& getAnalysisFeedbackLabel()
 { return *analysisFeedback; }
    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;
    void sliderValueChanged (juce::Slider* sliderThatWasMoved) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<ColouredScope> leftWaveformScope;
    std::unique_ptr<ColouredScope> rightWaveformScope;
    std::unique_ptr<ColouredScope> leftEnvelopeScope;
    std::unique_ptr<ColouredScope> rightEnvelopeScope;
    std::unique_ptr<juce::Label> juce__label;
    std::unique_ptr<juce::Slider> clumpingFrequency;
    std::unique_ptr<juce::Label> juce__label2;
    std::unique_ptr<juce::Slider> removeThreshold;
    std::unique_ptr<juce::Label> juce__label3;
    std::unique_ptr<juce::Slider> reshapeAmount;
    std::unique_ptr<juce::Label> analysisFeedback;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FreezeEditor)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

