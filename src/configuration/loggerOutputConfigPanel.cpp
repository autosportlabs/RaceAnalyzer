#include "configuration/configPanel.h"
#include "configuration/loggerOutputConfigPanel.h"


LoggerOutputConfigPanel::LoggerOutputConfigPanel(wxWindow *parent,
		ConfigPanelParams configParms,
			wxWindowID id,
			const wxPoint &pos,
			const wxSize &size,
			long style,
			const wxString &name
			)
			: BaseConfigPanel(	parent,
						configParms,
						id,
						pos,
						size,
						style,
						name)
{
}

LoggerOutputConfigPanel::~LoggerOutputConfigPanel(){

}

void LoggerOutputConfigPanel::OnConfigUpdated(){
	ConnectivityConfig &cfg = (m_configParams.config->connectivityConfig);
	m_sdLoggingModeCombo->Select(cfg.sdLoggingMode);
	m_telemetryModeCombo->Select(cfg.connectivityMode);
	m_p2pAddressHighTextCtrl->SetValue(wxString::Format("%u",cfg.p2pConfig.destinationAddrHigh));
	m_p2pAddressLowTextCtrl->SetValue(wxString::Format("%u",cfg.p2pConfig.destinationAddrLow));
	m_deviceIdTextCtrl->SetValue(cfg.telemetryConfig.telemetryDeviceId);
	m_apnHostTextCtrl->SetValue(cfg.cellularConfig.apnHost);
	m_apnUserTextCtrl->SetValue(cfg.cellularConfig.apnUser);
	m_apnPassTextCtrl->SetValue(cfg.cellularConfig.apnPass);

	UpdateTelemetryModeView(cfg.connectivityMode);
}


wxStaticBoxSizer * LoggerOutputConfigPanel::GetP2PPanel(){
	wxStaticBoxSizer *sizer = new wxStaticBoxSizer(new wxStaticBox(this,-1,"Peer to Peer Base Station Address"),wxVERTICAL);

	sizer->Add(new wxStaticText(this,wxID_ANY,"Specify the 2-part address of the Base Station Node"));
	sizer->AddSpacer(10);

	wxFlexGridSizer* p2pOptionsSizer = new wxFlexGridSizer(4,3,10,10);

	p2pOptionsSizer->AddStretchSpacer(1);
	p2pOptionsSizer->Add(new wxStaticText(this,wxID_ANY,"High"),1,wxALIGN_LEFT);
	p2pOptionsSizer->Add(new wxStaticText(this,wxID_ANY,"Low"),1,wxALIGN_LEFT);
	p2pOptionsSizer->Add(new wxStaticText(this,wxID_ANY,"Base Station Address (Decimal)"),1,wxALIGN_LEFT);

	m_p2pAddressHighTextCtrl =new wxTextCtrl(this,wxID_ANY,"",wxDefaultPosition,wxDefaultSize,0,wxTextValidator(wxFILTER_NUMERIC));
	m_p2pAddressHighTextCtrl->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(LoggerOutputConfigPanel::OnP2PAddressHighChanged),NULL,this);
	p2pOptionsSizer->Add(m_p2pAddressHighTextCtrl);

	m_p2pAddressLowTextCtrl =new wxTextCtrl(this,wxID_ANY,"",wxDefaultPosition,wxDefaultSize,0,wxTextValidator(wxFILTER_NUMERIC));
	m_p2pAddressLowTextCtrl->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(LoggerOutputConfigPanel::OnP2PAddressLowChanged),NULL,this);
	p2pOptionsSizer->Add(m_p2pAddressLowTextCtrl);

	sizer->Add(p2pOptionsSizer,1,wxEXPAND);
	sizer->Show(false);
	return sizer;
}

wxStaticBoxSizer * LoggerOutputConfigPanel::GetBluetoothPanel(){
	wxStaticBoxSizer *sizer = new wxStaticBoxSizer(new wxStaticBox(this,-1,"Bluetooth Connectivity"),wxVERTICAL);

	wxFlexGridSizer* optionsSizer = new wxFlexGridSizer(2,3,10,10);

	optionsSizer->Add(new wxStaticText(this,wxID_ANY,"Bluetooth Device Name"),1,wxALIGN_LEFT);
	m_bluetoothDeviceNameTextCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0);
