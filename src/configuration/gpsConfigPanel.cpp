#include "configuration/configPanel.h"
#include "configuration/gpsConfigPanel.h"
#include "BitmapWindow.h"

#include "finishLine.xpm"
#include "stopwatch.xpm"

GpsConfigPanel::GpsConfigPanel() : BaseChannelConfigPanel()
{
}

GpsConfigPanel::GpsConfigPanel(wxWindow *parent,
		ConfigPanelParams *configParams,
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
{ }

GpsConfigPanel::~GpsConfigPanel(){
}

void GpsConfigPanel::UpdatedExtendedFields(){
	GpsConfig &gpsConfig = m_configParams->config->gpsConfig;

	m_gpsInstalledCheckBox->SetValue(gpsConfig.gpsInstalled);
	m_startFinishLatitudeTextCtrl->SetValue(wxString::Format("%f",gpsConfig.startFinishLatitude));
	m_startFinishLongitudeTextCtrl->SetValue(wxString::Format("%f",gpsConfig.startFinishLongitude));
	m_startFinishTargetRadius->SetValue(wxString::Format("%f",gpsConfig.startFinishRadius));

	m_splitLatitudeTextCtrl->SetValue(wxString::Format("%f",gpsConfig.splitLatitude));
	m_splitLongitudeTextCtrl->SetValue(wxString::Format("%f",gpsConfig.splitLongitude));
	m_splitTargetRadius->SetValue(wxString::Format("%f",gpsConfig.splitRadius));

}

void GpsConfigPanel::UpdateExtendedChannelFields(int i){
}

ChannelConfig * GpsConfigPanel::GetChannelConfig(int i){
	GpsConfig &cfg = m_configParams->config->gpsConfig;
	switch(i){
		case 0:
			return &(cfg.latitudeCfg);
		case 1:
			return &(cfg.longitudeCfg);
		case 2:
			return &(cfg.speedCfg);
		case 3:
			return &(cfg.timeCfg);
		case 4:
			return &(cfg.satellitesCfg);
		case 5:
			return &(cfg.lapCountCfg);
		case 6:
			return &(cfg.lapTimeCfg);
		case 7:
			return &(cfg.splitTimeCfg);
		default:
			return NULL;
	}
}

int GpsConfigPanel::ChannelCount(){
	return CONFIG_GPS_CHANNELS;
}

ChannelConfigExtraFields GpsConfigPanel::CreateExtendedChannelFields(int i){
	ChannelConfigExtraFields extraFields;
	return extraFields;
}

wxString GpsConfigPanel::GetChannelLabel(int index){
	switch(index){
		case 0:
			return "Latitude";
		case 1:
			return "Longitude";
		case 2:
			return "Speed";
		case 3:
			return "Time";
		case 4:
			return "GPS Satellites";
		case 5:
			return "Lap Count";
		case 6:
			return "Lap Time";
		case 7:
			return "Split Time";
		default:
			return "";
	}
}

wxString GpsConfigPanel::GetChannelListTitle(){
	return "GPS Channels";
}

wxString GpsConfigPanel::GetChannelConfigPanelName(int index){
	return wxString::Format("gps_channel_%d",index);
}

wxSizer * GpsConfigPanel::GetStartFinishContainer(){
	wxStaticBoxSizer *sizer = new wxStaticBoxSizer(new wxStaticBox(m_gpsOptionsPanel,-1,"Start/Finish Line Target"),wxHORIZONTAL);
	wxFlexGridSizer *innerSizer = new wxFlexGridSizer(3,4,8,8);
	innerSizer->AddGrowableCol(2);

	//first row
	innerSizer->Add(new wxStaticText(m_gpsOptionsPanel,wxID_ANY,"Target Latitude/Longitude"),1,wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);

	m_startFinishLatitudeTextCtrl = new wxTextCtrl(m_gpsOptionsPanel,wxID_ANY,"",wxDefaultPosition,wxDefaultSize,0,wxTextValidator(wxFILTER_NUMERIC));
	m_startFinishLatitudeTextCtrl->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(GpsConfigPanel::OnStartFinishLatitudeChanged),NULL,this);
	innerSizer->Add(m_startFinishLatitudeTextCtrl,1);

	m_startFinishLongitudeTextCtrl =new wxTextCtrl(m_gpsOptionsPanel,wxID_ANY,"",wxDefaultPosition,wxDefaultSize,0,wxTextValidator(wxFILTER_NUMERIC));
	m_startFinishLongitudeTextCtrl->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(GpsConfigPanel::OnStartFinishLongitudeChanged),NULL,this);
	innerSizer->Add(m_startFinishLongitudeTextCtrl,1);

	innerSizer->Add(new wxStaticText(m_gpsOptionsPanel,wxID_ANY,"Degrees"),1,wxALIGN_CENTER_VERTICAL);

	//next row
	innerSizer->Add(new wxStaticText(m_gpsOptionsPanel,wxID_ANY,"Target Radius"),1,wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);

	m_startFinishTargetRadius = new wxTextCtrl(m_gpsOptionsPanel,wxID_ANY,"",wxDefaultPosition,wxDefaultSize,0,wxTextValidator(wxFILTER_NUMERIC));
	m_startFinishTargetRadius->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(GpsConfigPanel::OnStartFinishRadiusChanged),NULL,this);

	innerSizer->Add(m_startFinishTargetRadius,1);
	innerSizer->Add(new wxStaticText(m_gpsOptionsPanel,wxID_ANY,"Degrees"),1,wxALIGN_CENTER_VERTICAL);
	innerSizer->AddStretchSpacer(1);

	sizer->Add(innerSizer);
	sizer->Add(new BitmapWindow(m_gpsOptionsPanel,wxID_ANY,finishLine_xpm), 0, wxALIGN_CENTER_HORIZONTAL);

	return sizer;
}

