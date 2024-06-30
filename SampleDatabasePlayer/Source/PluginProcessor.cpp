/*
 ==============================================================================
 
 This file contains the basic framework code for a JUCE plugin processor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout SampleDatabasePlayerAudioProcessor::createParameterLayout()
{
 juce::AudioProcessorValueTreeState::ParameterLayout layout;
 
 /*
  
   //  =================== PUSH BUTTON ======================
  
   {
    juce::ParameterID id("ID", 1);
    juce::StringArray items {"Off", "On"};
  
    juce::AudioParameterChoice *param = new juce::AudioParameterChoice(id, "LABEL", items, 0);
    layout.add(std::unique_ptr<juce::AudioParameterChoice>(param));
    
    PluginParameterListener *listener = new PluginParameterListener(param, [&](float newValue)
    {
     std::unique_lock lock(mtx);
     bool turnedOn = (newValue > 0.0001);
     // Update code
    });
    
    listeners.
    push_back(std::unique_ptr<PluginParameterListener>(listener));
    param->addListener(listener);
   
    ParameterSpec pSpec =
    {
     id,
     ParameterSpec::Type::Button
    };
  
    paramSpecs.push_back(pSpec);
   }

  
  
  
  
  
  

  
  // =============== COMBO BOX PARAM ==================
  
  {
   juce::ParameterID id("ID", 1);
   juce::StringArray items {"1x", "2x", "4x"};
   
   juce::AudioParameterChoice *param = new juce::AudioParameterChoice(id, "LABEL", items, 0);
   layout.add(std::unique_ptr<juce::AudioParameterChoice>(param));
   
   PluginParameterListener *listener = new PluginParameterListener(param, [&](float newValue)
   {
    std::unique_lock lock(mtx);
    // Update code
   });
   
   listeners.
   push_back(std::unique_ptr<PluginParameterListener>(listener));
   param->addListener(listener);
   
   ParameterSpec pSpec =
   {
    id,
    ParameterSpec::Type::Combo
   };

   paramSpecs.push_back(pSpec);
  }
 */




 /*
  // =============== SLIDER PARAM ==================
  
  {
   juce::ParameterID id("ID", 1);
   auto attributes = juce::AudioParameterFloatAttributes().withLabel("UNITS");
   juce::AudioParameterFloat *param = new juce::AudioParameterFloat(id, "LABEL", juce::NormalisableRange<float>(MIN, MAX, STEP, EXP), DEFAULT, attributes);
   layout.add(std::unique_ptr<juce::AudioParameterFloat>(param));
  
   PluginParameterListener *listener = new PluginParameterListener(param, [&](float newValue)
   {
    std::unique_lock lock(mtx);
    // Update code
   });
    
   listeners.
   push_back(std::unique_ptr<PluginParameterListener>(listener));
   param->addListener(listener);

   paramSpecs.push_back(
   {
    id,
    ParameterSpec::Type::Slider
   });
  }

  */

 // =============== SLIDER PARAM ==================
 
 {
  juce::ParameterID id("playbacklevel", 1);
  auto attributes = juce::AudioParameterFloatAttributes().withLabel("dB");
  juce::AudioParameterFloat *param = new juce::AudioParameterFloat(id, "Playback KRMS", juce::NormalisableRange<float>(-30., -0., 0.1), DefaultPlaybackKRMS, attributes);
  layout.add(std::unique_ptr<juce::AudioParameterFloat>(param));
 
  PluginParameterListener *listener = new PluginParameterListener(param, [&](float newValue)
  {
   playbackKRMS = newValue;
   triggerAsyncUpdate();
  });
   
  listeners.
  push_back(std::unique_ptr<PluginParameterListener>(listener));
  param->addListener(listener);

  paramSpecs.push_back(
  {
   id,
   ParameterSpec::Type::Slider
  });
 }





 {
  juce::ParameterID id("samplelengthmode", 1);
  juce::StringArray items {"ms", "% of 1/4 beat"};
  
  juce::AudioParameterChoice *param = new juce::AudioParameterChoice(id, "Sample length mode", items, DefaultLengthIsTempoSync ? 1 : 0);
  layout.add(std::unique_ptr<juce::AudioParameterChoice>(param));
  
  PluginParameterListener *listener = new PluginParameterListener(param, [&](float newValue)
  {
   lengthIsTempoSync = static_cast<int>(newValue) == 1;
   triggerAsyncUpdate();
  });
  
  listeners.
  push_back(std::unique_ptr<PluginParameterListener>(listener));
  param->addListener(listener);
  
  ParameterSpec pSpec =
  {
   id,
   ParameterSpec::Type::Combo
  };

  paramSpecs.push_back(pSpec);
 }




 // =============== Length MS ==================
 
 {
  juce::ParameterID id("mslength", 1);
  auto attributes = juce::AudioParameterFloatAttributes().withLabel("ms");
  juce::AudioParameterFloat *param = new juce::AudioParameterFloat(id, "Length ms", juce::NormalisableRange<float>(10., 10000., 0.01, 0.33333), DefaultLengthMs, attributes);
  layout.add(std::unique_ptr<juce::AudioParameterFloat>(param));
 
  PluginParameterListener *listener = new PluginParameterListener(param, [&](float newValue)
  {
   lengthMs = newValue;
   triggerAsyncUpdate();
  });
   
  listeners.
  push_back(std::unique_ptr<PluginParameterListener>(listener));
  param->addListener(listener);

  paramSpecs.push_back(
  {
   id,
   ParameterSpec::Type::Slider
  });
 }

 
 


 // =============== Length fraction ==================
 
 {
  juce::ParameterID id("fraclength", 1);
  auto attributes = juce::AudioParameterFloatAttributes().withLabel("%");
  juce::AudioParameterFloat *param = new juce::AudioParameterFloat(id, "Length sync", juce::NormalisableRange<float>(1., 200., 1.), DefaultLengthFrac, attributes);
  layout.add(std::unique_ptr<juce::AudioParameterFloat>(param));
 
  PluginParameterListener *listener = new PluginParameterListener(param, [&](float newValue)
  {
   lengthFrac = 0.01*newValue;
   triggerAsyncUpdate();
  });
   
  listeners.
  push_back(std::unique_ptr<PluginParameterListener>(listener));
  param->addListener(listener);

  paramSpecs.push_back(
  {
   id,
   ParameterSpec::Type::Slider
  });
 }





 // =============== Fade Out ==================
 
 {
  juce::ParameterID id("fadeout", 1);
  auto attributes = juce::AudioParameterFloatAttributes().withLabel("ms");
  juce::AudioParameterFloat *param = new juce::AudioParameterFloat(id, "Fade Out", juce::NormalisableRange<float>(5., 200., 1.), DefaultMasterFadeOut, attributes);
  layout.add(std::unique_ptr<juce::AudioParameterFloat>(param));
 
  PluginParameterListener *listener = new PluginParameterListener(param, [&](float newValue)
  {
   masterFadeOut = newValue;
   triggerAsyncUpdate();
  });
   
  listeners.
  push_back(std::unique_ptr<PluginParameterListener>(listener));
  param->addListener(listener);

  paramSpecs.push_back(
  {
   id,
   ParameterSpec::Type::Slider
  });
 }





 //  =================== Freeze Toggle ======================

 {
  juce::ParameterID id("freeze", 1);
  juce::StringArray items {"Off", "Frozen"};

  juce::AudioParameterChoice *param = new juce::AudioParameterChoice(id, "Freeze", items, 0);
  layout.add(std::unique_ptr<juce::AudioParameterChoice>(param));
  
  PluginParameterListener *listener = new PluginParameterListener(param, [&](float newValue)
  {
   bool turnedOn = (newValue > 0.0001);
   setFreeze(turnedOn);
  });
  
  listeners.
  push_back(std::unique_ptr<PluginParameterListener>(listener));
  param->addListener(listener);
 
  ParameterSpec pSpec =
  {
   id,
   ParameterSpec::Type::Button
  };

  paramSpecs.push_back(pSpec);
 }





 return layout;
}


