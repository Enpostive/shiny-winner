/*
 ==============================================================================
 
 Zoom.h
 Created: 16 Jun 2024 9:31:21pm
 Author:  Adam Jackson
 
 ==============================================================================
 */

#pragma once

#include <JuceHeader.h>
#include "../../Source/Analyser.h"

//==============================================================================
/*
 */
class Zoomer : public juce::Component, public juce::Timer
{
 static constexpr float PixelsToMoveToDoubleZoomFactor = 50;
 static constexpr float DefaultZoomFactor = 0.125;
 static constexpr float VerticalScaleFactor = 0.9;
 
 
 AudioReaderCache *reader;
 Analysis *env;
 int sampleOffset {0};
 float zoomFactor {DefaultZoomFactor};
 
 juce::AffineTransform waveTransform;
 juce::AffineTransform invertedWaveTransform;
 juce::Path waveformPath;
 juce::Path envPath;
 

 bool editStatus {false};
 float waveformClickPoint;
 float initialZoomFactor;

 
 
 void recalculateTransform()
 {
  float halfHeight = 0.5*getHeight();
  waveTransform = (juce::AffineTransform()
                   .translated(-sampleOffset, 0.)
                   .scaled(zoomFactor, 0.9*halfHeight)
                   .translated(0., halfHeight));
  invertedWaveTransform = (juce::AffineTransform()
                           .translated(-sampleOffset, 0.)
                           .scaled(zoomFactor, -VerticalScaleFactor*halfHeight)
                           .translated(0., halfHeight));
 }
 
 
 
public:
 
 
 
 
 std::function<void ()> onUpdate;
 const int &offset {sampleOffset};
 const float &zoom {zoomFactor};
 
 bool drawStereo {false};
 
 
 float guideLevel {1.};
 
 
 
 
 Zoomer()
 {
 }
 
 ~Zoomer() override
 {
 }
 
 
 
 
 
 void timerCallback() override
 {
  stopTimer();
  if (onUpdate) onUpdate();
 }
 
 
 
 
 
 void connectWaveform(AudioReaderCache *_reader)
 {
  reader = _reader;
  updateWaveform();
 }
 
 void connectEnvelope(Analysis *analysis)
 {
  env = analysis;
  updateWaveform();
 }
 
 void disconnect()
 {
  reader = nullptr;
  env = nullptr;
  updateWaveform();
  repaint();
 }

 
 
 
 void updateWaveform()
 {
  waveformPath.clear();
  envPath.clear();
  
  juce::Path leftChannel;
  juce::Path rightChannel;
  juce::Path envChannel;
  
  if (reader)
  {
   jassert(reader->numChannels == 1 || reader->numChannels == 2);
   
   leftChannel.startNewSubPath(0., 0.);
   rightChannel.startNewSubPath(0., 0.);
   for (int i = 0; i < reader->lengthInSamples; ++i)
   {
    float x = static_cast<float>(i);
    leftChannel.lineTo(x, reader->read(0, i));
    if (drawStereo && reader->numChannels == 2) rightChannel.lineTo(x, reader->read(1, i));
   }
   waveformPath.addPath(leftChannel);
   if (drawStereo && reader->numChannels == 2) waveformPath.addPath(rightChannel);
  }
  
  if (env)
  {
   envPath.startNewSubPath(0, 0);
   for (auto &m : env->envMonoLeft->maxima)
   {
    envPath.lineTo(m.time, m.amplitude);
   }
   if (drawStereo && env->stereo)
   {
    envPath.startNewSubPath(0, 0);
    for (auto &m: env->envRight->maxima)
    {
     envPath.lineTo(m.time, m.amplitude);
    }
   }
  }
 }
 
 
 
 
 
 void paint (juce::Graphics& g) override
 {
  g.fillAll(juce::Colours::black);
   
  g.setColour(juce::Colours::white);
  juce::Path drawPath = waveformPath;
  drawPath.applyTransform(waveTransform);
  g.strokePath(drawPath, juce::PathStrokeType(1));
  
  g.setColour(juce::Colours::green);
  drawPath = envPath;
  drawPath.applyTransform(waveTransform);
  g.strokePath(drawPath, juce::PathStrokeType(1));
  drawPath = envPath;
  drawPath.applyTransform(invertedWaveTransform);
  g.strokePath(drawPath, juce::PathStrokeType(1));
  
  float guideY;
  g.setColour(juce::Colours::red);
  guideY = getHeight()*(0.5 - VerticalScaleFactor*0.5*guideLevel);
  g.drawHorizontalLine(guideY, 0, getWidth());
  guideY = getHeight()*(0.5 + VerticalScaleFactor*0.5*guideLevel);
  g.drawHorizontalLine(guideY, 0, getWidth());
 }
 
 
 
 
 
 void resized() override
 {
  recalculateTransform();
 }
 
 
 
 
 
 void mouseDown(const juce::MouseEvent &e) override
 {
  waveformClickPoint = e.getPosition().x/zoomFactor + sampleOffset;
  initialZoomFactor = zoomFactor;
  editStatus = true;
 }
 
 
 
 
 
 void mouseDrag(const juce::MouseEvent &e) override
 {
  zoomFactor = initialZoomFactor*powf(2., e.getDistanceFromDragStartY()/PixelsToMoveToDoubleZoomFactor);
  float samplesFromLeftBoundary = e.getPosition().x/zoomFactor;
  sampleOffset = waveformClickPoint - samplesFromLeftBoundary;
  recalculateTransform();
  repaint();
 }
 
 
 
 
 
 void mouseUp(const juce::MouseEvent &e) override
 {
  editStatus = false;
 }
 
 
 
 
 
 void mouseDoubleClick(const juce::MouseEvent &e) override
 {
  sampleOffset = 0;
  zoomFactor = DefaultZoomFactor;
  recalculateTransform();
  repaint();
 }

 
 
 
 
private:
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Zoomer)
};
