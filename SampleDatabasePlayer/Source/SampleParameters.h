/*
  ==============================================================================

    SampleParameters.h
    Created: 6 Jun 2024 9:19:50pm
    Author:  Adam Jackson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../../Source/Analyser.h"

struct SampleParameters
{
 bool active {false};
 juce::String filePath;
 int leftSlot {0};
 int rightSlot {0};
 int sampleOffset {0};
 int fadeInStart {0};
 int fadeInLength {5};
 int fadeOutLength {5};
 int fadeOutEnd {INT_MAX};
 float kRMSdB {-15.};
 float playbackLevelKdB {-15.};

 void sanitiseParameters(int length)
 {
  if (length > 0)
  {
   fadeOutEnd = XDDSP::boundary(fadeOutEnd, 1, length);
   fadeInStart = XDDSP::boundary(fadeInStart, 0,
                                         fadeOutEnd - 1);
   fadeInLength = XDDSP::boundary(fadeInLength, 1,
                                          fadeOutEnd - fadeInStart);
   fadeOutLength = XDDSP::boundary(fadeOutLength, 1,
                                           fadeOutEnd - fadeInStart);
  }
 }
 
 float getTotalGain()
 { return playbackLevelKdB - kRMSdB; }

 juce::String toString()
 {
  juce::String str = "";
  juce::DynamicObject::Ptr json = new juce::DynamicObject();
  
//  json->setProperty("version", version);
  json->setProperty("active", active);
  json->setProperty("path", filePath);
  json->setProperty("offset", sampleOffset);
  json->setProperty("fadestart", fadeInStart);
  json->setProperty("fadein", fadeInLength);
  json->setProperty("fadeout", fadeOutLength);
  json->setProperty("fadeend", fadeOutEnd);
  json->setProperty("krmsdb", kRMSdB);
  json->setProperty("playbackleveldb", playbackLevelKdB);
  
  juce::var jsonVar(json.get());
  
  str = juce::JSON::toString(jsonVar);
  return str;
 };
 
 void setFromString(const juce::String &str)
 {
  juce::var json = juce::JSON::fromString(str);
  
//  if (!json.hasProperty("stereo")) return;
//  stereo = json["stereo"];
  active = json.getProperty("active", false);
  filePath = json.getProperty("path", filePath);
  sampleOffset = json.getProperty("offset", 0);
  fadeInStart = json.getProperty("fadestart", fadeInStart);
  fadeInLength = json.getProperty("fadein", fadeInLength);
  fadeOutLength = json.getProperty("fadeout", fadeOutLength);
  fadeOutEnd = json.getProperty("fadeend", fadeOutEnd);
  kRMSdB = json.getProperty("krmsdb", -15.f);
  playbackLevelKdB = json.getProperty("playbackleveldb", -15.f);
  
 }
};
