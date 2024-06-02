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

void SampleDatabasePlayerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
 std::unique_lock lock(mtx);
 juce::ScopedNoDenormals noDenormals;
 processMIDI(midiMessages);
 dsp.signalIn.connectFloats({buffer.getWritePointer(0), buffer.getWritePointer(1)}, buffer.getNumSamples());
 dsp.process(0, buffer.getNumSamples());
 dsp.signalOut.fastTransfer<float>({buffer.getWritePointer(0), buffer.getWritePointer(1)}, buffer.getNumSamples());
}

void SampleDatabasePlayerAudioProcessor::processBlock (juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
 std::unique_lock lock(mtx);
 juce::ScopedNoDenormals noDenormals;
 processMIDI(midiMessages);
 dsp.signalIn.connectDoubles({buffer.getWritePointer(0), buffer.getWritePointer(1)}, buffer.getNumSamples());
 dsp.process(0, buffer.getNumSamples());
 dsp.signalOut.fastTransfer<double>({buffer.getWritePointer(0), buffer.getWritePointer(1)}, buffer.getNumSamples());
}

void SampleDatabasePlayerAudioProcessor::processMIDI (juce::MidiBuffer &midiMessages)
{
 juce::AudioPlayHead* playhead = getPlayHead();
 if (playhead)
 {
  double tempo;
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
  
  tempo = posInfo.getBpm().orFallback(tempo);
  ppq = posInfo.getPpqPosition().orFallback(ppq);
  seconds = posInfo.getTimeInSeconds().orFallback(seconds);
  
  dspParam.setTransportInformation(tempo, ppq, seconds);
 }
 else
 {
//  debugFile << "No tempo information\n";
  dspParam.clearTransportInformation();
 }
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
void SampleDatabasePlayerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
 auto p = parameters.copyState();
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
   juce::ValueTree p = juce::ValueTree::fromXml(*xmlState);
   parameters.replaceState (p);
  }
 }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
 return new SampleDatabasePlayerAudioProcessor();
}
