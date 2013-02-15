#include "configuration/configPanel.h"
#include "configuration/gpioPanel.h"

GpioPanel::GpioPanel() : BaseChannelConfigPanel()
{
}

GpioPanel::GpioPanel(wxWindow *parent,
		ConfigPanelParams *config,
			wxWindowID id,
			const wxPoint &pos,
			const wxSize &size,
			long style,
			const wxString &name
			)
			: BaseChannelConfigPanel(	parent,
						config,
						id,
						pos,
						size,
						style,
						name)
{

}

GpioPanel::~GpioPanel(){

}

void GpioPanel::UpdateExtendedChannelFields(int i){
	m_modeCombo[i]->SetSelection(m_configParams->config->gpioConfigs[i].mode);
}

void GpioPanel::InitModeCombo(wxComboBox * modeCombo){
	modeCombo->Append("Input");
	modeCombo->Append("Output");
	modeCombo->SetSelection(0);
}

ChannelConfig * GpioPanel::GetChannelConfig(int i){
	return &(m_configParams->config->gpioConfigs[i].channelConfig);
}

int GpioPanel::ChannelCount(){
	return CONFIG_GPIO_CHANNELS;
}

ChannelConfigExtraFields GpioPanel::CreateExtendedChannelFields(int i){
	GpioConfig &cfg = (m_configParams->config->gpioConfigs[i]);
	ChannelConfigExtraFields extraFields;
	{
		ChannelConfigExtraField f;
		wxComboBox *c = new wxComboBox(this,-1,"",wxDefaultPosition,wxDefaultSize,0,NULL,wxCB_DROPDOWN | wxCB_READONLY);
		c->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(GpioPanel::OnModeChanged),NULL,this);
		c->SetClientData(&cfg);
		InitModeCombo(c);
		f.control = m_modeCombo[i] = c;
		f.header = "Mode";
		extraFields.Add(f);
	}
	return extraFields;
}

wxString GpioPanel::GetChannelLabel(int index){
	return wxString::Format("GPIO %d",index + 1);
}

wxString GpioPanel::GetChannelListTitle(){
	return "General Purpose Input/Output Channels";
}

wxString GpioPanel::GetChannelConfigPanelName(int index){
	return wxString::Format("gpio_channel_%d",index);
}

wxPanel * GpioPanel::GetTopInnerPanel(){
	return NULL;
}

wxPanel * GpioPanel::GetBottomInnerPanel(){
	return NULL;
}

void GpioPanel::UpdatedExtendedFields()
{
}

DatalogChannels & GpioPanel::GetStandardChannels(){
	return m_configParams->appOptions->GetStandardGpioChannels();
}

void GpioPanel::OnModeChanged(wxCommandEvent &event){
	wxComboBox *c = dynamic_cast<wxComboBox*>(event.GetEventObject());
	if (NULL != c) {
		GpioConfig *cfg = (GpioConfig*)c->GetClientData();
		cfg->mode = (gpio_mode_t)c->GetSelection();
	}
}
BEGIN_EVENT_TABLE ( GpioPanel, wxPanel )
END_EVENT_TABLE()
