/*
 * chartBase.cpp
 *
 *  Created on: 2009-07-07
 *      Author: brent
 */
#include "chartBase.h"

#include <wx/arrimpl.cpp> // this is a magic incantation which must be done!

WX_DEFINE_OBJARRAY(RaceAnalyzerChannelViews);

WX_DEFINE_OBJARRAY(ViewDataHistoryArray);

DEFINE_EVENT_TYPE ( REQUEST_DATALOG_DATA_EVENT )

ViewChannel::ViewChannel() : datalogId(0), channelName(""){}

ViewChannel::ViewChannel(int datalogId, wxString &channelName): datalogId(datalogId), channelName(channelName){}

bool ViewChannel::operator==(const ViewChannel &rhs){
	return (rhs.channelName == channelName && rhs.datalogId == datalogId);
	}

wxString ViewChannel::ToString(){
	return wxString::Format("%s_%d", channelName.ToAscii(), datalogId);
}

