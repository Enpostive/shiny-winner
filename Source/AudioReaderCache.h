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
protected:
 double _sr;
 juce::int64 _len;
 int _chCount;
 
public:
 const double &sampleRate;
 const juce::int64 &lengthInSamples;
 const int &numChannels;
 
 AudioReaderCache() :
 sampleRate(_sr),
 lengthInSamples(_len),
 numChannels(_chCount)
 {}

 virtual ~AudioReaderCache() {}
 
 virtual float read(int channel, int pos) = 0;
};



class JuceFileReaderCache : public AudioReaderCache
{
 juce::AudioFormatReader &reader;
 juce::AudioBuffer<float> readCache;
 static constexpr XDDSP::PowerSize ReadCacheSize {9};
 int cachedSection {-1};
 
public:
 JuceFileReaderCache(juce::AudioFormatReader &_reader) :
 reader(_reader)
 {
  _sr = _reader.sampleRate;
  _len = _reader.lengthInSamples;
  _chCount = _reader.numChannels;
 }
 
 float read(int channel, int pos) override
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

 /*
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
 }*/
};




class MemoryBufferReader : public AudioReaderCache
{
 std::array<float*, 2> buffers;
 
public:
 template <unsigned long N>
 MemoryBufferReader(std::array<float*, N> _buffers,
                    double sampleRate,
                    juce::int64 sampleCount)
 {
  static_assert(N > 0 && N < 3, "MemoryBufferReader only supports one or two channels");
  buffers[0] = _buffers[0];
  if (N == 2) buffers[1] = _buffers[1];
  else buffers[1] = nullptr;
  _sr = sampleRate;
  _chCount = N;
  _len = sampleCount;
 }

 float read(int channel, int pos) override
 {
  jassert(channel < _chCount);
  if (pos < 0 || pos >= _len) return 0.;
  return buffers[channel][pos];
 }
};
