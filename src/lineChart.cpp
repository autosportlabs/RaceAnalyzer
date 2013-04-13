/*
 * lineChart.cpp
 *
 *  Created on: May 24, 2009
 *      Author: brent
 */
#include "lineChart.h"
#include "logging.h"

#define UNITS_LABEL_SPACING 		2
#define GRID_SIZE 					20
#define DEFAULT_ZOOM 				100
#define MIN_ZOOM					25
#define DEFAULT_OFFSET_SECONDS		0
#define DEFAULT_MIN_VALUE			0
#define DEFAULT_MAX_VALUE			100
#define CURRENT_VALUES_TOP_OFFSET 	5
#define CURRENT_VALUES_RIGHT_OFFSET	5

BEGIN_EVENT_TABLE( LineChart, wxWindow )
	EVT_PAINT( LineChart::OnPaint )
	EVT_SIZE( LineChart::OnSize )
	EVT_MOTION(LineChart::OnMouseMove)
	EVT_ENTER_WINDOW(LineChart::OnMouseEnter)
	EVT_LEAVE_WINDOW(LineChart::OnMouseExit)
    EVT_ERASE_BACKGROUND(LineChart::OnEraseBackground)
    EVT_LEFT_DCLICK(LineChart::OnMouseDoubleClick)
END_EVENT_TABLE()

#include <wx/arrimpl.cpp> // this is a magic incantation which must be done!
WX_DEFINE_OBJARRAY(RangeArray);

Series::Series(size_t bufferSize, int rangeId, size_t offset, wxString label, wxColor color, int precision) :
	m_rangeId(rangeId),
	m_offset(offset),
	m_label(label),
	m_color(color),
	m_precision(precision)
	{
	SetBufferSize(bufferSize);
	}

SeriesValues * Series::GetSeriesValues(){return &m_seriesValues;}

int Series::GetRangeId(){ return m_rangeId; }

void Series::SetRangeId(int seriesId){m_rangeId = seriesId; }

int Series::GetOffset(){ return m_offset; }

void Series::SetOffset(int offset){ m_offset = offset; }

wxString & Series::GetLabel(){ return m_label; }

void Series::SetLabel(wxString label){ m_label = label; }

void Series::SetColor(wxColor color){m_color = color; }

wxColor Series::GetColor(){return m_color;}

int Series::GetPrecision(){return m_precision;}

void Series::SetPrecision(int precision){m_precision = precision;}

void Series::SetBufferSize(size_t size){
	size_t currentCount = m_seriesValues.Count();
	while (currentCount < size){
		m_seriesValues.Add(NULL_VALUE);
		currentCount++;
	}
	while (currentCount > size){
		m_seriesValues.RemoveAt(m_seriesValues.Count() - 1);
		currentCount--;
	}
}

size_t Series::GetBufferSize(){
	return m_seriesValues.Count();
}

double Series::GetValueAtOrNear(size_t index){
	double value = NULL_VALUE;
	size_t lookDistance = 0;
	while (NULL_VALUE == value && lookDistance < DEFAULT_LOOK_DISTANCE){
		value = GetValueAt(index - lookDistance);
		lookDistance++;
	}
	return value;
}

double Series::GetValueAt(size_t index){
	int adjustedIndex = index + GetOffset();
	if (adjustedIndex < 0 ) return NULL_VALUE;
	else if ((size_t)adjustedIndex >= m_seriesValues.Count()) return NULL_VALUE;
	return m_seriesValues[adjustedIndex];
}

void Series::SetValueAt(size_t index, double value){
	if (index >= m_seriesValues.Count()) SetBufferSize(index + 1);
	m_seriesValues[index] = value;
}

LineChart::LineChart(): wxWindow(),
	m_viewOffsetFactor(0),
	m_markerIndex(0),
	_zoomPercentage(DEFAULT_ZOOM),
	_currentWidth(0),
	_currentHeight(0),
	_memBitmap(NULL),
	_backgroundColor(*wxBLACK),
	m_showScale(true),
	m_showData(false),
	m_mouseX(0),
	m_mouseY(0),
	m_leftEdge(0)
{}

LineChart::LineChart(		wxWindow *parent,
							wxWindowID id,
							const wxPoint &pos,
							const wxSize &size)
							: wxWindow(parent, id, pos, size),
								m_viewOffsetFactor(0),
								m_markerIndex(0),
								_zoomPercentage(DEFAULT_ZOOM),
								_currentWidth(0),
								_currentHeight(0),
								_memBitmap(NULL),
								_backgroundColor(*wxBLACK),
								m_showScale(true),
								m_showData(false),
								m_mouseX(0),
								m_mouseY(0),
								m_leftEdge(0)
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
	m_leftEdge = 0;
}


LineChart::~LineChart(){
	delete (_memBitmap);
}

void LineChart::OnMouseDoubleClick(wxMouseEvent &event){
	m_showScale = !m_showScale;
	Refresh();
}

void LineChart::OnMouseEnter(wxMouseEvent &event){

	m_showData = true;
	m_mouseX = event.GetX();
	m_mouseY = event.GetY();
	Refresh();
}

