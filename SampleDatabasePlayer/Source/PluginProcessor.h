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




class ProcessorAnalyserThread : public juce::Thread
{
 static constexpr float TimingRatio = (static_cast<float>(std::chrono::steady_clock::period::num)/
                                       static_cast<float>(std::chrono::steady_clock::period::den));
 
public:
 static constexpr float ClumpingFrequencyDefault = 200.;
 static constexpr float RemoveThresholdDefault = 0.05;
 
 float clumpingFrequency {ClumpingFrequencyDefault};
 float removeThreshold {RemoveThresholdDefault};

 std::unique_ptr<MemoryBufferReader> reader {nullptr};
 Analysis *resultsHolder {nullptr};
 std::function<void ()> onFinish;

 float timeLastAnalysisSeconds {-1.};

 ProcessorAnalyserThread() :
 juce::Thread("Foreground analyser thread")
 {}
 
 void run()
 {
  if (reader)
  {
   Analyser analyser(*reader);
   
   analyser.clumpingFrequency = clumpingFrequency;
   analyser.removeThreshold = removeThreshold;
   
   if (resultsHolder)
   {
    auto startTime = std::chrono::steady_clock::now();
    analyser.doAnalysis(*resultsHolder, [&]() { return threadShouldExit(); });
    auto endTime = std::chrono::steady_clock::now();
    
    timeLastAnalysisSeconds = (endTime - startTime).count()*TimingRatio;
    
    if (onFinish && !threadShouldExit()) onFinish();
   }
  }
 }
};




//==============================================================================
/**
 */
class SampleDatabasePlayerAudioProcessor  : public juce::AudioProcessor, public juce::AsyncUpdater, public juce::ChangeBroadcaster, public XDDSP::PiecewiseEnvelopeListener
{
 static constexpr int NumberOfSlots = 3;
 
 static constexpr float DefaultPlaybackKRMS = -15.0;
 static constexpr bool DefaultLengthIsTempoSync = true;
 static constexpr float DefaultLengthMs = 200.;
 static constexpr float DefaultLengthFrac = 33.;
 static constexpr float DefaultMasterFadeOut = 30.;
 static constexpr float DefaultReshapeAmount = 0.;
 
 
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
 
 void loadSample(int slot, const juce::String &path, float kRMSdB);
 void unloadSample(int slot);
 void setFreeze(bool shouldFreeze);
 void analyseFrozenSampleAgain();

 std::vector<float> leftProcessBuffer;
 std::vector<float> rightProcessBuffer;
 std::vector<float> leftFreezeBuffer;
 std::vector<float> rightFreezeBuffer;
 const bool &isFrozen {frozen};
 Analysis freezeAnalysis;
 std::array<SampleParameters, NumberOfSlots> samples;
 std::vector<std::vector<float>> sampleBuffer;
 
 int lengthOfBuffer()
 {
  return static_cast<int>(leftProcessBuffer.size());
 }

 float freezeParametersLoadedFromState {false};
 ProcessorAnalyserThread freezeAnalyseThread;
 float reshapeAmount {DefaultReshapeAmount};
 
 XDDSP::PiecewiseEnvelopeData<10, 10> envelopeData;
 virtual void piecewiseEnvelopeChanged() override;
 
private:
 
 static const juce::Identifier PresetDataIdentifier;
 static const juce::Identifier SampleIdentifier1;
 static const juce::Identifier SampleIdentifier2;
 static const juce::Identifier SampleIdentifier3;
 static const juce::Identifier ClumpingFrequencyIdentifier;
 static const juce::Identifier RemoveThresholdIdentifier;
 static const juce::Identifier AnalysisIdentifier;
 static const juce::Identifier EnvelopeIdentifier;
 static const juce::Identifier ReshapeAmountIdentifier;

 
 
 juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
 
 juce::AudioFormatManager audioFormatManager;
 
 bool playingSecondBuffer {false};
 bool fadingBetweenBuffers {false};
 std::vector<float> leftPlayBuffer1;
 std::vector<float> leftPlayBuffer2;
 std::vector<float> rightPlayBuffer1;
 std::vector<float> rightPlayBuffer2;

 bool reloadSamples {false};
 bool initFromSavedState {false};
 bool loadParametersFromSavedState {false};
 int userChosenLength {INT_MAX};
 bool lengthIsTempoSync {DefaultLengthIsTempoSync};
 float lengthMs {DefaultLengthMs};
 float lengthFrac {0.01*DefaultLengthFrac};
 float masterFadeOut {DefaultMasterFadeOut};
 void recalculateLength();
 void doReloadSamples();
 void processSample(int slot);
 void mainProcess();
 void processFade();

 bool frozen {false};
 void initFreezeThread();
 void freezeProcess();
 void beginFreeze();
 
 float playbackKRMS {DefaultPlaybackKRMS};
 
 // Put private fields in here BEFORE the parameters object, so that they are
 // constructed first and parameters is constructed last.

 std::vector<std::unique_ptr<PluginListener>> listeners;
 juce::AudioProcessorValueTreeState parameters;
 
 void connectDSP(int bufferLength);
 //==============================================================================
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleDatabasePlayerAudioProcessor)
};
