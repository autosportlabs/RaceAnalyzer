
#ifndef LOGGEROUTPUTPANEL_H_
#define LOGGEROUTPUTPANEL_H_

#include "wx/wxprec.h"
#include "wx/treebook.h"
#include "raceAnalyzerConfigBase.h"
#include "raceCapture/raceCaptureConfig.h"
#include "configuration/channelConfigPanel.h"
#include "configuration/baseChannelConfigPanel.h"
#include "configuration/loggerOutputAdvancedOptionsDialog.h"
#include "commonEvents.h"
#include "configPanelParams.h"

class LoggerOutputConfigPanel :  public BaseConfigPanel{

	public:
		LoggerOutputConfigPanel(wxWindow *parent,
					ConfigPanelParams configParams,
					wxWindowID id = -1,
					const wxPoint &pos = wxDefaultPosition,
					const wxSize &size = wxDefaultSize,
					long style = wxTAB_TRAVERSAL,
					const wxString &name = "LoggerOutputPanel"
					);

		~LoggerOutputConfigPanel();
		void OnConfigUpdated();
		void InitComponents();
		wxStaticBoxSizer * GetP2PPanel();
		wxStaticBoxSizer * GetBluetoothPanel();
		wxStaticBoxSizer * GetCellTelemetryPanel();

	private:
		//event handlers
		void OnP2PAddressHighChanged(wxCommandEvent &event);
		void OnP2PAddressLowChanged(wxCommandEvent &event);
		void OnTelemetryModeChanged(wxCommandEvent &event);
		void OnTelemetryDeviceIdChanged(wxCommandEvent &event);
		void UpdateTelemetryModeView(telemetry_mode_t mode);
		void OnLoggingModeChanged(wxCommandEvent &event);
  		void OnAdvancedOptions(wxCommandEvent &event);


		wxComboBox * m_sdLoggingModeCombo;
		wxComboBox * m_telemetryModeCombo;
		wxTextCtrl * m_p2pAddressHighTextCtrl;
		wxTextCtrl * m_p2pAddressLowTextCtrl;
		wxTextCtrl * m_bluetoothPasswordTextCtrl;
		wxTextCtrl * m_bluetoothDeviceNameTextCtrl;
		wxTextCtrl * m_deviceIdTextCtrl;

		wxSizer * m_p2pSizer;
		wxSizer * m_cellSizer;
		wxSizer * m_btSizer;

		DECLARE_EVENT_TABLE()
};




#endif /* LOGGEROUTPUTPANEL_H_ */