//==============================================================================
SampleDatabasePlayerAudioProcessor::SampleDatabasePlayerAudioProcessor() :
#ifndef JucePlugin_PreferredChannelConfigurations
AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
#endif
                .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
),
#endif
dsp(dspParam),
parameters(*this, nullptr, juce::Identifier("SampleDatabasePlayerAudioProcessor"), createParameterLayout())
{
 audioFormatManager.registerBasicFormats();
 initFreezeThread();
 envelopeData.addPoint(0, 1., 0.);
 envelopeData.addListener(this);
}

SampleDatabasePlayerAudioProcessor::~SampleDatabasePlayerAudioProcessor()
{
}

//==============================================================================
const juce::String SampleDatabasePlayerAudioProcessor::getName() const
{
 return JucePlugin_Name;
}

bool SampleDatabasePlayerAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
 return true;
#else
 return false;
#endif
}

bool SampleDatabasePlayerAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
 return true;
#else
 return false;
#endif
}

bool SampleDatabasePlayerAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
 return true;
#else
 return false;
#endif
}

double SampleDatabasePlayerAudioProcessor::getTailLengthSeconds() const
{
 return 0.0;
}

int SampleDatabasePlayerAudioProcessor::getNumPrograms()
{
 return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
 // so this should be at least 1, even if you're not really implementing programs.
}

