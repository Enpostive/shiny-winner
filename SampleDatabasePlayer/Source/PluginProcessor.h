/*
 ==============================================================================
 
 This file contains the basic framework code for a JUCE plugin processor.
 
 ==============================================================================
 */

#pragma once

#include <JuceHeader.h>
#include "PluginParameters.h"
#include "SampleParameters.h"
#include "../../Source/SampleDatabaseClasses.h"
#include "DSP.h"

//==============================================================================
/**
 */
class SampleDatabasePlayerAudioProcessor  : public juce::AudioProcessor, public juce::AsyncUpdater, public juce::ChangeBroadcaster
{
 static constexpr int NumberOfSlots = 3;
 
 static constexpr float DefaultPlaybackKRMS = -15.0;
 static constexpr bool DefaultLengthIsTempoSync = true;
 static constexpr float DefaultLengthMs = 200.;
 static constexpr float DefaultLengthFrac = 33.;
 static constexpr float DefaultMasterFadeOut = 30.;
 
 
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
 
 void handleAsyncUpdate() override;
 
 std::vector<ParameterSpec> paramSpecs;

 XDDSP::Parameters dspParam;
 XDDSP::PluginDSP dsp;
 
 std::mutex mtx;
 
 void loadSample(int slot, const juce::String &path);
 void unloadSample(int slot);
 void setAnalysis(int slot, const juce::String &analysisString);

 std::vector<float> leftProcessBuffer;
 std::vector<float> rightProcessBuffer;
 std::array<SampleParameters, NumberOfSlots> samples;
 std::vector<std::vector<float>> sampleBuffer;
 
 int lengthOfBuffer()
 {
  return static_cast<int>(leftProcessBuffer.size());
 }

private:
 juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
 
 juce::AudioFormatManager audioFormatManager;
 
 bool playingSecondBuffer {false};
 bool fadingBetweenBuffers {false};
 std::vector<float> leftPlayBuffer1;
 std::vector<float> leftPlayBuffer2;
 std::vector<float> rightPlayBuffer1;
 std::vector<float> rightPlayBuffer2;

 bool reloadSamples {false};
 int resultLength {INT_MAX};
 bool lengthIsTempoSync{DefaultLengthIsTempoSync};
 float lengthMs {DefaultLengthMs};
 float lengthFrac {0.01*DefaultLengthFrac};
 float masterFadeOut {DefaultMasterFadeOut};
 void recalculateLength();
 void processSample(int slot);
 void mainProcess();
 
 float playbackKRMS {DefaultPlaybackKRMS};
 
 // Put private fields in here BEFORE the parameters object, so that they are
 // constructed first and parameters is constructed last.

 std::vector<std::unique_ptr<PluginListener>> listeners;
 juce::AudioProcessorValueTreeState parameters;
 
 void connectDSP(int bufferLength);
 //==============================================================================
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleDatabasePlayerAudioProcessor)
};
