#include "raceAnalyzer.h"
#include "optionsDialog.h"
#include "fileIO.h"
#include "logging.h"
#include "lineChart.h"
#include "importWizardDialog.h"
#include "exceptions.h"

#include "filenew.xpm"
#include "fileopen.xpm"
#include "filesave.xpm"
#include "filesaveas.xpm"
#include "getconfig.xpm"
#include "writeconfig.xpm"
#include "commitflash.xpm"
#include "line-chart.xpm"
#include "runtime.xpm"
#include "advancetable.xpm"
#include "tuning.xpm"
#include "import.xpm"
#include "runtime_1.xpm"
#include "runtime_2.xpm"
#include "runtime_3.xpm"
#include "runtime_4.xpm"
#include "runtime_5.xpm"
#include "runtime_6.xpm"
#include "runtime_7.xpm"
#include "analysis.xpm"
#include "config.xpm"
#include "config_1.xpm"
#include "config_2.xpm"
#include "analysis_runtime2.xpm"
#include "script_edit.xpm"


#include "page_lightning.xpm"
#include "folder_database.xpm"
#include "wrench.xpm"
#include "go_bottom.xpm"

#include "race_capture_icon_tiny.xpm"
#include "race_capture_icon_small.xpm"
#include "race_capture_icon_med.xpm"
#include "race_capture_icon_large.xpm"



//wxAUI string definitions
#define PANE_CONFIGURATION 		"config"
#define PANE_ANALYSIS			"analysis"

#define CAPTION_CHANNELS 		"Channels"
#define CAPTION_CONFIG			"Configuration"
#define CAPTION_SCRIPT			"Script"

#define OPERATION_GUIDE_URL						"http://autosportlabs.net/RaceCapturePro_SoftwareOperation"
#define HARDWARE_INSTALL_GUIDE_URL 				"http://autosportlabs.net/RaceCapturePro_installation_guide"
#define SOFTWARE_FIRMWARE_DOWNLOADS 			"http://autosportlabs.net/RaceCapturePro_Downloads"

IMPLEMENT_APP(RaceAnalyzerApp);

enum{
	ID_OPTIONS = wxID_HIGHEST + 1,
	ID_GET_CONFIG,
	ID_WRITE_CONFIG,

	ID_CONFIG_MODE,
	ID_ANALYSIS_MODE,

	ID_HELP_ABOUT,
	ID_HELP_OPERATION_GUIDE,
	ID_HELP_INSTALL_GUIDE,
	ID_HELP_DOWNLOADS,
	ID_IMPORT_DATALOG,

	ID_ADD_LINE_CHART,
	ID_ADD_ANALOG_GAUGE,
	ID_ADD_DIGITAL_GAUGE,
	ID_ADD_GPS_VIEW,

	ID_NEW_CONFIG,
	ID_OPEN_CONFIG,
	ID_SAVE_CONFIG,
	ID_SAVE_CONFIG_AS,

	ID_RESTORE_DEFAULT_VIEWS

};

bool RaceAnalyzerApp::OnInit()
{
	// Create an instance of our frame, or window
	_mainFrame = new MainFrame(RACEANALYZER_WINDOW_TITLE, wxPoint(1, 1), wxSize(400,400));

	wxIconBundle iconBundle;
	iconBundle.AddIcon(wxIcon(race_capture_icon_large_xpm));
	iconBundle.AddIcon(wxIcon(race_capture_icon_med_xpm));
	iconBundle.AddIcon(wxIcon(race_capture_icon_small_xpm));
	iconBundle.AddIcon(wxIcon(race_capture_icon_tiny_xpm));
	_mainFrame->SetIcons(iconBundle);

	_mainFrame->Show(TRUE); // show the window
	SetTopWindow(_mainFrame); // and finally, set it as the main window

  return wxApp::OnInit();
}

void RaceAnalyzerApp::OnInitCmdLine(wxCmdLineParser& parser){
   parser.AddParam("",wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
   parser.Parse(false);
   if (parser.GetParamCount() > 0){
   		wxString mjljConfigFile = parser.GetParam(0);
   		_mainFrame->OpenRaceEvent(mjljConfigFile);
   }
}

bool RaceAnalyzerApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
   return true;
}

MainFrame::MainFrame(const wxString &title, const wxPoint &pos, const wxSize
  &size) : wxFrame((wxFrame*)NULL,  - 1, title, pos, size)
{
	m_appTerminated = false;
	//ShowSplashScreen();
	m_currentConfigFileName = NULL;
	m_activeConfig = -1;
	m_appOptions.LoadAppOptions();
	_appPrefs.LoadAppPrefs();

	InitializeFrame();
	SetSize(wxSize(800,700));
	Center();

	InitComms();

	InitDatalogPlayer();
	EnableVerbose(false);

	try{
	 LoadInitialConfig();
	}
	catch(...){}

}

MainFrame::~MainFrame(){
	TerminateApp();
	_frameManager.UnInit();
}


void MainFrame::InitComms(){

	try{
		m_raceAnalyzerComm.SetSerialPort(m_appOptions.GetSerialPort());
	}
	catch(CommException &e){
		SetStatusMessage(e.GetErrorMessage());
	}
}

void MainFrame::InitDatalogPlayer(){
	m_datalogPlayer.Create(&m_datalogStore, &m_channelViews);
	m_datalogPlayer.Run();
}

