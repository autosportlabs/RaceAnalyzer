/*
 * datalogData.cpp
 *
 *  Created on: Jun 10, 2009
 *      Author: brent
 */
#include "datalogData.h"
#include "wx/arrstr.h"
#include <wx/arrimpl.cpp> // this is a magic incantation which must be done!

WX_DEFINE_OBJARRAY(DatalogStoreRows);
WX_DEFINE_OBJARRAY(ViewChannels);
WX_DEFINE_OBJARRAY(DatalogSnapshots);

DatastoreRow & DatalogSnapshot::GetRow(size_t index){
	int adjustedIndex = index + offset;
	int rowCount = rows.Count();
	if (adjustedIndex < 0 ) adjustedIndex = 0;
	else if (adjustedIndex >= rowCount) adjustedIndex = rowCount - 1;
	return rows[adjustedIndex];
}

DatalogChannelType::DatalogChannelType() :
	name("Value"), unitsLabel(""), smoothingLevel(DEFAULT_SMOOTHING_LEVEL), minValue(DEFAULT_MIN), maxValue(DEFAULT_MAX), precision(DEFAULT_PRECISION)
{}

DatalogChannelType::DatalogChannelType(wxString newName, wxString newUnitsLabel, int newSmoothingLevel, double newMinValue, double newMaxValue, size_t precision) :
	name(newName), unitsLabel(newUnitsLabel), smoothingLevel(newSmoothingLevel), minValue(newMinValue), maxValue(newMaxValue), precision(precision)
{ }

DatalogChannel::DatalogChannel(wxString newName, wxString newType, wxString newDescription, int newSampleRate, bool en) :
	name(newName), type(newType), description(newDescription), sampleRate(newSampleRate), enabled(en)
{ }

DatalogChannel::DatalogChannel() :
	name(""), type("Value"), description(""), sampleRate(0), enabled(true)
{ }
