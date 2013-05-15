/*
 * importerThread.h
 *
 *  Created on: May 14, 2013
 *      Author: brent
 */

#ifndef IMPORTERTHREAD_H_
#define IMPORTERTHREAD_H_
#include "wx/wxprec.h"
#include "wx/thread.h"
#include "appPrefs.h"
#include "appOptions.h"
#include "datalogStore.h"
#include "commonEvents.h"

class ImportWizardParams{

public:
	ImportWizardParams() :
		appPrefs(NULL),
		appOptions(NULL),
		datalogStore(NULL),
		datalogFilePath(""),
		datalogName(GetDefaultDatalogName()),
		datalogDesc(""),
		datalogImported(false)
		{}

	ImportWizardParams(
			AppPrefs *prefs,
			AppOptions *options,
			DatalogStore *store,
			wxString filePath = "",
			wxString name = "",
			wxString desc = ""):
		appPrefs(prefs),
		appOptions(options),
		datalogStore(store),
		datalogFilePath(filePath),
		datalogName(name),
		datalogDesc(desc),
		datalogImported(false)
		{
				if(0 == name.Length()) datalogName = GetDefaultDatalogName();
		}

	AppPrefs 			*appPrefs;
	AppOptions			*appOptions;
	DatalogStore 		*datalogStore;
	wxString			datalogFilePath;
	wxString			datalogName;
	wxString			datalogDesc;
	DatalogChannels		datalogChannels;
	DatalogChannelTypes datalogChannelTypes;
	bool				datalogImported;

	wxString GetDefaultDatalogName();
};

class ImporterThread : public wxThread , public DatalogImportProgressListener{

public:
	ImporterThread();
	void SetParams(ImportWizardParams *params, wxWindow *owner);
	virtual wxThread::ExitCode Entry();
	virtual void UpdateProgress(int completePercent);

private:
	ImportWizardParams *m_params;
	wxWindow *m_owner;
};

#endif /* IMPORTERTHREAD_H_ */