int SampleDatabasePlayerAudioProcessor::getCurrentProgram()
{
 return 0;
}

void SampleDatabasePlayerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SampleDatabasePlayerAudioProcessor::getProgramName (int index)
{
 return {};
}

void SampleDatabasePlayerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SampleDatabasePlayerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
 dspParam.setSampleRate(sampleRate);
 dspParam.setBufferSize(samplesPerBlock);

 for (std::unique_ptr<PluginListener> &p: listeners)
 {
  p->sendInternalUpdate();
 }
}

void SampleDatabasePlayerAudioProcessor::releaseResources()
{
 // When playback stops, you can use this as an opportunity to free up any
 // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SampleDatabasePlayerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
 juce::ignoreUnused (layouts);
 return true;
#else
 // This is the place where you check if the layout is supported.
 // In this template code we only support mono or stereo.
 // Some plugin hosts, such as certain GarageBand versions, will only
 // load plugins that support stereo bus layouts.
 if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
  return false;
 
 // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
 if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
  return false;
#endif
 
 return true;
#endif
}
#endif

void SampleDatabasePlayerAudioProcessor::connectDSP(int bufferLength)
{
 if (fadingBetweenBuffers)
 {
  dsp.crossfadeRamp.setRampTime(0, bufferLength);
  dsp.crossfadeRamp.startIn.setControl(playingSecondBuffer ? 0. : 1.);
  dsp.crossfadeRamp.endIn.setControl(playingSecondBuffer ? 1. : 0.);
  fadingBetweenBuffers = false;
  dsp.samplePlayHead1.connectChannel(0, leftPlayBuffer1.data());
  dsp.samplePlayHead1.connectChannel(1, rightPlayBuffer1.data());
  dsp.samplePlayHead1.setLength(leftPlayBuffer1.size());
  dsp.samplePlayHead2.connectChannel(0, leftPlayBuffer2.data());
  dsp.samplePlayHead2.connectChannel(1, rightPlayBuffer2.data());
  dsp.samplePlayHead2.setLength(leftPlayBuffer2.size());
 }
 else
 {
  dsp.samplePlayHead1.connectChannel(0, playingSecondBuffer ? nullptr : leftPlayBuffer1.data());
  dsp.samplePlayHead1.connectChannel(1, playingSecondBuffer ? nullptr : rightPlayBuffer1.data());
  dsp.samplePlayHead1.setLength(playingSecondBuffer ? 0 : leftPlayBuffer1.size());
  dsp.samplePlayHead2.connectChannel(0, !playingSecondBuffer ? nullptr : leftPlayBuffer2.data());
  dsp.samplePlayHead2.connectChannel(1, !playingSecondBuffer ? nullptr : rightPlayBuffer2.data());
  dsp.samplePlayHead2.setLength(!playingSecondBuffer ? 0 : leftPlayBuffer2.size());
 }

 int maximum = (playingSecondBuffer ?
                static_cast<int>(leftPlayBuffer2.size()) :
                static_cast<int>(leftPlayBuffer1.size()));
 dsp.playbackPosition.setMaximum(maximum);

// dsp.playbackGain.startIn.setControl(1.);
// dsp.playbackGain.endIn.setControl(1.);
}

