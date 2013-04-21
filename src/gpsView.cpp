/*
 * gpsView.cpp
 *
 *  Created on: 2009-07-22
 *      Author: brent
 */

#include "gpsView.h"

#include "logging.h"
#include <iostream>

#define POINT_MIN_PERCENTILE 1
#define POINT_MAX_PERCENTILE 99

WX_DEFINE_OBJARRAY(GPSPoints);


static int DoubleCompare(double *n1, double *n2){
	return *n1 < *n2;
}

GPSView::GPSView(wxWindow *parent, wxWindowID id,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name)
    : wxWindow(parent, id, pos, size,style,name), m_xSortedPoints(DoubleCompare), m_ySortedPoints(DoubleCompare)
    {
	ClearGPSPoints();

	if (parent){
		SetBackgroundColour(parent->GetBackgroundColour());
	}
	else{
		SetBackgroundColour(*wxBLACK);
	}
	_currentWidth = size.GetWidth();
	_currentHeight = size.GetHeight();
	_memBitmap = new wxBitmap(_currentWidth, _currentHeight);

    }

GPSView::~GPSView(){}

#define SCALE(P, MIN, MAX, VIEWSIZE) ((P - MIN) / (MAX - MIN)) * VIEWSIZE

void GPSView::OnPaint( wxPaintEvent& event )
{

	wxPaintDC old_dc(this);

	/////////////////
	// Create a memory DC
	/////////////////
	int w,h ;
	GetClientSize(&w,&h);

	if (w != _currentWidth || h != _currentHeight){
		delete (_memBitmap);
		_currentWidth = w;
		_currentHeight = h;
		_memBitmap = new wxBitmap(_currentWidth, _currentHeight);
	}

	/////////////////
	// Create a memory DC
	wxMemoryDC dc;
	dc.SelectObject(*_memBitmap);

	wxColor backColor = *wxBLACK; //GetBackgroundColour();
	dc.SetBackground(*wxTheBrushList->FindOrCreateBrush(backColor,wxSOLID));
	dc.SetBrush(*wxTheBrushList->FindOrCreateBrush(backColor,wxSOLID));
	dc.Clear();

	dc.SetPen(*wxThePenList->FindOrCreatePen(*wxLIGHT_GREY, 1, wxSOLID));

	double lastX = 0;
	double lastY = 0;
	int pointCount = m_gpsPoints.size();

    for (int i = 0; i < pointCount; i++){
		GPSPoint p = m_gpsPoints[i];
		if (p.x != 0 && p.y != 0){
			if (lastX == 0 && lastY == 0){
				lastX = SCALE(p.x, m_minX, m_maxX, _currentWidth);
				lastY = SCALE(p.y, m_minY, m_maxY, _currentHeight);
			}
			else{
				double x = SCALE(p.x, m_minX, m_maxX, _currentWidth);
				double y = SCALE(p.y, m_minY, m_maxY, _currentHeight);

				dc.DrawLine(lastX, lastY, x, y);
				lastX = x;
				lastY = y;
			}
		}
    }


    dc.SetPen(*wxThePenList->FindOrCreatePen(*wxRED, 10, wxSOLID));
    double markerX = SCALE(m_marker.x, m_minX, m_maxX, _currentWidth);
    double markerY = SCALE(m_marker.y, m_minY, m_maxY, _currentHeight);

    dc.DrawCircle(wxPoint(markerX, markerY), 4);

	//blit into the real DC
	old_dc.Blit(0,0,_currentWidth,_currentHeight,&dc,0,0);
}

void GPSView::OnSize(wxSizeEvent& event)
{
	Refresh();
}

GPSPoint GPSView::GetMarker()
{
	return m_marker;
}

void GPSView::SetMarker(GPSPoint &p)
{
	m_marker.x = p.x;
	m_marker.y = p.y;
	Refresh();
}


void GPSView::OnEnterWindow( wxMouseEvent& event )
{
    SetFocus();
    event.Skip();
}

void GPSView::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
  // Do nothing, to avoid flashing.
}

void GPSView::ClearGPSPoints(){
	m_gpsPoints.Clear();
	m_xSortedPoints.Clear();
	m_ySortedPoints.Clear();
	m_minX = 0;
	m_maxX = 0;
	m_minY = 0;
	m_maxY = 0;
	m_marker.x = 0;
	m_marker.y = 0;
	Refresh();
}

void GPSView::AddGPSPoint(GPSPoint &p){
	m_gpsPoints.Add(p);
	UpdateMinMax(p);
	Refresh();
}

void GPSView::UpdateMinMax(GPSPoint &p){
	double *newX = (double*)malloc(sizeof(double));
	double *newY = (double*)malloc(sizeof(double));

	*newX = p.x;
	*newY = p.y;

	m_xSortedPoints.Add(newX);
	m_ySortedPoints.Add(newY);

	size_t xCount = m_xSortedPoints.Count();
	size_t yCount = m_ySortedPoints.Count();

	size_t maxPercentileIndex_x = xCount * POINT_MAX_PERCENTILE / 100;
	size_t minPercentileIndex_x = xCount * POINT_MIN_PERCENTILE / 100;

	size_t maxPercentileIndex_y = yCount * POINT_MAX_PERCENTILE / 100;
	size_t minPercentileIndex_y = yCount * POINT_MIN_PERCENTILE / 100;

	m_minX = *m_xSortedPoints[minPercentileIndex_x];
	m_maxX = *m_xSortedPoints[maxPercentileIndex_x];

	m_minY = *m_ySortedPoints[minPercentileIndex_y];
	m_maxY = *m_ySortedPoints[maxPercentileIndex_y];

	VERBOSE(FMT("point: %f, %f -- X index(min/max): %d %d -- Y index(min/max): %d %d -- %f %f -- %f %f",
			*newX,
			*newY,
			minPercentileIndex_x,
			maxPercentileIndex_x,
			minPercentileIndex_y,
			maxPercentileIndex_y,
			m_minX,
			m_maxX,
			m_minY,
			m_maxY
			));


}

BEGIN_EVENT_TABLE(GPSView, wxWindow)
    EVT_SIZE(GPSView::OnSize)
    EVT_PAINT(GPSView::OnPaint)
    EVT_ENTER_WINDOW(GPSView::OnEnterWindow)
    EVT_ERASE_BACKGROUND(GPSView::OnEraseBackground)
END_EVENT_TABLE()

