/*
 ==============================================================================
 
 This file contains the basic framework code for a JUCE plugin processor.
 
 ==============================================================================
 */

#pragma once

#include <JuceHeader.h>
#include "PluginParameters.h"
#include "../../Source/SampleDatabaseClasses.h"
#include "../../Source/Analyser.h"
#include "DSP.h"

//==============================================================================
/**
 */
class SampleDatabasePlayerAudioProcessor  : public juce::AudioProcessor
{
public:
 //==============================================================================
 SampleDatabasePlayerAudioProcessor();
 ~SampleDatabasePlayerAudioProcessor() override;
 
 //==============================================================================
 void prepareToPlay (double sampleRate, int samplesPerBlock) override;
 void releaseResources() override;
 
#ifndef JucePlugin_PreferredChannelConfigurations
 bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
#endif
 
 void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
 void processBlock (juce::AudioBuffer<double>&, juce::MidiBuffer&) override;
 
 void processMIDI (juce::MidiBuffer &);
 
 virtual bool supportsDoublePrecisionProcessing () const override { return true; }
 
 //==============================================================================
 juce::AudioProcessorEditor* createEditor() override;
 bool hasEditor() const override;
 
 //==============================================================================
 const juce::String getName() const override;
 
 bool acceptsMidi() const override;
 bool producesMidi() const override;
 bool isMidiEffect() const override;
 double getTailLengthSeconds() const override;
 
 //==============================================================================
 int getNumPrograms() override;
 int getCurrentProgram() override;
 void setCurrentProgram (int index) override;
 const juce::String getProgramName (int index) override;
 void changeProgramName (int index, const juce::String& newName) override;
 
 //==============================================================================
 void getStateInformation (juce::MemoryBlock& destData) override;
 void setStateInformation (const void* data, int sizeInBytes) override;
 
 std::vector<ParameterSpec> paramSpecs;

 XDDSP::Parameters dspParam;
 XDDSP::PluginDSP dsp;
 
 std::mutex mtx;

private:
 juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
 
 // Put private fields in here BEFORE the parameters object, so that they are
 // constructed first and parameters is constructed last.

 std::vector<std::unique_ptr<PluginListener>> listeners;
 juce::AudioProcessorValueTreeState parameters;
 //==============================================================================
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleDatabasePlayerAudioProcessor)
};
