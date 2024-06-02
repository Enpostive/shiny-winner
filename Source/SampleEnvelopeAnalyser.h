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
#include "AudioReaderCache.h"

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
 float wPeakMaxima {0.};
 int wPeakTime {0};

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
   if (i < wMaxima.size() && wMaxima[i].time == m.time)
   {
    wMaxima[i].amplitude = std::max(m.amplitude, wMaxima[i].amplitude);
   }
   else
   {
    wMaxima.insert(wMaxima.begin() + i, m);
   }
  }
 }

public:
 const std::vector<Maxima> &maxima {wMaxima};
 const float &peakAmplitude {wPeakMaxima};
 const int &peakTime {wPeakTime};
 
 WaveformEnvelope() {}
 
 WaveformEnvelope(const juce::String &stringRep)
 {
  // Throws an exception if an error is detected
  
  juce::StringArray tokens = juce::StringArray::fromTokens(stringRep, false);
  if (tokens.size() > 0)
  {
   int maximaCount = tokens[0].getIntValue();
   
   if (tokens.size() != (maximaCount*2 + 1))
   {
    throw std::runtime_error("Token count does not match up with maxima count");
   }
   
   for (int i = 1; i < tokens.size(); i += 2)
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
  // size time amplitude time amplitude time amplitude...
  
  stringRep += juce::String(maxima.size());
  
  for (auto &i : maxima)
  {
   stringRep += " " + juce::String(i.time) + " " + juce::String(i.amplitude, 10);
  }
  
  return stringRep;
 }
};










class WaveformEnvelopeAnalyser
{
private:
 AudioReaderCache &reader;

 const float sampleRate;
 float clumpingFrequency {100.};
 
 int refine = 12;
 float removeMaximaBelow = 0.05;
 
 int clumpingDistance()
 { return sampleRate/clumpingFrequency; }
 
 bool cancel()
 {
  if (checkShouldCancel) return checkShouldCancel();
  return false;
 }

public:
 std::function<bool ()> checkShouldCancel;
 
 WaveformEnvelopeAnalyser(AudioReaderCache &_reader) :
 reader(_reader),
 sampleRate(reader.sampleRate)
 {}
 
 ~WaveformEnvelopeAnalyser()
 {}
 
 void setClumpingFrequency(float cf)
 { if (cf > 0.) clumpingFrequency = cf; }
 
 void setRefine(int r)
 { if (r > 0) refine = r; }
 
 void setRemoveThreshold(float rt)
 { if (rt > 0) removeMaximaBelow = rt; }
 
 WaveformEnvelope* generateEnvelope(int channel)
 {
  int _refine = refine;
  std::unique_ptr<WaveformEnvelope> env {new WaveformEnvelope()};
  env->sampleRate = sampleRate;
  
  int count = static_cast<int>(reader.lengthInSamples);
  
  // ===========================================================
  // First pass
  // Record maxima between zero-crossings
  if (_refine > 0)
  {
   --_refine;
   int i = 0;
   while (i < count && reader.read(channel, i) == 0.) ++i;

   // If the sample is empty then a zero envelope will suffice;
   if (i == count) return env.release();
   
   
   while (i < count)
   {
    if (cancel()) return nullptr;
    float sign = XDDSP::signum(reader.read(channel, i));
    WaveformEnvelope::Maxima maxima = {i, fabs(reader.read(channel, i))};
    while ((++i < count) &&
           (XDDSP::signum(reader.read(channel, i)) == sign))
    {
     if (fabs(reader.read(channel, i)) > maxima.amplitude)
     {
      maxima.time = i;
      maxima.amplitude = fabs(reader.read(channel, i));
     }
    }
    env->insertMaxima(maxima);
   }
  }
  // ===========================================================
  // Second pass
  // Delete envelope points which have higher pointson both sides that are within
  // clumping distance
  if (_refine > 0)
  {
   --_refine;
   int i = static_cast<int>(env->wMaxima.size() - 1);
   auto b = env->wMaxima.begin();
   while (--i > 0)
   {
    if (cancel()) return nullptr;
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
  // Find the absolute maximum peak
  // Delete any maxima which amplitude is less than a percentage of the absolute maximum
  if (_refine > 0)
  {
   --_refine;
   if (cancel()) return nullptr;
   env->wPeakMaxima = env->wMaxima.front().amplitude;
   env->wPeakTime = env->wMaxima.front().time;
   for (int i = 1; i < env->wMaxima.size(); ++i)
   {
    if (env->wMaxima[i].amplitude > env->wPeakMaxima)
    {
     env->wPeakMaxima = env->wMaxima[i].amplitude;
     env->wPeakTime = env->wMaxima[i].time;
    }
   }
   float threshold = removeMaximaBelow*env->wPeakMaxima;
   if (cancel()) return nullptr;
   int i = static_cast<int>(env->wMaxima.size() - 1); // Don't check the last envelope point
   auto b = env->wMaxima.begin();
   while (--i > 1) // Don't check the first point either
   {
    if (env->wMaxima[i].amplitude < threshold)
    {
     env->wMaxima.erase(b + i);
    }
   }
  }
  // ===========================================================
  // Refine passes
  // Validate the envelope and add an extra maxima for each segment where the
  // waveform value exceeds the envelope value
  bool validated {false};
  while (!validated && _refine-- > 0)
  {
   validated = true;
   int i = 0;
   while (i < count)
   {
    if (cancel()) return nullptr;
    if (fabs(reader.read(channel, i)) > env->amplitudeAtSample(i))
    {
     validated = false;
     // Identify which segment we have landed on and set up loop points
     int maximaIndex = env->maximaAfterTimeOrEnd(i);
     // If we are at the start or end of the maxima envelope, simply add another maxima
     if (maximaIndex == 0 ||
         maximaIndex == env->maxima.size())
     {
      env->insertMaxima({i, reader.read(channel, i)});
      ++i;
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
       if (cancel()) return nullptr;
       WaveformEnvelope::Maxima p {j, fabs(reader.read(channel, j))};
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

  return env.release();
 }
};