//	m_bluetoothDeviceNameTextCtrl->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(LoggerOutputConfigPanel::OnTelemetryServerChanged),NULL,this);
	m_bluetoothDeviceNameTextCtrl->SetEditable(false);
	optionsSizer->Add(m_bluetoothDeviceNameTextCtrl, 1, wxEXPAND);

	optionsSizer->AddStretchSpacer(1);

	optionsSizer->Add(new wxStaticText(this,wxID_ANY,"Bluetooth Password"),1,wxALIGN_LEFT);
	m_bluetoothPasswordTextCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0);
//	m_bluetoothPasswordTextCtrl->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(LoggerOutputConfigPanel::OnTelemetryDeviceIdChanged),NULL,this);
	optionsSizer->Add(m_bluetoothPasswordTextCtrl, 1, wxEXPAND);
	m_bluetoothPasswordTextCtrl->SetEditable(false);
	optionsSizer->AddStretchSpacer(1);

	sizer->Add(optionsSizer,1,wxEXPAND);
	sizer->Show(false);
	return sizer;
}

wxStaticBoxSizer * LoggerOutputConfigPanel::GetCellTelemetryPanel(){
	wxStaticBoxSizer *sizer = new wxStaticBoxSizer(new wxStaticBox(this,-1,"Cellular Telemetry"),wxVERTICAL);

	wxFlexGridSizer* optionsSizer = new wxFlexGridSizer(6,3,10,10);

	optionsSizer->Add(new wxStaticText(this,wxID_ANY,"Telemetry Device Id"),1,wxALIGN_LEFT);
	m_deviceIdTextCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0);
	m_deviceIdTextCtrl->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(LoggerOutputConfigPanel::OnTelemetryDeviceIdChanged),NULL,this);
	optionsSizer->Add(m_deviceIdTextCtrl, 1, wxEXPAND);
	optionsSizer->AddStretchSpacer(1);

	optionsSizer->Add(new wxStaticText(this, wxID_ANY, "APN Host"), 1, wxALIGN_LEFT);
	m_apnHostTextCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0);
	m_apnHostTextCtrl->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(LoggerOutputConfigPanel::OnApnHostChanged),NULL,this);
	optionsSizer->Add(m_apnHostTextCtrl, 1, wxEXPAND);
	optionsSizer->AddStretchSpacer(1);

	optionsSizer->Add(new wxStaticText(this, wxID_ANY, "APN User"), 1, wxALIGN_LEFT);
	m_apnUserTextCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0);
	m_apnUserTextCtrl->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(LoggerOutputConfigPanel::OnApnUserChanged),NULL,this);
	optionsSizer->Add(m_apnUserTextCtrl, 1, wxEXPAND);
	optionsSizer->AddStretchSpacer(1);

	optionsSizer->Add(new wxStaticText(this, wxID_ANY, "APN Password"), 1, wxALIGN_LEFT);
	m_apnPassTextCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0);
	m_apnPassTextCtrl->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(LoggerOutputConfigPanel::OnApnPassChanged),NULL,this);
	optionsSizer->Add(m_apnPassTextCtrl, 1, wxEXPAND);
	optionsSizer->AddStretchSpacer(1);

	optionsSizer->AddGrowableRow(1);
	optionsSizer->AddStretchSpacer(1);
	optionsSizer->AddStretchSpacer(1);
	optionsSizer->AddStretchSpacer(1);


	wxButton * advancedButton = new wxButton(this, wxID_ANY, "Advanced Options...");
	advancedButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(LoggerOutputConfigPanel::OnAdvancedOptions),NULL,this);
	optionsSizer->Add(advancedButton);

	optionsSizer->AddStretchSpacer(1);
	sizer->Add(optionsSizer,1,wxEXPAND | wxALIGN_BOTTOM);
	sizer->Show(false);
	return sizer;
}


