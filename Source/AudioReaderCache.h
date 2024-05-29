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
 int _chCount;
 
public:
 const double &sampleRate;
 const juce::int64 &lengthInSamples;
 const int &numChannels;
 
 AudioReaderCache(juce::AudioFormatReader &_reader) :
 reader(_reader),
 sampleRate(_sr),
 lengthInSamples(_len),
 numChannels(_chCount)
 {
  _sr = _reader.sampleRate;
  _len = _reader.lengthInSamples;
  _chCount = _reader.numChannels;
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
 
 template <typename T, unsigned int N>
 bool read(int channel, int pos, std::array<T, N> &s)
 {
  readCache.setSize(reader.numChannels, N);
  cachedSection = -1;
  if (reader.read(&readCache, 0, N, pos, true, true))
  {
   memcpy(s.data(), readCache.getReadPointer(channel), sizeof(float)*N);
   return true;
  }
  
  return false;
 }
};
