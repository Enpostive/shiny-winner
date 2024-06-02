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









class PluginDSP : public Component<PluginDSP>
{
 // Private data members here
public:
 static constexpr int Count = 2;
 
 PluginInput<Count> signalIn;
 Connector<Count> signalOut;
 
 PluginDSP(Parameters &p) :
 signalOut({signalIn})
 {}
 
 // This function is responsible for clearing the output buffers to a default state when
 // the component is disabled.
 void reset()
 {
 }
 
 // stepProcess is called repeatedly with the start point incremented by step size
 void stepProcess(int startPoint, int sampleCount)
 {
 }
 
};
}
