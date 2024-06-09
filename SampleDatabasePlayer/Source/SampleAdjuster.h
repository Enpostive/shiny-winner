/*
 ==============================================================================
 
 SampleAdjuster.h
 Created: 6 Jun 2024 9:23:21pm
 Author:  Adam Jackson
 
 ==============================================================================
 */

#pragma once

#include <JuceHeader.h>
#include "SampleParameters.h"

//==============================================================================
/*
 */
class SampleAdjuster  : public juce::Component, public juce::Timer, public juce::SettableTooltipClient
{
 static constexpr char ThisID[] = "SampleAdjuster";
 static constexpr char FadeStartHandleID[] = "FadeStartHandle";
 static constexpr char FadeInHandleID[] = "FadeInHandle";
 static constexpr char FadeOutHandleID[] = "FadeOutHandle";
 static constexpr char FadeEndHandleID[] = "FadeEndHandle";
 
 static constexpr int FadeLineThickness = 2;
 
 
 
 class DragHandle : public juce::Component
 {
  static constexpr int HandleBoxLineThickness = FadeLineThickness;
  
 public:
  juce::Colour colour {juce::Colours::white};
  
  DragHandle()
  {
   setSize(6, 6);
   setRepaintsOnMouseActivity(true);
  }
  
  void paint (juce::Graphics& g) override
  {
   if (isEnabled() && isMouseOver()) g.setColour(colour);
   else g.setColour(colour.withBrightness(0.5));
   g.drawRect(getLocalBounds(), HandleBoxLineThickness);
  }
  
  int getCentreX()
  { return getX() + getWidth()/2; }
  
  int getCentreY()
  { return getY() + getHeight()/2; }
 };
 
 
 
 
 
 std::vector<float> *leftWaveform {nullptr};
 std::vector<float> *rightWaveform {nullptr};
 SampleParameters *params {nullptr};
 int length {0};
 float samplesPerPixel;
 float handlesPerLength;
 float halfHeight;
 juce::Path waveformPath;
 
 
 
 
 
 DragHandle fadeStartHandle;
 DragHandle fadeInHandle;
 DragHandle fadeOutHandle;
 DragHandle fadeEndHandle;
 
 enum
 {
  NotEditing = 0,
  EditingFadeStart,
  EditingFadeIn,
  EditingFadeOut,
  EditingFadeEnd,
  EditingSampleOffset
 };
 
 int editStatus {NotEditing};
 int editStartPoint;
 
 
 
 
 
 void rescale()
 {
  samplesPerPixel = float(length)/float(getWidth());
  halfHeight = 0.5*getHeight();
  if (length > 0) handlesPerLength = float(getWidth() - fadeInHandle.getWidth())/float(length);
  else handlesPerLength = 1.;
  setHandlePositions();
 }
   
 
 
 
 
public:
 
 
 
 
 std::function<void ()> onUpdate;
 
 
 
 
 
 SampleAdjuster()
 {
  setComponentID(ThisID);
//  addMouseListener(this, true);

  addAndMakeVisible(fadeStartHandle);
  fadeStartHandle.setComponentID(FadeStartHandleID);
  fadeStartHandle.addMouseListener(this, false);
  
  addAndMakeVisible(fadeInHandle);
  fadeInHandle.setComponentID(FadeInHandleID);
  fadeInHandle.addMouseListener(this, false);
  
  addAndMakeVisible(fadeOutHandle);
  fadeOutHandle.setComponentID(FadeOutHandleID);
  fadeOutHandle.addMouseListener(this, false);
  
  addAndMakeVisible(fadeEndHandle);
  fadeEndHandle.setComponentID(FadeEndHandleID);
  fadeEndHandle.addMouseListener(this, false);
  
  setTooltip("Drag the sample! Double click to reset. Drag handes to adjust fades.");
 }
 
 ~SampleAdjuster() override
 {
 }
 
 
 
 
 
 void timerCallback() override
 {
  stopTimer();
  if (onUpdate) onUpdate();
 }
 
 
 
 
 
 void connect(std::vector<float> &l,
              std::vector<float> &r,
              SampleParameters &p)
 {
  params = &p;
  leftWaveform = &l;
  rightWaveform = &r;
  setHandlePositions();
  repaint();
 }
 
 void disconnect()
 {
  params = nullptr;
  leftWaveform = nullptr;
  rightWaveform = nullptr;
  repaint();
 }
 
 
 
 
 
 void setHandlePositions()
 {
  if (params)
  {
   fadeStartHandle.setTopLeftPosition(params->fadeInStart*handlesPerLength,
                                      getHeight() - fadeInHandle.getHeight());
   fadeInHandle.setTopLeftPosition((params->fadeInStart + params->fadeInLength)*handlesPerLength, 0);
   fadeOutHandle.setTopLeftPosition((params->fadeOutEnd - params->fadeOutLength)*handlesPerLength, 0);
   fadeEndHandle.setTopLeftPosition(params->fadeOutEnd*handlesPerLength,
                                    getHeight() - fadeInHandle.getHeight());
  }
 }
 
 
 
 
 
