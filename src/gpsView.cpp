/*
 * gpsView.cpp
 *
 *  Created on: 2009-07-22
 *      Author: brent
 */

#include "gpsView.h"

#include "logging.h"
#include <iostream>
#include <math.h>
#include <algorithm>

#define POINT_MIN_PERCENTILE 1
#define POINT_MAX_PERCENTILE 99
#define DEFAULT_ZOOM 1
#define ZOOM_ADJUST 1.1
#define PI  3.141592653589793238462
#define MIN_PADDING 50

WX_DEFINE_OBJARRAY(GPSPoints);
WX_DEFINE_OBJARRAY(Points);

GPSView::GPSView(wxWindow *parent, wxWindowID id,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name)
    : wxWindow(parent, id, pos, size,style,name)
    {
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
	ClearGPSPoints();
    }

GPSView::~GPSView() {}

Point GPSView::ZoomPoint(Point &point, Point &centerPoint, Point &minPoint, Point &maxPoint, double currentSize, double zoom){
	Point zoomedPoint;
	zoomedPoint.x = Zoom(point.x, minPoint.x, maxPoint.x, currentSize, zoom) + centerPoint.x;
	zoomedPoint.y = Zoom(point.y, minPoint.y, maxPoint.y, currentSize, zoom) + centerPoint.y;
	return zoomedPoint;
}


double GPSView::Zoom(double point, double min, double max, double currentSize, double zoom){
	double scale = ((point - min) / (max - min)) * (currentSize * zoom);
	scale += (currentSize - (currentSize * zoom)) / 2;
	return scale;
}

Point GPSView::ScalePoint(Point point, int height){
	double adjustedX = (m_widthPadding + (point.x * m_globalRatio));
	// need to invert the Y since 0,0 starts at top left
	double adjustedY = ((double)height - m_heightPadding - (point.y * m_globalRatio));
	return Point(adjustedX, adjustedY);
}

int GPSView::GetDominantSize(int width, int height){
	if (width > height) return height; else return width;
}

Point GPSView::GetCenterPoint(int width, int height){
	Point centerPoint;
	int currentSize;
	if (width > height){
		centerPoint.y = 0;
		centerPoint.x = (width - height) / 2;
	}
	else{
		centerPoint.x = 0;
		centerPoint.y = (height - width) / 2;
	}
	return centerPoint;
}

void GPSView::RefreshBackground(){

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

	wxColor backColor = *wxBLACK;
	dc.SetBackground(*wxTheBrushList->FindOrCreateBrush(backColor,wxSOLID));
	dc.SetBrush(*wxTheBrushList->FindOrCreateBrush(backColor,wxSOLID));
	dc.Clear();

	dc.SetPen(*wxThePenList->FindOrCreatePen(*wxLIGHT_GREY, 1, wxSOLID));

	Point centerPoint = GetCenterPoint(_currentWidth, _currentHeight);
	int currentSize = GetDominantSize(_currentWidth, _currentHeight);

	Point minPoint = ScalePoint(m_minPoint, h);
	Point maxPoint = ScalePoint(m_maxPoint, h);

	Point lastZoomedPoint;
	for (size_t i = 0; i < m_trackPoints.size(); i++){
		Point point = m_trackPoints[i];
		Point offsetPoint = OffsetPoint(point);
		Point scaledPoint = ScalePoint(offsetPoint, h);
		Point zoomedPoint = ZoomPoint(scaledPoint, centerPoint, minPoint, maxPoint, currentSize, m_zoom);
		if (0 == i){
			lastZoomedPoint = zoomedPoint;
		}
		else{
			dc.DrawLine(lastZoomedPoint.x, lastZoomedPoint.y, zoomedPoint.x, zoomedPoint.y);
			lastZoomedPoint = zoomedPoint;
		}
	}
    Refresh();
}

void GPSView::OnPaint( wxPaintEvent& event )
{
	wxPaintDC mainDc(this);

	wxMemoryDC dc;
	dc.SelectObject(*_memBitmap);

	//blit into the real DC
	mainDc.Blit(0,0,_currentWidth,_currentHeight,&dc,0,0);

	Point centerPoint = GetCenterPoint(_currentWidth, _currentHeight);
	int currentSize = GetDominantSize(_currentWidth, _currentHeight);
	Point minPoint = ScalePoint(m_minPoint, _currentHeight);
	Point maxPoint = ScalePoint(m_maxPoint, _currentHeight);

    mainDc.SetPen(*wxThePenList->FindOrCreatePen(*wxRED, 10, wxSOLID));
	mainDc.SetBrush(*wxTheBrushList->FindOrCreateBrush(*wxRED,wxSOLID));
    Point offsetMarker = OffsetPoint(m_marker);
    Point scaledMarker = ScalePoint(offsetMarker, _currentHeight);
    Point zoomedPoint = ZoomPoint(scaledMarker, centerPoint, minPoint, maxPoint, currentSize, m_zoom);
    mainDc.DrawCircle(wxPoint(zoomedPoint.x, zoomedPoint.y), 4);
}


