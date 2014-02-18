#ifndef __RACE_ANALYZER_H_
#define __RACE_ANALYZER_H_

#include "wx/wxprec.h"
#include <wx/thread.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/splash.h>
#include <wx/cmdline.h>
#include <wx/wizard.h>

#include "datalogStore.h"
#include "lineChartPane.h"
#include "analogGaugePane.h"
#include "digitalGaugePane.h"
#include "gpsPanel.h"
#include "datalogChannelsPanel.h"
#include "raceAnalyzerConfigBase.h"
#include "LCDWindow.h"
#include "AngularMeter.h"
#include "configuration/configPanel.h"
#include "comm.h"
#include "configData.h"
#include "appOptions.h"
#include "appPrefs.h"
#include "commonEvents.h"
#include "digitalRPMPanel.h"
#include "chartBase.h"
#include "runtimeReader.h"
#include "datalogPlayer.h"
#include "runtimeDialog.h"
#include "LogFileDropTarget.h"

#include <wx/aui/aui.h>
#include <wx/aui/framemanager.h>


#define  CONFIG_FILE_FILTER 		"RaceCapture Configuration files (*.rcap)|*.rcap"
#define  OPEN_RACE_EVENT_FILTER 	"RaceAnalyzer Event Files (*.radb)|*.radb"
#define	RACEANALYZER_WINDOW_TITLE 	"RaceAnalyzer V%s"

#define PERSPECTIVE_CONFIG	"Configuration"
#define PERSPECTIVE_ANALYZE	"Analysis"
#define MAX_PERSPECTIVES	20


class MainFrame;

class RaceAnalyzerApp : public wxApp{
	  public:
	    virtual bool OnInit();
		bool OnCmdLineParsed(wxCmdLineParser& parser);
	    void OnInitCmdLine(wxCmdLineParser& parser);
	  private:
	  	MainFrame *_mainFrame;
  };


class MainFrame : public wxFrame
  {

  private:

	void InitDragDrop();
	void InitComms();
	void InitDatalogPlayer();
	void LoadInitialConfig();
	void ReadRaceCaptureConfig();
  	void NotifyConfigChanged();
  	void UpdateConfigFileStatus();
	void InitializeComponents();
	void InitializeFrame();
	void InitializeMenus();
	void SetActivityMessage(const wxString& message);
	void SetStatusMessage(const wxString& message);
	void ClearActivityMessage();
	void ClearStatusMessage();
	void UpdateCommControls();
	void UpdateAnalyzerView();
	void ConfigModified();
	void TerminateApp();
	bool QuerySaveModifications();
	bool QueryFileOverwrite();
	void SwitchToPerspective(wxString perspectiveName);
	void SaveCurrentPerspective();
	void SyncControls();
	void LoadConfigurationFile(const wxString fileName);

	void LoadInitialPerspective();
	void CreateDefaultPerspectives();
	void CreateDefaultConfigPerspective();
	void CreateDefaultRuntimePerspective();


	void NewRaceEvent(wxString fileName);
	void CloseRaceEvent();

	wxString GetMultipleSelectionLabel(DatalogChannelSelectionSet *selectionSet);
	void AddNewLineChart(DatalogChannelSelectionSet *selectionSet);
	void AddAnalogGauges(DatalogChannelSelectionSet *selectionSet);
	void AddDigitalGauges(DatalogChannelSelectionSet *selectionSet);
	void AddGPSView(DatalogChannelSelectionSet *selectionSet);

	//Config Related
	void SaveCurrentConfig();
	void SaveAsCurrentConfig();

	//Main Panels

  	//events
	void OnHelpDownloads(wxCommandEvent &event);
	void OnHelpHardwareInstallGuide(wxCommandEvent &event);
	void OnHelpOperationGuide(wxCommandEvent &event);
  	void OnHelpAbout(wxCommandEvent &event);

  	void OnRequestDatalogData(wxCommandEvent &event);
  	void OnConfigChanged(wxCommandEvent &event);
  	void OnFileExit(wxCommandEvent &event);
  	void OnExit(wxCloseEvent& WXUNUSED(event));
  	void RaceEventUpdated();
  	void OnNewRaceEvent(wxCommandEvent &event);
  	void OnOpenRaceEvent(wxCommandEvent &event);

	void OnImportDatalog(wxCommandEvent &event);
	void OnAppOptions(wxCommandEvent &event);
	void OnViewLogviewer(wxCommandEvent &event);
    void OnConfigPerspective(wxCommandEvent &event);
    void OnAnalysisPerspective(wxCommandEvent &event);
	void OnRestoreDefaultView(wxCommandEvent &event);
	void OnRuntimePerspective(wxCommandEvent &event);

	void OnImportWizardFinished(wxWizardEvent &event);

	void ShowNoChannelSelectedError(void);

	void OnOpenConfig(wxCommandEvent &event);
	void OnNewConfig(wxCommandEvent &event);
	void OnSaveAsConfig(wxCommandEvent &event);
	void OnSaveConfig(wxCommandEvent &event);

	//chart events
	void OnAddLineChart(wxCommandEvent &event);
	void OnAddAnalogGauge(wxCommandEvent &event);
	void OnAddDigitalGauge(wxCommandEvent &event);
	void OnAddGPSView(wxCommandEvent &event);
	void OnUpdateStatus(wxCommandEvent &event);
	void OnUpdateActivity(wxCommandEvent &event);
	void OnPlayFwdDatalog(wxCommandEvent &event);
	void OnPlayRevDatalog(wxCommandEvent &event);
	void OnPauseDatalog(wxCommandEvent &event);
	void OnJumpBeginningDatalog(wxCommandEvent &event);
	void OnJumpEndDatalog(wxCommandEvent &event);
	void OnSeekFwdDatalog(wxCommandEvent &event);
	void OnSeekRevDatalog(wxCommandEvent &event);
	void OnSeekAbsDatalog(wxCommandEvent &event);
	void OnTimeOffsetChanged(wxCommandEvent &event);

	//RuntimeListener
	void OnRuntimeValueUpdated(wxString &name, float value);

 public:

	void OpenRaceEvent(wxString fileName);
    MainFrame( const wxString &title, const wxPoint &pos, const wxSize &size );
	~MainFrame();

private:
	//Data
	RaceCaptureConfig			m_currentConfig;
	wxString 					*m_currentConfigFileName;
	bool						m_configModified;
	bool						m_appTerminated;

	int							m_activeConfig;

	RaceAnalyzerComm			m_raceAnalyzerComm;
	DatalogStore				m_datalogStore;

	DatalogStoreRows			m_datalogData;

	ConfigPanel					*m_configPanel;
	DatalogChannelsPanel 		*m_channelsPanel;

	RuntimeDialog				*m_sensorMonitorWindow;

	RaceAnalyzerChannelViews	m_channelViews;

	LogFileDropTarget			m_logFileDropTarget;

	//App Options and settings
	AppOptions			m_appOptions;

	AppPrefs			_appPrefs;

	//Frame Manager
	wxAuiManager		_frameManager;

	DatalogPlayer		m_datalogPlayer;

	DECLARE_EVENT_TABLE()

  };

#endif
