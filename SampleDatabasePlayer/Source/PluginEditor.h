/*
 ==============================================================================
 
 This file contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Controls.h"
#include "DatabaseTableModel.h"
#include "../../Source/ColouredScope.h"

//==============================================================================
/**
 */
class SampleDatabasePlayerAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer, public juce::ChangeListener
{
public:
 SampleDatabasePlayerAudioProcessorEditor (SampleDatabasePlayerAudioProcessor&, juce::AudioProcessorValueTreeState &);
 ~SampleDatabasePlayerAudioProcessorEditor() override;
 
 //==============================================================================
 void paint (juce::Graphics&) override;
 void resized() override;
 
 void timerCallback() override;
 void changeListenerCallback(juce::ChangeBroadcaster *source) override;
 
private:
 typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
 typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
 typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

 juce::TooltipWindow tooltipWindow;
 
 int selectedSlot {0};

 SampleDatabasePlayerAudioProcessor& audioProcessor;
 juce::AudioProcessorValueTreeState& valueTreeState;
 SampleDatabaseConnection dbConn;
 SampleDatabaseAccessor dbAccess;
 std::unique_ptr<XDLookAndFeel> lookAndFeel;
 Controls controls;
 DatabaseTableModel listModel;
 
 ColouredScope leftOutputBufferScope;
 ColouredScope rightOutputBufferScope;
 SampleBufferSource leftOutputBufferSource;
 SampleBufferSource rightOutputBufferSource;
 
 ColouredScope leftEnvelopeScope;
 ColouredScope rightEnvelopeScope;
 AnalysisWaveformSource leftEnvelopeSource;
 AnalysisWaveformSource rightEnvelopeSource;
 
 PiecewiseEnvelopeEditor<decltype(SampleDatabasePlayerAudioProcessor::envelopeData)> envelopeEditor;

// std::vector<std::unique_ptr<juce::Component>> components;
 std::vector<std::unique_ptr<SliderAttachment>> sliderAttach;
 std::vector<std::unique_ptr<ButtonAttachment>> buttonAttach;
 std::vector<std::unique_ptr<ComboBoxAttachment>> comboAttach;
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleDatabasePlayerAudioProcessorEditor)
};