void MainFrame::LoadInitialConfig(){

	if (m_appOptions.GetAutoLoadConfig()){
		ReadRaceCaptureConfig();
	}
	else{
		SetStatusMessage("Default Config Loaded");
	}
	NotifyConfigChanged();
	m_configModified = false;
	UpdateCommControls();
	SyncControls();
	UpdateConfigFileStatus();
}

void MainFrame::ReadRaceCaptureConfig(){

}

void MainFrame::InitializeFrame(){

	_frameManager.SetFrame(this);

#ifdef __WXMSW_
	_frameManager.SetFlags(_frameManager.GetFlags() ^ wxAUI_MGR_TRANSPARENT_DRAG);
#else
	//workaround for bug
	//http://trac.wxwidgets.org/ticket/4841
	_frameManager.SetFlags(wxAUI_MGR_ALLOW_FLOATING |
			wxAUI_MGR_NO_VENETIAN_BLINDS_FADE |
			wxAUI_MGR_RECTANGLE_HINT
			);
#endif

	CreateStatusBar(3);

	InitializeComponents();

	InitializeMenus();
	LoadInitialPerspective();
}

void MainFrame::OnRestoreDefaultView(wxCommandEvent &event){
	CreateDefaultPerspectives();
}

void MainFrame::SaveCurrentPerspective(){
	wxString config = _frameManager.SavePerspective();
	wxString perspectiveName = _appPrefs.GetCurrentPerspectiveName();
	_appPrefs.SavePerspectiveConfig(perspectiveName, config);
}


void MainFrame::LoadInitialPerspective(){

	wxString name = _appPrefs.GetCurrentPerspectiveName();

	if (name != ""){
		SwitchToPerspective(name);
	}
	else{
		CreateDefaultPerspectives();
	}
}

void MainFrame::CreateDefaultPerspectives(){
	CreateDefaultRuntimePerspective();
	CreateDefaultConfigPerspective();
	SwitchToPerspective(PANE_CONFIGURATION);
}

void MainFrame::CreateDefaultConfigPerspective(){

	_frameManager.GetPane(wxT(PANE_ANALYSIS)).Show(false);
	_frameManager.GetPane(wxT(PANE_CONFIGURATION)).Show(true);
	_frameManager.Update();

	wxString perspectiveConfig = _frameManager.SavePerspective();
	_appPrefs.SavePerspectiveConfig(PANE_CONFIGURATION, perspectiveConfig);
}

void MainFrame::CreateDefaultRuntimePerspective(){

	_frameManager.GetPane(wxT(PANE_CONFIGURATION)).Show(false);
	_frameManager.GetPane(wxT(PANE_ANALYSIS)).Show(true);
	_frameManager.Update();

	wxString perspectiveConfig = _frameManager.SavePerspective();
	_appPrefs.SavePerspectiveConfig(PANE_ANALYSIS, perspectiveConfig);
}

void MainFrame::SwitchToPerspective(wxString perspectiveName){
	//load the new perspective
	wxString perspectiveConfig = _appPrefs.ReadPerspectiveConfig(perspectiveName);
	_frameManager.LoadPerspective(perspectiveConfig,true);
	_appPrefs.SetCurrentPerspectiveName(perspectiveName);
}

void MainFrame::OnConfigPerspective(wxCommandEvent& event){
	//SaveCurrentPerspective();
	SwitchToPerspective(PANE_CONFIGURATION);
}

void MainFrame::OnAnalysisPerspective(wxCommandEvent& event){
	//SaveCurrentPerspective();
	SwitchToPerspective(PANE_ANALYSIS);
}

