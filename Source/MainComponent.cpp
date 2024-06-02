#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent(juce::PropertiesFile::Options &appOptions) :
analysisThreadPool(juce::ThreadPoolOptions()
                   .withThreadName("Background Analysis")
                   .withNumberOfThreads(4)),
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
   float peak = waveformAnalysis.peakdB;
   float gain;
   if (databaseControls.isKSelected())
   {
    gain = ref - waveformAnalysis.krmsdB;
   }
   else
   {
    gain = ref - waveformAnalysis.rmsdB;
   }
   if (gain > -peak) gain = -peak;
   transportSource.setGain(XDDSP::dB2Linear(gain));
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
   
   if (filesChosen.size() > 0)
   {
    juce::DialogWindow::LaunchOptions dialogLauncher;
    importDialog.setBounds(0, 0, 304, 152);
    dialogLauncher.content.setNonOwned(&importDialog);
    dialogLauncher.resizable = false;
    dialogLauncher.escapeKeyTriggersCloseButton = false;
    dialogLauncher.dialogTitle = "Please choose a category...";
    importDialogWindow = dialogLauncher.launchAsync();
   }
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
     int rowid = dbMod.insertRow(g.getFullPathName(), categoryid);
     startAnalysisOfRow(rowid);
    }
   }
   else
   {
    int rowid = dbMod.insertRow(f.getFullPathName(), categoryid);
    startAnalysisOfRow(rowid);
   }
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
  envScope[i].setVerticalScale(0.4);
  
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
  audioScope[i].setVerticalScale(0.4);
  audioScope[i].drawBackground = false;
  
  envScopeSource[i].comparison = &audioScopeSource[i];
 }
 
 tableModel.onRowSelected = [&](int rowid)
 {
  dbAccess.selectRowId(rowid);
  
  juce::File audioFile(dbAccess.getPath());
  if (audioFile.hasReadAccess())
  {
   transportSource.stop();
   
   if (waveAnalyserThread.isThreadRunning())
   {
    waveAnalyserThread.stopThread(-1);
    cancelPendingUpdate();
   }
   
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
    
    audioScope[0].repaint();
    audioScope[1].repaint();
    
    juce::String analString = dbAccess.getAnalysisForID(dbAccess.getRowId());
    if (analString.isNotEmpty())
    {
     waveformAnalysis.setFromString(analString);
     envScopeSource[0].setWindowSize(static_cast<int>(audioReader->lengthInSamples));
     envScopeSource[1].setWindowSize(static_cast<int>(audioReader->lengthInSamples));
     refreshEnvelopeDisplays();
    }
    else
    {
     updateAnalysis();
    }
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
 
 databaseControls.setProgressSpinnerVisible(false);
 
 startBackgroundTimer.startTimer(20);
}

MainComponent::~MainComponent()
{
 startBackgroundTimer.stopTimer();
 if (analysisThreadPool.getNumJobs() > 0)
 {
  analysisThreadPool.removeAllJobs(true, 10000);
//  juce::Thread::sleep(1000);
 }
 repaintTimer.stopTimer();
 settingsDialog.deleteAndZero();
 
 auto props = appProperties.getUserSettings();
 auto settings = deviceManager.createStateXml();
 props->setValue("audio", settings.get());
 
 // This shuts down the audio device and clears the audio source.
 shutdownAudio();
}

void MainComponent::startBackgroundAnalysis()
{
 startBackgroundTimer.stopTimer();
 std::vector<int> unanalysedRows = dbAccess.getUnanalysedRows();
 for (int i: unanalysedRows) startAnalysisOfRow(i);
}

void MainComponent::startAnalysisOfRow(int rowid)
{
 analysisThreadPool.addJob(new AnalyserJob(dbConn,
                                           audioFormatManager,
                                           rowid),
                           true);
 repaintTimer.startTimer(1000);
 const juce::MessageManagerLock mmLock;
 databaseControls.setProgressSpinnerVisible(true);
}

void MainComponent::updateAnalysisText()
{
 juce::String text;
 
 if (analysisPending)
 {
  text = "Analysing...";
  
 }
 else
 {
  if (audioReader)
  {
   float length = 1000.0*(static_cast<float>(audioReader->lengthInSamples) /
                          audioReader->sampleRate);
   text += "Sample Length: " + juce::String(length, 3) + "ms\n";
   if (audioReader->numChannels == 1) text += "Mono ";
   if (audioReader->numChannels == 2) text += "Stereo ";
   text += juce::String(static_cast<int>(audioReader->sampleRate)) + "Hz\n";
   text += "RMS: " + juce::String((waveformAnalysis.rmsdB), 3) + "dB\n";
   text += "K-RMS: " + juce::String((waveformAnalysis.krmsdB), 3) + "dB\n";
   text += "Peak: " + juce::String(waveformAnalysis.peakdB, 2) + "dB\n";

   if (waveformAnalysis.stereo &&
       waveformAnalysis.envMonoLeft &&
       waveformAnalysis.envRight)
   {
    text += "Left maxima count: " + juce::String(waveformAnalysis.envMonoLeft->maxima.size()) + "\n";
    text += "Right maxima count: " + juce::String(waveformAnalysis.envRight->maxima.size()) + "\n";
   }
   else if (waveformAnalysis.envMonoLeft)
   {
    text += "Maxima count: " + juce::String(waveformAnalysis.envMonoLeft->maxima.size()) + "\n";
   }
   //   text += waveformAnalysis.toString();
  }
 }

 analysisDisplay.setText(text, juce::dontSendNotification);
}

void MainComponent::updateAnalysis()
{
 envScopeSource[0].env = nullptr;
 envScopeSource[1].env = nullptr;
 envScope[0].update();
 envScope[1].update();
 envScope[0].repaint();
 envScope[1].repaint();

 if (audioReader)
 {
  envScopeSource[0].setWindowSize(static_cast<int>(audioReader->lengthInSamples));
  envScopeSource[1].setWindowSize(static_cast<int>(audioReader->lengthInSamples));

  if (waveAnalyserThread.isThreadRunning())
  {
   waveAnalyserThread.stopThread(-1);
   cancelPendingUpdate();
  }
  
  waveAnalyserThread.reader = audioReader.get();
  waveAnalyserThread.resultsHolder = &waveformAnalysis;
  waveAnalyserThread.onFinish = [&]()
  { triggerAsyncUpdate(); };

  analysisPending = true;
  updateAnalysisText();
  waveAnalyserThread.startThread();
 }
}

void MainComponent::repaintSampleList()
{
 databaseControls.listBox()->repaint();
 bool stillAnalysing = analysisThreadPool.getNumJobs() > 0;
 if (stillAnalysing) repaintTimer.startTimer(1000);
 else repaintTimer.stopTimer();
 databaseControls.setProgressSpinnerVisible(stillAnalysing);
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

void MainComponent::handleAsyncUpdate()
{
 if (waveformAnalysis.valid)
 {
  SampleDatabaseModifier dbMod(dbConn);
  dbMod.updateAnalysis(dbAccess.getRowId(), waveformAnalysis.toString());
  repaintTimer.startTimer(20);
  refreshEnvelopeDisplays();
 }
}

void MainComponent::refreshEnvelopeDisplays()
{
 envScopeSource[0].env = waveformAnalysis.envMonoLeft.get();
 envScopeSource[1].env = (waveformAnalysis.stereo ?
                          waveformAnalysis.envRight.get() :
                          nullptr);
 
 envScope[0].update();
 envScope[1].update();
 
 envScope[0].repaint();
 envScope[1].repaint();
 
 analysisPending = false;
 updateAnalysisText();
}
