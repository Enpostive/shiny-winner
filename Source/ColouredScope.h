/*
 ==============================================================================
 
 CircularBufferScope.h
 Created: 17 Aug 2023 8:20:42pm
 Author:  Adam Jackson
 
 ==============================================================================
 */

#pragma once

#include <JuceHeader.h>
#include <utility>
#include "XDDSP/XDDSP.h"
#include "SampleEnvelopeAnalyser.h"

//==============================================================================
/*
 */
class ScopeDataSource
{
protected:
 juce::Colour translateSpectrumToColour(float bass,
                                        float mids,
                                        float high,
                                        const juce::Colour &defaultColour)
 {
  float head = std::max(std::max(bass, mids), high);
  if (head > 0.)
  {
   head = 255. / head;
   bass *= head;
   mids *= head;
   high *= head;
   
   if (std::isinf(bass))
   {
    bass = defaultColour.getRed();
    mids = defaultColour.getGreen();
    high = defaultColour.getBlue();
   }
  }
  else
  {
   bass = defaultColour.getRed();
   mids = defaultColour.getGreen();
   high = defaultColour.getBlue();
  }
  return juce::Colour(static_cast<uint8_t>(bass),
                      static_cast<uint8_t>(mids),
                      static_cast<uint8_t>(high));
 }

public:
// typedef std::pair<float, float> MinMaxPair;
 struct ScopePoint
 {
  float min;
  float max;
  juce::Colour colour;
 };
 
 virtual ~ScopeDataSource() {};
 virtual ScopePoint getRange(int start, int end) = 0;
 virtual unsigned int getRangeSize() = 0;
};










template <typename BufferType>
class CircularBufferSource : public ScopeDataSource
{
 BufferType &buffer;
 BufferType &bassBuffer;
 BufferType &midsBuffer;
 BufferType &highBuffer;
 
 unsigned int windowSize;
 
 void constrain(int &index)
 {
  index = XDDSP::boundary<int>(index, 0, buffer.getSize() - 1);
 }
 
 void prepareIndexes(int &start, int &end)
 {
  constrain(start);
  constrain(end);
  if (end < start) std::swap(start, end);
 }
 
public:
 juce::Colour defaultColour {juce::Colours::white.withBrightness(0.5)};

 CircularBufferSource(BufferType &fullAudio,
                      BufferType &bass,
                      BufferType &mids,
                      BufferType &highs) :
 buffer(fullAudio),
 bassBuffer(bass),
 midsBuffer(mids),
 highBuffer(highs),
 windowSize(fullAudio.getSize())
 {}
 
 virtual ScopePoint getRange(int start, int end) override
 {
  prepareIndexes(start, end);
  ScopePoint result = {buffer.tapOut(start),
   buffer.tapOut(start),
   juce::Colours::white};
  float mb = fabs(bassBuffer.tapOut(start));
  float mm = fabs(midsBuffer.tapOut(start));
  float mh = fabs(highBuffer.tapOut(start));

  for (int i = start + 1; i < end; ++i)
  {
   float t = buffer.tapOut(i);
   result.min = std::min(result.min, t);
   result.max = std::max(result.max, t);
   mb = std::max(mb, fabs(bassBuffer.tapOut(i)));
   mm = std::max(mm, fabs(midsBuffer.tapOut(i)));
   mh = std::max(mh, fabs(highBuffer.tapOut(i)));
  }
  
  result.colour = translateSpectrumToColour(mb, mm, mh, defaultColour);
  
  return result;
 }
 
 void setWindowSize(unsigned int newSize)
 {
  windowSize = std::min(newSize, buffer.getSize());
 }
 
 virtual unsigned int getRangeSize() override
 { return windowSize; }
};










class AudioFileScopeSource : public ScopeDataSource
{
 static constexpr unsigned long ProcessingLeadIn = 100;
 static constexpr int NumberBufferChannels = 5;
 
 juce::AudioFormatReader *reader {nullptr};
 int channelSelected {0};
 juce::AudioBuffer<float> buffer;
 juce::AudioBuffer<float> leadIn;
 
 XDDSP::Parameters dspParam;
 XDDSP::BiquadFilterCoefficients lowLPCoeff;
 XDDSP::BiquadFilterCoefficients highLPCoeff;
 XDDSP::BiquadFilterKernel lowLP;
 XDDSP::BiquadFilterKernel highLP;
 
 float bassSample;
 float midsSample;
 float highSample;
 
 void procFilters(float sample)
 {
  bassSample = lowLP.process(lowLPCoeff, sample);
  sample -= bassSample;
  midsSample = highLP.process(highLPCoeff, sample);
  highSample = sample - midsSample;
 }

