#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent(juce::PropertiesFile::Options &appOptions) :
rmsAnalyser(param),
resizerBar(&layout, 2, false),
dbAccess(dbConn),
tableModel(dbConn)
{
 appProperties.setStorageParameters(appOptions);
 auto props = appProperties.getUserSettings();
 auto audioState = props->getXmlValue("audio");
 
 tableModel.connectToTable(databaseControls.listBox());
 
 databaseControls.onPreviewClicked = [&]()
 {
  if (audioReader)
  {
   transportSource.setPosition(0.);
   float ref = databaseControls.getReferenceLevel();
   if (databaseControls.isKSelected())
   {
    transportSource.setGain(XDDSP::dB2Linear(ref - measuredKRMS));
   }
   else
   {
    transportSource.setGain(XDDSP::dB2Linear(ref - measuredRMS));
   }
   transportSource.start();
  }
 };

 databaseControls.onSearchStringChanged = [&](const juce::String &term)
 {
  if (term.isNotEmpty()) tableModel.setSearchTerm(term);
  else tableModel.resetSearch();
 };

 databaseControls.onFilterChange = [&](int categoryIDSelected)
 {
  if (categoryIDSelected == -1)
  {
   tableModel.clearFilterCategory();
  }
  else
  {
   tableModel.setFilterCategory(categoryIDSelected);
  }
 };
 
 databaseControls.addFilterItem("No filter", -1);
 importDialog.comboBox().addItem("No category", -1);
 for (auto i: dbConn.categories)
 {
  databaseControls.addFilterItem(i.second, i.first);
  importDialog.comboBox().addItem(i.second, i.first);
 }
 importDialog.comboBox().setSelectedId(-1);
 
 tableModel.onRedrawRequired = [&]()
 {
  repaintTimer.startTimer(20);
 };
 
// addAndMakeVisible(dummyLabel);
// dummyLabel.setText("Yay!", juce::dontSendNotification);
 
 addAndMakeVisible(databaseControls);
 addAndMakeVisible(resizerBar);
 
 layout.setItemLayout(0, 50., -1., -0.5);
 layout.setItemLayout(1, 50., -1., -0.5);
 layout.setItemLayout(2, 8, 8, 8);
 layout.setItemLayout(3, 280., -1., -0.5);
 

 setSize (800, 600);
 
 // Some platforms require permissions to open input channels so request that here
 if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
     && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
 {
  juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                     [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2, audioState.get()); });
 }
 else
 {
  // Specify the number of input and output channels that we want to open
  setAudioChannels (2, 2, audioState.get());
 }
 
 audioFormatManager.registerBasicFormats();
 
 importFileChooser = std::make_unique<juce::FileChooser>("Import Files",
                                                         juce::File(),
                                                         audioFormatManager.getWildcardForAllFormats());
 
 databaseControls.onSettingsClicked = [&]()
 {
  juce::DialogWindow::LaunchOptions dialogLauncher;
  dialogLauncher.content.setOwned(new juce::AudioDeviceSelectorComponent(deviceManager,
                                                                         0, 0, 2, 2, false,
                                                                         false, true, false));
  dialogLauncher.content->setBounds(0, 0, 780, 150);
  dialogLauncher.dialogBackgroundColour = findColour(juce::DocumentWindow::backgroundColourId);
  dialogLauncher.resizable = false;
  dialogLauncher.dialogTitle = "Audio Settings";
  settingsDialog = dialogLauncher.launchAsync();
 };
 
 databaseControls.onImportClicked = [&]()
 {
  auto chooserFlags = (juce::FileBrowserComponent::openMode |
                       juce::FileBrowserComponent::canSelectMultipleItems |
                       juce::FileBrowserComponent::canSelectFiles |
                       juce::FileBrowserComponent::canSelectDirectories);
  
  importFileChooser->launchAsync(chooserFlags, [&](const juce::FileChooser &chooser)
  {
   filesChosen = chooser.getResults();
   
   juce::DialogWindow::LaunchOptions dialogLauncher;
   dialogLauncher.content.setNonOwned(&importDialog);
   dialogLauncher.resizable = false;
   dialogLauncher.escapeKeyTriggersCloseButton = false;
   dialogLauncher.dialogTitle = "Please choose a category...";
   importDialogWindow = dialogLauncher.launchAsync();
  });
 };
 
 importDialog.onImportClicked = [&]()
 {
  importDialogWindow->exitModalState();
  importDialogWindow = nullptr;
  
  int categoryid = importDialog.comboBox().getSelectedId();
  if (categoryid == -1) categoryid = 0;
  
  SampleDatabaseModifier dbMod(dbConn);
  
  for (auto &f: filesChosen)
  {
   if (f.isDirectory())
   {
    auto fileTypes = juce::StringArray::fromTokens(audioFormatManager.getWildcardForAllFormats(),
                                                   ";", "");
    juce::Array<juce::File> contents;
    for (auto &type: fileTypes)
    {
     contents.addArray(f.findChildFiles(juce::File::TypesOfFileToFind::findFilesAndDirectories |
                                        juce::File::TypesOfFileToFind::ignoreHiddenFiles,
                                        true,
                                        type));
    }
    
    for (auto &g: contents)
    {
     dbMod.insertRow(g.getFullPathName(), categoryid);
    }
   }
   else dbMod.insertRow(f.getFullPathName(), categoryid);
  }
  filesChosen.clear();
  
  tableModel.refreshTable();
 };
 
 for (int i = 0; i < 2; ++i)
 {
  addAndMakeVisible(envScope[i]);
  envScope[i].source = &envScopeSource[i];
  envScope[i].reverse = false;
  envScope[i].fillEnable = false;
  envScope[i].strokeEnable = true;
  envScope[i].setVerticalScale(0.3);
  
  addAndMakeVisible(audioScope[i]);
  audioScope[i].source = &audioScopeSource[i];
  audioScopeSource[i].setOffsetAndWindowSize(0, 44100);
  audioScopeSource[i].setCrossovers(600., 4000.);
  audioScope[i].reverse = false;
  audioScope[i].centreEnable = true;
  audioScope[i].centreLineColour = juce::Colours::white.withBrightness(0.5).withAlpha(0.5f);
  audioScope[i].guideEnable = true;
  audioScope[i].minimumThickness = 0.5;
  audioScope[i].strokeEnable = true;
  audioScope[i].setVerticalScale(0.3);
  audioScope[i].drawBackground = false;
  
  envScopeSource[i].comparison = &audioScopeSource[i];
 }
 
 tableModel.onRowSelected = [&](int rowid)
 {
  dbAccess.selectRowId(rowid);
  
  juce::File audioFile(dbAccess.getPath());
  if (audioFile.hasReadAccess())
  {
   audioReader.reset(audioFormatManager.createReaderFor(audioFile));
   
   if (audioReader)
   {
    auto newSource = std::make_unique<juce::AudioFormatReaderSource>(audioReader.get(), false);
    transportSource.setSource(newSource.get(), 0, nullptr, audioReader->sampleRate);
    readerSource.reset(newSource.release());
    
    audioScopeSource[0].attachReader(audioReader.get(), 0, true);
    audioScope[0].update();
    if (audioReader->numChannels < 2)
    {
     audioScopeSource[1].attachReader(nullptr, 1, true);
    }
    else
    {
     audioScopeSource[1].attachReader(audioReader.get(), 1, true);
    }
    audioScope[1].update();

    updateAnalysis(*audioReader);
   }
  }
  else
  {
   juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                          "Oops!", "Can't load that file!");
  }
  
  addAndMakeVisible(analysisDisplay);
 };
 