void SampleDatabasePlayerAudioProcessor::recalculateLength()
{
 std::unique_lock lock(mtx);
 int newLength;
 if (lengthIsTempoSync)
 {
  newLength = static_cast<int>(floor(lengthFrac*dspParam.sampleRate()*60./
                                        dspParam.getTempo()));
 }
 else
 {
  newLength = static_cast<int>(floor(lengthMs*dspParam.sampleRate()*0.001));
 }
 
 if (!loadParametersFromSavedState)
 {
  for (auto &s: samples)
  {
   if (s.fadeOutEnd == userChosenLength)
   {
    s.fadeOutEnd = newLength;
   }
   s.sanitiseParameters(newLength);
  }
 }
  
 userChosenLength = newLength;
}

void SampleDatabasePlayerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
 std::unique_lock lock(mtx);
 juce::ScopedNoDenormals noDenormals;
 connectDSP(buffer.getNumSamples());
 processMIDI(midiMessages);
// dsp.signalIn.connectFloats({buffer.getWritePointer(0), buffer.getWritePointer(1)}, buffer.getNumSamples());
 dsp.process(0, buffer.getNumSamples());
 dsp.signalOut.fastTransfer<float>({buffer.getWritePointer(0), buffer.getWritePointer(1)}, buffer.getNumSamples());
}

void SampleDatabasePlayerAudioProcessor::processBlock (juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
 std::unique_lock lock(mtx);
 juce::ScopedNoDenormals noDenormals;
 connectDSP(buffer.getNumSamples());
 processMIDI(midiMessages);
// dsp.signalIn.connectDoubles({buffer.getWritePointer(0), buffer.getWritePointer(1)}, buffer.getNumSamples());
 dsp.process(0, buffer.getNumSamples());
 dsp.signalOut.fastTransfer<double>({buffer.getWritePointer(0), buffer.getWritePointer(1)}, buffer.getNumSamples());
}

void SampleDatabasePlayerAudioProcessor::processMIDI (juce::MidiBuffer &midiMessages)
{
 juce::AudioPlayHead* playhead = getPlayHead();
 if (playhead)
 {
  double tempo;
  double newTempo;
  double ppq;
  double seconds;
  /*
   juce::AudioPlayHead::CurrentPositionInfo currentPosition;
   playhead->getCurrentPosition(currentPosition);
   
   tempo = currentPosition.bpm;
   ppq = currentPosition.ppqPosition;
   seconds = currentPosition.timeInSeconds;
   //  debugFile << tempo << "BPM, " << ppq << "@" << seconds << "s\n";
   if (!currentPosition.isPlaying) dspParam.clearTransportInformation();
   //  debugFile << (currentPosition.isPlaying ? "Playing" : "Not Playing") << "\n";
   */
  auto optPositionInfo = playhead->getPosition();
  
  juce::AudioPlayHead::PositionInfo posInfo = optPositionInfo.orFallback(juce::AudioPlayHead::PositionInfo());
  
  dspParam.getTransportInformation(tempo, ppq, seconds);
  
  newTempo = posInfo.getBpm().orFallback(tempo);
  ppq = posInfo.getPpqPosition().orFallback(ppq);
  seconds = posInfo.getTimeInSeconds().orFallback(seconds);
  
  dspParam.setTransportInformation(newTempo, ppq, seconds);

  if (!juce::approximatelyEqual(newTempo, tempo))
  {
   triggerAsyncUpdate();
  }
 }
 else
 {
//  debugFile << "No tempo information\n";
  dspParam.clearTransportInformation();
 }
 
 auto it = midiMessages.begin();
 while (it != midiMessages.end())
 {
  juce::MidiMessage message = (*it).getMessage();
  if (message.isNoteOn())
  {
   dsp.noteOn(message.getTimeStamp());
  }
  it++;
 }
 midiMessages.clear();
}

