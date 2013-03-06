
#ifndef BASECONFIGPANEL_H_
#define BASECONFIGPANEL_H_

#include "wx/wxprec.h"
#include "raceAnalyzerConfigBase.h"
#include "raceCapture/raceCaptureConfig.h"
#include "configuration/channelConfigPanel.h"
#include "commonEvents.h"
#include "comm.h"
#include "configuration/configPanelParams.h"

class BaseConfigPanel : public wxPanel{

	public:
		BaseConfigPanel();
		BaseConfigPanel(wxWindow *parent,
					ConfigPanelParams config,
					wxWindowID id = -1,
					const wxPoint &pos = wxDefaultPosition,
					const wxSize &size = wxDefaultSize,
					long style = wxTAB_TRAVERSAL,
					const wxString &name = "baseConfigPanel"
					);

		~BaseConfigPanel();
		virtual void OnConfigUpdated() = 0;
		virtual void InitComponents() = 0;

		//event handlers
	protected:
		ConfigPanelParams m_configParams;

	private:
	DECLARE_EVENT_TABLE()
};




#endif /* BASECONFIGPANEL_H_ */
