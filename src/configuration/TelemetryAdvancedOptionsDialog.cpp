
#include "configuration/telemetryAdvancedOptionsDialog.h"

#define NUMBER_COM_PORTS 20
#define LOAD_TYPE_SELECTION_MAP 	0
#define LOAD_TYPE_SELECTION_TPS 	1

IMPLEMENT_CLASS (TelemetryAdvancedOptionsDialog, wxDialog)

TelemetryAdvancedOptionsDialog::TelemetryAdvancedOptionsDialog(){}

TelemetryAdvancedOptionsDialog::TelemetryAdvancedOptionsDialog(wxWindow* parent,
  			wxWindowID id,
  			const wxString& caption,
  			const wxPoint& pos,
  			const wxSize& size,
  			long style) : m_telemetryServerTextCtrl(NULL){
	Create(parent,id,caption,pos,size,style);
}

ConnectivityConfig  TelemetryAdvancedOptionsDialog::GetConfig(){
	return m_config;
}

void TelemetryAdvancedOptionsDialog::SetConfig(ConnectivityConfig *config){
	if (NULL != config) m_config = *config;
}

bool TelemetryAdvancedOptionsDialog::Create (wxWindow* parent,
  			wxWindowID id,
  			const wxString& caption,
  			const wxPoint& pos,
  			const wxSize& size,
  			long style){

 	SetExtraStyle(wxWS_EX_BLOCK_EVENTS);

 	if (! wxDialog::Create (parent, id, caption, pos, size, style)) return false;

 	CreateControls();
 	RefreshValues();

 	GetSizer()->Fit(this);
 	GetSizer()->SetSizeHints(this);
 	Center();
 	return true;
}

void TelemetryAdvancedOptionsDialog::RefreshValues(){
	if (NULL != m_telemetryServerTextCtrl) m_telemetryServerTextCtrl->SetValue(m_config.telemetryConfig.telemetryServer);
}

void TelemetryAdvancedOptionsDialog::TelemetryServerChanged(wxCommandEvent &event){
	wxTextCtrl *c = dynamic_cast<wxTextCtrl*>(event.GetEventObject());
	if (NULL != c) {
		m_config.telemetryConfig.telemetryServer = c->GetValue();
	}
}

void TelemetryAdvancedOptionsDialog::CreateControls(){

 	wxBoxSizer* outerSizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(outerSizer);

	wxFlexGridSizer* optionsSizer = new wxFlexGridSizer(5,2,6,6);
	optionsSizer->AddGrowableCol(1);
	wxBoxSizer* okCancelSizer = new wxBoxSizer(wxHORIZONTAL);

	outerSizer->Add(optionsSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL,1);
	outerSizer->Add(okCancelSizer,0,wxALIGN_CENTER_HORIZONTAL | wxALL,3);

	m_telemetryServerTextCtrl = new wxTextCtrl(this,-1, "");

	optionsSizer->Add(new wxStaticText(this, -1, "Telemetry Server"));
	optionsSizer->Add(m_telemetryServerTextCtrl);
	m_telemetryServerTextCtrl->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(TelemetryAdvancedOptionsDialog::TelemetryServerChanged),NULL,this);

 	wxButton* ok = new wxButton(this, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize,0);
	okCancelSizer->Add(ok, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);

	wxButton* cancel = new wxButton(this, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize,0);
	okCancelSizer->Add(cancel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
}


BEGIN_EVENT_TABLE( TelemetryAdvancedOptionsDialog, wxDialog)
END_EVENT_TABLE()
