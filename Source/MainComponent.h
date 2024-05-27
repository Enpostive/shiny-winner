#pragma once

#include <JuceHeader.h>
#include "DatabaseControls.h"
#include "DatabaseTableModel.h"
#include "ImportDialog.h"
#include "ColouredScope.h"
#include "SampleEnvelopeAnalyser.h"
#include "RMSAnalyser.h"

//==============================================================================
/*
 This component lives inside our window, and this is where you should put all
 your controls and content.
 */
class MainComponent  : public juce::AudioAppComponent
{
public:
//==============================================================================
 MainComponent(juce::PropertiesFile::Options &appOptions);
 ~MainComponent() override;
 
//==============================================================================
 void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
 void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
 void releaseResources() override;
 
//==============================================================================
 void paint (juce::Graphics& g) override;
 void resized() override;
 
private:
//==============================================================================
 juce::ApplicationProperties appProperties;
 juce::Component::SafePointer<juce::DialogWindow> settingsDialog;
 
 XDDSP::Parameters param;
 RMSAnalyser rmsAnalyser;
 
 juce::StretchableLayoutManager layout;
 juce::StretchableLayoutResizerBar resizerBar;
 
// juce::Label dummyLabel;
 
 SampleDatabaseConnection dbConn;
 SampleDatabaseAccessor dbAccess;
 
 DatabaseControls databaseControls;
 DatabaseTableModel tableModel;
 juce::Label analysisDisplay;
 
 std::array<std::unique_ptr<WaveformEnvelope>, 2> waveformEnvelope;
 std::array<AudioFileScopeSource, 2> audioScopeSource;
 std::array<AnalysisWaveformSource, 2> envScopeSource;
 std::array<ColouredScope, 2> audioScope;
 std::array<ColouredScope, 2> envScope;
 float measuredRMS;
 float measuredKRMS;
 std::map<int, int> histogram;
 
 int refineParameter {0};
 float clumpingFrequency {200.};
 float deleteThreshold {0.1};
 
 juce::TooltipWindow tooltipWindow;
 
 void updateAnalysisText();
 void repaintSampleList();
 
 juce::TimedCallback repaintTimer {[&](){ repaintSampleList(); }};
 
 juce::AudioFormatManager audioFormatManager;
 std::unique_ptr<juce::AudioFormatReader> audioReader;
 
 std::unique_ptr<juce::FileChooser> importFileChooser;
 ImportDialog importDialog;
 juce::DialogWindow *importDialogWindow {nullptr};
 juce::Array<juce::File> filesChosen;
 
 void updateAnalysisForChannel(WaveformEnvelopeAnalyser &analyser, int channel);
 void updateAnalysis(juce::AudioFormatReader &reader);
 
 std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
 juce::AudioTransportSource transportSource;
 
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
