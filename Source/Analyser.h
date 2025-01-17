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
#include "RMSAnalyser.h"
#include "SampleEnvelopeAnalyser.h"
#include "SampleDatabaseClasses.h"





struct Analysis
{
 bool valid {false};
 int version {0};
 bool stereo {false};
 float peakdB {0.};
 float rmsdB {0.};
 float krmsdB {0.};
 std::unique_ptr<WaveformEnvelope> envMonoLeft;
 std::unique_ptr<WaveformEnvelope> envRight;
  
 juce::String toString()
 {
  juce::String str = "";
  if (valid)
  {
   juce::DynamicObject::Ptr json = new juce::DynamicObject();
   
   json->setProperty("version", version);
   json->setProperty("stereo", stereo);
   json->setProperty("peak", peakdB);
   json->setProperty("rms", rmsdB);
   json->setProperty("krms", krmsdB);
   if (stereo)
   {
    if (envMonoLeft) json->setProperty("envLeft", envMonoLeft->toString());
    else json->setProperty("envLeft", "");
    
    if (envRight) json->setProperty("envRight", envRight->toString());
    else json->setProperty("envRight", "");
   }
   else
   {
    if (envMonoLeft) json->setProperty("env", envMonoLeft->toString());
    else json->setProperty("env", "");
   }
   
   juce::var jsonVar(json.get());
   
   str = juce::JSON::toString(jsonVar);
   
  }
  return str;
 };
 
 void setFromString(const juce::String &str)
 {
  juce::var json = juce::JSON::fromString(str);
  
  if (json.hasProperty("version")) version = json["version"];
  else version = 1;
  if (version > 1) return;
  if (!json.hasProperty("stereo")) return;
  if (!json.hasProperty("peak")) return;
  if (!json.hasProperty("rms")) return;
  if (!json.hasProperty("krms")) return;
  stereo = json["stereo"];
  peakdB = json["peak"];
  rmsdB = json["rms"];
  krmsdB = json["krms"];
  if (stereo)
  {
   if (!json.hasProperty("envLeft")) return;
   if (!json.hasProperty("envRight")) return;
   envMonoLeft.reset(new WaveformEnvelope(json["envLeft"]));
   envRight.reset(new WaveformEnvelope(json["envRight"]));
  }
  else
  {
   if (!json.hasProperty("env")) return;
   envMonoLeft.reset(new WaveformEnvelope(json["env"]));
   envRight.reset();
  }
  valid = true;
 }
};




class Analyser
{
 AudioReaderCache &reader;
 XDDSP::Parameters param;
 
public:
 float clumpingFrequency {200.};
 float removeThreshold {0.05};
 int refineParam {12};
 
 Analyser(AudioReaderCache &_reader) :
 reader(_reader)
 {
  param.setSampleRate(reader.sampleRate);
 }
 
 void doAnalysis(Analysis &analysis, const std::function<bool ()> &shouldExit)
 {
  RMSAnalyser rmsAnalyser(param);
  analysis.valid = false;
  
  analysis.version = 1;
  analysis.stereo = (reader.numChannels == 2);
  analysis.rmsdB = XDDSP::linear2dB(rmsAnalyser.calculateRMS(reader));
  analysis.krmsdB = XDDSP::linear2dB(rmsAnalyser.calculateKWeightedRMS(reader));
  
  WaveformEnvelopeAnalyser envAnal(reader);
  envAnal.setClumpingFrequency(clumpingFrequency);
  envAnal.setRefine(refineParam);
  envAnal.setRemoveThreshold(removeThreshold);
  envAnal.checkShouldCancel = shouldExit;

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
     analysis.valid = true;
    }
   }
   else
   {
    analysis.envRight.reset();
    analysis.peakdB = XDDSP::linear2dB(analysis.envMonoLeft->peakAmplitude);
    analysis.valid = true;
   }
  }
 }
};





class AnalyserThread : public juce::Thread
{
public:
 static constexpr float ClumpingFrequencyDefault = 200.;
 static constexpr float RemoveThresholdDefault = 0.05;
 
 float clumpingFrequency {ClumpingFrequencyDefault};
 float removeThreshold {RemoveThresholdDefault};
 int refineParam {12};


 juce::AudioFormatReader *reader;
 Analysis *resultsHolder {nullptr};
 std::function<void ()> onFinish;


 AnalyserThread() :
 juce::Thread("Foreground analyser thread")
 {}
 
 void run()
 {
  JuceFileReaderCache cache(*reader);
  Analyser analyser(cache);
  
  analyser.clumpingFrequency = clumpingFrequency;
  analyser.removeThreshold = removeThreshold;
  analyser.refineParam = refineParam;

  if (resultsHolder)
  {
   analyser.doAnalysis(*resultsHolder, [&]() { return threadShouldExit(); });
  }
  if (onFinish && !threadShouldExit()) onFinish();
 }
};




class AnalyserJob : public juce::ThreadPoolJob
{
 SampleDatabaseConnection &dbConn;
 juce::AudioFormatManager &formatManager;
 const int rowid;
 
 Analysis results;
public:
 AnalyserJob(SampleDatabaseConnection &_dbConn, juce::AudioFormatManager &_formatManager, int _rowid) :
 juce::ThreadPoolJob("Background analyser thread"),
 dbConn(_dbConn),
 formatManager(_formatManager),
 rowid(_rowid)
 {}
 
 juce::ThreadPoolJob::JobStatus runJob() override
 {
  SampleDatabaseAccessor dbAccess(dbConn);
  SampleDatabaseModifier dbMod(dbConn);
  
  dbAccess.selectRowId(rowid);
  juce::File audioFile(dbAccess.getPath());
  
  if (audioFile.hasReadAccess())
  {
   std::unique_ptr<juce::AudioFormatReader> audioReader {formatManager.createReaderFor(audioFile)};
   if (audioReader)
   {
    JuceFileReaderCache cache(*audioReader);
    Analyser analyser(cache);
    analyser.doAnalysis(results, [&](){ return shouldExit(); });
    if (results.valid)
    {
     dbMod.updateAnalysis(rowid, results.toString());
    }
   }
  }
  
  return juce::ThreadPoolJob::jobHasFinished;
 }
};
