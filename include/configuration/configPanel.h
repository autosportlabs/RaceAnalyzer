#ifndef CONFIGPANEL_H_
#define CONFIGPANEL_H_

#include "wx/wxprec.h"
#include "wx/treebook.h"
#include "commonEvents.h"
#include "raceAnalyzerConfigBase.h"
#include "raceCapture/raceCaptureConfig.h"
#include "configuration/analogInputPanel.h"
#include "configuration/pulseInputPanel.h"
#include "configuration/accelInputPanel.h"
#include "configuration/analogPwmOutputPanel.h"
#include "configuration/loggerOutputConfigPanel.h"
#include "configuration/gpioPanel.h"
#include "configuration/gpsConfigPanel.h"
#include "configuration/scriptPanel.h"
#include "configPanelParams.h"

class ConfigPanel : public wxPanel, public RaceAnalyzerCommCallback{

	public:
		ConfigPanel(wxWindow *parent,
					ConfigPanelParams params,
					wxWindowID id = -1,
					const wxPoint &pos = wxDefaultPosition,
					const wxSize &size = wxDefaultSize,
					long style = wxTAB_TRAVERSAL,
					const wxString &name = "panel"
					);


		~ConfigPanel();

		void OnProgress(int pct);
		void OnConfigUpdated();
		void ReadConfigComplete(bool success, wxString msg);
		void WriteConfigComplete(bool success, wxString msg);
		void FlashConfigComplete(bool success, wxString msg);


		//event handlers
	private:

		void OnConfigStale(wxCommandEvent &event);
		void OnReadConfig(wxCommandEvent &event);
		void OnWriteConfig(wxCommandEvent &event);
		void InitComponents();
		void InitOptions();
		void UpdateActivity(wxString msg);
		void UpdateStatus(wxString msg);


		wxPanel *m_configSettings;
		wxTreebook *m_configNavigation;
		wxCheckBox *m_alsoFlashConfigCheckBox;
		AnalogInputPanel *m_analogInputPanel;
		PulseInputPanel *m_timerInputPanel;
		AccelInputPanel *m_accelInputPanel;
		AnalogPulseOutputPanel *m_analogPulseOutPanel;
		GpioPanel *m_gpioPanel;
		GpsConfigPanel *m_gpsPanel;
		LoggerOutputConfigPanel *m_loggerOutputPanel;
		ScriptPanel *m_scriptPanel;

		ConfigPanelParams m_configParams;
		AsyncRaceAnalyzerComm * m_asyncComm;


	DECLARE_EVENT_TABLE()
};




#endif /* CONFIGPANEL_H_ */
