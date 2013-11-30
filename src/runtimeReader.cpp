/*
 * runtimeReader.cpp
 *
 *  Created on: Jun 23, 2012
 *      Author: brent
 */
#include "runtimeReader.h"
#include "commonEvents.h"


RuntimeReader::RuntimeReader() : wxThread(wxTHREAD_JOINABLE){}

void RuntimeReader::Create(RaceAnalyzerComm *comm, wxWindow *parent){
	m_comm = comm;
	m_parent = parent;
	wxThread::Create();
}

void * RuntimeReader::Entry(){
	while (! TestDestroy()){
		ReadRuntimeValues();
	}
	return NULL;
}

void RuntimeReader::ReadRuntimeValues(){
	RuntimeValues *values  = new RuntimeValues();
	wxString statusMsg;
	try{
		m_comm->ReadRuntime(*values);
	}
	catch (CommException &e){
		statusMsg = e.GetErrorMessage();
	}
	//Read the runtime values here
	NotifyRuntimeValues(values, statusMsg);
	Sleep(50);
}

void RuntimeReader::NotifyRuntimeValues(RuntimeValues *values, wxString &statusMsg){
	wxCommandEvent event (wxEVT_COMMAND_THREAD, RUNTIME_UPDATED);
	event.SetString(statusMsg);
	event.SetClientData(values);
	m_parent->GetEventHandler()->AddPendingEvent(event);
}




