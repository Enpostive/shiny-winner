/*
  ==============================================================================

    Analyser.h
    Created: 28 May 2024 9:24:36pm
    Author:  Adam Jackson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "AudioReaderCache.h"
#include "AutoCorrelator.h"
#include "RMSAnalyser.h"
#include "SampleEnvelopeAnalyser.h"





struct Analysis
{
 bool stereo {false};
 float peakdB {0.};
 float rmsdB {0.};
 float krmsdB {0.};
 std::unique_ptr<WaveformEnvelope> envMonoLeft;
 std::unique_ptr<WaveformEnvelope> envRight;
 
 juce::String toString()
 {
  juce::String str;
  jassert(false);
  return str;
 };
 
 Analysis()
 {}
 
 Analysis(const juce::String &str)
 {
  jassert(false);
 }
};




class Analyser : public juce::Thread
{
 AudioReaderCache reader;
 XDDSP::Parameters param;
 
 void doAnalysis(Analysis &analysis)
 {
  RMSAnalyser rmsAnalyser(param);
  
  analysis.stereo = (reader.numChannels == 2);
  analysis.rmsdB = XDDSP::linear2dB(rmsAnalyser.calculateRMS(reader));
  analysis.krmsdB = XDDSP::linear2dB(rmsAnalyser.calculateKWeightedRMS(reader));
  
  WaveformEnvelopeAnalyser envAnal(reader);
  envAnal.setClumpingFrequency(200.);
  envAnal.setRefine(12);
  envAnal.setRemoveThreshold(0.05);
  envAnal.checkShouldCancel = [&]() { return threadShouldExit(); };

  analysis.envMonoLeft.reset(envAnal.generateEnvelope(0));
  if (analysis.envMonoLeft)
  {
   if (analysis.stereo)
   {
    analysis.envRight.reset(envAnal.generateEnvelope(1));
    if (analysis.envRight)
    {
     analysis.peakdB = XDDSP::linear2dB(std::max(analysis.envMonoLeft->peakAmplitude,
                                                 analysis.envRight->peakAmplitude));
    }
   }
   else
   {
    analysis.envRight.reset();
    analysis.peakdB = XDDSP::linear2dB(analysis.envMonoLeft->peakAmplitude);
   }
  }
 }

public:
 Analyser(juce::AudioFormatReader &_reader) :
 juce::Thread("AnalyserThread"),
 reader(_reader)
 {
  param.setSampleRate(reader.sampleRate);
 }
 
 Analysis *resultsHolder {nullptr};
 std::function<void ()> onFinish;

 void run()
 {
  if (resultsHolder)
  {
   doAnalysis(*resultsHolder);
  }
  if (onFinish) onFinish();
 }
};
