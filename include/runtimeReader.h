/*
 * runtimeReader.h
 *
 *  Created on: Jun 23, 2012
 *      Author: brent
 */

#ifndef RUNTIMEREADER_H_
#define RUNTIMEREADER_H_

#include "wx/wxprec.h"
#include "raceCapture/raceCaptureRuntime.h"
#include "comm.h"

class RuntimeReader : public wxThread {

public:
	void Create(RaceAnalyzerComm *comm, wxWindow *parent);
	void * Entry();

private:
	void ReadRuntimeValues();
	void NotifyRuntimeValues(RuntimeValues &values, wxString &statusMsg);
	wxWindow *m_parent;
	RaceAnalyzerComm *m_comm;


};


#endif /* RUNTIMEREADER_H_ */