void LineChart::OnMouseMove(wxMouseEvent &event){

	m_mouseX = event.GetX();
	m_mouseY = event.GetY();
	Refresh();
}

void LineChart::OnMouseExit(wxMouseEvent &event){

	m_showData = false;
	Refresh();
}

int LineChart::GetChartWidth(){
	return GetSize().GetWidth() - m_leftEdge;
}

void LineChart::SetViewOffsetFactor(double offset){
	m_viewOffsetFactor = offset;
	Refresh();
}

double LineChart::GetViewOffsetPercent(){
	return m_viewOffsetFactor;
}

void LineChart::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
  // Do nothing, to avoid flashing.
}

int LineChart::GetZoom(){
	return _zoomPercentage;
}

void LineChart::SetZoom(int zoomPercentage){
	_zoomPercentage = zoomPercentage >= MIN_ZOOM ? zoomPercentage : MIN_ZOOM;
	Refresh();
}

void LineChart::ShowScale(bool showScale){
	m_showScale = showScale;
}

bool LineChart::GetShowScale(){
	return m_showScale;
}

void LineChart::OnSize(wxSizeEvent &event){
	Refresh();
}

size_t LineChart::GetMaxSeriesBufferSize(){
	size_t maxSize = 0;
	for (SeriesMap::iterator it = m_seriesMap.begin(); it != m_seriesMap.end(); ++it){
		size_t size = it->second->GetBufferSize();
		if (size > maxSize) maxSize = size;
	}
	return maxSize;
}

void LineChart::OnPaint(wxPaintEvent &event){

	wxPaintDC old_dc(this);

	float zoomFactor = (float)_zoomPercentage / 100;

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

	wxColor backColor = GetBackgroundColour();
	dc.SetBackground(*wxTheBrushList->FindOrCreateBrush(backColor,wxSOLID));
	dc.SetBrush(*wxTheBrushList->FindOrCreateBrush(backColor,wxSOLID));
	dc.Clear();
	DrawGrid(dc);

	if (m_showScale){
		m_leftEdge = DrawScale(dc);
	}
	else{
		m_leftEdge = 0;
	}

	size_t largestBufferSize = GetMaxSeriesBufferSize();
	double lastValue = 0;
	for (SeriesMap::iterator it = m_seriesMap.begin(); it != m_seriesMap.end(); ++it){

		float currentX = (float)m_leftEdge;
		int lastX = (int)currentX;
		int lastY;

		Series *series = it->second;
		dc.SetPen(*wxThePenList->FindOrCreatePen(series->GetColor(), 1, wxSOLID));
		size_t bufSize = series->GetBufferSize();
		Range *range = m_rangeArray[series->GetRangeId()];
		if (bufSize > 0){

			double minValue = range->GetMin();
			double maxValue = range->GetMax();

			double loggedValue = series->GetValueAt(0);

			double percentageOfMax = (loggedValue - minValue) / (maxValue - minValue);
			lastY = h - (int)(((double)h) * percentageOfMax);

			size_t i = (size_t)(((double)largestBufferSize) * m_viewOffsetFactor);

			while (i < bufSize && currentX < _currentWidth ){
				if (i == m_markerIndex){
					wxPen pen = dc.GetPen();
					dc.SetPen(*wxThePenList->FindOrCreatePen(*wxLIGHT_GREY, 1, wxSOLID));
					dc.DrawLine(currentX, 0, currentX, _currentHeight);
					DrawCurrentValues(dc, i, currentX, CURRENT_VALUES_TOP_OFFSET);
					dc.SetPen(pen);
				}

				loggedValue = series->GetValueAt(i);

				if (DatalogValue::NULL_VALUE == loggedValue){
					loggedValue = lastValue;
				}
				else{
					lastValue = loggedValue;
				}

				double percentageOfMax = (loggedValue - minValue) / (maxValue - minValue);

				int y = h - (int)(((double)h) * percentageOfMax);

				dc.DrawLine(lastX, lastY, (int)currentX, y);
				lastX = (int)currentX;
				lastY = y;
				currentX += zoomFactor;
				i++;
			}
		}
	}
	if (m_showData) DrawMouseoverMarker(dc);
	//blit into the real DC
	old_dc.Blit(0,0,_currentWidth,_currentHeight,&dc,0,0);

}

size_t LineChart::GetMarkerIndex(){
	return m_markerIndex;
}

void LineChart::SetMarkerIndex(size_t index){
	m_markerIndex = index;
}


