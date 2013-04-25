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
#define DEFAULT_ZOOM 1
#define ZOOM_ADJUST 1.1


WX_DEFINE_OBJARRAY(GPSPoints);

GPSView::GPSView(wxWindow *parent, wxWindowID id,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name)
    : wxWindow(parent, id, pos, size,style,name)
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
	m_zoom = DEFAULT_ZOOM;
    }

GPSView::~GPSView(){}



inline double GPSView::SCALE(double point, double min, double max, double currentSize, double zoom){

	double scale = ((point - min) / (max - min)) * (currentSize * zoom);
	scale += (currentSize - (currentSize * zoom)) / 2;
	return scale;
}

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

	int currentSize, xCenter, yCenter;
	if (_currentWidth > _currentHeight){
		currentSize = _currentHeight;
		yCenter = 0;
		xCenter = (_currentWidth - _currentHeight) / 2;
	}
	else{
		currentSize = _currentWidth;
		xCenter = 0;
		yCenter = (_currentHeight - _currentWidth) / 2;
	}

    for (int i = 0; i < pointCount; i++){
		GPSPoint p = m_gpsPoints[i];
		if (p.x != 0 && p.y != 0){
			if (lastX == 0 && lastY == 0){
				lastX = SCALE(p.x, m_minX, m_maxX, currentSize, m_zoom) + xCenter;
				lastY = SCALE(p.y, m_minY, m_maxY, currentSize, m_zoom) + yCenter;
			}
			else{
				double x = SCALE(p.x, m_minX, m_maxX, currentSize, m_zoom) + xCenter;
				double y = SCALE(p.y, m_minY, m_maxY, currentSize, m_zoom) + yCenter;

				dc.DrawLine(lastX, lastY, x, y);
				lastX = x;
				lastY = y;
			}
		}
    }


    dc.SetPen(*wxThePenList->FindOrCreatePen(*wxRED, 10, wxSOLID));
    double markerX = SCALE(m_marker.x, m_minX, m_maxX, _currentWidth, m_zoom) + xCenter;
    double markerY = SCALE(m_marker.y, m_minY, m_maxY, _currentHeight, m_zoom) + yCenter;

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
	m_minX = 0;
	m_maxX = 0;
	m_minY = 0;
	m_maxY = 0;
	m_marker.x = 0;
	m_marker.y = 0;
	Refresh();
}

void GPSView::OnMouseWheel(wxMouseEvent &event){

	int rotation = event.GetWheelRotation();
	if (rotation > 0){
		m_zoom *= ZOOM_ADJUST;
	}
	else{
		m_zoom /= ZOOM_ADJUST;
	}
	Refresh();
}

void GPSView::AddGPSPoints(GPSPoints &p){

    std::vector<double> m_xSortedPoints;
    std::vector<double> m_ySortedPoints;

    for (size_t i = 0; i < p.Count(); i++){
    	m_xSortedPoints.push_back(p[i].x);
    	m_ySortedPoints.push_back(p[i].y);
    	m_gpsPoints.Add(p[i]);
    }

	std::sort(m_xSortedPoints.begin(), m_xSortedPoints.end());
	std::sort(m_ySortedPoints.begin(), m_ySortedPoints.end());

	double xUbound = m_xSortedPoints.size() - 1;
	double yUbound = m_ySortedPoints.size() - 1;

	int maxPercentileIndex_x = (int)(xUbound * POINT_MAX_PERCENTILE / 100);
	int minPercentileIndex_x = (int)(xUbound * POINT_MIN_PERCENTILE / 100);

	int maxPercentileIndex_y = (int)(yUbound * POINT_MAX_PERCENTILE / 100);
	int minPercentileIndex_y = (int)(yUbound * POINT_MIN_PERCENTILE / 100);

	m_minX = m_xSortedPoints[minPercentileIndex_x];
	m_maxX = m_xSortedPoints[maxPercentileIndex_x];
	m_minY = m_ySortedPoints[minPercentileIndex_y];
	m_maxY = m_ySortedPoints[maxPercentileIndex_y];

	Refresh();
}

BEGIN_EVENT_TABLE(GPSView, wxWindow)
    EVT_SIZE(GPSView::OnSize)
    EVT_PAINT(GPSView::OnPaint)
    EVT_ENTER_WINDOW(GPSView::OnEnterWindow)
    EVT_ERASE_BACKGROUND(GPSView::OnEraseBackground)
    EVT_MOUSEWHEEL(GPSView::OnMouseWheel)
END_EVENT_TABLE()

