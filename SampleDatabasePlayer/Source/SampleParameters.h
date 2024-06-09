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
 Analysis analysis;
 int leftSlot {0};
 int rightSlot {0};
 int sampleOffset {0};
 int fadeInStart {0};
 int fadeInLength {5};
 int fadeOutLength {5};
 int fadeOutEnd {INT_MAX};
 float playbackLevelKdB {-15.};
 bool highPassEnable {false};
 bool paramEnable {false};
 bool lowPassEnable {false};
 float highPassHz {100.};
 float paramHz {2000.};
 float paramGain {0.};
 float paramQ {0.7};
 float lowPassHz {15000.};

 void sanitiseParameters(int length)
 {
  fadeOutEnd = XDDSP::boundary(fadeOutEnd, 1, length);
  fadeInStart = XDDSP::boundary(fadeInStart, 0,
                                        fadeOutEnd - 1);
  fadeInLength = XDDSP::boundary(fadeInLength, 1,
                                         fadeOutEnd - fadeInStart);
  fadeOutLength = XDDSP::boundary(fadeOutLength, 1,
                                          fadeOutEnd - fadeInStart);
 }
};
