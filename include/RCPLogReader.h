#ifndef RCPLOGREADER_H_
#define RCPLOGREADER_H_
#include "wx/wxprec.h"
#include "raceCapture/raceCaptureRuntime.h"
#include "comm.h"

class RCPLogReader : public wxThread {

public:
	RCPLogReader();
	void Create(RaceAnalyzerComm *comm, wxWindow *parent);
	void * Entry();
	void SetShouldReadLog(bool shouldRead);

private:
	void ReadLog();
	void NotifyLogData(wxString &data);
	bool m_shouldReadLog;
	wxWindow *m_parent;
	RaceAnalyzerComm *m_comm;
	int m_sleepTime;
};


#endif /* RCPLOGREADER_H_ */
