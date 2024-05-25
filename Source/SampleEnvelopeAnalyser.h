/*
  ==============================================================================

    SampleEnvelopeAnalyser.h
    Created: 23 May 2024 11:06:59am
    Author:  Adam Jackson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "XDDSP/XDDSP.h"

class WaveformEnvelopeAnalyser;

class WaveformEnvelope
{
 friend class WaveformEnvelopeAnalyser;
 
public:
 struct Maxima
 {
  int time;
  float amplitude;
 };
 
private:
 float sampleRate;
 std::vector<Maxima> wMaxima;

 int maximaAfterTimeOrEnd(int t) const
 {
  if (wMaxima.empty()) throw std::runtime_error("Oops");
  int i = 0;
  while (i < maxima.size() && t > maxima[i].time) ++i;
  
  return i;
 }
 
 void insertMaxima(Maxima m)
 {
  if (wMaxima.empty()) wMaxima.push_back(m);
  else
  {
   const int i = maximaAfterTimeOrEnd(m.time);
   wMaxima.insert(wMaxima.begin() + i, m);
  }
 }

public:
 const std::vector<Maxima> &maxima {wMaxima};
 
 WaveformEnvelope() {}
 
 WaveformEnvelope(juce::String stringRep)
 {
  // Throws an exception if an error is detected
  
  juce::StringArray tokens = juce::StringArray::fromTokens(stringRep, false);
  if (tokens.size() %2 != 0) throw std::runtime_error("Envelope decode expects even number of tokens");
  
  for (int i = 0; i < tokens.size(); i += 2)
  {
   Maxima m;
   m.time = tokens[i].getIntValue();
   m.amplitude = tokens[i + 1].getFloatValue();
   
   wMaxima.push_back(m);
  }
  
  // Check to see if the list is sorted. If not, throw an error
  if (!std::is_sorted(maxima.begin(),
                      maxima.end(),
                      [](const Maxima &lhs, const Maxima &rhs)
                      { return lhs.time < rhs.time; }))
  {
   throw std::runtime_error("Envelope decode expects sorted array of maxima");
  }
 }
 
 float amplitudeAtNegativeInfinity() const
 {
  if (maxima.empty()) return 0.;
  return maxima.front().amplitude;
 }
 
 float amplitudeAtPositiveInfinity() const
 {
  if (maxima.empty()) return 0.;
  return maxima.back().amplitude;
 }
 
 float amplitudeAtSample(float s) const
 {
  if (maxima.empty()) return 0.;
  if (maxima.size() == 1) return maxima[0].amplitude;

  const int t = static_cast<int>(ceil(s));
  const int i = maximaAfterTimeOrEnd(t);
  if (i == 0) return amplitudeAtNegativeInfinity();
  if (i == maxima.size()) return amplitudeAtPositiveInfinity();

  const int im1 = i - 1;
  const float intervalBetweenMaxima = static_cast<float>(maxima[i].time - maxima[im1].time);
  const float intervalWithinMaxima = s - static_cast<float>(maxima[im1].time);
  const float fraction = intervalWithinMaxima/intervalBetweenMaxima;
  const float delta = maxima[i].amplitude - maxima[im1].amplitude;
  return maxima[im1].amplitude + delta*fraction;
 }
 
 float amplitudeAtTime(float sec) const
 { return amplitudeAtSample(sec*sampleRate); }
 
 juce::String toString() const
 {
  juce::String stringRep;
  
  // The envelope will be represented as a list of numbers.
  // time amplitude time amplitude time amplitude...
  
  for (auto &i : maxima)
  {
   if (stringRep.isNotEmpty()) stringRep += " ";
   stringRep += juce::String(i.time) + " " + juce::String(i.amplitude, 10);
  }
  
  return stringRep;
 }
};










class WaveformEnvelopeAnalyser
{
private:
 juce::AudioFormatReader &reader;
 juce::AudioBuffer<float> readCache;
 static constexpr XDDSP::PowerSize ReadCacheSize {9};
 int cachedSection {-1};
 
 float rsc(int channel, int pos)
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

 const float sampleRate;
 float clumpingFrequency {100.};
 
 int clumpingDistance()
 { return sampleRate/clumpingFrequency; }

public:
 WaveformEnvelopeAnalyser(juce::AudioFormatReader &_reader) :
 reader(_reader),
 sampleRate(reader.sampleRate)
 {}
 
 ~WaveformEnvelopeAnalyser()
 {}
 
 void setClumpingFrequency(float cf)
 { if (cf > 0.) clumpingFrequency = cf; }
 
 WaveformEnvelope* generateEnvelope(int channel, int refine = 0, double removeMaximaBelow = 0.)
 {
  WaveformEnvelope *env = new WaveformEnvelope();
  env->sampleRate = sampleRate;
  
  int count = static_cast<int>(reader.lengthInSamples);
  
  // ===========================================================
  // First pass
  // Record maxima between zero-crossings
  if (refine > 0)
  {
   int i = 0;
   while (i < count && rsc(channel, i) == 0.) ++i;

   // If the sample is empty then a zero envelope will suffice;
   if (i == count) return env;
   
   while (i < count)
   {
    float sign = XDDSP::signum(rsc(channel, i));
    WaveformEnvelope::Maxima maxima = {i, fabs(rsc(channel, i))};
    while ((++i < count) &&
           (XDDSP::signum(rsc(channel, i)) == sign))
    {
     if (fabs(rsc(channel, i)) > maxima.amplitude)
     {
      maxima.time = i;
      maxima.amplitude = fabs(rsc(channel, i));
     }
    }
    env->insertMaxima(maxima);
   }
  }
  // ===========================================================
  // Second pass
  // Delete envelope points which have higher points that are within
  // clumping distance
  if (refine > 0)
  {
   --refine;
   int i = static_cast<int>(env->wMaxima.size() - 1);
   auto b = env->wMaxima.begin();
   while (--i > 1)
   {
    float tAmp = env->wMaxima[i].amplitude;
    int t = env->wMaxima[i].time;
    int j = i - 1;
    bool erase = false;
    while (!erase &&
           (j >= 0) &&
           (clumpingDistance() > t - env->wMaxima[j].time))
    {
     erase = (env->wMaxima[j].amplitude > tAmp);
     --j;
    }
    j = i + 1;
    while (!erase &&
           (j < env->wMaxima.size()) &&
           (clumpingDistance() > env->wMaxima[j].time - t))
    {
     erase = (env->wMaxima[j].amplitude > tAmp);
     ++j;
    }
    if (erase)
    {
     env->wMaxima.erase(b + i);
    }
   }
  }
  // ===========================================================
  // Delete any maxima which amplitude is less than a percentage of the absolute maximum
  if (env->wMaxima.size() > 0)
  {
   float absoluteMax = env->wMaxima.front().amplitude;
   for (int i = 1; i < env->wMaxima.size(); ++i)
   {
    absoluteMax = std::max(env->wMaxima[i].amplitude, absoluteMax);
   }
   float threshold = removeMaximaBelow*absoluteMax;
   int i = static_cast<int>(env->wMaxima.size() - 1); // Don't check the last envelope point
   auto b = env->wMaxima.begin();
   while (--i > 1) // Don't check the first point either
   {
    if (env->wMaxima[i].amplitude < threshold) env->wMaxima.erase(b + i);
   }
  }
  // ===========================================================
  // Refine passes
  // Validate the envelope and add an extra maxima for each segment where the
  // waveform value exceeds the envelope value
  bool validated {false};
  while (!validated && refine-- > 0)
  {
   validated = true;
   int i = 0;
   while (i < count)
   {
    if (fabs(rsc(channel, i)) > env->amplitudeAtSample(i))
    {
     validated = false;
     // Identify which segment we have landed on and set up loop points
     int maximaIndex = env->maximaAfterTimeOrEnd(i);
     // If we are at the start or end of the maxima envelope, simply add another maxima
     if (maximaIndex == 0 ||
         maximaIndex == env->maxima.size())
     {
      env->insertMaxima({i, fabs(rsc(channel, i))});
     }
     else
     {
      int mim1 = maximaIndex - 1;
      WaveformEnvelope::Maxima mm1 = env->maxima[mim1];
      WaveformEnvelope::Maxima m = env->maxima[maximaIndex];
      float envAngle = atan((m.amplitude - mm1.amplitude) /
                            static_cast<float>(m.time - mm1.time));
      float amp = -1.;
      WaveformEnvelope::Maxima peak;
      int end = env->maxima[maximaIndex].time;
      for (int j = i; j < end; ++j)
      {
       WaveformEnvelope::Maxima p {j, fabs(rsc(channel, j))};
       float dTime = p.time - mm1.time;
       float dAmp = p.amplitude - mm1.amplitude;
       float peakAngle = atan(dAmp/dTime);
       float h = hypot(dTime, dAmp);
       float theta = peakAngle - envAngle;
       float projection = h*sin(theta);
       if (projection > amp)
       {
        amp = projection;
        peak = p;
       }
      }
      env->insertMaxima(peak);
      i = end + 1;
     }
    }
    else ++i;
   }
  }
  // ===========================================================
  // Done

  return env;
 }
};
