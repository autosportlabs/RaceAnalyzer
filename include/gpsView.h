/*
 * gpsView.h
 *
 *  Created on: 2009-07-22
 *      Author: brent
 */

#ifndef GPSVIEW_H_
#define GPSVIEW_H_
#include <wx/wx.h>
#include <wx/dynarray.h>
#include <wx/arrimpl.cpp> // this is a magic incantation which must be done!
#include <vector>
#include <algorithm>

using std::vector;

#define POINT_SCALING	100000

class Point{
public:
	Point() : x(0), y(0){}
	Point(double newX, double newY) : x(newX), y(newY) {}
	double x;
	double y;
};

class GPSPoint{
public:
	GPSPoint() :
		x(0),y(0)
		{}
	GPSPoint(double newX, double newY) :
		x(newX), y(newY)
		{}
	double x;
	double y;
};

WX_DECLARE_OBJARRAY(GPSPoint, GPSPoints);
WX_DECLARE_OBJARRAY(Point, Points);
//WX_DEFINE_SORTED_ARRAY(double *, SortedPoints);

class GPSView : public wxWindow
{
public:

	 GPSView( wxWindow *parent, wxWindowID id = wxID_ANY,
	        const wxPoint& pos = wxDefaultPosition,
	        const wxSize& size = wxDefaultSize,
	        long style = 0, const wxString& name = _T("GPSView") );

	    ~GPSView();
	    void OnSize(wxSizeEvent& event);
	    double SCALE(double point, double min, double max, double currentSize, double zoom);
	    void OnPaint(wxPaintEvent& event);
	    void ClearGPSPoints();
	    void UpdateMinMax(Points &points);
	    void AddGPSPoints(GPSPoints &point);
	    void OnEraseBackground(wxEraseEvent& event);
	    void OnEnterWindow( wxMouseEvent& event );
	    void OnMouseWheel( wxMouseEvent& event );
	    void SetMarker(GPSPoint &p);
	    GPSPoint GetMarker();
private:

	    Point ProjectPoint(GPSPoint &gpsPoint);
	    Point ScalePoint(Point point, int height);

	    Points			m_trackPoints;
	    Point			m_offsetPoint;
	    GPSPoints		m_gpsPoints;
	    GPSPoint		m_marker;
	    double			m_minX;
	    double			m_maxX;
	    double			m_minY;
	    double			m_maxY;
	    double 			m_zoom;
	    double			m_widthPadding;
	    double			m_heightPadding;
	    double			m_globalRatio;

		int				_currentWidth;
		int				_currentHeight;
		wxBitmap 		*_memBitmap;
	    DECLARE_EVENT_TABLE()

};



#endif /* GPSVIEW_H_ */
