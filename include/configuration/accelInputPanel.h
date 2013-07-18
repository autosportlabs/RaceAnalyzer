
#ifndef ACCELINPUTPANEL_H_
#define ACCELINPUTPANEL_H_

#include "wx/wxprec.h"
#include "raceAnalyzerConfigBase.h"
#include "raceCapture/raceCaptureConfig.h"
#include "configuration/channelConfigPanel.h"
#include "configuration/baseChannelConfigPanel.h"
#include "commonEvents.h"
#include "configPanel.h"
#include "comm.h"
#include "BitmapWindow.h"

class AccelInputPanel : public BaseChannelConfigPanel{

	public:
		AccelInputPanel();
		AccelInputPanel(wxWindow *parent,
					ConfigPanelParams config,
					wxWindowID id = -1,
					const wxPoint &pos = wxDefaultPosition,
					const wxSize &size = wxDefaultSize,
					long style = wxTAB_TRAVERSAL,
					const wxString &name = "accelInputPanel"
					);

		~AccelInputPanel();
		wxString GetChannelListTitle();
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

	private:
		//event handlers
		void OnCalibrateAccel(wxCommandEvent &event);
		void OnChannelModeChanged(wxCommandEvent &event);
		void OnChannelMappingChanged(wxCommandEvent &event);
		void OnAccelZeroChanged(wxCommandEvent &event);
		void InitMappingCombo(wxComboBox *combo);
		void InitModeCombo(wxComboBox *combo);
		void UpdateRcpBitmap();

		wxComboBox *m_accelMapping[CONFIG_ACCEL_CHANNELS];
		wxComboBox *m_accelMode[CONFIG_ACCEL_CHANNELS];
		SteppedSpinCtrl *m_accelZeroValue[CONFIG_ACCEL_CHANNELS];
		BitmapWindow *m_rcpBitmap;
		wxStaticText *m_orientationMsg;

	DECLARE_EVENT_TABLE()
};




#endif /* ACCELINPUTPANEL_H_ */
