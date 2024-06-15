/*
  ==============================================================================

    PiecewiseEnvelopeEditor.h
    Created: 10 Jun 2024 8:04:41pm
    Author:  Adam Jackson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../../Source/XDDSP/XDDSP.h"



template <typename PiecewiseEnvelopeData>
class PiecewiseEnvelopeEditor : public juce::Component, public PiecewiseEnvelopeData::Listener
{
 typedef XDDSP::SampleType SampleType;
 
 int activePoint {-1};
 bool isEditing {false};
 bool curveActive {false};
 bool slowEdit {false};
 SampleType initX;
 SampleType initY;
 SampleType initCurve;
 
 PiecewiseEnvelopeData &env;
 
 float length {4.};
 float loopStart {0.};
 float minimum {0.};
 float maximum {1.};
 float range {1.};
 int grid {4};
 
 float pointSize {5.};
 float loopBracketSize {7.};
 
 std::minstd_rand minstdrand;
 std::uniform_real_distribution<SampleType> sRand;
 
 float transformTimeToX(SampleType time)
 {
  return time*getWidth()/length;
 }
 
 float transformValueToY(SampleType value)
 {
  float y = value;
  y = (y - minimum)/range;
  return getHeight() * (1. - y);
  //  return getHeight() - (value - minimum)*getHeight()/range;
 }
 
 SampleType transformXToTime(float x, bool snapToGrid = false)
 {
  SampleType time = x*length/getWidth();
  if (snapToGrid && grid > 0)
  {
   SampleType gridSize = length/grid;
   time = gridSize*round(time/gridSize);
  }
  return time;
 }
 
 SampleType transformYToValue(float y)
 {
  SampleType value = 1. - (y/getHeight());
  value = value*range + minimum;
  return value;
 }
 
 float resolve(float x)
 {
  //  float y = env.resolveRandomPoint(x);
  //  y = (y - minimum)/range;
  //  return getHeight() * (1. - y);
  return transformValueToY(env.resolveRandomPoint(x));
 }
 
 void callMouseCallback(float x)
 {
  if (mouseCallback)
  {
   SampleType time = transformXToTime(x);
   SampleType value = env.resolveRandomPoint(time);
   mouseCallback(time, value);
  }
 }
 
 SampleType generateRandomValue()
 {
  return minimum + range*sRand(minstdrand);
 }
 
 std::vector<SampleType> generateRandomTimes(bool skipSome = false)
 {
  std::vector<SampleType> result;
  result.reserve(grid < 2 ? 15 : grid);
  
  if (grid < 2)
  {
   // No grid, evaluate a random number of points that are randomly spaced apart
   std::uniform_int_distribution<int> iRand (2, 15);
   std::uniform_real_distribution<SampleType> tRand(0., length);
   
   int count = iRand(minstdrand);
   for (int i = 0; i < count; ++i)
   {
    result.push_back(tRand(minstdrand));
   }
   std::sort(result.begin(), result.end());
  }
  else if (grid == 2)
  {
   // Grid of 2, simply return the begin and the end points
   result.push_back(0.);
   result.push_back(length);
  }
  else if (skipSome)
  {
   // Grid, evaluate a random number of points between 2 and the grid size
   std::uniform_int_distribution<int> iRand (2, grid);
   std::uniform_int_distribution<int> tRand(0, grid);
   std::vector<int> indexes;
   
   int count = iRand(minstdrand);
   indexes.reserve(count);
   for (int i = 0; i < count; ++i)
   {
    // Make sure that no point appears twice
    int pick;
    do
    {
     pick = tRand(minstdrand);
    } while (std::find(indexes.begin(), indexes.end(), pick) != indexes.end());
    indexes.push_back(pick);
   }
   std::sort(indexes.begin(), indexes.end());
   for (int i: indexes)
   {
    result.push_back(i*length/static_cast<SampleType>(grid));
   }
  }
  else
  {
   for (int i = 0; i <= grid; ++i)
   {
    result.push_back(i*length/static_cast<SampleType>(grid));
   }
  }
  
  return result;
 }
 
 void addRandomRamp(SampleType start, SampleType end)
 {
  if (start < 0.0001) env.addPoint(0., generateRandomValue(), 0.);
  env.addPoint(end, generateRandomValue(), 0.);
 }
 
 void addRandomBrokenRamp(SampleType start, SampleType end)
 {
  env.addPoint(start, generateRandomValue(), 0.);
  env.addPoint(end, generateRandomValue(), 0.);
 }
 
 void addRandomStep(SampleType start, SampleType end)
 {
  SampleType value = generateRandomValue();
  env.addPoint(start, value, 0.);
  env.addPoint(end, value, 0.);
 }
 
 void addRandomSpike(SampleType start, SampleType end, SampleType curve)
 {
  env.addPoint(start, 0., curve);
  env.addPoint(end, generateRandomValue(), 0.);
 }
 
 int searchPoints(juce::Point<float> position)
 {
  int searchPoint = -1;
  for (int i = 0; i < env.getPointCount(); ++i)
  {
   SampleType time, value, curve;
   env.getPoint(i, time, value, curve);
   juce::Point<float> p {transformTimeToX(time), transformValueToY(value)};
   if (position.getDistanceFrom(p) <= pointSize)
   {
    // If we find one, set it as active
    searchPoint = i;
   }
  }
  
  return searchPoint;
 }
 
public:
 juce::Colour backgroundColour {juce::Colours::blue.withBrightness(0.1)};
 juce::Colour lineColour {juce::Colours::white.withBrightness(0.9)};
 juce::Colour pointColour {juce::Colours::white.withBrightness(0.6)};
 juce::Colour activePointColour {juce::Colours::white};
 juce::Colour cursorColour {juce::Colours::white};
 juce::Colour gridColour {juce::Colours::white.withBrightness(0.4)};
 
 bool enableLoopPoints {false};
 bool enableSustainPoint {false};
 
 std::function<void (SampleType, SampleType)> mouseCallback;
 
 float cursorLocation {-1.};
 
 PiecewiseEnvelopeEditor(PiecewiseEnvelopeData &e):
 env(e),
 minstdrand(std::random_device{}()),
 sRand(0., 1.)
 {
  env.addListener(this);
 }
 
 virtual ~PiecewiseEnvelopeEditor()
 {
  env.removeListener(this);
 }
 
 virtual void piecewiseEnvelopeChanged() override
 {
  repaint();
 }
 
 void setLength(float l)
 {
  if (l > 0.) length = l;
  repaint();
 }
 
 void setLoop(float l)
 {
  if (l > 0.) loopStart = l;
  repaint();
 }
 
 void setGrid(int g)
 {
  if (g >= 0) grid = g;
  repaint();
 }
 
 void setPointSize(float ps)
 {
  if (ps > 0.) pointSize = ps;
  repaint();
 }
 
 void setLoopBracketSize(float lbs)
 {
  if (lbs > 0.) loopBracketSize = lbs;
  repaint();
 }
 
 void setMinMax(float min, float max)
 {
  if (min > max) std::swap(min, max);
  minimum = min;
  maximum = max;
  range = max - min;
  repaint();
 }
 
 void setMinimum(float min)
 { setMinMax(min, maximum); }
 
 void setMaximum(float max)
 { setMinMax(minimum, max); }
 
 void flipPoints()
 {
  env.beginEdit();
  for (int i = 0; i < env.getPointCount(); ++i)
  {
   SampleType time, value, curve;
   env.getPoint(i, time, value, curve);
   env.changePoint(i, time, maximum + minimum - value, curve);
  }
  env.endEdit();
 }
 
 void setRamp()
 {
  env.beginEdit();
  env.clearPoints();
  env.addPoint(0., minimum, 0.);
  env.addPoint(length, maximum, 0.);
  env.endEdit();
 }
 
 void setSteps()
 {
  env.beginEdit();
  env.clearPoints();
  if (grid < 3)
  {
   env.addPoint(0., minimum, 0.);
   env.addPoint(0.5*length, minimum, 0.);
   env.addPoint(0.5*length, maximum, 0.);
   env.addPoint(length, maximum, 0.);
  }
  else
  {
   SampleType gridP = 0.;
   SampleType lenP = 0.;
   for (int i = 0; i < grid; ++i)
   {
    SampleType gridN = static_cast<SampleType>(i + 1)/static_cast<SampleType>(grid);
    SampleType value = minimum + range*gridP;
    SampleType lenN = length*gridN;
    env.addPoint(lenP, value, 0.);
    env.addPoint(lenN, value, 0.);
    gridP = gridN;
    lenP = lenN;
   }
  }
  env.endEdit();
 }
 
 void setSteepCurve()
 {
  env.beginEdit();
  env.clearPoints();
  env.addPoint(0., minimum, -1.);
  env.addPoint(length, maximum, 0.);
  env.endEdit();
 }
 
 void setShallowCurve()
 {
  env.beginEdit();
  env.clearPoints();
  env.addPoint(0., minimum, 1.);
  env.addPoint(length, maximum, 0.);
  env.endEdit();
 }
 
 void generateRandomRamps(bool skipSome = false)
 {
  env.beginEdit();
  env.clearPoints();
  auto times = generateRandomTimes(skipSome);
  
  for (int i = 0; i < times.size() - 1; ++i) addRandomRamp(times[i], times[i + 1]);
  env.endEdit();
 }
 
 void generateRandomBrokenRamps(bool skipSome = false)
 {
  env.beginEdit();
  env.clearPoints();
  auto times = generateRandomTimes(skipSome);
  
  for (int i = 0; i < times.size() - 1; ++i) addRandomBrokenRamp(times[i], times[i + 1]);
  env.endEdit();
 }
 
 void generateRandomSteps(bool skipSome = false)
 {
  env.beginEdit();
  env.clearPoints();
  auto times = generateRandomTimes(skipSome);
  
  for (int i = 0; i < times.size() - 1; ++i) addRandomStep(times[i], times[i + 1]);
  env.endEdit();
 }
 
 void generateRandomSpikes(SampleType curve, bool skipSome = false)
 {
  env.beginEdit();
  env.clearPoints();
  auto times = generateRandomTimes(skipSome);
  
  if (skipSome && times[0] > 0.)
  {
   addRandomStep(0., times[0]);
  }
  
  for (int i = 0; i < times.size() - 1; ++i)
  {
   // Put the end of the spike to be just before the beginning of the next spike
   auto endTimeDelta = times[i + 1] - times[i];
   addRandomSpike(times[i], times[i] + 0.99*endTimeDelta, curve);
  }
  env.endEdit();
 }
 
 void generateRandom(bool skipSome = false)
 {
  env.beginEdit();
  env.clearPoints();
  auto times = generateRandomTimes(skipSome);
  std::uniform_int_distribution<int> pRand (0, 4);
  
  for (int i = 0; i < times.size() - 1; ++i)
  {
   int c = pRand(minstdrand);
   switch (c)
   {
    case 0:
     addRandomRamp(times[i], times[i + 1]);
     break;
     
    case 1:
     addRandomBrokenRamp(times[i], times[i + 1]);
     break;
     
    case 2:
     addRandomStep(times[i], times[i + 1]);
     break;
     
    case 3:
     addRandomSpike(times[i], times[i + 1], -1);
     break;
     
    case 4:
     addRandomSpike(times[i], times[i + 1], 1);
     break;
   }
  }
  env.endEdit();
 }
 
 void paint (juce::Graphics& g) override
 {
  g.setColour(backgroundColour);
  g.fillAll();
  
  if (grid > 1)
  {
   g.setColour(gridColour);
   float gridStep = static_cast<float>(getWidth()) / static_cast<float>(grid);
   for (int i = 1; i < grid; ++i)
   {
    float gridX = gridStep*i;
    g.drawLine(gridX, 0., gridX, getHeight());
   }
  }
  
  if (cursorLocation >= 0. && cursorLocation <= length)
  {
   float x = cursorLocation*getWidth()/length;
   g.setColour(cursorColour);
   g.drawLine(x, 0., x, getHeight());
  }
  
  int iWidth = static_cast<int>(getWidth());
  float iStep = length/iWidth;
  float py = resolve(0.);
  g.setColour(lineColour);
  for (int i = 1; i < iWidth; ++i)
  {
   float y = resolve(i*iStep);
   g.drawLine(i - 1, py, i, y);
   py = y;
  }
  
  for (int i = 0; i < env.getPointCount(); ++i)
  {
   SampleType time, value, curve;
   env.getPoint(i, time, value, curve);
   float x = transformTimeToX(time);
   float y = transformValueToY(value);
   g.setColour(i == activePoint ? activePointColour : pointColour);
   g.fillEllipse(x - 0.5*pointSize, y - 0.5*pointSize, pointSize, pointSize);
   if (i == env.getLoopStartPoint())
   {
    g.drawLine(x - 0.5*loopBracketSize,
               y - 0.5*loopBracketSize,
               x - 0.5*loopBracketSize,
               y + 0.5*loopBracketSize);
    g.drawLine(x - 0.5*loopBracketSize,
               y - 0.5*loopBracketSize,
               x - 0.2*loopBracketSize,
               y - 0.5*loopBracketSize);
    g.drawLine(x - 0.5*loopBracketSize,
               y + 0.5*loopBracketSize,
               x - 0.2*loopBracketSize,
               y + 0.5*loopBracketSize);
   }
   if (i == env.getLoopEndPoint())
   {
    g.drawLine(x + 0.5*loopBracketSize,
               y - 0.5*loopBracketSize,
               x + 0.5*loopBracketSize,
               y + 0.5*loopBracketSize);
    g.drawLine(x + 0.5*loopBracketSize,
               y - 0.5*loopBracketSize,
               x + 0.2*loopBracketSize,
               y - 0.5*loopBracketSize);
    g.drawLine(x + 0.5*loopBracketSize,
               y + 0.5*loopBracketSize,
               x + 0.2*loopBracketSize,
               y + 0.5*loopBracketSize);
   }
  }
 }
 
 void mouseDown(const juce::MouseEvent &event) override
 {
  // If there isn't an active point, try adding a new point
  if (activePoint == -1)
  {
   activePoint = env.addPoint(transformXToTime(event.position.x, true),
                              transformYToValue(event.position.y),
                              0.);
   // If adding the point was successful, active point will contain the new index
   // otherwise, it will be set to -1 by the error return code of addPoint
  }
  
  // If there is an active point, begin the edit
  if (activePoint > -1)
  {
   // Are we moving or deleting?
   if (event.mods.isRightButtonDown())
   {
    // Deleting
    env.beginEdit();
    env.removePoint(activePoint);
    env.endEdit();
    activePoint = -1;
   }
   else
   {
    // Adding/moving
    env.beginEdit();
    isEditing = true;
    initX = event.position.x;
    initY = event.position.y;
    
    SampleType time, value, curve;
    env.getPoint(activePoint, time, value, curve);
    initCurve = curve;
    
    curveActive = event.mods.isShiftDown();
    slowEdit = event.mods.isCtrlDown();
    env.setConstrainEdits(!event.mods.isAltDown());
   }
  }
 }
 
 void mouseDoubleClick(const juce::MouseEvent &event) override
 {
  if (enableLoopPoints)
  {
   int searchPoint = searchPoints(event.position);
   if (searchPoint != -1)
   {
    if (env.isLoopSustainPoint() && env.getLoopEndPoint() == searchPoint)
    {
     env.clearLoopPoints();
    }
    else if (!env.isLoopSustainPoint() && env.getLoopEndPoint() == searchPoint)
    {
     env.clearLoopPoints();
     env.setLoopPoint(searchPoint);
    }
    else env.setLoopPoint(searchPoint);
   }
  }
  else if (enableSustainPoint)
  {
   int searchPoint = searchPoints(event.position);
   if (searchPoint != -1)
   {
    if (env.isLoopSustainPoint() && env.getLoopEndPoint() == searchPoint)
    {
     env.clearLoopPoints();
    }
    else
    {
     env.clearLoopPoints();
     env.setLoopPoint(searchPoint);
    }
   }
  }
 }
 
 void mouseUp(const juce::MouseEvent &event) override
 {
  // If we are editing, end the edit
  if (isEditing)
  {
   activePoint = -1;
   env.endEdit();
   isEditing = false;
   repaint();
  }
 }
 
 void mouseMove(const juce::MouseEvent &event) override
 {
  // Look for a point that the mouse could be over
  int searchPoint = searchPoints(event.position);
  if (activePoint != searchPoint) repaint();
  activePoint = searchPoint;
  
  callMouseCallback(event.position.x);
 }
 
 void mouseDrag(const juce::MouseEvent &event) override
 {
  // Perform an actual edit
  if (isEditing && activePoint > -1)
  {
   if (slowEdit)
   {
    if (curveActive)
    {
     env.changePointCurve(activePoint, initCurve - 0.001*event.getDistanceFromDragStartY());
     callMouseCallback(event.position.x);
    }
    else
    {
     float xOffset = event.position.x - initX;
     float yOffset = event.position.y - initY;
     SampleType time = XDDSP::fastBoundary(transformXToTime(initX + xOffset*0.1),
                                            0., length);
     SampleType value = XDDSP::fastBoundary(transformYToValue(initY + yOffset*0.1),
                                             minimum, maximum);
     activePoint = env.changePoint(activePoint,
                                   time,
                                   value,
                                   initCurve);
     callMouseCallback(transformTimeToX(time));
    }
   }
   else
   {
    if (curveActive)
    {
     env.changePointCurve(activePoint, initCurve - 0.01*event.getDistanceFromDragStartY());
     callMouseCallback(event.position.x);
    }
    else
    {
     SampleType time = XDDSP::fastBoundary(transformXToTime(event.position.x, true),
                                            0., length);
     SampleType value = XDDSP::fastBoundary(transformYToValue(event.position.y),
                                             minimum, maximum);
     activePoint = env.changePoint(activePoint,
                                   time,
                                   value,
                                   initCurve);
     callMouseCallback(transformTimeToX(time));
    }
   }
  }
 }
};
