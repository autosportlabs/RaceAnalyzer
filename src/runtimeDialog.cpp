
#include "runtimeDialog.h"

#define NUMBER_COM_PORTS 20
#define LOAD_TYPE_SELECTION_MAP 	0
#define LOAD_TYPE_SELECTION_TPS 	1

IMPLEMENT_CLASS (RuntimeDialog, wxDialog)

RuntimeDialog::~RuntimeDialog(){
	m_runtimeReader.Delete();
}

RuntimeDialog::RuntimeDialog(wxWindow* parent,
			RaceAnalyzerComm *comm,
  			wxWindowID id,
  			const wxString& caption,
  			const wxPoint& pos,
  			const wxSize& size,
  			long style) {
	Create(parent,comm,id,caption,pos,size,style);
}

bool RuntimeDialog::Create (wxWindow* parent,
			RaceAnalyzerComm *comm,
  			wxWindowID id,
  			const wxString& caption,
  			const wxPoint& pos,
  			const wxSize& size,
  			long style){

	m_comm = comm;
 	SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
 	if (! wxFrame::Create (parent, id, caption, pos, size, style)) return false;
 	CreateControls();
 	GetSizer()->Fit(this);
 	GetSizer()->SetSizeHints(this);
 	Center();
 	m_runtimeReader.Create(m_comm, this);
 	m_runtimeReader.Run();
 	return true;
}

void RuntimeDialog::CreateControls(){
	this->SetBackgroundColour(wxColor(30,30,30));

	m_statusBar = new LedStatusBar(this);
	SetStatusBar(m_statusBar);

	m_mainSizer = new wxGridSizer(4,5,5);
	this->SetSizer(m_mainSizer);
}

void RuntimeDialog::AddChannel(wxString& name) {
	LCDDisplay *lcd = new LCDDisplay(this);
	lcd->SetLabel(name);
	lcd->SetBackgroundColour(*wxBLACK);
	lcd->SetMinSize(wxSize(100,50));
	m_currentValues[name] = lcd;
	m_mainSizer->Add(lcd, 1, wxEXPAND | wxALL);
	m_mainSizer->SetSizeHints(this);
}

void RuntimeDialog::OnRuntimeValueUpdated(wxCommandEvent &event){
	RuntimeValues *values = (RuntimeValues *)event.GetClientData();

	RuntimeValues::iterator it;
	for (it = values->begin(); it != values->end(); ++it ){
		wxString name = it->first;
		float value = it->second;
		if (m_currentValues.find(name) == m_currentValues.end()){
			AddChannel(name);
		}
		LCDDisplay *lcd = m_currentValues[name];
		lcd->SetValue(wxString::Format("%4.2f",value));
	}
	m_statusBar->GetIndicator() ? m_statusBar->SetIndicator(false) : m_statusBar->SetIndicator(true);
}

BEGIN_EVENT_TABLE(RuntimeDialog, wxFrame)
EVT_COMMAND  (RUNTIME_UPDATED, wxEVT_COMMAND_THREAD, RuntimeDialog::OnRuntimeValueUpdated)
END_EVENT_TABLE()