 long offset {0};
 int windowSize {0};
 float gain {1.f};
 
 void constrain(int &index)
 {
  index = XDDSP::boundary<int>(index, 0, windowSize - 1);
 }
 
 void prepareIndexes(int &start, int &end)
 {
  constrain(start);
  constrain(end);
  if (end < start) std::swap(start, end);
 }
 
 void readIntoBuffer(long offset, long startInSource, int length)
 {
  float *bp[2];
  bp[0] = buffer.getWritePointer(0, static_cast<int>(offset));
  bp[1] = buffer.getWritePointer(1, static_cast<int>(offset));
  reader->read(bp, 2, startInSource, length);
 }

 void update(long newOffset, int newWindowSize, bool doReset = false)
 {
  if (reader)
  {
   // Resize the buffer if required
   if (newWindowSize != windowSize)
   {
    buffer.setSize(NumberBufferChannels, static_cast<int>(newWindowSize), !doReset);
   }
   
   // Do the lead in processing
   lowLP.reset();
   highLP.reset();
   reader->read(&leadIn, 0, ProcessingLeadIn, newOffset - ProcessingLeadIn, true, true);
   for (int i = 0; i < ProcessingLeadIn; ++i) procFilters(leadIn.getSample(channelSelected, i));
   
   //    reader->read(&buffer, 0, fillBeginningAmount, newOffset, true, true);
   readIntoBuffer(0, newOffset, newWindowSize);
   for (int i = 0; i < newWindowSize; ++i)
   {
    procFilters(buffer.getSample(channelSelected, i));
    buffer.setSample(2, i, bassSample);
    buffer.setSample(3, i, midsSample);
    buffer.setSample(4, i, highSample);
   }
  }
  else
  {
   if (newWindowSize != windowSize)
   {
    buffer.setSize(NumberBufferChannels, static_cast<int>(newWindowSize));
   }
   buffer.clear();
  }
  
  offset = newOffset;
  windowSize = newWindowSize;
 }
 
public:
 juce::Colour defaultColour {juce::Colours::white.withBrightness(0.5)};
 
 AudioFileScopeSource() :
 lowLPCoeff(dspParam),
 highLPCoeff(dspParam)
 {
  leadIn.setSize(2, ProcessingLeadIn);
 }
 
 virtual ~AudioFileScopeSource() {}
 
 void attachReader(juce::AudioFormatReader *r, int channel = 0, bool resetWindowSize = false)
 {
  reader = r;
  channelSelected = channel;

  if (reader)
  {
   dspParam.setSampleRate(reader->sampleRate);
   update(offset,
          resetWindowSize ? static_cast<int>(getFileLength()) : windowSize,
          true);
  }
 }
 
 void setGain(float linearGain)
 { gain = linearGain; }
 
 void setGainDB(float gainDB)
 { gain = XDDSP::dB2Linear(gainDB); }
 
 long getFileLength()
 {
  if (!reader) return 0;
  return reader->lengthInSamples;
 }
 
 void setCrossovers(float low, float high)
 {
  lowLPCoeff.setLowPassFilter(low, 0.707);
  highLPCoeff.setLowPassFilter(high, 0.707);
 }
 
 void setWindowSize(int newWindowSize)
 { update(offset, newWindowSize); }
 
 void setOffset(long newOffset)
 { update(newOffset, windowSize); }
 
 void setOffsetAndWindowSize(long newOffset, int newWindowSize)
 { update(newOffset, newWindowSize); }
 
 virtual ScopePoint getRange(int start, int end) override
 {
  prepareIndexes(start, end);
  float t = gain*(buffer.getSample(channelSelected, start));
  ScopePoint result = {t, t, defaultColour};

  float mb = fabs(buffer.getSample(2, start));
  float mm = fabs(buffer.getSample(3, start));
  float mh = fabs(buffer.getSample(4, start));

  for (int i = start + 1; i < end; ++i)
  {
   float t = gain*(buffer.getSample(channelSelected, i));
   result.min = std::min(result.min, t);
   result.max = std::max(result.max, t);
   mb = std::max(mb, fabs(buffer.getSample(2, i)));
   mm = std::max(mm, fabs(buffer.getSample(3, i)));
   mh = std::max(mh, fabs(buffer.getSample(4, i)));
  }
  
  result.colour = translateSpectrumToColour(mb, mm, mh, defaultColour);

  return result;
 }
 
 virtual unsigned int getRangeSize() override
 { return windowSize; }
};










