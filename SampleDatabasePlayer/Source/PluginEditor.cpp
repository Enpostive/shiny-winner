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
listModel(dbConn)
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
 
 listModel.connectToTable(&controls.sampleList());
 
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
 leftOutputBufferScope.setBounds(0, 420, 600, 90);
 rightOutputBufferScope.setBounds(0, 510, 600, 90);
  
 setSize (controls.getWidth(), controls.getHeight());
 
 listModel.onRowSelected = [&](int rowid)
 {
  dbAccess.selectRowId(rowid);
  audioProcessor.loadSample(selectedSlot, dbAccess.getPath());
  audioProcessor.setAnalysis(selectedSlot, dbAccess.getAnalysisForID(rowid));
  audioProcessor.triggerAsyncUpdate();
 };
 
 controls.onUnload = [&]()
 {
  audioProcessor.unloadSample(selectedSlot);
  audioProcessor.triggerAsyncUpdate();
 };
 
 controls.onSlotSelected = [&](int selected)
 {
  selectedSlot = selected;
  changeListenerCallback(&audioProcessor);
 };

 controls.onSearchStringChanged = [&](const juce::String &term)
 {
  if (term.isNotEmpty()) listModel.setSearchTerm(term);
  else listModel.resetSearch();
 };
 
 controls.addFilterItem("No filter", -1);
 for (auto i: dbConn.categories)
 {
  controls.addFilterItem(i.second, i.first);
 }

 controls.onFilterChange = [&](int categoryIDSelected)
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
 
 controls.getAdjuster()->onUpdate = [&]()
 {
  audioProcessor.triggerAsyncUpdate();
 };
 
 audioProcessor.addChangeListener(this);
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
 int lSlot = audioProcessor.samples[selectedSlot].leftSlot;
 int rSlot = audioProcessor.samples[selectedSlot].rightSlot;
 if (audioProcessor.samples[selectedSlot].active &&
     audioProcessor.sampleBuffer.size() > 0)
 {
  controls.connect(audioProcessor.sampleBuffer[lSlot],
                   audioProcessor.sampleBuffer[rSlot],
                   audioProcessor.samples[selectedSlot],
                   audioProcessor.lengthOfBuffer());
 }
 else
 {
  controls.disconnect(audioProcessor.lengthOfBuffer());
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