int LineChart::DrawScale(wxMemoryDC &dc){
	int leftOrientationEdge = 0;
	int rightOrientationEdge = _currentWidth - 1;

	int scaleOrientation = LineChart::ORIENTATION_LEFT;

	wxFont labelFont = GetFont();



	int tickLabelWidth = 0;
	for (SeriesMap::iterator it = m_seriesMap.begin(); it != m_seriesMap.end(); ++it){

		int maxLabelWidth = 0;
		Series *series = it->second;
		Range * range = m_rangeArray[series->GetRangeId()];


		double minValue = range->GetMin();
		double maxValue = range->GetMax();
		double rangeSize = maxValue - minValue;
		double stepInterval = (maxValue - minValue) / 10;
		if (stepInterval == 0) stepInterval = 1;

		dc.SetPen(*wxThePenList->FindOrCreatePen(series->GetColor(), 1, wxSOLID));

		dc.SetTextForeground(series->GetColor());

		bool labelOn = false;
		int tickLabelHeight,tickDescent,tickExternalLeading;
		dc.DrawLine(leftOrientationEdge, 0, leftOrientationEdge, _currentHeight);

		for (double tick = minValue; tick <=maxValue; tick += stepInterval){

			int y = _currentHeight - (double)_currentHeight * ((tick - minValue) / rangeSize);
			int nextY = _currentHeight - (double)_currentHeight * ((tick + stepInterval - minValue) / rangeSize);

			if (labelOn){
				wxString numberFormat = "%." + wxString::Format("%df", range->GetPrecision());
				wxString tickLabel = wxString::Format(numberFormat, tick);
				dc.GetTextExtent(tickLabel, &tickLabelHeight, &tickLabelWidth, &tickDescent, &tickExternalLeading, &labelFont);
				if (tickLabelHeight > maxLabelWidth) maxLabelWidth = tickLabelHeight;

				if (tickLabelWidth < y - nextY ){
					switch (scaleOrientation){
						case LineChart::ORIENTATION_LEFT:
						{
							dc.DrawRotatedText(tickLabel, leftOrientationEdge, y, 0);
							break;
						}
						case LineChart::ORIENTATION_RIGHT:
						{
							dc.DrawRotatedText(tickLabel, rightOrientationEdge, y, 0);
							break;
						}
					}
				}
			}
			labelOn = !labelOn;
			dc.DrawLine(leftOrientationEdge, y, leftOrientationEdge + tickLabelWidth, y);
		}

		maxLabelWidth+=(tickLabelWidth / 2);
		switch (scaleOrientation){
			case LineChart::ORIENTATION_LEFT:
			{
				leftOrientationEdge += (maxLabelWidth);
				break;
			}
			case LineChart::ORIENTATION_RIGHT:
			{
				rightOrientationEdge -= (maxLabelWidth);
				break;
			}
		}
	}
	return leftOrientationEdge;
}

void LineChart::DrawMouseoverMarker(wxMemoryDC &dc){

	dc.SetPen(*wxThePenList->FindOrCreatePen(*wxWHITE, 1, wxSOLID));
	dc.DrawLine(m_mouseX, 0, m_mouseX, _currentHeight);

	size_t dataIndex = (size_t)(((double)GetMaxSeriesBufferSize()) * m_viewOffsetFactor) + m_mouseX - m_leftEdge;
	DrawCurrentValues(dc, dataIndex, m_mouseX, m_mouseY);
}

void LineChart::DrawCurrentValues(wxMemoryDC &dc, size_t dataIndex, int x, int y){
	int currentOffset = 0;
	wxFont labelFont = GetFont();
	int labelWidth,labelHeight,descent,externalLeading;

	for (SeriesMap::iterator it = m_seriesMap.begin(); it != m_seriesMap.end(); ++it){

		Series *series = it->second;
		double dataValue = series->GetValueAtOrNear(dataIndex);
		wxString numberFormat = "% 2." + wxString::Format("%df", series->GetPrecision());
		wxString valueString = (DatalogValue::NULL_VALUE == dataValue ? "---" : wxString::Format(numberFormat.ToAscii(), dataValue)) + " " + series->GetLabel();
		dc.SetTextForeground(series->GetColor());
		dc.GetTextExtent(valueString, &labelHeight, &labelWidth, &descent, &externalLeading, &labelFont);

		dc.DrawRotatedText(valueString, x + CURRENT_VALUES_RIGHT_OFFSET, y + currentOffset,0);
		currentOffset += labelWidth;
	}
}

void LineChart::DrawGrid(wxMemoryDC &dc){

	dc.SetPen(*wxThePenList->FindOrCreatePen(wxColor(40,40,40), 1, wxSOLID));

	int width = _currentWidth;
	int height = _currentHeight;

	float zoomFactor = (float)_zoomPercentage / 100;

	int gridIncrement = (int)(GRID_SIZE * zoomFactor);

	for (int x = width; x >=0 ; x -= gridIncrement){
		dc.DrawLine(x, 0, x, height);
	}

	float i = 0;
	while (i < 1){
		int y = (int)(((float)height) * i);
		dc.DrawLine(0, y, width, y);
		i = i + 0.1;
	}
}

int LineChart::AddRange(Range *range){
	m_rangeArray.Add(range);
	return m_rangeArray.size() - 1;
}

void LineChart::ClearAllSeries(){

	for (SeriesMap::iterator it = m_seriesMap.begin(); it != m_seriesMap.end(); ++it){
		delete it->second;
	}
	m_seriesMap.clear();
}

void LineChart::AddSeries(wxString channel, Series *series){
	m_seriesMap[channel] = series;
}

Series * LineChart::GetSeries(wxString channel){
	SeriesMap::iterator it = m_seriesMap.find(channel);
	if (it != m_seriesMap.end()){
		return it->second;
	}
	else{
		return NULL;
	}
}

