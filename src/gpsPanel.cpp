/*
 * gpsPanel.cpp
 *
 *  Created on: 2009-07-22
 *      Author: brent
 */
#include "gpsPanel.h"
#include "logging.h"


GPSPane::GPSPane(wxWindow *parent,
			ChartParams chartParams,
			wxWindowID id,
			const wxPoint &pos,
			const wxSize &size,
			long style,
			const wxString &name
			)
			:
			  wxPanel(	parent,
						id,
						pos,
						size,
						style,
						name),
			m_chartParams(chartParams),
			m_currentLatitude(0),
			m_currentLongitude(0)

{
	InitComponents();
}

GPSPane::~GPSPane(){}

void GPSPane::CreateGPSView(ViewChannel &latitudeChannel, ViewChannel &longitudeChannel){
	m_latitudeChannel = latitudeChannel;
	m_longitudeChannel = longitudeChannel;
	ClearGPSPoints();
}

void GPSPane::SetBufferSize(ViewChannels &channels, size_t size, int offset){

	wxCommandEvent addEvent(REQUEST_DATALOG_DATA_EVENT, ID_REQUEST_DATALOG_DATA);
	RequestDatalogRangeParams *params = new RequestDatalogRangeParams(this, channels, 0, size - 1);
	addEvent.SetClientData(params);
	GetParent()->GetEventHandler()->AddPendingEvent(addEvent);
}

void GPSPane::UpdateValueRange(ViewDataHistoryArray &historyArray, size_t fromIndex, size_t toIndex){

	ChartValues *latitudeValues = NULL;
	ChartValues *longitudeValues = NULL;

	for (size_t i = 0; i < historyArray.Count(); i++){
		ViewDataHistory &history = historyArray[i];
		if (history.channel == m_latitudeChannel){
			latitudeValues = &history.values;
		}
		else if(history.channel == m_longitudeChannel){
			longitudeValues = &history.values;
		}
	}

	if (latitudeValues && longitudeValues){
		ClearGPSPoints();
		for (size_t i = fromIndex; i < toIndex; i++){
			AddGPSPoint((*latitudeValues)[i], (*longitudeValues)[i]);
		}
	}
}


void GPSPane::ClearGPSPoints(){
	m_gpsView->ClearGPSPoints();
}



void GPSPane::AddGPSPoint(double latitude, double longitude){
	m_gpsView->AddGPSPoint(ProjectPoint(latitude,longitude));
}


GPSPoint GPSPane::ProjectPoint(double latitude, double longitude){

	if (latitude == DatalogValue::NULL_VALUE && DatalogValue::NULL_VALUE) return GPSPoint();

	double pi = 3.14159;
	// convert lat/long to radians
	latitude = pi * latitude / 180;
	longitude = pi * longitude / 180;

	// adjust position by radians
	latitude -= 1.570795765134; // subtract 90 degrees (in radians)

	double world_radius = 180;
	// and switch z and y
	double x = (world_radius) * sin(latitude) * cos(longitude);
	double z = (world_radius) * sin(latitude) * sin(longitude);
	double y = (world_radius) * cos(latitude);

	double focal_length = 150;

	double projected_x = x * focal_length / (focal_length + z);
	double projected_y = y * focal_length / (focal_length + z);

	projected_x = - projected_x;
	projected_y = - projected_y;
	return GPSPoint(projected_x, projected_y, 0, 0, 0);
}

void GPSPane::InitComponents(){

	wxFlexGridSizer *sizer = new wxFlexGridSizer(1,1,0,0);
	sizer->AddGrowableCol(0);
	sizer->AddGrowableRow(0);
	m_gpsView = new GPSView(this,-1);
	m_gpsView->SetMinSize(wxSize(400,400));
	sizer->Add(m_gpsView,1,wxEXPAND);
	this->SetSizer(sizer);
}


void GPSPane::UpdateValue(ViewChannel &channel, size_t index, double value){
	if (channel == m_latitudeChannel){
		m_currentLatitude = value;
	}
	if (channel == m_longitudeChannel){
		m_currentLongitude = value;
	}
	m_gpsView->SetMarker(ProjectPoint(m_currentLatitude, m_currentLongitude));
}

void GPSPane::SetOffset(ViewChannels &channels, int offset){

}

BEGIN_EVENT_TABLE ( GPSPane , wxPanel )
END_EVENT_TABLE()

