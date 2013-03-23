/*
 * analogGaugePane.h
 *
 *  Created on: 2009-07-20
 *      Author: brent
 */

#ifndef ANALOGGAUGEPANE_H_
#define ANALOGGAUGEPANE_H_

#include "wx/wxprec.h"
#include "lineChart.h"
#include "chartBase.h"
#include "AngularMeter.h"

class AnalogGaugePane : public wxPanel, public RaceAnalyzerChannelView {

public:
	AnalogGaugePane(wxWindow *parent,
				ChartParams chartParams,
				wxWindowID id = -1,
				const wxPoint &pos = wxDefaultPosition,
				const wxSize &size = wxDefaultSize,
				long style = wxTAB_TRAVERSAL,
				const wxString &name = "panel"
				);
	~AnalogGaugePane();
	void InitComponents();

	void CreateGauge(ViewChannel &channel);

	//from RaceAnalyzerChannelView
	void UpdateValue(ViewChannel &channel, size_t index, double value);


private:
	ChartParams			m_chartParams;
	AngularMeter 		*m_angularMeter;
	DatalogStoreRows 	m_channelData;
	int					m_dataOffset;
	ViewChannel			m_channel;

	DECLARE_EVENT_TABLE()
};

#endif /* ANALOGGAUGEPANE_H_ */
