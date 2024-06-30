#pragma once

#include <JuceHeader.h>
#include "../../Source/Analyser.h"
#include "ControlLayout.h"


class DemoAnalyserThread : public juce::Thread
{
public:
 static constexpr float ClumpingFrequencyDefault = 200.;
 static constexpr float RemoveThresholdDefault = 0.05;
 
 float clumpingFrequency {ClumpingFrequencyDefault};
 float removeThreshold {RemoveThresholdDefault};
 int refineParam {12};


 juce::AudioFormatReader *reader;
 Analysis *resultsHolder {nullptr};
 std::function<void ()> onFinish;
 
 std::mutex threadMutex;


 DemoAnalyserThread() :
 juce::Thread("Foreground analyser thread")
 {}
 
 void run()
 {
  std::unique_lock lock(threadMutex);
  JuceFileReaderCache cache(*reader);
  Analyser analyser(cache);
  
  analyser.clumpingFrequency = clumpingFrequency;
  analyser.removeThreshold = removeThreshold;
  analyser.refineParam = refineParam;

  if (resultsHolder)
  {
   analyser.doAnalysis(*resultsHolder, [&]() { return threadShouldExit(); });
  }
  if (onFinish && !threadShouldExit()) onFinish();
 }
};







//==============================================================================
/*
 This component lives inside our window, and this is where you should put all
 your controls and content.
 */
class MainComponent  : public juce::AudioAppComponent, public juce::FileDragAndDropTarget, public juce::AsyncUpdater
{
public:
 //==============================================================================
 MainComponent()
 {
  // Make sure you set the size of the component after
  // you add any child components.
  setSize (1024, 576);
  
  // Some platforms require permissions to open input channels so request that here
  if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
      && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
  {
   juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                      [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
  }
  else
  {
   // Specify the number of input and output channels that we want to open
   setAudioChannels (2, 2);
  }
  
  addAndMakeVisible(zoomer);
  addAndMakeVisible(layout);
  
  layout.resetParameters(200., 0.05, 12);
  
  layout.onClumpingFrequencyChange = [&](float newFrequency)
  {
   analyser.clumpingFrequency = newFrequency;
   if (readerCache) restartAnalysis();
  };
  
  layout.onRemoveThresholdChange = [&](float newThreshold)
  {
   analyser.removeThreshold = newThreshold;
   if (readerCache) restartAnalysis();
  };
  
  layout.onRefineParamChange = [&](int newRefine)
  {
   analyser.refineParam = newRefine;
   if (readerCache) restartAnalysis();
  };
  
  layout.onGuideLevelChange = [&](float newLevel)
  {
   zoomer.guideLevel = XDDSP::dB2Linear(newLevel);
   zoomer.repaint();
  };
  
  formatManager.registerBasicFormats();
 }
 
 ~MainComponent() override
 {
  // This shuts down the audio device and clears the audio source.
  analyser.stopThread(-1);
  shutdownAudio();
 }
 
 void restartAnalysis()
 {
  analyser.stopThread(-1);
  analyser.startThread();
 }
 
 void handleAsyncUpdate() override
 { zoomer.repaint(); }
 
 bool isInterestedInFileDrag(const juce::StringArray &files) override
 { return true; }
 
 void filesDropped(const juce::StringArray &files, int x, int y) override
 {
  zoomer.disconnect();
  analyser.stopThread(-1);

  // Interested in the first file only
  if (files.size() > 0)
  {
   std::unique_lock lock(analyser.threadMutex);
   formatReader.reset(formatManager.createReaderFor(juce::File(files[0])));
   if (formatReader)
   {
    readerCache.reset(new JuceFileReaderCache(*formatReader));

    zoomer.connectWaveform(readerCache.get());

    analyser.reader = formatReader.get();
    analyser.resultsHolder = &waveformAnalysis;
    analyser.onFinish = [&]()
    {
     zoomer.connectEnvelope(&waveformAnalysis);
     triggerAsyncUpdate();
    };
    
    analyser.startThread();
    triggerAsyncUpdate();
   }
  }
 }
 
 //==============================================================================
 void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
 {
  // This function will be called when the audio device is started, or when
  // its settings (i.e. sample rate, block size, etc) are changed.
  
  // You can use this function to initialise any resources you might need,
  // but be careful - it will be called on the audio thread, not the GUI thread.
  
  // For more details, see the help for AudioProcessor::prepareToPlay()
 }
 
 void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override
 {
  // Your audio-processing code goes here!
  
  // For more details, see the help for AudioProcessor::getNextAudioBlock()
  
  // Right now we are not producing any data, in which case we need to clear the buffer
  // (to prevent the output of random noise)
  bufferToFill.clearActiveBufferRegion();
 }
 
 void releaseResources() override
 {
  // This will be called when the audio device stops, or when it is being
  // restarted due to a setting change.
  
  // For more details, see the help for AudioProcessor::releaseResources()
 }
 
 //==============================================================================
 void paint (juce::Graphics& g) override
 {
  // (Our component is opaque, so we must completely fill the background with a solid colour)
  g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
  
  // You can add your drawing code here!
 }
 
 void resized() override
 {
  zoomer.setBounds(getLocalBounds().withTrimmedBottom(80).reduced(10));
  layout.setBounds(10, getHeight() - 80, getWidth() - 20, 200);
 }
 
 
private:
 //==============================================================================
 // Your private member variables go here...
 
 Zoomer zoomer;
 ControlLayout layout;
 
 juce::AudioFormatManager formatManager;
 std::unique_ptr<juce::AudioFormatReader> formatReader;
 std::unique_ptr<AudioReaderCache> readerCache;
 DemoAnalyserThread analyser;

 Analysis waveformAnalysis;

 
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
