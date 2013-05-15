/*
 * importWizardDialog.h
 *
 *  Created on: Jun 22, 2009
 *      Author: brent
 */

#ifndef IMPORTWIZARDDIALOG_H_
#define IMPORTWIZARDDIALOG_H_
#include "wx/wxprec.h"
#include "wx/wizard.h"
#include "wx/grid.h"
#include "wx/gauge.h"
#include "appPrefs.h"
#include "appOptions.h"
#include "datalogStore.h"
#include "exceptions.h"
#include "importerThread.h"
#include "commonEvents.h"



class DatalogFileSelectPage : public wxWizardPageSimple{

public:
	DatalogFileSelectPage(wxWizard *parent, ImportWizardParams *params);
	void OnDatalogPathTextChange(wxCommandEvent &event);
	void OnBrowse(wxCommandEvent &event);
	void OnWizardPageChanging(wxWizardEvent &event);
	void OnWizardPageChanged(wxWizardEvent &event);
	wxString GetDatalogFilePath();
    void SetDatalogFilePath(wxString filePath);
    bool IsFileValid(wxString filePath);
    void UpdateUIState();
    DECLARE_EVENT_TABLE()

private:
	ImportWizardParams 	*m_params;
	wxTextCtrl 			*m_filePath;
    wxStaticText		*m_helpText;
    wxColor				m_defaultColor;
};

class DatalogInfoPage : public wxWizardPageSimple{

public:
	DatalogInfoPage(wxWizard *parent, ImportWizardParams *params);
    void UpdateUIState();
	void OnNameChange(wxCommandEvent &event);
	void OnWizardPageChanging(wxWizardEvent &event);
	void OnWizardPageChanged(wxWizardEvent &event);
    DECLARE_EVENT_TABLE()

private:
	ImportWizardParams 	*m_params;
	wxTextCtrl			*m_name;
	wxTextCtrl 			*m_description;
	wxColor				m_defaultColor;

};

class MapDatalogChannelsPage : public wxWizardPageSimple{

public:
	MapDatalogChannelsPage(wxWizard *parent, ImportWizardParams *params);
	void OnWizardPageChanged(wxWizardEvent &event);
	void OnWizardPageChanging(wxWizardEvent &event);
	void PopulateChannels();
	void AddExistingChannels(
			DatalogHeaders &headers,
			DatalogHeaders &remainingHeaders,
			DatalogChannels &channels,
			DatalogChannelTypes &channelTypes,
			DatalogChannels &existingChannels,
			DatalogChannelTypes &existingChannelTypes
			);

	void RefreshChannelGrid();
	void UpdateUIState();
	DECLARE_EVENT_TABLE()

private:
	ImportWizardParams 	*m_params;
	wxGrid				*m_channelMapGrid;
	wxStaticText		*m_infoMessage;
};


class DatalogImporterPage : public wxWizardPageSimple{

public:
	DatalogImporterPage(wxWizard *parent, ImportWizardParams *params);
	void OnWizardPageChanged(wxWizardEvent &event);
	void OnWizardPageChanging(wxWizardEvent &event);
	void OnWizardPageFinished(wxWizardEvent &event);
	void DoImportDatalog();
	void OnStartImport(wxCommandEvent &event);
	void OnImportProgress(wxCommandEvent &event);
	void OnImportResult(wxCommandEvent &event);
	void UpdateUIState();

	DECLARE_EVENT_TABLE()

private:
	ImportWizardParams *m_params;
	wxGauge *m_progressGauge;
	wxStaticText *m_progressMessage;
	wxStaticText *m_statusMessage;
	bool	m_importing;
	ImporterThread *m_importerThread;
};

class ImportDatalogWizard : public wxWizard{

public:
    ImportDatalogWizard(wxFrame *frame, ImportWizardParams params);

    wxWizardPage *GetFirstPage() const { return m_page1; }


private:
    wxWizardPageSimple 	*m_page1;
    ImportWizardParams	m_params;
};

#endif /* IMPORTWIZARDDIALOG_H_ */
