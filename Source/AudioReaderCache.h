/*
  ==============================================================================

    AudioReaderCache.h
    Created: 25 May 2024 4:45:16pm
    Author:  Adam Jackson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "XDDSP/XDDSP.h"

class AudioReaderCache
{
 juce::AudioFormatReader &reader;
 juce::AudioBuffer<float> readCache;
 static constexpr XDDSP::PowerSize ReadCacheSize {9};
 int cachedSection {-1};
 
 double _sr;
 juce::int64 _len;
 
public:
 const double &sampleRate;
 const juce::int64 &lengthInSamples;
 
 AudioReaderCache(juce::AudioFormatReader &_reader) :
 reader(_reader),
 sampleRate(_sr),
 lengthInSamples(_len)
 {
  _sr = _reader.sampleRate;
  _len = _reader.lengthInSamples;
 }
 
 float read(int channel, int pos)
 {
  int i = pos & ReadCacheSize.mask();
  int sec = pos - i;
  if (sec != cachedSection)
  {
   readCache.setSize(reader.numChannels, ReadCacheSize.size());
   if (!reader.read(&readCache, 0, ReadCacheSize.size(), sec, true, true)) return 0.;
   cachedSection = sec;
  }
  
  return readCache.getSample(channel, i);
 }
};