void MainFrame::InitializeMenus(){

	//initialize main menu
	wxMenuBar* menuBar = new wxMenuBar();
	wxMenu* fileMenu = new wxMenu();
	fileMenu->Append(wxID_NEW, wxT("New Race Event\tCtrl+N"), wxT("Create a new Race Event"));
	fileMenu->Append(wxID_OPEN, wxT("Open Race Event\tCtrl+O"), wxT("Open an existing Race Event"));
	fileMenu->Append(ID_IMPORT_DATALOG, wxT("Import Datalog\tCtrl-I"),wxT("Import Datalog"));
	fileMenu->AppendSeparator();
	fileMenu->Append(ID_NEW_CONFIG, wxT("New Configuration\tCtrl+N"), wxT("Create a new Race Capture Configuration"));
	fileMenu->Append(ID_OPEN_CONFIG, wxT("Open Configuration\tCtrl+O"), wxT("Open a saved Race Capture Configuration"));
	fileMenu->Append(ID_SAVE_CONFIG, wxT("Save Configuration\tCtrl+S"), wxT("Save the current Race Capture Configuration"));
	fileMenu->Append(ID_SAVE_CONFIG_AS, wxT("Save Configuration As\tCtrl+Shift+S"), wxT("Save the current Race Capture Configuration under a new file"));
	fileMenu->AppendSeparator();

	fileMenu->Append(wxID_EXIT, wxT("Exit"), wxT("Exit the program"));
	menuBar->Append(fileMenu, wxT("File"));

	wxMenu* toolsMenu = new wxMenu();
	toolsMenu->Append(ID_OPTIONS, wxT("Configurator Options"));
	menuBar->Append(toolsMenu, "Tools");

	wxMenu* viewMenu = new wxMenu();
	viewMenu->Append(ID_CONFIG_MODE, "Configuration\tF2");
	viewMenu->Append(ID_ANALYSIS_MODE, "Analysis\tF3");
	viewMenu->AppendSeparator();
	viewMenu->Append(ID_RESTORE_DEFAULT_VIEWS, "Restore Default View");

	menuBar->Append(viewMenu, "View");

	wxMenu *chartsMenu = new wxMenu();
	chartsMenu->Append(ID_ADD_LINE_CHART,wxT("Line Chart"));
	chartsMenu->Append(ID_ADD_ANALOG_GAUGE, wxT("Analog Gauge"));
	chartsMenu->Append(ID_ADD_DIGITAL_GAUGE, wxT("Digital Gauge"));
	chartsMenu->Append(ID_ADD_GPS_VIEW, wxT("GPS View"));

	menuBar->Append(chartsMenu, "Charts");

	wxMenu* helpMenu = new wxMenu();
	helpMenu->Append(ID_HELP_OPERATION_GUIDE, wxT("Operation Guide"));
	helpMenu->Append(ID_HELP_INSTALL_GUIDE, wxT("Installation Guide"));
	helpMenu->Append(ID_HELP_DOWNLOADS, wxT("Software and Firmware Downloads"));
	helpMenu->AppendSeparator();
	helpMenu->Append(ID_HELP_ABOUT, wxT("About Race Analyzer"));
	menuBar->Append(helpMenu, wxT("Help"));

	SetMenuBar(menuBar);

	//initialize tool bar
	wxToolBar* toolBar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL | wxTB_FLAT | wxTB_NODIVIDER);
	wxBitmap bmpNew(page_lightning_xpm);
	wxBitmap bmpOpen(folder_database_xpm);
	wxBitmap bmpGetConfig(getconfig_xpm);
	wxBitmap bmpWriteConfig(writeconfig_xpm);
	wxBitmap bmpCommitFlash(commitflash_xpm);
	wxBitmap bmpChart(line_chart_xpm);
	wxBitmap bmpRuntime(runtime_7_xpm);
	wxBitmap bmpImport(import_xpm);

	toolBar->AddTool(wxID_NEW, "", bmpNew, "Create new Race Event");
	toolBar->AddTool(wxID_OPEN, "", bmpOpen, "Open a Race Event");
	toolBar->AddTool(ID_IMPORT_DATALOG, "", go_bottom_xpm, "ImportDatalog");

	toolBar->AddSeparator();
	toolBar->AddTool(ID_CONFIG_MODE, "", wrench_xpm, "Configuration Mode");
	toolBar->AddTool(ID_ANALYSIS_MODE, "", analysis_runtime2_xpm, "Analysis Mode");

	toolBar->AddSeparator();

	toolBar->Realize();
	SetToolBar(toolBar);
}

void MainFrame::InitializeComponents(){

	m_channelsPanel = new DatalogChannelsPanel(DatalogChannelsParams(&m_currentConfig, &_appPrefs, &m_appOptions, &m_datalogStore),	this);
	m_datalogPlayer.SetPlayerListener(m_channelsPanel);
	_frameManager.AddPane(m_channelsPanel, wxAuiPaneInfo().Name(wxT(PANE_ANALYSIS)).Caption(wxT(CAPTION_CHANNELS)).Center().Hide().CloseButton(false).Show(true));

	m_configPanel = new ConfigPanel(this, ConfigPanelParams(&m_raceAnalyzerComm, &m_currentConfig, &m_appOptions));
	_frameManager.AddPane(m_configPanel, wxAuiPaneInfo().Name(wxT(PANE_CONFIGURATION)).Caption(wxT(CAPTION_CONFIG)).Center().Hide().CloseButton(false).Show(true));
	_frameManager.Update();
}



void MainFrame::OnHelpDownloads(wxCommandEvent &event){
	wxLaunchDefaultBrowser(SOFTWARE_FIRMWARE_DOWNLOADS);
}

void MainFrame::OnHelpHardwareInstallGuide(wxCommandEvent &event){
	wxLaunchDefaultBrowser(HARDWARE_INSTALL_GUIDE_URL);
}

void MainFrame::OnHelpOperationGuide(wxCommandEvent &event){
	wxLaunchDefaultBrowser(OPERATION_GUIDE_URL);
}

void MainFrame::OnHelpAbout(wxCommandEvent &event){

	wxString rcpVersionInfo = "";
	try{
		VersionData versionData;
		m_raceAnalyzerComm.ReadVersion(versionData);
		rcpVersionInfo = wxString::Format("RaceCapture/Pro Version %s", versionData.ToString());

	}
	catch(CommException &ex){
		rcpVersionInfo = wxString::Format("Could not read RaceCapture/Pro version. Error: %s", ex.GetErrorMessage().ToAscii());
	}

	wxString msg = wxString::Format("Race Analyzer %s\n\nhttp://www.autosportlabs.net\n\nCopyright (c) 2008-2013 Autosport Labs\n\n%s",RACE_ANALYZER_VERSION, rcpVersionInfo.ToAscii());
	wxMessageDialog dlg(this,msg, "About", wxOK);
	dlg.ShowModal();
}



void MainFrame::NotifyConfigChanged(){

	wxCommandEvent event( CONFIG_STALE_EVENT, CONFIG_STALE );
	event.SetEventObject(this);
	m_configPanel->GetEventHandler()->AddPendingEvent(event);
	m_channelsPanel->GetEventHandler()->AddPendingEvent(event);
}


