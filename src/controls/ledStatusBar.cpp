#include "controls/ledStatusBar.h""

LedStatusBar::LedStatusBar(wxWindow *parent)
           : wxStatusBar(parent, wxID_ANY)
{

    SetFieldsCount(3);
    _statusWidths[0] = -1;
    _statusWidths[1] = -1;
    _statusWidths[2] = 100;
    SetStatusWidths(3, _statusWidths);
    InitComponents();
}

LedStatusBar::~LedStatusBar()
{
	if (_updateTimer.IsRunning()) _updateTimer.Stop();
}

void LedStatusBar::SetIndicator(bool enabled){
	m_indicator->Enable(enabled);
}

bool LedStatusBar::GetIndicator(){
	return m_indicator->IsEnabled();
}

void LedStatusBar::OnSize(wxSizeEvent& event)
{

	wxSize size = GetSize();
	int width = size.GetWidth();
	int height = size.GetHeight();

	wxSize ledSize = m_indicator->GetSize();
	int ledWidth = ledSize.GetWidth();
	int ledHeight = ledSize.GetHeight();

	int vCenter = (height - ledHeight) / 2;
	int hPos = width - ledWidth - 15;

	m_indicator->Move(hPos,vCenter);
	hPos-=ledWidth + 3;

	_statusWidths[2] = (width-hPos) + 5;
	SetStatusWidths(3, _statusWidths);

    event.Skip();
}

void LedStatusBar::InitComponents(){

	m_indicator = new wxLed(this, -1,"005000");
	m_indicator->SetToolTip("Activity");
	m_indicator->SetColor("00FF00");
}

BEGIN_EVENT_TABLE ( LedStatusBar, wxStatusBar )
  	EVT_SIZE	(LedStatusBar::OnSize)
END_EVENT_TABLE()

