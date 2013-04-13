/*
 * datalogData.h
 *
 *  Created on: Jun 10, 2009
 *      Author: brent
 */

#include "wx/wxprec.h"
#include <limits>
#include <wx/dynarray.h>
#include "datalogStore.h"

#ifndef DATALOGDATA_H_
#define DATALOGDATA_H_

class ViewChannel{
public:
	ViewChannel();
	ViewChannel(int datalogId, wxString &channelName);
	bool operator==(const ViewChannel &rhs);
	wxString ToString();
	int datalogId;
	wxString channelName;
};

WX_DECLARE_OBJARRAY(ViewChannel, ViewChannels);

class DatalogInfo{
public:
	DatalogInfo();
	DatalogInfo(int timeOffset, int maxSampleRate, const wxString &name, const wxString &notes);
	int timeOffset;
	int maxSampleRate;
	wxString name;
	wxString notes;
};

class DatalogValue{
public:
	DatalogValue(double value): value(value){}
	const static double NULL_VALUE = -0xFFFFFFFFFFFFFFF;
	double value;
};

WX_DEFINE_ARRAY_DOUBLE(double, RowValues);

class DatastoreRow{

public:

	DatastoreRow() : timePoint(0)
	{}

	DatastoreRow(int tp, RowValues v) : timePoint(tp), values(v)
	{}
	~DatastoreRow(){ }

	int timePoint;
	RowValues values;
};

WX_DECLARE_OBJARRAY(DatastoreRow, DatalogStoreRows);


class DatalogChannelType{

public:
	static const int DEFAULT_PRECISION = 2;
	static const int DEFAULT_MIN = -1000;
	static const int DEFAULT_MAX = 1000;
	static const int DEFAULT_SMOOTHING_LEVEL = 0;
	DatalogChannelType();
	DatalogChannelType(wxString newName, wxString newUnitsLabel, int newSmoothingLevel, double newMinValue, double newMaxValue, size_t precision);
	bool operator==(const DatalogChannelType &rh){
		return (name == rh.name);
	}
	wxString name;
	wxString unitsLabel;
	int smoothingLevel;
	double minValue;
	double maxValue;
	size_t precision;
};

class DatalogChannelSystemTypes {

public:
	static DatalogChannelType GetLatitudeChannelType(){ return DatalogChannelType("Latitude", "Degrees", 0, -90.0, 90.0, 5);}
	static DatalogChannelType GetLongitudeChannelType(){ return DatalogChannelType ("Longitude", "Degrees", 0, -180.0, 180.0, 5);}
};


class DatalogChannel{

public:
	DatalogChannel(wxString newName, wxString newType = "Value", wxString newDescription = "", int newSampleRate = 0, bool en = true);
	DatalogChannel();

	wxString name;
	wxString type;
	wxString description;
	int sampleRate;
	bool enabled;
};

WX_DECLARE_STRING_HASH_MAP(DatalogChannel, DatalogChannels );
WX_DECLARE_STRING_HASH_MAP(DatalogChannelType, DatalogChannelTypes );

WX_DECLARE_STRING_HASH_MAP( int, NameIndexMap);

class DatalogSnapshot{
public:
	DatalogSnapshot() : datalogId(0), offset(0){}

	int					datalogId;
	int					offset;
	DatalogInfo 		datalogInfo;
	DatalogStoreRows 	rows;
	DatalogChannels 	channels;
	wxArrayString		rowChannelNames;
	NameIndexMap 		rowChannelColumns;

	DatastoreRow & GetRow(size_t index);
};

WX_DECLARE_OBJARRAY(DatalogSnapshot, DatalogSnapshots);

#endif /* DATALOGDATA_H_ */