//==============================================================================
bool SampleDatabasePlayerAudioProcessor::hasEditor() const
{
 return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SampleDatabasePlayerAudioProcessor::createEditor()
{
 return new SampleDatabasePlayerAudioProcessorEditor (*this, parameters);
}

//==============================================================================
const juce::Identifier SampleDatabasePlayerAudioProcessor::PresetDataIdentifier {"Samples"};
const juce::Identifier SampleDatabasePlayerAudioProcessor::SampleIdentifier1 {"SampleJSON1"};
const juce::Identifier SampleDatabasePlayerAudioProcessor::SampleIdentifier2 {"SampleJSON2"};
const juce::Identifier SampleDatabasePlayerAudioProcessor::SampleIdentifier3 {"SampleJSON3"};
const juce::Identifier SampleDatabasePlayerAudioProcessor::ClumpingFrequencyIdentifier {"ClumpingFrequency"};
const juce::Identifier SampleDatabasePlayerAudioProcessor::RemoveThresholdIdentifier {"RemoveThreshold"};
const juce::Identifier SampleDatabasePlayerAudioProcessor::AnalysisIdentifier {"AnalysisJSON"};
const juce::Identifier SampleDatabasePlayerAudioProcessor::EnvelopeIdentifier {"Envelope"};
const juce::Identifier SampleDatabasePlayerAudioProcessor::ReshapeAmountIdentifier {"ReshapeAmount"};

void SampleDatabasePlayerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
 auto p = parameters.copyState();
 juce::ValueTree strings = p.getOrCreateChildWithName(PresetDataIdentifier, nullptr);
 strings.setProperty(SampleIdentifier1, samples[0].toString(), nullptr);
 strings.setProperty(SampleIdentifier2, samples[1].toString(), nullptr);
 strings.setProperty(SampleIdentifier3, samples[2].toString(), nullptr);
 strings.setProperty(ClumpingFrequencyIdentifier, freezeAnalyseThread.clumpingFrequency, nullptr);
 strings.setProperty(RemoveThresholdIdentifier, freezeAnalyseThread.removeThreshold, nullptr);
 strings.setProperty(AnalysisIdentifier, freezeAnalysis.toString(), nullptr);
 strings.setProperty(EnvelopeIdentifier, juce::String(envelopeData.saveStateToString()), nullptr);
 strings.setProperty(ReshapeAmountIdentifier, reshapeAmount, nullptr);
// p.appendChild(strings, nullptr);
 std::unique_ptr<juce::XmlElement> xml (p.createXml());
 copyXmlToBinary (*xml, destData);
}

void SampleDatabasePlayerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
 std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
 
 if (xmlState.get() != nullptr)
 {
  if (xmlState->hasTagName (parameters.state.getType()))
  {
   initFromSavedState = true;
   reloadSamples = true;
   loadParametersFromSavedState = true;
   changeSelectionInEditor = true;
   juce::ValueTree p = juce::ValueTree::fromXml(*xmlState);
   juce::ValueTree strings = p.getChildWithName(PresetDataIdentifier);
   if (strings.isValid())
   {
    samples[0].setFromString(strings.getProperty(SampleIdentifier1).toString());
    samples[1].setFromString(strings.getProperty(SampleIdentifier2).toString());
    samples[2].setFromString(strings.getProperty(SampleIdentifier3).toString());
    freezeAnalyseThread.clumpingFrequency = strings.getProperty(ClumpingFrequencyIdentifier);
    freezeAnalyseThread.removeThreshold = strings.getProperty(RemoveThresholdIdentifier);
    freezeAnalysis.setFromString(strings.getProperty(AnalysisIdentifier));
    envelopeData.loadStateFromString(strings.getProperty(EnvelopeIdentifier).toString().toStdString());
    reshapeAmount = strings.getProperty(ReshapeAmountIdentifier);
   }
   parameters.replaceState(p);
   loadParametersFromSavedState = false;
   triggerAsyncUpdate();
  }
 }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
 return new SampleDatabasePlayerAudioProcessor();
}




