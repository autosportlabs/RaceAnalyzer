/*
 * gpsPanel.h
 *
 *  Created on: 2009-07-22
 *      Author: brent
 */

#ifndef GPSPANEL_H_
#define GPSPANEL_H_

#include "wx/wxprec.h"
#include "gpsView.h"
#include "chartBase.h"


class GPSPane : public wxPanel, public RaceAnalyzerChannelView, public HistoricalView {

public:
	GPSPane(wxWindow *parent,
				ChartParams chartParams,
				wxWindowID id = -1,
				const wxPoint &pos = wxDefaultPosition,
				const wxSize &size = wxDefaultSize,
				long style = wxTAB_TRAVERSAL,
				const wxString &name = "panel"
				);
	~GPSPane();
	void InitComponents();
	GPSPoint ProjectPoint(double latitude, double longitude);

	void CreateGPSView(ViewChannel &latitudeChannel, ViewChannel &longitudeChannel);
	void ClearGPSPoints();
	void UpdateValueRange(ViewDataHistoryArray &historyArray, size_t fromIndex, size_t toIndex);

	void SetBufferSize(ViewChannels &channels, size_t size, int offset);

	void UpdateValue(ViewChannel &channel, size_t index, double value);
	void SetOffset(ViewChannels &channels, int offset);

private:
	void AddGPSPoint(double latitude, double longitude);


	static const size_t		LATITUDE_INDEX = 0;
	static const size_t		LONGITUDE_INDEX = 1;

	ChartParams			m_chartParams;
	double				m_currentLatitude;
	double 				m_currentLongitude;

	GPSView				*m_gpsView;
	DatalogStoreRows 	m_channelData;

	ViewChannel			m_longitudeChannel;
	ViewChannel			m_latitudeChannel;
	DECLARE_EVENT_TABLE()
};



#endif /* GPSPANEL_H_ */