class AnalysisWaveformSource : public ScopeDataSource
{
 unsigned int windowSize;

 void constrain(int &index)
 {
  index = XDDSP::boundary<int>(index, 0, windowSize - 1);
 }
 
 void prepareIndexes(int &start, int &end)
 {
  constrain(start);
  constrain(end);
  if (end < start) std::swap(start, end);
 }

public:
 WaveformEnvelope *env {nullptr};
 AudioFileScopeSource *comparison {nullptr};
 
 virtual ScopePoint getRange(int start, int end) override
 {
  ScopePoint point;
  point.min = 0.;
  point.max = 0.;
  point.colour = juce::Colours::grey;//.withAlpha(0.3f);
  
  if (env)
  {
   prepareIndexes(start, end);
   float e = env->amplitudeAtSample(start);

   for (int i = start + 1; i < end; ++i)
   {
    e = std::max(e, env->amplitudeAtSample(i));
   }

   point.min = -e;
   point.max = e;
   
   if (comparison)
   {
    ScopePoint cmp = comparison->getRange(start, end);
    if (cmp.max > point.max ||
        cmp.min < point.min)
    {
     point.colour = juce::Colours::red;
    }
   }
  }
  
  return point;
 }
 
 void setWindowSize(unsigned int newSize)
 {
  windowSize = newSize;
 }
 
 virtual unsigned int getRangeSize() override
 { return windowSize; }
};









class ColouredScope  : public juce::Component
{
 juce::Image colourBuffer;
 int calculatedWidth {0};
 juce::Path waveformShape;
 std::vector<float> minimums;
 float lastDetectedScaleFactor {1.};
 float guideY1 {0.};
 float guideY2 {0.};
 
 int bufferFlag {0};
 std::array<juce::Image, 2> waveformBuffer;
 
 juce::Image& currBuff()
 { return waveformBuffer[bufferFlag]; }
 
 juce::Image& otherBuff()
 { return waveformBuffer[1 - bufferFlag]; }

 void switchBuffers()
 { bufferFlag = 1 - bufferFlag; }
 
 void resizeBuffers()
 {
  int w = getWidth()*lastDetectedScaleFactor;
  int h = getHeight()*lastDetectedScaleFactor;
  if (w > 0 && h > 0)
  {
   if (resizeBackgroundImage) resizeBackgroundImage(w, h);
   currBuff() = juce::Image(juce::Image::PixelFormat::ARGB, w, h, true);
   otherBuff() = juce::Image(juce::Image::PixelFormat::ARGB, w, h, true);
  }
//  fullRefresh = true;
 }
 
 bool fullRefresh {false};

 float verticalScale {0.5};

public:
 bool strokeEnable {false};
 bool fillEnable {true};
 bool centreEnable {false};
 bool guideEnable {false};
 bool drawBackground {true};
 float minimumThickness {1.};
 
 juce::Image backgroundImage;
 std::function<void (int, int)> resizeBackgroundImage;
 juce::Colour backgroundColour {juce::Colours::blue.withBrightness(0.1)};
 juce::Colour centreLineColour {juce::Colours::transparentWhite};

 bool enableScope {true};
 ScopeDataSource *source {nullptr};
 
 // Default values are suitable for an audio waveform view
 float verticalMidPoint {0.5};
 
 float guidePosition {1.};
 juce::Colour guideColour {juce::Colours::red.withBrightness(0.5)};
 
 bool reverse {true};
 
 ColouredScope()
 {
 }
 
 ~ColouredScope() override
 {
 }
 
 void setVerticalScale(float scale)
 {
  verticalScale = scale;
//  fullRefresh = true;
  currBuff().clear(currBuff().getBounds());
 }
 
 void forceRedrawBackground()
 {
  resizeBuffers();
  repaint();
 }
 