void MainFrame::OnRequestDatalogData(wxCommandEvent &event){
	event.StopPropagation();
	RequestDatalogRangeParams *params = (RequestDatalogRangeParams *)event.GetClientData();

	m_datalogPlayer.UpdateDataHistory(params->view, params->channels, params->fromIndex, params->toIndex);

	delete params;
}


void MainFrame::OnConfigChanged(wxCommandEvent &event){

	event.StopPropagation();
	wxObject *obj = event.GetEventObject();
	if (obj == NULL){
		wxLogMessage("null event object");
		return;
	}

	event.Skip(false);
	ConfigModified();
	NotifyConfigChanged();
}

void MainFrame::ConfigModified(){
	SyncControls();
}

void MainFrame::SyncControls(){

}

void MainFrame::SetActivityMessage(const wxString& message){
	SetStatusText(message,1);
}

void MainFrame::SetStatusMessage(const wxString& message){
	SetStatusText(message,0);
}

void MainFrame::ClearStatusMessage(){
	SetStatusMessage("");
}

void MainFrame::ClearActivityMessage(){
	SetActivityMessage("");
}



bool MainFrame::QueryFileOverwrite(){

	wxMessageDialog dlg(this, "File Exists- Overwrite?","File exists", wxYES_NO | wxNO_DEFAULT);
	return ( wxID_YES == dlg.ShowModal() );
}


void MainFrame::OnAppOptions(wxCommandEvent& event){

	OptionsDialog optionsDialog;
	optionsDialog.SetAppOptions(&m_appOptions);
	optionsDialog.Create(this);

	if (optionsDialog.ShowModal() == wxID_OK) {
		m_appOptions.SaveAppOptions();
		m_raceAnalyzerComm.SetSerialPort(m_appOptions.GetSerialPort());
	}
}


bool MainFrame::QuerySaveModifications(){
	wxMessageDialog dlg(this, "Current Configuration has not been saved. Save Changes?","Configuration Modified", wxYES_NO | wxNO_DEFAULT);
	return ( wxID_YES == dlg.ShowModal() );
}


void MainFrame::OnNewRaceEvent(wxCommandEvent &event){

	wxString defaultDir = _appPrefs.GetLastConfigFileDirectory();
	wxString defaultFile = "";
	wxFileDialog fileDialog(this, "New Race Event", defaultDir, defaultFile, OPEN_RACE_EVENT_FILTER, wxFD_SAVE);

	int result = fileDialog.ShowModal();

	if (wxID_OK == result){
		try{
			const wxString fileName = fileDialog.GetPath();
			if (wxFile::Exists(fileName)){
				wxMessageDialog dlg(this, wxString::Format("Race Event file already exists. Open this Event?"),"Open Existing Race Event", wxYES_DEFAULT | wxYES_NO);
				int result = dlg.ShowModal();
				if (result == wxID_YES){
					OpenRaceEvent(fileName);
				}
			}else{
				NewRaceEvent(fileName);
			}

			RaceEventUpdated();
		}
		catch(DatastoreException &e){
			wxMessageDialog dlg(this, wxString::Format("Failed to Create Race Event:\n\n%s", e.GetMessage().ToAscii()), "Error Creating Race Event", wxOK | wxICON_HAND);
		}
		_appPrefs.SetLastConfigFileDirectory(fileDialog.GetDirectory());
	}
	//Set default values
	NotifyConfigChanged();
}

