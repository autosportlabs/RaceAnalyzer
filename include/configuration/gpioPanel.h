
#ifndef GPIOPANEL_H_
#define GPIOPANEL_H_

#include "wx/wxprec.h"
#include "raceAnalyzerConfigBase.h"
#include "raceCapture/raceCaptureConfig.h"
#include "configuration/channelConfigPanel.h"
#include "configuration/baseChannelConfigPanel.h"
#include "commonEvents.h"
#include "comm.h"


class GpioPanel : public BaseChannelConfigPanel{

	public:
		GpioPanel();
		GpioPanel(wxWindow *parent,
					ConfigPanelParams config,
					wxWindowID id = -1,
					const wxPoint &pos = wxDefaultPosition,
					const wxSize &size = wxDefaultSize,
					long style = wxTAB_TRAVERSAL,
					const wxString &name = "gpioPanel"
					);

		~GpioPanel();
		wxPanel * GetTopInnerPanel();
		wxPanel * GetBottomInnerPanel();
		ChannelConfig * GetChannelConfig(int i);
		DatalogChannels & GetStandardChannels();
		int ChannelCount();
		void UpdatedExtendedFields();
		void UpdateExtendedChannelFields(int i);
		ChannelConfigExtraFields CreateExtendedChannelFields(int i);
		wxString GetChannelConfigPanelName(int index);
		wxString GetChannelLabel(int index);
		wxString GetChannelListTitle();
		//event handlers
	private:
		void OnModeChanged(wxCommandEvent &event);
		void InitModeCombo(wxComboBox *modeCombo);

		wxComboBox * m_modeCombo[CONFIG_GPIO_CHANNELS];

	DECLARE_EVENT_TABLE()
};




#endif /* GPIOPANEL_H_ */
