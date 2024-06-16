/*
 ==============================================================================
 
 This file contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SampleDatabasePlayerAudioProcessorEditor::SampleDatabasePlayerAudioProcessorEditor (SampleDatabasePlayerAudioProcessor& p, juce::AudioProcessorValueTreeState &vts)
: AudioProcessorEditor (&p), audioProcessor (p), valueTreeState(vts),
dbAccess(dbConn),
listModel(dbConn),
envelopeEditor(audioProcessor.envelopeData)
{
 std::unique_lock lock(audioProcessor.mtx);
 juce::Slider::RotaryParameters rotaryParam;
 rotaryParam.startAngleRadians = knobStartAngle;
 rotaryParam.endAngleRadians = knobEndAngle;
 rotaryParam.stopAtEnd = true;
 
 lookAndFeel = std::make_unique<XDLookAndFeel>();
 setLookAndFeel(lookAndFeel.get());
 
 addAndMakeVisible(controls);
 controls.setTopLeftPosition(0, 0);
 
 listModel.connectToTable(&controls.getEditor().sampleList());
 
 for (const ParameterSpec &pS : p.paramSpecs)
 {
  juce::Component *component = controls.findChildWithID(pS.paramID.getParamID());
  
  if (component)
  {
   switch (pS.type)
   {
    default:
     break;
     
    case ParameterSpec::Type::Slider:
    {
     juce::Slider *slider = dynamic_cast<juce::Slider*>(component);
     std::unique_ptr<SliderAttachment> attachment = std::make_unique<SliderAttachment>(valueTreeState, pS.paramID.getParamID(), *slider);
     
     sliderAttach.push_back(std::move(attachment));
    }
     break;
     
    case ParameterSpec::Type::Combo:
    {
     juce::ComboBox *combo = dynamic_cast<juce::ComboBox*>(component);
     std::unique_ptr<ComboBoxAttachment> attachment = std::make_unique<ComboBoxAttachment>(valueTreeState, pS.paramID.getParamID(), *combo);
     comboAttach.push_back(std::move(attachment));
    }
     break;
     
    case ParameterSpec::Type::Button:
    {
     juce::Button *button = dynamic_cast<juce::Button*>(component);
     std::unique_ptr<ButtonAttachment> attachment = std::make_unique<ButtonAttachment>(valueTreeState, pS.paramID.getParamID(), *button);
     buttonAttach.push_back(std::move(attachment));
    }
     break;
   }
  }
 }
 
 addAndMakeVisible(leftOutputBufferScope);
 addAndMakeVisible(rightOutputBufferScope);
 leftOutputBufferScope.source = &leftOutputBufferSource;
 rightOutputBufferScope.source = &rightOutputBufferSource;
 leftOutputBufferScope.drawBackground = false;
 rightOutputBufferScope.drawBackground = false;
 leftOutputBufferScope.reverse = false;
 rightOutputBufferScope.reverse = false;
 leftOutputBufferScope.centreEnable = true;
 rightOutputBufferScope.centreEnable = true;
 leftOutputBufferScope.setVerticalScale(0.4);
 rightOutputBufferScope.setVerticalScale(0.4);
 leftOutputBufferSource.defaultColour = juce::Colours::lime;
 rightOutputBufferSource.defaultColour = juce::Colours::magenta;

 leftOutputBufferScope.strokeEnable = true;
 rightOutputBufferScope.strokeEnable = true;
 /*
 leftOutputBufferScope.fillEnable = false;
 rightOutputBufferScope.fillEnable = false;
*/
 leftOutputBufferScope.setBounds(8, 420, 584, 90);
 rightOutputBufferScope.setBounds(8, 510, 584, 90);
  
 setSize (controls.getWidth(), controls.getHeight());
 
 listModel.onRowSelected = [&](int rowid)
 {
  dbAccess.selectRowId(rowid);
  Analysis analysis;
  analysis.setFromString(dbAccess.getAnalysisForID(rowid));
  audioProcessor.loadSample(selectedSlot, dbAccess.getPath(), analysis.krmsdB);
  audioProcessor.triggerAsyncUpdate();
 };
 
 controls.getEditor().onUnload = [&]()
 {
  audioProcessor.unloadSample(selectedSlot);
  audioProcessor.triggerAsyncUpdate();
 };
 
 controls.getEditor().onSlotSelected = [&](int selected)
 {
  selectedSlot = selected;
  changeListenerCallback(&audioProcessor);
 };

 controls.getEditor().onSearchStringChanged = [&](const juce::String &term)
 {
  if (term.isNotEmpty()) listModel.setSearchTerm(term);
  else listModel.resetSearch();
 };
 
 controls.getEditor().addFilterItem("No filter", -1);
 for (auto i: dbConn.categories)
 {
  controls.getEditor().addFilterItem(i.second, i.first);
 }

 controls.getEditor().onFilterChange = [&](int categoryIDSelected)
 {
  if (categoryIDSelected == -1)
  {
   listModel.clearFilterCategory();
  }
  else
  {
   listModel.setFilterCategory(categoryIDSelected);
  }
 };
 
 controls.getEditor().getAdjuster()->onUpdate = [&]()
 {
  audioProcessor.triggerAsyncUpdate();
 };
 
 controls.getFreezeEditor().resetParameters(ProcessorAnalyserThread::ClumpingFrequencyDefault,
                                            ProcessorAnalyserThread::RemoveThresholdDefault,
                                            audioProcessor.reshapeAmount);
 controls.getFreezeEditor().onClumpingFrequencyChange = [&](float clump)
 {
  audioProcessor.freezeAnalyseThread.clumpingFrequency = clump;
  audioProcessor.analyseFrozenSampleAgain();
 };
 
 controls.getFreezeEditor().onRemoveThresholdChange = [&](float remThresh)
 {
  audioProcessor.freezeAnalyseThread.removeThreshold = remThresh;
  audioProcessor.analyseFrozenSampleAgain();
 };
 
 controls.getFreezeEditor().onReshapeAmountChange = [&](float reshape)
 {
  audioProcessor.reshapeAmount = reshape;
  audioProcessor.triggerAsyncUpdate();
 };
 
 controls.getFreezeEditor().addAndMakeVisible(envelopeEditor);
 envelopeEditor.setBounds(8, 180, 584, 60);
 envelopeEditor.setGrid(0);
 
 audioProcessor.addChangeListener(this);
 audioProcessor.triggerAsyncUpdate();
 // If you have animated components, uncomment this line
 // startTimerHz(30);
}

