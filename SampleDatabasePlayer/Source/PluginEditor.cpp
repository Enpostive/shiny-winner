/*
 ==============================================================================
 
 This file contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SampleDatabasePlayerAudioProcessorEditor::SampleDatabasePlayerAudioProcessorEditor (SampleDatabasePlayerAudioProcessor& p, juce::AudioProcessorValueTreeState &vts)
: AudioProcessorEditor (&p), audioProcessor (p), valueTreeState(vts)
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
  
 setSize (controls.getWidth(), controls.getHeight());

 // If you have animated components, uncomment this line
 // startTimerHz(30);
}

SampleDatabasePlayerAudioProcessorEditor::~SampleDatabasePlayerAudioProcessorEditor()
{
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

//==============================================================================
void SampleDatabasePlayerAudioProcessorEditor::paint (juce::Graphics& g)
{
 // (Our component is opaque, so we must completely fill the background with a solid colour)
 g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void SampleDatabasePlayerAudioProcessorEditor::resized()
{
}
