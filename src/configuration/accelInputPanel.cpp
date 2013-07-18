#include "configuration/configPanel.h"
#include "configuration/accelInputPanel.h"

#include "accelX_pos.xpm"
#include "accelX_neg.xpm"
#include "accelY_pos.xpm"
#include "accelY_neg.xpm"

#include "rcp.xpm"
#include "rcp_180.xpm"
#include "rcp_90_ccw.xpm"
#include "rcp_90_cw.xpm"

#define MSG_NORMAL_MOUNTING "Upright mounting"
#define MSG_INVERTED_MOUNTING "Upside-down mounting"
#define MSG_INVALID_CONFIGURATION "Invalid configuration"
#define MSG_INVALID_MODE_COMBO "Invert X, Y or Z-Axis"

AccelInputPanel::AccelInputPanel() : BaseChannelConfigPanel()
{
}

AccelInputPanel::AccelInputPanel(wxWindow *parent,
			ConfigPanelParams configParams,
			wxWindowID id,
			const wxPoint &pos,
			const wxSize &size,
			long style,
			const wxString &name
			)
			: BaseChannelConfigPanel(	parent,
						configParams,
						id,
						pos,
						size,
						style,
						name)
{

}

AccelInputPanel::~AccelInputPanel(){

}


void AccelInputPanel::UpdateExtendedChannelFields(int i){
	AccelConfig &cfg = (m_configParams.config->accelConfigs[i]);
	m_accelMode[i]->Select(cfg.mode);
	m_accelMapping[i]->Select(cfg.channel);
	m_accelZeroValue[i]->SetValue(cfg.zeroValue);
}

ChannelConfig * AccelInputPanel::GetChannelConfig(int i){
	return &(m_configParams.config->accelConfigs[i].channelConfig);
}

int AccelInputPanel::ChannelCount(){
	return CONFIG_ACCEL_CHANNELS;
}

ChannelConfigExtraFields AccelInputPanel::CreateExtendedChannelFields(int i){
	AccelConfig &cfg = (m_configParams.config->accelConfigs[i]);
	ChannelConfigExtraFields extraFields;
	{
		ChannelConfigExtraField f;
		wxComboBox *c = new wxComboBox(this,wxID_ANY,"",wxDefaultPosition,wxDefaultSize,0,NULL,wxCB_DROPDOWN | wxCB_READONLY);
		c->SetClientData(&cfg);
		c->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(AccelInputPanel::OnChannelModeChanged),NULL,this);
		InitModeCombo(c);
		m_accelMode[i] = c;
		f.control = c;
		f.header = "Mode";
		extraFields.Add(f);
	}
	{
		ChannelConfigExtraField f;
		wxComboBox *c = new wxComboBox(this,wxID_ANY,"",wxDefaultPosition,wxDefaultSize,0,NULL,wxCB_DROPDOWN | wxCB_READONLY);
		c->SetClientData(&cfg);
		c->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(AccelInputPanel::OnChannelMappingChanged),NULL,this);
		InitMappingCombo(c);
		m_accelMapping[i] = c;
		f.control = c;
		f.header = "Hardware\nMapping";
		extraFields.Add(f);
	}
	{
		ChannelConfigExtraField f;
		SteppedSpinCtrl *spin = new SteppedSpinCtrl(this,wxID_ANY);
		spin->SetStepValue(1);
		spin->SetRange(MIN_ACCEL_RAW,MAX_ACCEL_RAW);
		spin->SetClientData(&cfg);
		spin->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(AccelInputPanel::OnAccelZeroChanged),NULL,this);
		m_accelZeroValue[i] = spin;
		f.control = spin;
		f.header = "Zero Value";
		extraFields.Add(f);
	}

	return extraFields;
}

void AccelInputPanel::InitMappingCombo(wxComboBox *combo){
	combo->Append("X");
	combo->Append("Y");
	combo->Append("Z");
	combo->Append("Yaw");
	combo->Select(0);
}

void AccelInputPanel::InitModeCombo(wxComboBox *combo){
	combo->Append("Disabled");
	combo->Append("Normal");
	combo->Append("Inverted");
	combo->Select(0);
}