SampleDatabasePlayerAudioProcessorEditor::~SampleDatabasePlayerAudioProcessorEditor()
{
 audioProcessor.removeChangeListener(this);
 setLookAndFeel(nullptr);
}

void SampleDatabasePlayerAudioProcessorEditor::timerCallback()
{
 {
  std::unique_lock lock(audioProcessor.mtx);
  // Do all the interfacing with the processor here
 }
 
 // Here update the plugin UI
}

void SampleDatabasePlayerAudioProcessorEditor::changeListenerCallback(juce::ChangeBroadcaster *source)
{
 if (audioProcessor.freezeParametersLoadedFromState)
 {
  audioProcessor.freezeParametersLoadedFromState = false;
  controls.getFreezeEditor().resetParameters(audioProcessor.freezeAnalyseThread.clumpingFrequency,
                                             audioProcessor.freezeAnalyseThread.removeThreshold,
                                             audioProcessor.reshapeAmount);
 }
 
 {
  int lSlot = audioProcessor.samples[selectedSlot].leftSlot;
  int rSlot = audioProcessor.samples[selectedSlot].rightSlot;
  if (audioProcessor.samples[selectedSlot].active &&
      audioProcessor.sampleBuffer.size() > 0)
  {
   controls.getEditor().connect(audioProcessor.sampleBuffer[lSlot],
                                audioProcessor.sampleBuffer[rSlot],
                                audioProcessor.samples[selectedSlot],
                                audioProcessor.lengthOfBuffer());
  }
  else
  {
   controls.getEditor().disconnect(audioProcessor.lengthOfBuffer());
  }
 }

 leftOutputBufferSource.sampleBuffer = audioProcessor.leftProcessBuffer.data();
 leftOutputBufferSource.bufLength = audioProcessor.lengthOfBuffer();
 rightOutputBufferSource.sampleBuffer = audioProcessor.rightProcessBuffer.data();
 rightOutputBufferSource.bufLength = audioProcessor.lengthOfBuffer();
 
 {
  std::unique_lock lock(audioProcessor.mtx);
  leftOutputBufferScope.update();
  rightOutputBufferScope.update();
 }
 leftOutputBufferScope.repaint();
 rightOutputBufferScope.repaint();
 
 if (audioProcessor.isFrozen)
 {
  int size = static_cast<int>(audioProcessor.leftFreezeBuffer.size());
  controls.getFreezeEditor().leftWaveformSource.sampleBuffer = audioProcessor.leftFreezeBuffer.data();
  controls.getFreezeEditor().leftWaveformSource.bufLength = size;
  controls.getFreezeEditor().rightWaveformSource.sampleBuffer = audioProcessor.rightFreezeBuffer.data();
  controls.getFreezeEditor().rightWaveformSource.bufLength = size;
  controls.getFreezeEditor().leftEnvelopeSource.env = audioProcessor.freezeAnalysis.envMonoLeft.get();
  controls.getFreezeEditor().leftEnvelopeSource.setWindowSize(size);
  controls.getFreezeEditor().rightEnvelopeSource.env = audioProcessor.freezeAnalysis.envRight.get();
  controls.getFreezeEditor().rightEnvelopeSource.setWindowSize(size);
  controls.getFreezeEditor().updateScopes();
  
  controls.getFreezeEditor().getAnalysisFeedbackLabel().setText(juce::String("Finished in ") + juce::String(audioProcessor.freezeAnalyseThread.timeLastAnalysisSeconds, 5), juce::dontSendNotification);
  
  envelopeEditor.setLength(size);
 }
}

//==============================================================================
void SampleDatabasePlayerAudioProcessorEditor::paint (juce::Graphics& g)
{
 // (Our component is opaque, so we must completely fill the background with a solid colour)
 g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void SampleDatabasePlayerAudioProcessorEditor::resized()
{
}
