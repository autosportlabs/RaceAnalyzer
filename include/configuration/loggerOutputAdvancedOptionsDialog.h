#ifndef LOGGEROUTPUTADVANCEDOPTIONSDIALOG_H_
#define LOGGEROUTPUTADVANCEDOPTIONSDIALOG_H_

#include "wx/wxprec.h"
#include "wx/valgen.h"
#include "raceCapture/raceCaptureConfig.h"

  class LoggerOutputAdvancedOptionsDialog : public wxDialog
  {
  	DECLARE_CLASS( OptionsDialog )
  	DECLARE_EVENT_TABLE();

  	public:
  	LoggerOutputAdvancedOptionsDialog();

  		LoggerOutputAdvancedOptionsDialog(wxWindow* parent,
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

  		LoggerOutputConfig  GetConfig();
  		void SetConfig(LoggerOutputConfig *config);

  		void CreateControls();
  		void RefreshValues();
  		void TelemetryServerChanged(wxCommandEvent &event);

  	private:
		void UpdateControlStates();

  		LoggerOutputConfig m_config;
  		wxTextCtrl * m_telemetryServerTextCtrl;

  };

#endif