std::vector<float> applyGain(const std::vector<float> &input,
                             float gaindb)
{
 std::vector<float> result;
 result.reserve(input.size());
 
 const float gain = XDDSP::dB2Linear(gaindb);
 
 for (float s: input) result.push_back(s*gain);
 
 return result;
}




inline constexpr float safeReciprocal(float x)
{ return (x == 0) ? 1 : 1./x; }




template <typename T>
inline bool validFloat(T f)
{ return isfinite(f); }






std::vector<float> resample(const std::vector<float> &input,
                            float inputSampleRate,
                            float outputSampleRate)
{
 std::vector<float> result;
 
 if (inputSampleRate == outputSampleRate)
 {
  result = input;
 }
 else
 {
  float resampleRatio = outputSampleRate/inputSampleRate;
  float recipRatio = 1./resampleRatio;
  int newLength = static_cast<int>(floor(static_cast<float>(input.size())*resampleRatio));
  result.resize(newLength);
  for (int o = 0; o < newLength; ++o)
  {
   const float fo = o;
   const float fi = fo*recipRatio;
   XDDSP::IntegerAndFraction<> iAF(fi);
   float xm1 = iAF.intRep() < 1 ? 0. : input[iAF.intRep() - 1];
   float x0 = input[iAF.intRep()];
   float x1 = iAF.intRep() < input.size() - 1 ? input[iAF.intRep() + 1] : 0.;
   float x2 = iAF.intRep() < input.size() - 2 ? input[iAF.intRep() + 2] : 0.;
   result[o] = XDDSP::hermite(iAF.fracPart(), xm1, x0, x1, x2);
   if (!validFloat(result[o]))
   {
    ;
   }
  }
 }
 
 return result;
}




void SampleDatabasePlayerAudioProcessor::doReloadSamples()
{
 sampleBuffer.clear();
 for (int i = 0; i < NumberOfSlots; ++i) if (samples[i].active)
 {
  juce::File audioFile(samples[i].filePath);
  if (audioFile.hasReadAccess())
  {
   std::unique_ptr<juce::AudioFormatReader> reader {audioFormatManager.createReaderFor(audioFile)};
   if (reader)
   {
    if (reader->numChannels == 2)
    {
     // Load stereo file
     std::vector<float> tmpL(reader->lengthInSamples);
     std::vector<float> tmpR(reader->lengthInSamples);
     float *channels[2] = {tmpL.data(), tmpR.data()};
     reader->read(channels, 2, 0, static_cast<int>(reader->lengthInSamples));
     sampleBuffer.push_back(resample(tmpL, reader->sampleRate, dspParam.sampleRate()));
     sampleBuffer.push_back(resample(tmpR, reader->sampleRate, dspParam.sampleRate()));
     samples[i].leftSlot = static_cast<int>(sampleBuffer.size() - 2);
     samples[i].rightSlot = static_cast<int>(sampleBuffer.size() - 1);
    }
    else
    {
     // Load mono file
     std::vector<float> tmp(reader->lengthInSamples);
     float *channels[1] = {tmp.data()};
     reader->read(channels, 1, 0, static_cast<int>(reader->lengthInSamples));
     sampleBuffer.push_back(resample(tmp, reader->sampleRate, dspParam.sampleRate()));
     samples[i].leftSlot = static_cast<int>(sampleBuffer.size() - 1);
     samples[i].rightSlot = samples[i].leftSlot;
    }
   }
  }
 }
}



