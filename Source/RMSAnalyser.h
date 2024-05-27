/*
  ==============================================================================

    RMSAnalyser.h
    Created: 25 May 2024 4:18:30pm
    Author:  Adam Jackson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "XDDSP/XDDSP.h"
#include "AudioReaderCache.h"

class RMSAnalyser : public XDDSP::Parameters::ParameterListener
{
 static constexpr float SecondsToAnalyse = 0.1;
 
 XDDSP::BiquadFilterCoefficients kshelfcoeff;
 XDDSP::BiquadFilterCoefficients khpcoeff;

 class AnalysisAccumulator
 {
  int count {0};
  float accum {0.};
  
 public:
  void addSample(float x)
  {
   ++count;
   accum += x*x;
  }
  
  float getRMS()
  {
   const float div = accum / static_cast<float>(count);
   return sqrt(div);
  }
 };
 
 void setCoefficients(double sr)
 {
  double a;
  
  a = tan(M_PI*1681.974450955533/sr);
  kshelfcoeff.setCustomFilter((a*a + 1.258720930232562*(a/0.7071752369554196) +
                               1.584864701130855) /
                              (a*a + (a/0.7071752369554196) + 1.),
                              2.*(a*a - 1.584864701130855) /
                              (a*a + (a/0.7071752369554196) + 1.),
                              (a*a - 1.258720930232562*(a/0.7071752369554196) +
                               1.584864701130855) /
                              (a*a + (a/0.7071752369554196) + 1.),
                              2.*(a*a - 1.) /
                              (a*a + (a/0.7071752369554196) + 1.),
                              (a*a - (a/0.7071752369554196) + 1.) /
                              (a*a + (a/0.7071752369554196) + 1.));
  
  a = tan(M_PI*38.13547087602444/sr);
  khpcoeff.setCustomFilter(1.,
                           -2.,
                           1.,
                           2.*(a*a - 1.) /
                           (a*a + (a/0.5003270373238773) + 1.),
                           (a*a - (a/0.5003270373238773) + 1.) /
                           (a*a + (a/0.5003270373238773) + 1.));
  /*
  kHighshelfCoeff.b0 = ;
  kHighshelfCoeff.b1 = ;
  kHighshelfCoeff.b2 = ;
  kHighshelfCoeff.a1 = ;
  kHighshelfCoeff.a2 = ;

  kHighpassCoeff.b0 = kHighpassCoeff.b2 = 1.;
  kHighpassCoeff.b1 = -2;
  kHighpassCoeff.a1 = ;
  kHighpassCoeff.a2 = ;
   */
 }
 
public:
 RMSAnalyser(XDDSP::Parameters &p) :
 XDDSP::Parameters::ParameterListener(p),
 kshelfcoeff(p),
 khpcoeff(p)
 {
  updateSampleRate(p.sampleRate(), p.getSampleOffset());
 }
 
 virtual void updateSampleRate(double sr, double isr) override
 {
  setCoefficients(sr);
 }

 float calculateRMS(juce::AudioFormatReader &_reader)
 {
  AudioReaderCache reader(_reader);
  AnalysisAccumulator accum;
  
  int samplesToAnalyse = std::min(static_cast<int>(reader.lengthInSamples),
                                  static_cast<int>(ceil(reader.sampleRate*SecondsToAnalyse)));
  
  for (int i = 0; i < samplesToAnalyse; ++i)
  {
   for (int j = 0; j < _reader.numChannels; ++j)
   {
    accum.addSample(reader.read(j, i));
   }
  }
  
  return accum.getRMS();
 }

 float calculateKWeightedRMS(juce::AudioFormatReader &_reader)
 {
  AudioReaderCache reader(_reader);
  AnalysisAccumulator accum;
  XDDSP::BiquadFilterKernel hpKern;
  XDDSP::BiquadFilterKernel shelfKern;
  
  int samplesToAnalyse = std::min(static_cast<int>(reader.lengthInSamples),
                                  static_cast<int>(ceil(reader.sampleRate*SecondsToAnalyse)));
  for (int channel = 0; channel < _reader.numChannels; ++channel)
  {
   hpKern.reset();
   shelfKern.reset();
   for (int i = 0; i < samplesToAnalyse; ++i)
   {
    float x = reader.read(channel, i);
    x = shelfKern.process(kshelfcoeff, x);
    x = hpKern.process(khpcoeff, x);
    accum.addSample(x);
   }
  }
  
  return accum.getRMS();
 }
};
