#include "configuration/baseConfigPanel.h"

BaseConfigPanel::BaseConfigPanel() :
		wxPanel()
{}

BaseConfigPanel::BaseConfigPanel(wxWindow *parent,
								ConfigPanelParams config,
								wxWindowID id,
								const wxPoint &pos,
								const wxSize &size,
								long style,
								const wxString &name) :
						wxPanel(parent,
								id,
								pos,
								size,
								style,
								name),
						m_configParams(config)
{
}

BaseConfigPanel::~BaseConfigPanel(){

}

BEGIN_EVENT_TABLE ( BaseConfigPanel, wxPanel )
END_EVENT_TABLE()