wxString AccelInputPanel::GetChannelLabel(int index){
	return wxString::Format("Axis %d",index + 1);
}

wxString AccelInputPanel::GetChannelListTitle(){
	return "Accelerometer Channels";
}

wxString AccelInputPanel::GetChannelConfigPanelName(int index){
	return wxString::Format("accel_channel_%d",index);
}

wxPanel * AccelInputPanel::GetTopInnerPanel(){
	return NULL;
}

wxPanel * AccelInputPanel::GetBottomInnerPanel(){
	wxPanel *panel = new wxPanel(this);
	wxFlexGridSizer *sizer = new wxFlexGridSizer(1,5,5);
	panel->SetSizer(sizer);


	m_orientationMsg = new wxStaticText(panel, wxID_DEFAULT, "");
	wxFont f = m_orientationMsg->GetFont();
	f.SetWeight(wxBOLD);
	m_orientationMsg->SetFont(f);

	sizer->Add(m_orientationMsg, 1, wxALIGN_CENTER_HORIZONTAL);

	sizer->Add(new wxStaticText(panel, wxID_DEFAULT, "Front of Vehicle"),1, wxALIGN_CENTER_HORIZONTAL);

	sizer->Add(new BitmapWindow(panel, wxID_ANY, accelY_pos_xpm), 1, wxALIGN_CENTER_HORIZONTAL);

	wxFlexGridSizer *innerSizer = new wxFlexGridSizer(1,3,5,5);
	innerSizer->Add(new BitmapWindow(panel, wxID_ANY, accelX_neg_xpm), 1, wxALIGN_CENTER_VERTICAL);
	m_rcpBitmap = new BitmapWindow(panel, wxID_ANY, rcp_xpm);
	innerSizer->Add(m_rcpBitmap, 1, wxALIGN_CENTER_HORIZONTAL);
	innerSizer->Add(new BitmapWindow(panel, wxID_ANY, accelX_pos_xpm), 1, wxALIGN_CENTER_VERTICAL);

	sizer->Add(innerSizer, 1, wxALIGN_CENTER_HORIZONTAL);
	sizer->Add(new BitmapWindow(panel, wxID_ANY, accelY_neg_xpm), 1, wxALIGN_CENTER_HORIZONTAL);\

	return panel;
}

void AccelInputPanel::UpdatedExtendedFields()
{
	UpdateRcpBitmap();
}

