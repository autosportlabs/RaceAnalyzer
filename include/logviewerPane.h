/*
 * LogviewerPane.h
 *
 *  Created on: 2009-07-20
 *      Author: brent
 */

#ifndef LOGVIEWERPANE_H_
#define LOGVIEWERPANE_H_

#include "wx/wxprec.h"
#include "RCPLogReader.h"
#include "comm.h"

class LogviewerPane : public wxPanel {

public:
	LogviewerPane(wxWindow *parent,
				RaceAnalyzerComm * comm,
				wxWindowID id = -1,
				const wxPoint &pos = wxDefaultPosition,
				const wxSize &size = wxDefaultSize,
				long style = wxTAB_TRAVERSAL,
				const wxString &name = "panel"
				);
	~LogviewerPane();
	void InitComponents();

private:

	virtual void OnLogfileUpdated(wxCommandEvent &event);

	void OnShow(wxShowEvent &event);

	wxTextCtrl * m_logviewTextCtrl;
	RCPLogReader m_logReader;
	RaceAnalyzerComm * m_comm;

	DECLARE_EVENT_TABLE()
};

#endif /* LOGVIEWERPANE_H_ */
