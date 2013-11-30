#ifndef RuntimeDialog_H_
#define RuntimeDialog_H_

#include "wx/wxprec.h"
#include "wx/valgen.h"
#include "raceCapture/raceCaptureConfig.h"
#include "comm.h"
#include "runtimeReader.h"
#include "wx/hashmap.h"
#include "LCDWindow.h"
#include "LED.h"
#include "commonEvents.h"
#include "controls/ledStatusBar.h"

WX_DECLARE_STRING_HASH_MAP( LCDDisplay *, RuntimeChannelValues);

  class RuntimeDialog : public wxFrame
  {
  	DECLARE_CLASS( OptionsDialog )
  	DECLARE_EVENT_TABLE();

  	public:
  		~RuntimeDialog();
  		RuntimeDialog(wxWindow* parent,
  			RaceAnalyzerComm * comm,
  			wxWindowID id = wxID_ANY,
  			const wxString& caption = wxT("Sensor Monitor"),
  			const wxPoint& pos = wxDefaultPosition,
  			const wxSize& size = wxDefaultSize,
  			long style = wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX | wxRESIZE_BORDER);

  		bool Create (wxWindow* parent,
  			RaceAnalyzerComm * comm,
  			wxWindowID id = wxID_ANY,
  			const wxString& caption = wxT("Sensor Monitor"),
  			const wxPoint& pos = wxDefaultPosition,
  			const wxSize& size = wxDefaultSize,
  			long style = wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX | wxRESIZE_BORDER);

  		void OnClose(wxCloseEvent &event);
  		void CreateControls();
  		void AddChannel(wxString &name);
  		virtual void OnRuntimeValueUpdated(wxCommandEvent &event);
  	private:
		void UpdateControlStates();
		wxGridSizer * m_mainSizer;
		RaceAnalyzerComm * m_comm;
		RuntimeReader m_runtimeReader;
		wxStaticText * m_statusMessage;
		RuntimeChannelValues m_currentValues;
		LedStatusBar *m_statusBar;
  };

#endif
