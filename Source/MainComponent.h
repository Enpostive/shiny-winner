#pragma once

#include <JuceHeader.h>
#include "DatabaseControls.h"
#include "DatabaseTableModel.h"

//==============================================================================
/*
 This component lives inside our window, and this is where you should put all
 your controls and content.
 */
class MainComponent  : public juce::AudioAppComponent
{
public:
//==============================================================================
 MainComponent();
 ~MainComponent() override;
 
//==============================================================================
 void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
 void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
 void releaseResources() override;
 
//==============================================================================
 void paint (juce::Graphics& g) override;
 void resized() override;
 
private:
//==============================================================================
// Your private member variables go here...
 
 juce::StretchableLayoutManager layout;
 juce::StretchableLayoutResizerBar resizerBar;
 
 juce::Label dummyLabel;
 
 SampleDatabaseConnection dbConn;
 
 DatabaseControls databaseControls;
 DatabaseTableModel tableModel;
 
 juce::TooltipWindow tooltipWindow;
 
 void repaintSampleList();
 
 juce::TimedCallback repaintTimer {[&](){ repaintSampleList(); }};
 
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
