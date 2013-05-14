/*
 * runtimeReader.cpp
 *
 *  Created on: Jun 23, 2012
 *      Author: brent
 */
#include "runtimeReader.h"
#include "commonEvents.h"


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
	RuntimeValues values;

	m_comm->ReadRuntime(values);
	//Read the runtime values here
	NotifyRuntimeValues(values);
	Sleep(50);
}

void RuntimeReader::NotifyRuntimeValues(RuntimeValues &values){
	wxCommandEvent event (wxEVT_COMMAND_THREAD, RUNTIME_UPDATED);
	event.SetClientData(&values);
	m_parent->GetEventHandler()->AddPendingEvent(event);
}




