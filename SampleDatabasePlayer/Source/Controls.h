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
#include "SampleAdjuster.h"
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
 juce::TableListBox& sampleList()
 { return *sampleListBox; }

 std::function<void (const juce::String&)> onSearchStringChanged;
 std::function<void (int)> onFilterChange;
 std::function<void ()> onUnload;
 std::function<void (int)> onSlotSelected;

 void resetFilterItems();
 void addFilterItem(const juce::String &item, int itemId);

 SampleAdjuster* getAdjuster()
 { return adjuster.get(); }

 void connect(std::vector<float> &l,
              std::vector<float> &r,
              SampleParameters &p,
              int length);

 void disconnect(int length);
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
    std::unique_ptr<juce::TableListBox> sampleListBox;
    std::unique_ptr<juce::Label> juce__label;
    std::unique_ptr<juce::Slider> playbacklevel;
    std::unique_ptr<juce::TextEditor> searchbar;
    std::unique_ptr<juce::ComboBox> categoryFilter;
    std::unique_ptr<juce::ComboBox> samplelengthmode;
    std::unique_ptr<juce::Slider> mslength;
    std::unique_ptr<juce::Slider> fraclength;
    std::unique_ptr<juce::Slider> fadeout;
    std::unique_ptr<juce::Label> juce__label2;
    std::unique_ptr<SampleAdjuster> adjuster;
    std::unique_ptr<juce::TextButton> unloadButton;
    std::unique_ptr<juce::TextButton> selectSlot1Button;
    std::unique_ptr<juce::TextButton> selectSlot2Button;
    std::unique_ptr<juce::TextButton> selectSlot3Button;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Controls)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

