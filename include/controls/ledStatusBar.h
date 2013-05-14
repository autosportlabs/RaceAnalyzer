/*
 * ledStatusBar.h
 *
 *  Created on: May 13, 2013
 *      Author: brent
 */

#ifndef LEDSTATUSBAR_H_
#define LEDSTATUSBAR_H_

#include <wx/wx.h>
#include <wx/timer.h>
#include "LED.h"

#define STATUS_UPDATE_INTERVAL 75


class LedStatusBar : public wxStatusBar
{
public:
    LedStatusBar(wxWindow *parent);
    virtual ~LedStatusBar();

    void OnSize(wxSizeEvent& event);
    void SetIndicator(bool enabled);
    bool GetIndicator();

	private:

	void InitComponents();

	wxTimer				_updateTimer;

	wxLed				*m_indicator;
    int _statusWidths[3];

	DECLARE_EVENT_TABLE()

enum{
	STATUS_TIMER_UPDATE = wxID_HIGHEST + 1
	};

};


#endif /* LEDSTATUSBAR_H_ */