void LoggerOutputConfigPanel::InitComponents(){

 	wxBoxSizer* outerSizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(outerSizer);

	////////////////////////////////////////////////////////////////////////////////////////////////
	{
		wxStaticBoxSizer *loggingModeSizer = new wxStaticBoxSizer(new wxStaticBox(this,-1,"SD Logging Mode"),wxVERTICAL);

		loggingModeSizer->Add(new wxStaticText(this,wxID_ANY,"Specify the logging mode used for logging to the SD Memory card.\n\nSelect CSV for easy import to analysis programs & spreadsheets;\nSelect Binary format for maximum performance"));
		loggingModeSizer->AddSpacer(10);

		wxFlexGridSizer* optionsSizer = new wxFlexGridSizer(1,2,6,6);
		optionsSizer->Add(new wxStaticText(this,wxID_ANY,"Logging Mode"),0,wxALIGN_LEFT);

		m_sdLoggingModeCombo = new wxComboBox(this,wxID_ANY,"",wxDefaultPosition,wxDefaultSize);
		m_sdLoggingModeCombo->Append("SD Logging Disabled");
		m_sdLoggingModeCombo->Append("Comma Separated Values (CSV)");
		m_sdLoggingModeCombo->Append("Binary (not yet supported)");
		m_sdLoggingModeCombo->Select(0);
		optionsSizer->Add(m_sdLoggingModeCombo,0,wxALIGN_LEFT);
		m_sdLoggingModeCombo->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(LoggerOutputConfigPanel::OnLoggingModeChanged),NULL,this);

		loggingModeSizer->Add(optionsSizer,0,wxEXPAND);
		outerSizer->Add(loggingModeSizer, 1, wxEXPAND | wxALL,1);
	}
	////////////////////////////////////////////////////////////////////////////////////////////////
	{
		wxStaticBoxSizer *telemetryModeSizer = new wxStaticBoxSizer(new wxStaticBox(this,-1,"Telemetry Mode"),wxVERTICAL);

		telemetryModeSizer->Add(new wxStaticText(this,wxID_ANY,"Specify the device attached to the connectivity expansion port on RaceCapture/Pro"));
		telemetryModeSizer->AddSpacer(10);

		wxFlexGridSizer* optionsSizer = new wxFlexGridSizer(1,2,6,6);
		optionsSizer->Add(new wxStaticText(this,wxID_ANY,"Connectivity"),1,wxALIGN_LEFT);

		m_telemetryModeCombo = new wxComboBox(this,wxID_ANY,"",wxDefaultPosition,wxDefaultSize);
		m_telemetryModeCombo->Append("No Device");
		m_telemetryModeCombo->Append("Bluetooth adapter");
		m_telemetryModeCombo->Append("Cellular telemetry");
		m_telemetryModeCombo->Append("Peer to peer mesh network adapter");
		m_telemetryModeCombo->Select(0);
		optionsSizer->Add(m_telemetryModeCombo,0,wxALIGN_LEFT);
		m_telemetryModeCombo->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(LoggerOutputConfigPanel::OnTelemetryModeChanged),NULL,this);
		telemetryModeSizer->Add(optionsSizer,0);
		telemetryModeSizer->AddSpacer(20);

		m_p2pSizer = GetP2PPanel();
		telemetryModeSizer->Add(m_p2pSizer, 1, wxEXPAND);

		m_cellSizer = GetCellTelemetryPanel();
		telemetryModeSizer->Add(m_cellSizer, 1, wxEXPAND);

		m_btSizer = GetBluetoothPanel();
		telemetryModeSizer->Add(m_btSizer, 1, wxEXPAND);

		outerSizer->Add(telemetryModeSizer,1,wxEXPAND | wxALL,1);
	}
	////////////////////////////////////////////////////////////////////////////////////////////////
}

void LoggerOutputConfigPanel::UpdateTelemetryModeView(connectivity_mode_t mode){

	switch (mode){
		case telemetry_mode_disabled:
			m_p2pSizer->Show(false);
			m_btSizer->Show(false);
			m_cellSizer->Show(false);
			break;
		case telemetry_mode_bluetooth:
			m_p2pSizer->Show(false);
			m_cellSizer->Show(false);
			m_btSizer->Show(true);
			break;
		case telemetry_mode_cell:
			m_p2pSizer->Show(false);
			m_btSizer->Show(false);
			m_cellSizer->Show(true);
			break;
		case telemetry_mode_p2p:
			m_btSizer->Show(false);
			m_cellSizer->Show(false);
			m_p2pSizer->Show(true);
			break;
		default:
			break;
	}
	Layout();
}


