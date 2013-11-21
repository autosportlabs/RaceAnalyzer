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
	    Point ZoomPoint(Point &point, Point &centerPoint, Point &minPoint, Point &maxPoint, double currentSize, double zoom);
	    double Zoom(double point, double min, double max, double currentSize, double zoom);
	    void OnPaint(wxPaintEvent& event);
	    void ClearGPSPoints();
	    void UpdateMinMax(Points &points);
	    void AddGPSPoints(GPSPoints &point);
	    void OnEraseBackground(wxEraseEvent& event);
	    void OnEnterWindow( wxMouseEvent& event );
	    void OnMouseWheel( wxMouseEvent& event );
	    virtual void Refresh( bool eraseBackground = true, const wxRect *rect = (const wxRect *) NULL );
	    void RefreshBackground();
	    void SetMarker(GPSPoint &p);
	    Point GetMarker();
private:

	    int GetDominantSize(int width, int height);
	    Point GetCenterPoint(int width, int height);
	    Point ProjectPoint(GPSPoint &gpsPoint);
	    Point ScalePoint(Point point, int height);
	    Point OffsetPoint(Point &point);

	    Points			m_trackPoints;
	    Point			m_offsetPoint;
	    GPSPoints		m_gpsPoints;
	    Point			m_marker;
	    Point			m_minPoint;
	    Point			m_maxPoint;
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