 void update(bool scroll = false, unsigned int amount = 0)
 {
  const float width = static_cast<float>(otherBuff().getWidth());//*lastDetectedScaleFactor;
  const int iWidth = static_cast<int>(ceil(width));
  const float height = static_cast<float>(otherBuff().getHeight());//*lastDetectedScaleFactor;
  const float midPoint = verticalMidPoint*static_cast<float>(height);
  const float scale = verticalScale*height;
  waveformShape.clear();
  
  bool widthChanged {false};
  if (iWidth != calculatedWidth)
  {
   minimums.resize(iWidth);
   calculatedWidth = iWidth;
   widthChanged = true;
  }
  
  if ((colourBuffer.isNull() || widthChanged) && width > 0)
  {
   colourBuffer = juce::Image(juce::Image::PixelFormat::RGB, width, 1, true);
  }
  
  unsigned int updateArea = 0.;
  float spp;
  
  if (source)
  {
   unsigned int rangeSize = source->getRangeSize();
//   unsigned int al = std::max(rangeSize, static_cast<unsigned int>(iWidth));
   unsigned int al = rangeSize;
   spp = static_cast<float>(al) / static_cast<float>(width);
   
   int gStart = 0;
   int gEnd = iWidth - 1;
   
   if (scroll && !fullRefresh && amount < rangeSize)
   {
    updateArea = static_cast<unsigned int>(ceil(amount/spp));
    if (reverse) gStart = gEnd - updateArea;
    else gEnd = updateArea;
   }
   fullRefresh = false;

//   waveformShape.startNewSubPath(0., midPoint);
   for (int i = gStart; i <= gEnd; ++i)
   {
    unsigned int sIndex = reverse ? iWidth - i - 1 : i;
    unsigned int fSample = static_cast<unsigned int>(static_cast<float>(sIndex)*spp);
    unsigned int lSample;
    if (spp < 1.) lSample = fSample + 1;
    else lSample = static_cast<unsigned int>(static_cast<float>(sIndex + 1)*spp);
    auto scopePoint = source->getRange(fSample, lSample);

    minimums[i] = midPoint - scale*scopePoint.min;
    float xCoord = midPoint - scale*scopePoint.max;
    const float graphThickness = fabs(xCoord - minimums[i]);
    if (graphThickness < minimumThickness)
    {
     const float girthAdded = minimumThickness - graphThickness;
     minimums[i] -= 0.5*girthAdded;
     xCoord += 0.5*girthAdded;
    }
    
    colourBuffer.setPixelAt(i, 0, scopePoint.colour);
    if (i == gStart)
    {
     waveformShape.startNewSubPath(static_cast<float>(gStart), xCoord);
    }
    else
    {
     waveformShape.lineTo(static_cast<float>(i), xCoord);
    }
        
    fSample = lSample;
   }
   
   for (int i = gEnd; i >= gStart; --i)
   {
    waveformShape.lineTo(static_cast<float>(i), minimums[i]);
   }
   
   waveformShape.closeSubPath();
  }
  
  otherBuff().clear(otherBuff().getBounds());
  auto g = juce::Graphics(otherBuff());
  
  if (source && enableScope)
  {
   juce::Graphics::ScopedSaveState saveState(g);
//   g.addTransform(juce::AffineTransform::scale(1./lastDetectedScaleFactor));
   g.setTiledImageFill(colourBuffer, 0, 0, 1.0f);
   if (fillEnable) g.fillPath(waveformShape);
   if (strokeEnable) g.strokePath(waveformShape, juce::PathStrokeType(1));
   
   if (scroll)
   {
    g.drawImageWithin(currBuff(), -updateArea + 1, 0, iWidth, height, juce::RectanglePlacement::stretchToFit);
   }
  }
  
  switchBuffers();
 }
 
 void paint (juce::Graphics& g) override
 {
  if (g.getInternalContext().getPhysicalPixelScaleFactor() != lastDetectedScaleFactor)
  {
   lastDetectedScaleFactor = g.getInternalContext().getPhysicalPixelScaleFactor();
   int w = getWidth()*lastDetectedScaleFactor;
   int h = getHeight()*lastDetectedScaleFactor;
   update();
   if (resizeBackgroundImage) resizeBackgroundImage(w, h);
  }

  if (backgroundImage.isValid())
  {
   g.drawImage(backgroundImage, getLocalBounds().toFloat(), juce::RectanglePlacement::stretchToFit);
  }
  else if (drawBackground) g.fillAll (backgroundColour);   // clear the background
  
  g.drawImage(currBuff(), getLocalBounds().toFloat(),  juce::RectanglePlacement::stretchToFit);

  if (centreEnable)
  {
   g.setColour(centreLineColour);
   float y = verticalMidPoint*getHeight();
   g.drawLine(0., y, 1.*getWidth(), y);
  }
  
  guideY1 = (verticalMidPoint - guidePosition*verticalScale)*getHeight();
  guideY2 = (verticalMidPoint + guidePosition*verticalScale)*getHeight();

  if (guideEnable)
  {
   g.setColour(guideColour);
   g.drawLine(0., guideY1, 1.*getWidth(), guideY1);
   g.drawLine(0., guideY2, 1.*getWidth(), guideY2);
  }
 }
 
 void resized() override
 {
  resizeBuffers();
  update();
 }
 
private:
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ColouredScope)
};
