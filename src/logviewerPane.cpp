#include "logviewerPane.h"
#include "logging.h"

LogviewerPane::LogviewerPane(wxWindow *parent,
			RaceAnalyzerComm * comm,
			wxWindowID id,
			const wxPoint &pos,
			const wxSize &size,
			long style,
			const wxString &name
			)
			: wxPanel(	parent,
						id,
						pos,
						size,
						style,
						name),
						m_comm(comm)
{
	InitComponents();
}

LogviewerPane::~LogviewerPane(){

}

void LogviewerPane::OnLogfileUpdated(wxCommandEvent &event){
	m_logviewTextCtrl->AppendText(event.GetString());
}

void LogviewerPane::InitComponents(){
	wxFlexGridSizer *sizer = new wxFlexGridSizer(1,1,0,0);
	sizer->AddGrowableCol(0);
	sizer->AddGrowableRow(0);
	m_logviewTextCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
	m_logviewTextCtrl->SetEditable(false);

	sizer->Add(m_logviewTextCtrl, 1, wxEXPAND);

	this->SetSizer(sizer);
	m_logReader.Create(m_comm, this);
	m_logReader.Run();
}

void LogviewerPane::OnShow(wxShowEvent &event){
	m_logReader.SetShouldReadLog(event.IsShown());
}

BEGIN_EVENT_TABLE ( LogviewerPane, wxPanel )
EVT_SHOW(LogviewerPane::OnShow)
EVT_COMMAND  (RCP_LOG_UPDATE, wxEVT_COMMAND_THREAD, LogviewerPane::OnLogfileUpdated)
END_EVENT_TABLE()