void LoggerOutputConfigPanel::OnP2PAddressHighChanged(wxCommandEvent &event){
	wxTextCtrl *c = dynamic_cast<wxTextCtrl*>(event.GetEventObject());
	if (NULL != c) {
		ConnectivityConfig &cfg = (m_configParams.config->connectivityConfig);
		cfg.p2pConfig.destinationAddrHigh = atoi(c->GetValue());
	}
}

void LoggerOutputConfigPanel::OnP2PAddressLowChanged(wxCommandEvent &event){
	wxTextCtrl *c = dynamic_cast<wxTextCtrl*>(event.GetEventObject());
	if (NULL != c) {
		ConnectivityConfig &cfg = (m_configParams.config->connectivityConfig);
		cfg.p2pConfig.destinationAddrLow = atoi(c->GetValue());
	}
}

void LoggerOutputConfigPanel::OnTelemetryModeChanged(wxCommandEvent &event){
	wxComboBox *c = dynamic_cast<wxComboBox*>(event.GetEventObject());
	if (NULL != c) {
		ConnectivityConfig &cfg = (m_configParams.config->connectivityConfig);
		connectivity_mode_t mode = (connectivity_mode_t)c->GetSelection();
		cfg.connectivityMode = mode;
		UpdateTelemetryModeView(mode);
	}
}

void LoggerOutputConfigPanel::OnApnHostChanged(wxCommandEvent &event){
	wxTextCtrl *c = dynamic_cast<wxTextCtrl*>(event.GetEventObject());
	if (NULL != c) {
		ConnectivityConfig &cfg = (m_configParams.config->connectivityConfig);
		cfg.cellularConfig.apnHost = c->GetValue();
	}
}

void LoggerOutputConfigPanel::OnApnUserChanged(wxCommandEvent &event){
	wxTextCtrl *c = dynamic_cast<wxTextCtrl*>(event.GetEventObject());
	if (NULL != c) {
		ConnectivityConfig &cfg = (m_configParams.config->connectivityConfig);
		cfg.cellularConfig.apnUser = c->GetValue();
	}
}

void LoggerOutputConfigPanel::OnApnPassChanged(wxCommandEvent &event){
	wxTextCtrl *c = dynamic_cast<wxTextCtrl*>(event.GetEventObject());
	if (NULL != c) {
		ConnectivityConfig &cfg = (m_configParams.config->connectivityConfig);
		cfg.cellularConfig.apnPass = c->GetValue();
	}
}

void LoggerOutputConfigPanel::OnTelemetryDeviceIdChanged(wxCommandEvent &event){
	wxTextCtrl *c = dynamic_cast<wxTextCtrl*>(event.GetEventObject());
	if (NULL != c) {
		ConnectivityConfig &cfg = (m_configParams.config->connectivityConfig);
		cfg.telemetryConfig.telemetryDeviceId = c->GetValue();
	}
}

void LoggerOutputConfigPanel::OnLoggingModeChanged(wxCommandEvent &event){
	wxComboBox *c = dynamic_cast<wxComboBox*>(event.GetEventObject());
	if (NULL != c) {
		ConnectivityConfig &cfg = (m_configParams.config->connectivityConfig);
		cfg.sdLoggingMode = (sd_logging_mode_t)c->GetSelection();
	}
}

void LoggerOutputConfigPanel::OnAdvancedOptions(wxCommandEvent &event){

	TelemetryAdvancedOptionsDialog dlg;
	ConnectivityConfig *c = &(m_configParams.config->connectivityConfig);
	dlg.SetConfig(c);
	dlg.Create(this);
	if (dlg.ShowModal() == wxID_OK){
		*c = dlg.GetConfig();
	}
}

BEGIN_EVENT_TABLE ( LoggerOutputConfigPanel, wxPanel )
END_EVENT_TABLE()