void SampleDatabasePlayerAudioProcessor::processSample(int slot)
{
 if (samples[slot].active)
 {
  const float fInStep = safeReciprocal(samples[slot].fadeInLength);
  const float fOutStep = safeReciprocal(samples[slot].fadeOutLength);
  const int fadeInEnd = samples[slot].fadeInStart + samples[slot].fadeInLength;
  const int fadeOutStart = samples[slot].fadeOutEnd - samples[slot].fadeOutLength;
  
  const float gain = XDDSP::dB2Linear(samples[slot].getTotalGain());
  
  for (int i = samples[slot].fadeInStart;
       i < samples[slot].fadeOutEnd;
       ++i)
  {
   int b = i + samples[slot].sampleOffset;
   if (b > 0 && b < sampleBuffer[samples[slot].leftSlot].size())
   {
    float g = gain;
    if (i < fadeInEnd) g *= fInStep*static_cast<float>(i - samples[slot].fadeInStart);
    if (i > fadeOutStart) g *= 1. - fOutStep*static_cast<float>(i - fadeOutStart);
    
    float l = sampleBuffer[samples[slot].leftSlot][b];
    float r = sampleBuffer[samples[slot].rightSlot][b];
    
    leftProcessBuffer[i] += g*l;
    rightProcessBuffer[i] += g*r;
   }
  }
 }
}


void SampleDatabasePlayerAudioProcessor::mainProcess()
{
 RMSAnalyser rms(dspParam);
 std::array<float*, 2> buffers {leftProcessBuffer.data(), rightProcessBuffer.data()};
 MemoryBufferReader reader(buffers, dspParam.sampleRate(), leftProcessBuffer.size());
 float kRMS = XDDSP::linear2dB(rms.calculateKWeightedRMS(reader));
 
 float gain = XDDSP::dB2Linear(playbackKRMS - kRMS);
 if (isinf(gain) || isnan(gain)) gain = 1.;
 gain = std::min(36.f, gain);
 
 int length = static_cast<int>(leftProcessBuffer.size());
 for (int i = 0; i < length; ++i)
 {
  leftProcessBuffer[i] = gain*leftProcessBuffer[i];
  rightProcessBuffer[i] = gain*rightProcessBuffer[i];
 }
}




void SampleDatabasePlayerAudioProcessor::processFade()
{
 std::vector<float> &left = playingSecondBuffer ? leftPlayBuffer1 : leftPlayBuffer2;
 std::vector<float> &right = playingSecondBuffer ? rightPlayBuffer1 : rightPlayBuffer2;
 
 int length = static_cast<int>(leftProcessBuffer.size());
 left.resize(length, 0.);
 right.resize(length, 0.);

 int fadeOutLength = static_cast<int>(masterFadeOut*dspParam.sampleRate()*0.001);
 const float fInStep = 0.2;
 const float fOutStep = safeReciprocal(fadeOutLength);
 int fOutStart = length - fadeOutLength;
 for (int i = 0; i < length; ++i)
 {
  float g = 1.;
  if (i < 5) g *= fInStep*static_cast<float>(i);
  if (i > fOutStart) g *= 1. - fOutStep*static_cast<float>(i - fOutStart);
  g *= g;
  left[i] = g*leftProcessBuffer[i];
  right[i] = g*rightProcessBuffer[i];
 }
 left[length - 1] = 0.;
 right[length - 1] = 0.;
}





void SampleDatabasePlayerAudioProcessor::freezeProcess()
{
 jassert(leftProcessBuffer.size() == leftFreezeBuffer.size());
 
 for (int i = 0; i < leftFreezeBuffer.size(); ++i)
 {
  float sampleEnv = std::max(freezeAnalysis.envMonoLeft->amplitudeAtSample(i),
                             freezeAnalysis.envRight->amplitudeAtSample(i));
  sampleEnv = std::max(sampleEnv, float(XDDSP::dB2Linear(-60.)));
  const float userEnv = envelopeData.resolveRandomPoint(i);
  const float amp = userEnv/sampleEnv;
  const float gain = XDDSP::LERP(reshapeAmount, 1., amp);
  leftProcessBuffer[i] = gain*leftFreezeBuffer[i];
  rightProcessBuffer[i] = gain*rightFreezeBuffer[i];
 }
}




