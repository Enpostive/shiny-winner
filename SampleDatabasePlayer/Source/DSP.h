/*
  ==============================================================================

    jucer_AudioPluginDSPTemplate.h
    Created: 11 Apr 2024 5:14:40pm
    Author:  Adam Jackson

  ==============================================================================
*/

#pragma once

#include "../../Source/XDDSP/XDDSP.h"

namespace XDDSP
{










/*
 template <typename SignalIn>
 class NewComponent : public Component<NewComponent<SignalIn>>
 {
  // Private data members here
 public:
  static constexpr int Count = SignalIn::Count;
  
  // Specify your inputs as public members here
  SignalIn signalIn;
  
  // Specify your outputs like this
  Output<Count> signalOut;
  
  // Include a definition for each input in the constructor
  NewComponent(Parameters &p, SignalIn _signalIn) :
  signalIn(_signalIn),
  signalOut(p)
  {}
  
  // This function is responsible for clearing the output buffers to a default state when
  // the component is disabled.
  void reset()
  {
   signalOut.reset();
  }
  
  // startProcess prepares the component for processing one block and returns the step
  // size. By default, it returns the entire sampleCount as one big step.
 // int startProcess(int startPoint, int sampleCount)
 // { return std::min(sampleCount, StepSize); }

  // stepProcess is called repeatedly with the start point incremented by step size
  void stepProcess(int startPoint, int sampleCount)
  {
   for (int c = 0; c < Count; ++c)
   {
    for (int i = startPoint, s = sampleCount; s--; ++i)
    {
     // DSP work done here
    }
   }
  }
  
  // finishProcess is called after the block has been processed
 // void finishProcess()
 // {}
 };

  
  
  
  
  
  
  
  
  

 */
template <int ChannelCount = 1>
class Counter : public Component<Counter<ChannelCount>>
{
 // Private data members here
 bool counting {false};
 int count {0};
 int minimum {0};
 int maximum {INT_MAX};
public:
 static constexpr int Count = ChannelCount;
 
 // Specify your inputs as public members here
 // This component has no inputs
 
 // Specify your outputs like this
 Output<Count> signalOut;
 
 // Include a definition for each input in the constructor
 Counter(Parameters &p) :
 signalOut(p)
 {}
 
 void setMinMax(int min, int max)
 {
  if (min > max) std::swap(min, max);
  minimum = min;
  maximum = max;
 }
 
 void setMinimum(int min)
 { setMinMax(min, maximum); }
 
 void setMaximum(int max)
 { setMinMax(minimum, max); }
 
 void setCounter(int value = 0)
 {
  count = value;
  counting = true;
 }
 
 // This function is responsible for clearing the output buffers to a default state when
 // the component is disabled.
 void reset()
 {
  counting = false;
  signalOut.reset();
 }
 
 // startProcess prepares the component for processing one block and returns the step
 // size. By default, it returns the entire sampleCount as one big step.
// int startProcess(int startPoint, int sampleCount)
// { return std::min(sampleCount, StepSize); }

 // stepProcess is called repeatedly with the start point incremented by step size
 void stepProcess(int startPoint, int sampleCount)
 {
  for (int i = startPoint, s = sampleCount; s--; ++i)
  {
   int xi;
   if (counting)
   {
    xi = count++;
    if (count == maximum) counting = false;
   }
   else
   {
    xi = 0;
   }
   SampleType x = static_cast<SampleType>(xi);
   
   for (int c = 0; c < Count; ++c)
   {
    signalOut.buffer(c, i) = x;
   }
  }
 }
 
 // finishProcess is called after the block has been processed
// void finishProcess()
// {}
};









class PluginDSP : public Component<PluginDSP>
{
 // Private data members here
 int startDelta {-1};
 
 void proc(int startPoint, int sampleCount)
 {
  playbackPosition.process(startPoint, sampleCount);
  playbackGain.process(startPoint, sampleCount);
  crossfadeRamp.process(startPoint, sampleCount);
  crossfader.process(startPoint, sampleCount);
  amp.process(startPoint, sampleCount);
 }
 
public:
 static constexpr int Count = 2;
 
// PluginInput<Count> signalIn;
 Counter<2> playbackPosition;
 Ramp<ControlConstant<2>, ControlConstant<2>> playbackGain;
 Ramp<ControlConstant<1>, ControlConstant<1>> crossfadeRamp;
 
 SamplePlaybackHead<float, 2> samplePlayHead1;
 SamplePlaybackHead<float, 2> samplePlayHead2;
 
 Crossfader<
 Connector<2>,
 Connector<2>,
 Connector<1>,
 MixingLaws::LinearFadeLaw
 > crossfader;
 
 SimpleGain<Connector<2>, Connector<2>> amp;
 
 Connector<Count> signalOut;
 
 PluginDSP(Parameters &p) :
 playbackPosition(p),
 playbackGain(p, {0.}, {1.}),
 crossfadeRamp(p, {0.}, {0.}),
 samplePlayHead1(playbackPosition.signalOut),
 samplePlayHead2(playbackPosition.signalOut),
 crossfader(p, samplePlayHead1, samplePlayHead2, crossfadeRamp.rampOut),
 amp(p, crossfader.signalOut, playbackGain.rampOut),
 signalOut({amp.signalOut})
 {}
 
 // This function is responsible for clearing the output buffers to a default state when
 // the component is disabled.
 void reset() override
 {
  playbackPosition.reset();
  playbackGain.reset();
  crossfadeRamp.reset();
  crossfader.reset();
  amp.reset();
 }
 
 int startProcess(int startPoint, int sampleCount) override
 {
  if (startDelta >= startPoint && startDelta < startPoint + sampleCount)
  {
   playbackPosition.setCounter(startPoint - startDelta);
   playbackGain.setRampTime(startPoint - startDelta, 1);
  }
  if (startDelta >= 0) startDelta -= sampleCount;
  return sampleCount;
 }
 
 // stepProcess is called repeatedly with the start point incremented by step size
 void stepProcess(int startPoint, int sampleCount) override
 {
  proc(startPoint, sampleCount);
 }
 
 void noteOn(int posDelta)
 {
  startDelta = posDelta;
  playbackGain.startIn.setControl(1.);
 }
};





}