wxSizer * GpsConfigPanel::GetSplitTargetContainer(){

	wxStaticBoxSizer *sizer = new wxStaticBoxSizer(new wxStaticBox(m_gpsOptionsPanel,-1,"Split Time Target"),wxHORIZONTAL);
	wxFlexGridSizer *innerSizer = new wxFlexGridSizer(3,4,8,8);
	innerSizer->AddGrowableCol(2);

	innerSizer->Add(new wxStaticText(m_gpsOptionsPanel,wxID_ANY,"Split Latitude/Longitude"),1,wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);

	m_splitLatitudeTextCtrl = new wxTextCtrl(m_gpsOptionsPanel,wxID_ANY,"",wxDefaultPosition,wxDefaultSize,0,wxTextValidator(wxFILTER_NUMERIC));
	m_splitLatitudeTextCtrl->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(GpsConfigPanel::OnSplitLatitudeChanged),NULL,this);
	innerSizer->Add(m_splitLatitudeTextCtrl,1);

	m_splitLongitudeTextCtrl =new wxTextCtrl(m_gpsOptionsPanel,wxID_ANY,"",wxDefaultPosition,wxDefaultSize,0,wxTextValidator(wxFILTER_NUMERIC));
	m_splitLongitudeTextCtrl->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(GpsConfigPanel::OnSplitLongitudeChanged),NULL,this);
	innerSizer->Add(m_splitLongitudeTextCtrl,1);

	innerSizer->Add(new wxStaticText(m_gpsOptionsPanel,wxID_ANY,"Degrees"),1,wxALIGN_CENTER_VERTICAL);

	//next row
	innerSizer->Add(new wxStaticText(m_gpsOptionsPanel,wxID_ANY,"Target Radius"),1,wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);

	m_splitTargetRadius = new wxTextCtrl(m_gpsOptionsPanel,wxID_ANY,"",wxDefaultPosition,wxDefaultSize,0,wxTextValidator(wxFILTER_NUMERIC));
	m_splitTargetRadius->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(GpsConfigPanel::OnSplitRadiusChanged),NULL,this);
	innerSizer->Add(m_splitTargetRadius,1);

	innerSizer->Add(new wxStaticText(m_gpsOptionsPanel,wxID_ANY,"Degrees"),1,wxALIGN_CENTER_VERTICAL);
	innerSizer->AddStretchSpacer(1);

	sizer->Add(innerSizer);
	sizer->Add(new BitmapWindow(m_gpsOptionsPanel,wxID_ANY,stopwatch_xpm),0, wxALIGN_CENTER_HORIZONTAL);

	return sizer;
}