/*
 databaseControls.setClumpingParameter(200.);
 databaseControls.setDeleteThresholdParamter(0.05);
 databaseControls.setRefineParameter(10);
*/

 analysisDisplay.setJustificationType(juce::Justification::topLeft);
}

MainComponent::~MainComponent()
{
 settingsDialog.deleteAndZero();
 
 auto props = appProperties.getUserSettings();
 auto settings = deviceManager.createStateXml();
 props->setValue("audio", settings.get());
 
 // This shuts down the audio device and clears the audio source.
 shutdownAudio();
}

void MainComponent::updateAnalysisText()
{
 juce::String text;
 
 if (audioReader)
 {
  float length = 1000.0*(static_cast<float>(audioReader->lengthInSamples) /
                         audioReader->sampleRate);
  text += "Sample Length: " + juce::String(length, 3) + "ms\n";
  if (audioReader->numChannels == 1) text += "Mono ";
  if (audioReader->numChannels == 2) text += "Stereo ";
  text += juce::String(static_cast<int>(audioReader->sampleRate)) + "Hz\n";
  text += "RMS: " + juce::String((measuredRMS), 3) + "dB\n";
  text += "K-RMS: " + juce::String((measuredKRMS), 3) + "dB\n";
 }
 
 if (waveformEnvelope[0])
 {
  text += "Left maxima count: " + juce::String(waveformEnvelope[0]->maxima.size()) + "\n";
 }
 if (waveformEnvelope[1])
 {
  text += "Right maxima count: " + juce::String(waveformEnvelope[1]->maxima.size()) + "\n";
 }

 analysisDisplay.setText(text, juce::dontSendNotification);
}