void GPSView::Refresh( bool eraseBackground, const wxRect *rect )
{
	wxWindow::Refresh(eraseBackground, rect);
}

void GPSView::OnSize(wxSizeEvent& event)
{
	RefreshBackground();
}

Point GPSView::GetMarker()
{
	return m_marker;
}

void GPSView::SetMarker(GPSPoint &p)
{
	m_marker = ProjectPoint(p);
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
	RefreshBackground();
	Refresh();
}

Point GPSView::OffsetPoint(Point &point){
	Point offsetPoint;
    offsetPoint.x = point.x - m_offsetPoint.x;
    offsetPoint.y = point.y - m_offsetPoint.y;
    return offsetPoint;
}

Point GPSView::ProjectPoint(GPSPoint &gpsPoint){
    double latitude = gpsPoint.y * PI / 180.0;
    double longitude = gpsPoint.x * PI / 180.0;

	Point point;
	point.x = longitude;
	point.y = log(tan((PI / 4.0) + 0.5 * latitude));
	return point;
}

void GPSView::AddGPSPoints(GPSPoints &p){

	int width;
	int height;
	GetClientSize(&width, &height);

	Point minXY(-1, -1);
	Point maxXY(-1, -1);

	for (size_t i = 0; i < p.Count(); i++){
		Point point = ProjectPoint(p[i]);
		minXY.x = ((minXY.x == -1) ? point.x : std::min(minXY.x, point.x));
		minXY.y = ((minXY.y == -1) ? point.y : std::min(minXY.y, point.y));
		m_trackPoints.Add(point);
	}

	for (size_t i = 0; i < m_trackPoints.size(); i++){
		Point point = m_trackPoints[i];
		point.x = point.x - minXY.x;
        point.y = point.y - minXY.y;
        // now, we need to keep track the max X and Y values
        maxXY.x = (maxXY.x == -1) ? point.x : std::max(maxXY.x, point.x);
        maxXY.y = (maxXY.y == -1) ? point.y : std::max(maxXY.y, point.y);
	}

	int paddingBothSides = MIN_PADDING * 2;

	// the actual drawing space for the map on the image
    int mapWidth = width - paddingBothSides;
    int mapHeight = height - paddingBothSides;

    // determine the width and height ratio because we need to magnify the map to fit into the given image dimension
    double mapWidthRatio = mapWidth / maxXY.x;
    double mapHeightRatio = mapHeight / maxXY.y;

    // using different ratios for width and height will cause the map to be stretched. So, we have to determine
    // the global ratio that will perfectly fit into the given image dimension
    m_globalRatio = std::min(mapWidthRatio, mapHeightRatio);

    // now we need to readjust the padding to ensure the map is always drawn on the center of the given image dimension
    m_heightPadding = (height - (m_globalRatio * maxXY.y)) / 2;
    m_widthPadding = (width - (m_globalRatio * maxXY.x)) / 2;
    m_offsetPoint = minXY;
    UpdateMinMax(m_trackPoints);
    RefreshBackground();
}

void GPSView::UpdateMinMax(Points &p){
    std::vector<double> m_xSortedPoints;
    std::vector<double> m_ySortedPoints;

    for (size_t i = 0; i < p.Count(); i++){
    	m_xSortedPoints.push_back(p[i].x);
    	m_ySortedPoints.push_back(p[i].y);
    }

	std::sort(m_xSortedPoints.begin(), m_xSortedPoints.end());
	std::sort(m_ySortedPoints.begin(), m_ySortedPoints.end());

	double xUbound = m_xSortedPoints.size() - 1;
	double yUbound = m_ySortedPoints.size() - 1;

	int maxPercentileIndex_x = (int)(xUbound * POINT_MAX_PERCENTILE / 100);
	int minPercentileIndex_x = (int)(xUbound * POINT_MIN_PERCENTILE / 100);

	int maxPercentileIndex_y = (int)(yUbound * POINT_MAX_PERCENTILE / 100);
	int minPercentileIndex_y = (int)(yUbound * POINT_MIN_PERCENTILE / 100);

	m_minPoint.x = m_xSortedPoints[minPercentileIndex_x];
	m_maxPoint.x = m_xSortedPoints[maxPercentileIndex_x];
	m_minPoint.y = m_ySortedPoints[minPercentileIndex_y];
	m_maxPoint.y = m_ySortedPoints[maxPercentileIndex_y];

	m_minPoint = OffsetPoint(m_minPoint);
	m_maxPoint = OffsetPoint(m_maxPoint);
}

BEGIN_EVENT_TABLE(GPSView, wxWindow)
    EVT_SIZE(GPSView::OnSize)
    EVT_PAINT(GPSView::OnPaint)
    EVT_ENTER_WINDOW(GPSView::OnEnterWindow)
    EVT_ERASE_BACKGROUND(GPSView::OnEraseBackground)
    EVT_MOUSEWHEEL(GPSView::OnMouseWheel)
END_EVENT_TABLE()