 void updateWaveform(int l)
 {
  length = l;
  rescale();

  waveformPath.clear();
  if (leftWaveform && rightWaveform && params)
  {
   
   waveformPath.startNewSubPath(0.f, 0.f);
   
   std::vector<float> &left = *leftWaveform;
   std::vector<float> &right = *rightWaveform;
   int si = params->sampleOffset;
   int fi;
   
   int width = length/samplesPerPixel;
   
   for (int x = 0; x < width; ++x)
   {
    fi = params->sampleOffset + static_cast<int>(round(static_cast<float>(x + 1)*samplesPerPixel)) - 1;
    si = XDDSP::boundary(si, 0, static_cast<int>(left.size() - 1));
    fi = XDDSP::boundary(fi, 0, static_cast<int>(left.size() - 1));
    float max = std::max(fabs(left[si]), fabs(right[si]));
    for (int i = si + 1; i < fi; ++i)
    {
     max = std::max(max, std::max(fabs(left[i]), fabs(right[i])));
    }
    waveformPath.lineTo(x, max);
    si = fi + 1;
   }
   waveformPath.lineTo(getWidth(), 0.f);
   waveformPath.closeSubPath();
  }
 }
 
 
 
 
 
 void paint (juce::Graphics& g) override
 {
  g.fillAll(juce::Colours::black);
   
  g.setColour(juce::Colours::white);
  auto t = (juce::AffineTransform()
            .scale(1.f, halfHeight)
            .translated(0.f, halfHeight));
  g.fillPath(waveformPath, t);
  t = (juce::AffineTransform()
       .scale(1.f, -halfHeight)
       .translated(0.f, halfHeight));
  g.fillPath(waveformPath, t);
  
  if (isEnabled())
  {
   g.setColour(juce::Colours::yellow);
   g.drawLine(params->fadeInStart/samplesPerPixel, getHeight(),
              (params->fadeInStart + params->fadeInLength)/samplesPerPixel, 0.,
              FadeLineThickness);
   g.drawLine((params->fadeOutEnd - params->fadeOutLength)/samplesPerPixel,
              0., params->fadeOutEnd/samplesPerPixel, getHeight(),
              FadeLineThickness);
  }
 }
 
 
 
 
 
 void resized() override
 {
  setHandlePositions();
  updateWaveform(length);
 }
 
 
 
 
 
 void mouseDown(const juce::MouseEvent &e) override
 {
  if (e.originalComponent->getComponentID() == FadeStartHandleID)
  {
   editStatus = EditingFadeStart;
   if (params) editStartPoint = params->fadeInStart;
  }
  else if (e.originalComponent->getComponentID() == FadeInHandleID)
  {
   editStatus = EditingFadeIn;
   if (params) editStartPoint = params->fadeInLength;
  }
  else if (e.originalComponent->getComponentID() == FadeOutHandleID)
  {
   editStatus = EditingFadeOut;
   if (params) editStartPoint = params->fadeOutLength;
  }
  else if (e.originalComponent->getComponentID() == FadeEndHandleID)
  {
   editStatus = EditingFadeEnd;
   if (params) editStartPoint = params->fadeOutEnd;
  }
  else
  {
   editStatus = EditingSampleOffset;
   if (params) editStartPoint = params->sampleOffset;
  }
 }
 
 
 
 
 
 void mouseDrag(const juce::MouseEvent &e) override
 {
  int changeAmount = e.getScreenX() - e.getMouseDownScreenX();
  if (e.mods.isShiftDown())
  {
   // Fine adjustments
   e.source.enableUnboundedMouseMovement(true);
  }
  else
  {
   // Coarse adjustments
   e.source.enableUnboundedMouseMovement(false);
   changeAmount = changeAmount*samplesPerPixel;
  }
  
  if (params) switch (editStatus)
  {
   case EditingFadeStart:
    params->fadeInStart = editStartPoint + changeAmount;
    goto WasEdit;
    
   case EditingFadeIn:
    params->fadeInLength = editStartPoint + changeAmount;
    goto WasEdit;

   case EditingFadeOut:
    params->fadeOutLength = editStartPoint - changeAmount;
    goto WasEdit;

   case EditingFadeEnd:
    params->fadeOutEnd = editStartPoint + changeAmount;

    WasEdit:
    params->sanitiseParameters(length);
    startTimer(10);
    break;
    
   case EditingSampleOffset:
    params->sampleOffset = editStartPoint - changeAmount;
    startTimer(10);
    
   default:
    break;
  }
 }
 
 
 
 
 
 void mouseUp(const juce::MouseEvent &e) override
 {
  switch (editStatus)
  {
   case EditingFadeStart:
   case EditingFadeIn:
   case EditingFadeOut:
   case EditingFadeEnd:
    editStatus = NotEditing;

   default:
    break;
  }
 }
 
 
 
 
 
 void mouseDoubleClick(const juce::MouseEvent &e) override
 {
  if (params)
  {
   params->sampleOffset = 0;
   startTimer(10);
  }
 }

 
 
 
 
private:
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleAdjuster)
};