void AccelInputPanel::UpdateRcpBitmap(){

	AccelConfig &hardX = (m_configParams.config->accelConfigs[0]);
	AccelConfig &hardY = (m_configParams.config->accelConfigs[1]);
	AccelConfig &hardZ = (m_configParams.config->accelConfigs[2]);


	AccelConfig logicalX = hardX;
	AccelConfig logicalY = hardY;
	AccelConfig logicalZ = hardZ;
	const char ** mount_normal 	= rcp_xpm;
	const char ** mount_180		= rcp_180_xpm;
	int x_normal = accel_mode_normal;
	int x_inverted = accel_mode_inverted;
	int y_normal = accel_mode_normal;
	int y_inverted = accel_mode_inverted;
	int z_normal = accel_mode_normal;
	int z_inverted = accel_mode_inverted;

	if (hardX.channel == accel_channel_x && hardY.channel == accel_channel_y){
		logicalX 		= hardX;
		logicalY 		= hardY;
		logicalZ 		= hardZ;
		mount_normal 	= rcp_xpm;
		mount_180		= rcp_180_xpm;
		x_normal = accel_mode_normal;
		x_inverted = accel_mode_inverted;
		y_normal = accel_mode_normal;
		y_inverted = accel_mode_inverted;
		z_normal = accel_mode_normal;
		z_inverted = accel_mode_inverted;
	}

	if (hardX.channel == accel_channel_y && hardY.channel == accel_channel_x){
		logicalX = hardY;
		logicalY = hardX;
		logicalZ = hardZ;
		mount_normal 	= rcp_90_cw_xpm;
		mount_180		= rcp_90_ccw_xpm;
		x_normal = accel_mode_normal;
		x_inverted = accel_mode_inverted;
		y_normal = accel_mode_inverted;
		y_inverted = accel_mode_normal;
		z_normal = accel_mode_normal;
		z_inverted = accel_mode_inverted;
	}


	//Default mounting
	if (logicalX.mode == x_normal && logicalY.mode == y_normal && logicalZ.mode == z_normal){
		m_rcpBitmap->SetBitmap(mount_normal);
		m_orientationMsg->SetLabel(MSG_NORMAL_MOUNTING);
	}

	//Upside mounting
	if (logicalX.mode == x_inverted && logicalY.mode == y_normal &&	logicalZ.mode == z_inverted){
		m_rcpBitmap->SetBitmap(mount_normal);
		m_orientationMsg->SetLabel(MSG_INVERTED_MOUNTING);
	}

	if (logicalX.mode == x_inverted && logicalY.mode == y_inverted && logicalZ.mode == z_normal){
		m_rcpBitmap->SetBitmap(mount_180);
		m_orientationMsg->SetLabel(MSG_NORMAL_MOUNTING);
	}

	if (logicalX.mode == x_normal && logicalY.mode == y_inverted &&	logicalZ.mode == z_inverted){
		m_rcpBitmap->SetBitmap(mount_180);
		m_orientationMsg->SetLabel(MSG_INVERTED_MOUNTING);
	}

	if (logicalX.mode == x_normal && logicalY.mode == y_inverted &&	logicalZ.mode == z_normal){
		m_rcpBitmap->SetBitmap(mount_180);
		m_orientationMsg->SetLabel(wxString::Format("%s: %s", MSG_INVALID_CONFIGURATION, MSG_INVALID_MODE_COMBO));
	}

	if (logicalX.mode == x_inverted && logicalY.mode == y_normal && logicalZ.mode == z_normal){
		m_rcpBitmap->SetBitmap(mount_normal);
		m_orientationMsg->SetLabel(wxString::Format("%s: %s", MSG_INVALID_CONFIGURATION, MSG_INVALID_MODE_COMBO));
	}

	if (logicalX.mode == x_normal && logicalY.mode == y_normal && logicalZ.mode == z_inverted){
		m_rcpBitmap->SetBitmap(mount_normal);
		m_orientationMsg->SetLabel(wxString::Format("%s: %s", MSG_INVALID_CONFIGURATION, MSG_INVALID_MODE_COMBO));
	}

	if (logicalX.mode == x_inverted && logicalY.mode == y_inverted && logicalZ.mode == z_inverted){
		m_rcpBitmap->SetBitmap(mount_180);
		m_orientationMsg->SetLabel(wxString::Format("%s: %s", MSG_INVALID_CONFIGURATION, MSG_INVALID_MODE_COMBO));
	}

	this->Layout();

}


void AccelInputPanel::OnChannelModeChanged(wxCommandEvent &event){
	wxComboBox *c = dynamic_cast<wxComboBox*>(event.GetEventObject());
	if (NULL != c) {
		AccelConfig *cfg = (AccelConfig*)c->GetClientData();
		cfg->mode = (accel_mode_t)c->GetSelection();
	}
	UpdateRcpBitmap();
}

void AccelInputPanel::OnChannelMappingChanged(wxCommandEvent &event){
	wxComboBox *c = dynamic_cast<wxComboBox*>(event.GetEventObject());
	if (NULL != c) {
		AccelConfig *cfg = (AccelConfig*)c->GetClientData();
		cfg->channel = (accel_channel_t)c->GetSelection();
	}
	UpdateRcpBitmap();
}

void AccelInputPanel::OnAccelZeroChanged(wxCommandEvent &event){
	SteppedSpinCtrl *s = dynamic_cast<SteppedSpinCtrl*>(event.GetEventObject());
	if (NULL != s) {
		AccelConfig *cfg = (AccelConfig*)s->GetClientData();
		cfg->zeroValue = s->GetValue();
	}
}

DatalogChannels & AccelInputPanel::GetStandardChannels(){
	return m_configParams.appOptions->GetStandardAccelChannels();
}


BEGIN_EVENT_TABLE ( AccelInputPanel, wxPanel )
END_EVENT_TABLE()