void SampleDatabasePlayerAudioProcessor::handleAsyncUpdate()
{
 recalculateLength();

 leftProcessBuffer.assign(userChosenLength, 0.);
 rightProcessBuffer.assign(userChosenLength, 0.);

 // Load samples into slots
 if (reloadSamples)
 {
  doReloadSamples();
 }
 
 // Process the resulting sample
 if (!sampleBuffer.empty() && (!frozen || reloadSamples))
 {
  processSample(0);
  processSample(1);
  processSample(2);
 }


 
 if (frozen && !initFromSavedState)
 {
  freezeProcess();
 }
 
 if (leftProcessBuffer.size() > 0) mainProcess();

 reloadSamples = false;

 if (frozen && initFromSavedState)
 {
  beginFreeze();
  initFromSavedState = false;
 }
 else
 {
  { // CRITICAL SECTION START
   // We can't touch any of the buffers if/while the audio thread is crossfading
   // between them. So if a crossfade is pending then wait!
   initFromSavedState = false;
   std::unique_lock lock(mtx);

   // If we're still crossfading the last buffer, unlock and wait then relock
   while (fadingBetweenBuffers)
   {
    mtx.unlock();
    juce::Thread::sleep(dspParam.bufferSize()/dspParam.sampleRate()*1000.0);
    mtx.lock();
   }
  } // CRITICAL SECTION END

  
  
  
  processFade();
  
  
  
  
  { // CRITICAL SECTION START
   // Lock the mutex and load the new processed sample
   std::unique_lock lock(mtx);

   playingSecondBuffer = !playingSecondBuffer;
   fadingBetweenBuffers = true;
  } // CRITICAL SECTION END
 }

 sendChangeMessage();
}




void SampleDatabasePlayerAudioProcessor::loadSample(int slot,
                                                    const juce::String &path,
                                                    float kRMSdB)
{
 jassert(slot >= 0 && slot < 3);
 samples[slot].filePath = path;
 samples[slot].kRMSdB = kRMSdB;
 samples[slot].active = true;
 reloadSamples = true;
}

void SampleDatabasePlayerAudioProcessor::unloadSample(int slot)
{
 jassert(slot >= 0 && slot < 3);
 samples[slot].active = false;
 reloadSamples = true;
}




void SampleDatabasePlayerAudioProcessor::initFreezeThread()
{
 freezeAnalyseThread.onFinish = [&]()
 {
  frozen = true;
  triggerAsyncUpdate();
 };
 
 freezeAnalyseThread.resultsHolder = &freezeAnalysis;
}

void SampleDatabasePlayerAudioProcessor::analyseFrozenSampleAgain()
{
 if (!freezeAnalyseThread.isThreadRunning()) freezeAnalyseThread.stopThread(-1);
 freezeAnalyseThread.startThread();
}

void SampleDatabasePlayerAudioProcessor::beginFreeze()
{
 if (!initFromSavedState)
 {
  leftFreezeBuffer = leftProcessBuffer;
  rightFreezeBuffer = rightProcessBuffer;

  std::array<float*, 2> buf {leftFreezeBuffer.data(), rightFreezeBuffer.data()};
  freezeAnalyseThread.reader.reset(new MemoryBufferReader(buf,
                                                          dspParam.sampleRate(),
                                                          leftFreezeBuffer.size()));
  
  analyseFrozenSampleAgain();
 }
 else
 {
  leftFreezeBuffer = leftProcessBuffer;
  rightFreezeBuffer = rightProcessBuffer;
  frozen = true;
  triggerAsyncUpdate();
 }
}

void SampleDatabasePlayerAudioProcessor::setFreeze(bool shouldFreeze)
{
 if (frozen != shouldFreeze)
 {
  if (freezeAnalyseThread.isThreadRunning()) freezeAnalyseThread.stopThread(-1);
  if (shouldFreeze)
  {
   beginFreeze();
  }
  else
  {
   frozen = false;
   changeSelectionInEditor = true;
  }
  triggerAsyncUpdate();
 }
}

void SampleDatabasePlayerAudioProcessor::piecewiseEnvelopeChanged()
{
 triggerAsyncUpdate();
}