void MainFrame::OnOpenRaceEvent(wxCommandEvent& event){


	wxString defaultDir = _appPrefs.GetLastRaceEventDirectory();
	wxString defaultFile = "";
	wxFileDialog fileDialog(this, "Open Race Event", defaultDir, defaultFile, OPEN_RACE_EVENT_FILTER, wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	int result = fileDialog.ShowModal();

	if (wxID_OK == result){
		try{
			const wxString fileName = fileDialog.GetPath();
			OpenRaceEvent(fileName);
			RaceEventUpdated();
		}
		catch(DatastoreException &e){
			wxMessageDialog dlg(this, wxString::Format("Failed to open Race Event:\n\n%s", e.GetMessage().ToAscii()), "Error Opening", wxOK | wxICON_HAND);
			dlg.ShowModal();
			return;
		}
		_appPrefs.SetLastRaceEventDirectory(fileDialog.GetDirectory());
	}
}


void MainFrame::NewRaceEvent(wxString fileName){

	if (m_datalogStore.IsOpen()) m_datalogStore.Close();
	m_datalogStore.CreateNew(fileName);
}

void MainFrame::CloseRaceEvent(){

	if (m_datalogStore.IsOpen()){
		m_datalogStore.Close();
	}
}

void MainFrame::OpenRaceEvent(wxString fileName){

	m_datalogStore.Open(fileName);
}

void MainFrame::OnImportDatalog(wxCommandEvent& event){

	if (!m_datalogStore.IsOpen()){
		wxMessageDialog dlg(this, "No Race Event Loaded. Create a new one?", "Race Event", wxYES_NO | wxNO_DEFAULT);
		if (wxID_YES == dlg.ShowModal() )	OnNewRaceEvent(event);
		}
	if (m_datalogStore.IsOpen()){
		ImportDatalogWizard *wiz = new ImportDatalogWizard(this,ImportWizardParams(&_appPrefs,&m_appOptions,&m_datalogStore));
		wiz->ShowPage(wiz->GetFirstPage());
		wiz->Show(true);
	}

}


void MainFrame::OnImportWizardFinished(wxWizardEvent &event){
	RaceEventUpdated();
}

void MainFrame::UpdateAnalyzerView(){

	INFO("Updating Analyzer view");
	m_channelsPanel->DatalogSessionsUpdated();
	INFO("Updating Analyzer view complete");
}

void MainFrame::RaceEventUpdated(){
	UpdateAnalyzerView();
	m_datalogPlayer.DatalogSessionsUpdated();
	m_datalogPlayer.RequeryAll();
	UpdateCommControls();
	NotifyConfigChanged();
	SyncControls();
	UpdateConfigFileStatus();
	SetStatusMessage("Race Event Loaded");
}


void MainFrame::UpdateCommControls(){


}

void MainFrame::UpdateConfigFileStatus(){

	wxString windowTitle = wxString::Format(RACEANALYZER_WINDOW_TITLE,RACE_ANALYZER_VERSION);

	wxString fileName = m_datalogStore.GetFileName();
	wxString title = wxString::Format("%s - Race Event %s", windowTitle.ToAscii(), fileName.ToAscii());
	SetTitle(title);
}


void MainFrame::OnFileExit(wxCommandEvent &event){
	TerminateApp();
}

void MainFrame::OnExit(wxCloseEvent& WXUNUSED(event)){
	TerminateApp();
}


void MainFrame::TerminateApp(){

	if (m_appTerminated) return;
	m_appTerminated = true;
	//Cancel exit if there are unsaved changes and the user opts to not save

	m_datalogPlayer.StopPlayback();
	CloseRaceEvent();
	SaveCurrentPerspective();
	_appPrefs.SaveAppPrefs();

	Destroy();
}

wxString MainFrame::GetMultipleSelectionLabel(DatalogChannelSelectionSet *selectionSet){

	wxString label;
	size_t selCount = selectionSet->Count();
	for (size_t selIndex = 0; selIndex < selCount; selIndex++){
		DatalogChannelSelection &sel = selectionSet->Item(selIndex);
		label.Append(wxString::Format("%s %s", (selIndex == 0  ? " " : " : "), sel.channelName.ToAscii()));
	}
	return label;
}

void MainFrame::AddNewLineChart(DatalogChannelSelectionSet *selectionSet){

	LineChartPane *logViewer = new LineChartPane(this, ChartParams(&_appPrefs,&m_appOptions));
	logViewer->ConfigureChart(selectionSet);

	m_channelViews.Add(logViewer);
	wxString name = wxString::Format("lineChart_%lu", (unsigned long)m_channelViews.Count());
	wxString caption = wxString::Format(GetMultipleSelectionLabel(selectionSet));

	_frameManager.AddPane(logViewer,
			wxAuiPaneInfo().
			BestSize(100,50).
			MinSize(100,50).
			Name(name).
			Caption(caption).
			Bottom().
			Layer(1).
			Position(2).
			Show(true));

	_frameManager.Update();
	m_datalogPlayer.AddView(logViewer);
}

void MainFrame::AddAnalogGauges(DatalogChannelSelectionSet *selectionSet){

	size_t selectionSetCount = selectionSet->Count();
	for (size_t i = 0; i < selectionSetCount; i++){
		DatalogChannelSelection &sel = (*selectionSet)[i];
		int datalogId = sel.datalogId;

		DatalogInfo datalogInfo;
		m_datalogStore.ReadDatalogInfo(datalogId, datalogInfo);

		wxString channelName = sel.channelName;

		AnalogGaugePane *gaugePane = new AnalogGaugePane(this, ChartParams(&_appPrefs,&m_appOptions));
		ViewChannel viewChannel(datalogId, channelName);

		gaugePane->CreateGauge(viewChannel);
		m_channelViews.Add(gaugePane);

		wxString name = wxString::Format("analogGauge_%lu", (unsigned long)m_channelViews.Count());
		wxString caption = wxString::Format("%s - %s", datalogInfo.name, channelName.ToAscii());

		_frameManager.AddPane(gaugePane,
				wxAuiPaneInfo().
				BestSize(150,150).
				MinSize(150,150).
				Name(name).
				Caption(caption).
				Bottom().
				Layer(1).
				Position(2).
				Show(true));

		_frameManager.Update();
	}
}

void MainFrame::AddDigitalGauges(DatalogChannelSelectionSet *selectionSet){

	size_t selectionSetCount = selectionSet->Count();
	for (size_t i = 0; i < selectionSetCount; i++){
		DatalogChannelSelection &sel = (*selectionSet)[i];
		int datalogId = sel.datalogId;

		DatalogInfo datalogInfo;
		m_datalogStore.ReadDatalogInfo(datalogId, datalogInfo);

		wxString channelName = sel.channelName;

		DigitalGaugePane *gaugePane = new DigitalGaugePane(this, ChartParams(&_appPrefs,&m_appOptions));

		ViewChannel viewChannel(datalogId, channelName);
		gaugePane->CreateGauge(viewChannel);
		m_channelViews.Add(gaugePane);
		wxString name = wxString::Format("digitalGauge_%lu", (unsigned long)m_channelViews.Count());
		wxString caption = wxString::Format("%s - %s", datalogInfo.name.ToAscii(), channelName.ToAscii());

		_frameManager.AddPane(gaugePane,
				wxAuiPaneInfo().
				BestSize(150,50).
				MinSize(150,50).
				Name(name).
				Caption(caption).
				Bottom().
				Layer(1).
				Position(2).
				Show(true));

		_frameManager.Update();
	}
}

void MainFrame::AddGPSView(DatalogChannelSelectionSet *selectionSet){
	size_t selectionSetCount = selectionSet->Count();
	wxString longitudeChannelName = "";
	wxString latitudeChannelName = "";

	int latitudeDatalogId = 0;
	int longitudeDatalogId = 0;

	for (size_t i = 0; i < selectionSetCount; i++){

		DatalogChannelSelection &sel = (*selectionSet)[i];
		int datalogId = sel.datalogId;

		DatalogChannelTypes channelTypes;
		m_datalogStore.GetChannelTypes(channelTypes);

		DatalogChannel channel;

		m_datalogStore.GetChannel(datalogId, sel.channelName, channel);

		DatalogChannelType &ct = channelTypes[channel.type];
		if (ct == DatalogChannelSystemTypes::GetLongitudeChannelType()){
			longitudeChannelName = sel.channelName;
			longitudeDatalogId = sel.datalogId;
		}
		if (ct == DatalogChannelSystemTypes::GetLatitudeChannelType()){
			latitudeChannelName = sel.channelName;
			latitudeDatalogId = sel.datalogId;
		}
	}

	if (	longitudeChannelName.Len() > 0 &&
			latitudeChannelName.Len() >= 0 &&
			longitudeDatalogId == latitudeDatalogId &&
			longitudeDatalogId > 0){

		int datalogId = longitudeDatalogId; //pick one...
		GPSPane *gpsPane = new GPSPane(this, ChartParams(&_appPrefs,&m_appOptions));

		DatalogInfo datalogInfo;
		m_datalogStore.ReadDatalogInfo(datalogId, datalogInfo);
		ViewChannel latitudeChannel(latitudeDatalogId, latitudeChannelName);
		ViewChannel longitudeChannel(longitudeDatalogId, longitudeChannelName);
		gpsPane->CreateGPSView(latitudeChannel, longitudeChannel);
		wxString name = wxString::Format("GPS_%lu", (unsigned long)m_channelViews.Count());
		wxString caption = wxString::Format("%s - GPS", datalogInfo.name.ToAscii());

		_frameManager.AddPane(gpsPane,
				wxAuiPaneInfo().
				BestSize(400,400).
				MinSize(200,200).
				Name(name).
				Caption(caption).
				Bottom().
				Layer(1).
				Position(2).
				Show(true));

		_frameManager.Update();
		m_channelViews.Add(gpsPane);
		m_datalogPlayer.AddView(gpsPane);
	}
	else{
		wxMessageDialog dlg(this,"Please Select the Channels containing the GPS Latitude and Longitude information", "Error", wxOK);
		dlg.ShowModal();
	}
}

void MainFrame::ShowNoChannelSelectedError(void){
	wxMessageDialog dlg(this, "Please select one or more channels to display", "Channels", wxOK | wxICON_HAND);
	dlg.ShowModal();
	SwitchToPerspective(PANE_ANALYSIS);
}

void MainFrame::OnAddLineChart(wxCommandEvent &event){

	DatalogChannelSelectionSet *addData = static_cast<DatalogChannelSelectionSet *>(event.GetClientData());
	if (NULL == addData) addData = m_channelsPanel->GetSelectedChannels();
	if (NULL != addData && addData->Count() > 0){
		AddNewLineChart(addData);
		delete addData;
	}
	else{
		ShowNoChannelSelectedError();
	}
}

void MainFrame::OnAddAnalogGauge(wxCommandEvent &event){
	DatalogChannelSelectionSet *addData = static_cast<DatalogChannelSelectionSet *>(event.GetClientData());
	if (NULL == addData) addData = m_channelsPanel->GetSelectedChannels();
	if (NULL != addData && addData->Count() > 0){
		AddAnalogGauges(addData);
		delete addData;
	}
	else{
		ShowNoChannelSelectedError();
	}
}

void MainFrame::OnAddDigitalGauge(wxCommandEvent &event){
	DatalogChannelSelectionSet *addData = static_cast<DatalogChannelSelectionSet *>(event.GetClientData());
	if (NULL == addData) addData = m_channelsPanel->GetSelectedChannels();
	if (NULL != addData && addData->Count() > 0){
		AddDigitalGauges(addData);
		delete addData;
	}
	else{
		ShowNoChannelSelectedError();
	}
}

void MainFrame::OnAddGPSView(wxCommandEvent &event){
	DatalogChannelSelectionSet *addData = static_cast<DatalogChannelSelectionSet *>(event.GetClientData());
	if (NULL == addData) addData = m_channelsPanel->GetSelectedChannels();
	if (NULL != addData && addData->Count() > 0){
		AddGPSView(addData);
		delete addData;
	}
	else{
		ShowNoChannelSelectedError();
	}
}

void MainFrame::OnUpdateStatus(wxCommandEvent &event){
	SetStatusMessage(event.GetString());
}

void MainFrame::OnUpdateActivity(wxCommandEvent &event){
	SetActivityMessage(event.GetString());
}

void MainFrame::OnPlayFwdDatalog(wxCommandEvent &event){
	m_datalogPlayer.PlayFwd();
}

void MainFrame::OnPlayRevDatalog(wxCommandEvent &event){
	m_datalogPlayer.PlayRev();
}

void MainFrame::OnPauseDatalog(wxCommandEvent &event){
	m_datalogPlayer.Pause();
}

void MainFrame::OnJumpBeginningDatalog(wxCommandEvent &event){
	m_datalogPlayer.SkipRev();
}

void MainFrame::OnJumpEndDatalog(wxCommandEvent &event){
	m_datalogPlayer.SkipFwd();
}

void MainFrame::OnSeekFwdDatalog(wxCommandEvent &event){
	m_datalogPlayer.SeekFwd();
}

void MainFrame::OnSeekRevDatalog(wxCommandEvent &event){
	m_datalogPlayer.SeekRev();
}

void MainFrame::OnTimeOffsetChanged(wxCommandEvent &event){
	TimeOffsetChange *offsetChange = static_cast<TimeOffsetChange *>(event.GetClientData());
	if (NULL != offsetChange){
		m_datalogPlayer.AdjustOffset(offsetChange->datalogId, offsetChange->offset);
		delete offsetChange;
	}
	else{
		ShowNoChannelSelectedError();
	}
}

void MainFrame::OnSeekAbsDatalog(wxCommandEvent &event){
	m_datalogPlayer.SeekAbsPercent(((double)event.GetInt()) / 100); //.01 % resolution conveyed via int
}

void MainFrame::OnRuntimeValueUpdated(wxString &name, float value){
//TODO re-enable me when runtime is working
/*	for (size_t i = 0; i < m_channelViews.Count(); i++){
		m_channelViews[i]->UpdateValue(name, 0, value);
	}
	*/
}

void MainFrame::OnSaveAsConfig(wxCommandEvent& event){
	SaveAsCurrentConfig();
}

void MainFrame::OnSaveConfig(wxCommandEvent& event){
	SaveCurrentConfig();
}

void MainFrame::OnNewConfig(wxCommandEvent &event){

	if (m_configModified){
		if (QuerySaveModifications()) SaveCurrentConfig();
	}
	m_currentConfig.SetDefaults();
	NotifyConfigChanged();
}

void MainFrame::OnOpenConfig(wxCommandEvent& event){

	if (m_configModified){
		if (QuerySaveModifications()) SaveCurrentConfig();
	}

	wxString defaultDir = _appPrefs.GetLastConfigFileDirectory();
	wxString defaultFile = "";
	wxFileDialog fileDialog(this, "Open Configuration", defaultDir, defaultFile, CONFIG_FILE_FILTER, wxFD_OPEN);

	int result = fileDialog.ShowModal();

	if (wxID_OK == result){
		const wxString fileName = fileDialog.GetPath();
		LoadConfigurationFile(fileName);
		_appPrefs.SetLastConfigFileDirectory(fileDialog.GetDirectory());
	}
}

void MainFrame::LoadConfigurationFile(const wxString fileName){

	RaceCaptureConfigFileReader reader;

	reader.SetFileName(fileName);

	try{
		reader.ReadConfiguration(m_currentConfig);
		if (m_currentConfigFileName) delete m_currentConfigFileName;
		m_currentConfigFileName = new wxString(fileName);
		m_configModified = false;
		UpdateCommControls();
		NotifyConfigChanged();
		SyncControls();
		UpdateConfigFileStatus();
		SetStatusMessage("RaceCapture Configuration Loaded");
	}
	catch( FileAccessException &e ){
		wxMessageDialog dlg(this, wxString::Format("Failed to load RaceCapture Configuration:\n\n%s", e.GetMessage().ToAscii()), "Error loading", wxOK | wxICON_HAND);
		dlg.ShowModal();
		return;
	}
	UpdateConfigFileStatus();
}

void MainFrame::SaveCurrentConfig(){
	if ( m_currentConfigFileName ){
		RaceCaptureConfigFileWriter writer;
		const wxString fileName(*m_currentConfigFileName);
		writer.SetFileName(fileName);

		try{
			writer.WriteConfigData(m_currentConfig);
			m_configModified = false;
			SetStatusMessage("Configuration Saved");
		}
		catch (FileAccessException &e){
			wxMessageDialog dlg(this, wxString::Format("Failed to save Configuration:\n\n%s", e.GetMessage().ToAscii()), "Error saving", wxOK | wxICON_HAND);
			dlg.ShowModal();
			return;
		}
	}
	else{
		SaveAsCurrentConfig();
	}
}

void MainFrame::SaveAsCurrentConfig(){

	wxString defaultDir = _appPrefs.GetLastConfigFileDirectory();
	wxString defaultFile = "";
	wxFileDialog fileDialog(this, "Save As Configuration", defaultDir, defaultFile, CONFIG_FILE_FILTER,wxFD_SAVE);

	int result = fileDialog.ShowModal();

	if (wxID_OK == result){
		RaceCaptureConfigFileWriter writer;

		const wxString fileName = fileDialog.GetPath();
		if (!wxFile::Exists(fileName) || (wxFile::Exists(fileName) && QueryFileOverwrite())){
			writer.SetFileName(fileName);

			try{
				writer.WriteConfigData(m_currentConfig);
				m_currentConfigFileName = new wxString(fileName);
				m_configModified = false;
				SetStatusMessage("RaceCapture Configuration Saved");
				_appPrefs.SetLastConfigFileDirectory(fileDialog.GetDirectory());
			}
			catch (FileAccessException &e){
				wxMessageDialog dlg(this, wxString::Format("Failed to save RaceCapture Configuration:\n\n%s", e.GetMessage().ToAscii()), "Error saving", wxOK | wxICON_HAND);
				dlg.ShowModal();
				return;
			}
		}
	}
	UpdateConfigFileStatus();
}


BEGIN_EVENT_TABLE ( MainFrame, wxFrame )
  	EVT_CLOSE (MainFrame::OnExit)

    EVT_MENU(wxID_EXIT, MainFrame::OnFileExit)
    EVT_MENU(ID_OPTIONS, MainFrame::OnAppOptions)

    EVT_MENU(wxID_NEW, MainFrame::OnNewRaceEvent)
    EVT_MENU(wxID_OPEN,MainFrame::OnOpenRaceEvent)
    EVT_MENU(ID_IMPORT_DATALOG,MainFrame::OnImportDatalog)

    EVT_COMMAND( CONFIG_CHANGED, CONFIG_CHANGED_EVENT, MainFrame::OnConfigChanged )
    EVT_COMMAND( ID_REQUEST_DATALOG_DATA, REQUEST_DATALOG_DATA_EVENT, MainFrame::OnRequestDatalogData )

	EVT_MENU( ID_CONFIG_MODE, MainFrame::OnConfigPerspective)
	EVT_MENU( ID_ANALYSIS_MODE, MainFrame::OnAnalysisPerspective)

	EVT_MENU( ID_HELP_ABOUT, MainFrame::OnHelpAbout)
	EVT_MENU( ID_HELP_OPERATION_GUIDE, MainFrame::OnHelpOperationGuide)
	EVT_MENU( ID_HELP_INSTALL_GUIDE, MainFrame::OnHelpHardwareInstallGuide)
	EVT_MENU( ID_HELP_DOWNLOADS, MainFrame::OnHelpDownloads)

	EVT_WIZARD_FINISHED(wxID_ANY, MainFrame::OnImportWizardFinished)

	EVT_MENU(ID_ADD_LINE_CHART, MainFrame::OnAddLineChart)
	EVT_MENU(ID_ADD_ANALOG_GAUGE, MainFrame::OnAddAnalogGauge)
	EVT_MENU(ID_ADD_DIGITAL_GAUGE, MainFrame::OnAddDigitalGauge)
	EVT_MENU(ID_ADD_GPS_VIEW, MainFrame::OnAddGPSView)

	EVT_MENU(ID_NEW_CONFIG, MainFrame::OnNewConfig)
	EVT_MENU(ID_OPEN_CONFIG, MainFrame::OnOpenConfig)
	EVT_MENU(ID_SAVE_CONFIG, MainFrame::OnSaveConfig)
	EVT_MENU(ID_SAVE_CONFIG_AS, MainFrame::OnSaveAsConfig)
	EVT_MENU(ID_RESTORE_DEFAULT_VIEWS, MainFrame::OnRestoreDefaultView)

	EVT_COMMAND(ADD_NEW_LINE_CHART, ADD_NEW_LINE_CHART_EVENT, MainFrame::OnAddLineChart)
	EVT_COMMAND(ADD_NEW_ANALOG_GAUGE, ADD_NEW_ANALOG_GAUGE_EVENT, MainFrame::OnAddAnalogGauge)
	EVT_COMMAND(ADD_NEW_DIGITAL_GAUGE, ADD_NEW_DIGITAL_GAUGE_EVENT, MainFrame::OnAddDigitalGauge)
	EVT_COMMAND(ADD_NEW_GPS_VIEW, ADD_NEW_GPS_VIEW_EVENT, MainFrame::OnAddGPSView)

	EVT_COMMAND(UPDATE_STATUS, UPDATE_STATUS_EVENT, MainFrame::OnUpdateStatus)
	EVT_COMMAND(UPDATE_ACTIVITY, UPDATE_ACTIVITY_EVENT, MainFrame::OnUpdateActivity)

	EVT_COMMAND(PLAY_FWD_DATALOG, PLAY_FWD_DATALOG_EVENT, MainFrame::OnPlayFwdDatalog)
	EVT_COMMAND(PLAY_REV_DATALOG, PLAY_REV_DATALOG_EVENT, MainFrame::OnPlayRevDatalog)
	EVT_COMMAND(PAUSE_DATALOG, PAUSE_DATALOG_EVENT, MainFrame::OnPauseDatalog)
	EVT_COMMAND(SEEK_ABS_DATALOG, SEEK_ABS_DATALOG_EVENT, MainFrame::OnSeekAbsDatalog)
	EVT_COMMAND(JUMP_BEGINNING_DATALOG, JUMP_BEGINNING_DATALOG_EVENT, MainFrame::OnJumpBeginningDatalog)
	EVT_COMMAND(JUMP_END_DATALOG, JUMP_END_DATALOG_EVENT, MainFrame::OnJumpEndDatalog)
	EVT_COMMAND(SEEK_FWD_DATALOG, SEEK_FWD_DATALOG_EVENT, MainFrame::OnSeekFwdDatalog)
	EVT_COMMAND(SEEK_REV_DATALOG, SEEK_REV_DATALOG_EVENT, MainFrame::OnSeekRevDatalog)
	EVT_COMMAND(TIME_OFFSET_CHANGED, TIME_OFFSET_CHANGED_EVENT, MainFrame::OnTimeOffsetChanged)

END_EVENT_TABLE()