wxPanel * GpsConfigPanel::GetTopInnerPanel(){

	m_gpsOptionsPanel = new wxPanel(this);

	wxFlexGridSizer *sizer =new wxFlexGridSizer(3,1,8,8);
	sizer->AddGrowableRow(0);
	sizer->AddGrowableRow(1);
	sizer->AddGrowableCol(0);

	m_gpsInstalledCheckBox = new wxCheckBox(m_gpsOptionsPanel,wxID_ANY,"GPS Installed");
	m_gpsInstalledCheckBox->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(GpsConfigPanel::OnGpsInstalledChanged),NULL,this);
	sizer->Add(m_gpsInstalledCheckBox,1,wxEXPAND | wxALIGN_CENTER_VERTICAL);

	sizer->Add(GetStartFinishContainer(), 1, wxEXPAND);
	sizer->Add(GetSplitTargetContainer(), 1, wxEXPAND);

	m_gpsOptionsPanel->SetSizer(sizer);

	return m_gpsOptionsPanel;
}

wxPanel * GpsConfigPanel::GetBottomInnerPanel(){
	return NULL;
}

void GpsConfigPanel::OnGpsInstalledChanged(wxCommandEvent &event){
	wxCheckBox *c = dynamic_cast<wxCheckBox*>(event.GetEventObject());
	if (NULL != c) m_configParams->config->gpsConfig.gpsInstalled = c->GetValue();
}

void GpsConfigPanel::OnStartFinishLatitudeChanged(wxCommandEvent &event){
	wxTextCtrl *c = dynamic_cast<wxTextCtrl*>(event.GetEventObject());
	if (NULL != c) m_configParams->config->gpsConfig.startFinishLatitude = atof(c->GetValue());
}

void GpsConfigPanel::OnStartFinishLongitudeChanged(wxCommandEvent &event){
	wxTextCtrl *c = dynamic_cast<wxTextCtrl*>(event.GetEventObject());
	if (NULL != c) m_configParams->config->gpsConfig.startFinishLongitude = atof(c->GetValue());

}

void GpsConfigPanel::OnStartFinishRadiusChanged(wxCommandEvent &event){
	wxTextCtrl *c = dynamic_cast<wxTextCtrl*>(event.GetEventObject());
	if (NULL != c) m_configParams->config->gpsConfig.startFinishRadius = atof(c->GetValue());
}

void GpsConfigPanel::OnSplitLatitudeChanged(wxCommandEvent &event){
	wxTextCtrl *c = dynamic_cast<wxTextCtrl*>(event.GetEventObject());
	if (NULL != c) m_configParams->config->gpsConfig.splitLatitude = atof(c->GetValue());
}

void GpsConfigPanel::OnSplitLongitudeChanged(wxCommandEvent &event){
	wxTextCtrl *c = dynamic_cast<wxTextCtrl*>(event.GetEventObject());
	if (NULL != c) m_configParams->config->gpsConfig.splitLongitude = atof(c->GetValue());

}

void GpsConfigPanel::OnSplitRadiusChanged(wxCommandEvent &event){
	wxTextCtrl *c = dynamic_cast<wxTextCtrl*>(event.GetEventObject());
	if (NULL != c) m_configParams->config->gpsConfig.splitRadius = atof(c->GetValue());
}

DatalogChannels & GpsConfigPanel::GetStandardChannels(){
	return m_configParams->appOptions->GetStandardGpsChannels();
}


BEGIN_EVENT_TABLE ( GpsConfigPanel, wxPanel )
END_EVENT_TABLE()
