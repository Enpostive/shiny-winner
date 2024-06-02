/*
 ==============================================================================
 
 ProgressSpinner.h
 Created: 31 May 2024 3:49:08pm
 Author:  Adam Jackson
 
 ==============================================================================
 */

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
 */
class ProgressSpinner  : public juce::Component, public juce::Timer
{
 static constexpr float OuterRadius = 10;
 static constexpr float InnerRadius = 7.5;
 static constexpr int SegmentCount = 8;
 static constexpr float SegmentAngle = 2.*M_PI/static_cast<float>(SegmentCount);
 static constexpr float FillFraction = 0.9;
 static constexpr float FillAngle = 0.5*FillFraction*SegmentAngle;
 
 int step {0};
 juce::Path segment;
 
 float centreX;
 float centreY;
 
public:
 juce::Colour colour1 {juce::Colours::lightblue};
 juce::Colour colour2 {juce::Colours::lightblue.withBrightness(0.5)};
 
 ProgressSpinner()
 {
  segment.startNewSubPath(OuterRadius*sin(-FillAngle), OuterRadius*cos(-FillAngle));
  segment.lineTo(OuterRadius*sin(FillAngle), OuterRadius*cos(FillAngle));
  segment.lineTo(InnerRadius*sin(FillAngle), InnerRadius*cos(FillAngle));
  segment.lineTo(InnerRadius*sin(-FillAngle), InnerRadius*cos(-FillAngle));
  segment.closeSubPath();
  
  startTimerHz(10);
 }
 
 ~ProgressSpinner() override
 {}
 
 void timerCallback() override
 {
  step = (step + 1) % SegmentCount;
  repaint();
 }
 
 void paint (juce::Graphics& g) override
 {
  for (int i = 0; i < SegmentCount; ++i)
  {
   g.saveState();
   g.addTransform(juce::AffineTransform()
                  .rotation(SegmentAngle*i)
                  .withAbsoluteTranslation(centreX, centreY));
   bool illuminated = (i == step);
   g.setColour(illuminated ? colour1 : colour2);
   g.fillPath(segment);
   g.restoreState();
  }
 }
 
 void resized() override
 {
  centreX = 0.5*static_cast<float>(getWidth());
  centreY = 0.5*static_cast<float>(getHeight());
 }
 
private:
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProgressSpinner)
};
