#ifndef TELEMETRYADVANCEDOPTIONSDIALOG_H_
#define TELEMETRYADVANCEDOPTIONSDIALOG_H_

#include "wx/wxprec.h"
#include "wx/valgen.h"
#include "raceCapture/raceCaptureConfig.h"

  class TelemetryAdvancedOptionsDialog : public wxDialog
  {
  	DECLARE_CLASS( OptionsDialog )
  	DECLARE_EVENT_TABLE();

  	public:
  	TelemetryAdvancedOptionsDialog();

  		TelemetryAdvancedOptionsDialog(wxWindow* parent,
  			wxWindowID id = wxID_ANY,
  			const wxString& caption = wxT("Options"),
  			const wxPoint& pos = wxDefaultPosition,
  			const wxSize& size = wxDefaultSize,
  			long style = wxCAPTION | wxSYSTEM_MENU );

  		bool Create (wxWindow* parent,
  			wxWindowID id = wxID_ANY,
  			const wxString& caption = wxT("Options"),
  			const wxPoint& pos = wxDefaultPosition,
  			const wxSize& size = wxDefaultSize,
  			long style = wxCAPTION | wxSYSTEM_MENU );

  		ConnectivityConfig  GetConfig();
  		void SetConfig(ConnectivityConfig *config);

  		void CreateControls();
  		void RefreshValues();
  		void TelemetryServerChanged(wxCommandEvent &event);

  	private:
		void UpdateControlStates();

  		ConnectivityConfig m_config;
  		wxTextCtrl * m_telemetryServerTextCtrl;

  };

#endif
