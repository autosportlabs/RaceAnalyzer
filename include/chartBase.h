/*
 * chartBase.h
 *
 *  Created on: 2009-07-07
 *      Author: brent
 */
#include "wx/wxprec.h"
#include <wx/dynarray.h>
#include "datalogStore.h"
#include "datalogData.h"
#include "appOptions.h"
#include "appPrefs.h"
#include "commonEvents.h"
#include <wx/dynarray.h>

#ifndef CHARTBASE_H_
#define CHARTBASE_H_

DECLARE_EVENT_TYPE ( REQUEST_DATALOG_DATA_EVENT, -1 )


WX_DEFINE_ARRAY_DOUBLE(double, ChartValues);


class ChartParams{

public:
	ChartParams() :
		appPrefs(NULL),
		appOptions(NULL)
		{}

	ChartParams(
			AppPrefs *prefs,
			AppOptions *options
			):
		appPrefs(prefs),
		appOptions(options)
		{

		}

	AppPrefs 					*appPrefs;
	AppOptions					*appOptions;
};


class ViewDataHistory {

public:

	ViewDataHistory(ViewChannel channel, ChartValues values): channel(channel), values(values) {}

	ViewChannel channel;
	ChartValues values;
};

WX_DECLARE_OBJARRAY(ViewDataHistory, ViewDataHistoryArray);

class HistoricalView{
public:
	virtual void SetBufferSize(ViewChannels &channels, size_t size, int offset) = 0;
	virtual void UpdateValueRange(ViewDataHistoryArray &historyArray, size_t fromIndex, size_t toIndex) = 0;
	virtual void SetOffset(ViewChannels &channels, int offset) = 0;
};



class RaceAnalyzerChannelView{

public:
	virtual void UpdateValue(ViewChannel &channelName, size_t index, double value) = 0;
};

WX_DECLARE_OBJARRAY(RaceAnalyzerChannelView *,RaceAnalyzerChannelViews);

class RequestDatalogRangeParams {
public:

	RequestDatalogRangeParams(): view(NULL), fromIndex(0), toIndex(0){}
	RequestDatalogRangeParams(HistoricalView *view, ViewChannels channels, size_t fromIndex, size_t toIndex): view(view), channels(channels), fromIndex(fromIndex), toIndex(toIndex){}

	HistoricalView *view;
	ViewChannels channels;
	size_t fromIndex;
	size_t toIndex;
};

enum{
   ID_LOGVIEWER_SCROLL			= wxID_HIGHEST + 1,
   ID_REQUEST_DATALOG_DATA
};


#endif /* CHARTBASE_H_ */