void MainComponent::updateAnalysisForChannel(WaveformEnvelopeAnalyser &analyser, int channel)
{
 if (audioReader)
 {
  waveformEnvelope[channel].reset(analyser.generateEnvelope(channel));
  envScopeSource[channel].env = waveformEnvelope[channel].get();
  envScopeSource[channel].setWindowSize(static_cast<int>(audioReader->lengthInSamples));
  envScope[channel].update();
  envScope[channel].repaint();
  
  audioScope[channel].repaint();
 }
}

void MainComponent::updateAnalysis(juce::AudioFormatReader &reader)
{
 param.setSampleRate(reader.sampleRate);
  
 WaveformEnvelopeAnalyser analyser(reader);
 analyser.setClumpingFrequency(clumpingFrequency);
 analyser.setRefine(refineParameter);
 analyser.setRemoveThreshold(deleteThreshold);

 updateAnalysisForChannel(analyser, 0);
  
 if (reader.numChannels > 1)
 {
  updateAnalysisForChannel(analyser, 1);
 }
 else
 {
  waveformEnvelope[1].reset();
  envScopeSource[1].env = nullptr;
  envScope[1].update();
  envScope[1].repaint();
  
  audioScope[1].repaint();
 }
  
 measuredRMS = XDDSP::linear2dB(rmsAnalyser.calculateRMS(reader));
 measuredKRMS = XDDSP::linear2dB(rmsAnalyser.calculateKWeightedRMS(reader));
  
 updateAnalysisText();
}

void MainComponent::repaintSampleList()
{
 databaseControls.listBox()->repaint();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
 transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
 if (!audioReader)
 {
  bufferToFill.clearActiveBufferRegion();
 }
 else
 {
  transportSource.getNextAudioBlock(bufferToFill);
 }
}

void MainComponent::releaseResources()
{
 transportSource.releaseResources();
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
 // (Our component is opaque, so we must completely fill the background with a solid colour)
 g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
 
 // You can add your drawing code here!
}

void MainComponent::resized()
{
 juce::Component* comps[] = {&envScope[0], &envScope[1], &resizerBar, &databaseControls};
 layout.layOutComponents(comps, 4, 0, 0, getWidth(), getHeight(), true, true);
 audioScope[0].setBounds(envScope[0].getBounds());
 audioScope[1].setBounds(envScope[1].getBounds());
 analysisDisplay.setBounds(envScope[0].getBounds().
                           withHeight(envScope[0].getHeight()).
                           withLeft(3*getWidth()/4));
}
