#include "RCPLogReader.h"
#include "commonEvents.h"
#include "logging.h"

#define DEFAULT_SLEEP_TIME 500
#define ERROR_SLEEP_TIME 3000
RCPLogReader::RCPLogReader() : wxThread(wxTHREAD_JOINABLE), m_shouldReadLog(true), m_parent(NULL), m_comm(NULL), m_sleepTime(500){}

void RCPLogReader::Create(RaceAnalyzerComm *comm, wxWindow *parent){
	m_comm = comm;
	m_parent = parent;
	wxThread::Create();
}

void * RCPLogReader::Entry(){
	while (! TestDestroy()){
		if (m_shouldReadLog) ReadLog();
		Sleep(m_sleepTime);
	}
	return NULL;
}

void RCPLogReader::SetShouldReadLog(bool shouldRead){
	m_shouldReadLog = shouldRead;
}

void RCPLogReader::ReadLog(){
	wxString data;
	try{
		data = m_comm->GetLogfile();
		m_sleepTime = DEFAULT_SLEEP_TIME;
	}
	catch (CommException &e){
		data = wxString::Format("error reading logfile: %s\r\n", e.GetErrorMessage());
		m_sleepTime = ERROR_SLEEP_TIME;
	}
	//Read the runtime values here
	NotifyLogData(data);
}

void RCPLogReader::NotifyLogData(wxString &data){
	wxCommandEvent event (wxEVT_COMMAND_THREAD, RCP_LOG_UPDATE);
	event.SetString(data);
	m_parent->GetEventHandler()->AddPendingEvent(event);
}
