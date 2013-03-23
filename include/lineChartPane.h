/*
 * lineChartPane.h
 *
 *  Created on: Jun 7, 2009
 *      Author: brent
 */

#ifndef LOGVIEWER_H_
#define LOGVIEWER_H_

#include "wx/wxprec.h"
#include "lineChart.h"
#include "chartBase.h"


class LineChartPane : public wxPanel, public RaceAnalyzerChannelView, public HistoricalView {

public:
	LineChartPane(wxWindow *parent,
				ChartParams params,
				wxWindowID id = -1,
				const wxPoint &pos = wxDefaultPosition,
				const wxSize &size = wxDefaultSize,
				long style = wxTAB_TRAVERSAL,
				const wxString &name = "panel"
				);
	~LineChartPane();
	void InitComponents();
	LineChart * GetLineChart();

	void OnScroll(wxScrollEvent &event);
	void ConfigureChart(DatalogChannelSelectionSet *selectionSet);

	//from RaceAnalyzerChannelView
	void SetBufferSize(ViewChannels &channelNames, size_t size, int offset);
	void UpdateValueRange(ViewDataHistoryArray &historyArray, size_t fromIndex, size_t toIndex);
	void UpdateValue(ViewChannel &channel, size_t index, double value);
	void ScrollLineChart(int thumbPosition);
	void SetOffset(ViewChannels &channels, int offset);

private:
	ChartColors m_chartColors;
	wxColor GetNextChartColor();
	static size_t m_currentColorIndex;
	wxScrollBar *m_scrollBar;
	LineChart 	*m_lineChart;
	ChartParams	m_chartParams;

	DECLARE_EVENT_TABLE()
};

#endif /* LOGVIEWER_H_ */
