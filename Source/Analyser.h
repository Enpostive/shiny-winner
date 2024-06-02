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
  
  juce::String str = juce::JSON::toString(jsonVar);

  return str;
 };
 
 Analysis()
 {}
 
 void setFromString(const juce::String &str)
 {
  juce::var json = juce::JSON::fromString(str);
  
  if (json.hasProperty("version")) version = json["version"];
  else version = 1;
  stereo = json["stereo"];
  peakdB = json["peak"];
  rmsdB = json["rms"];
  krmsdB = json["krms"];
  if (stereo)
  {
   envMonoLeft.reset(new WaveformEnvelope(json["envLeft"]));
   envRight.reset(new WaveformEnvelope(json["envRight"]));
  }
  else
  {
   envMonoLeft.reset(new WaveformEnvelope(json["env"]));
   envRight.reset();
  }
  valid = true;
 }
};




class Analyser
{
 AudioReaderCache reader;
 XDDSP::Parameters param;
 
public:
 Analyser(juce::AudioFormatReader &_reader) :
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
  envAnal.setClumpingFrequency(200.);
  envAnal.setRefine(12);
  envAnal.setRemoveThreshold(0.05);
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
 juce::AudioFormatReader *reader;
 Analysis *resultsHolder {nullptr};
 std::function<void ()> onFinish;


 AnalyserThread() :
 juce::Thread("Foreground analyser thread")
 {}
 
 void run()
 {
  Analyser analyser(*reader);
  
  if (resultsHolder)
  {
   analyser.doAnalysis(*resultsHolder, [&]() { return threadShouldExit(); });
  }
  if (onFinish) onFinish();
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
    Analyser analyser(*audioReader);
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
