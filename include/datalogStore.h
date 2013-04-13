/*
 * datalogStore.h
 *
 *  Created on: May 19, 2009
 *      Author: brent
 */

#ifndef DATALOGSTORE_H_
#define DATALOGSTORE_H_

#include "wx/wxprec.h"
#include "sqlite3.h"
#include "exceptions.h"
#include "wx/ffile.h"
#include "logging.h"
#include "wx/dynarray.h"
#include "datalogData.h"


class DatalogImportProgressListener {

public:
	virtual void UpdateProgress(int completePercent) = 0;

protected:
	~DatalogImportProgressListener(){}

};

class DatastoreException : public RuntimeException{
public:
	DatastoreException(wxString msg, int errorCode):RuntimeException(msg),m_errorCode(errorCode) {
		VERBOSE(FMT("DatastoreException thrown- code %d; message: %s",GetErrorCode(),GetMessage().ToAscii()));
	}
	int GetErrorCode(){return m_errorCode;}

private:
	int m_errorCode;
};

class DatalogHeader{
public:
	DatalogHeader();
	DatalogHeader(wxString &channelName, wxString &units, int sampleRate);
	static bool ParseRawHeader(wxString &rawHeader, DatalogHeader &header);
	wxString channelName;
	wxString units;
	int sampleRate;
};


WX_DECLARE_OBJARRAY(DatalogHeader, DatalogHeaders);

class DatalogHeaderUtil{
public:
	static int FindMaxSampleRate(DatalogHeaders &datalogHeaders);
};

class DatalogStore {

public:

	static const int TIMEPOINT_REST = -1;

	DatalogStore();
	~DatalogStore();
	void Open(wxString filePath);
	void Close();
	void CreateNew(wxString filePath);
	wxString GetFileName();
	bool IsOpen();
	int ImportDatalog(const wxString &filePath, const wxString &name, const wxString &notes, DatalogChannels &channels, DatalogChannelTypes &channelTypes, DatalogImportProgressListener *progressListener = NULL);

	size_t CountFileLines(wxFFile &file);
	void ReadDatalogInfo(int datalogId, DatalogInfo &info);
	void ReadChannels(DatalogChannels &channels);
	void ReadChannelTypes(DatalogChannelTypes &channelTypes);
	void ReadDatalog(DatalogStoreRows &data, int datalogId, wxArrayString &names, int fromTimePoint, int toTimepoint = TIMEPOINT_REST);
	void ReadDatalogIds(wxArrayInt &data);

	void GetChannelNames(wxArrayString &channelNames);

	void ClearChannelTypes();
	int GetChannelTypeId(wxString &type);
	wxString GetChannelTypeName(int id);
	void GetChannelType(wxString &type, DatalogChannelType &channelType);
	void GetChannelTypes(DatalogChannelTypes &channelTypes);

	void ClearChannels();
	void GetAllChannels(DatalogChannels &channels);
	void GetChannel(int datalogId, wxString &channelName, DatalogChannel &channel);
	void GetChannels(int datalogId, DatalogChannels &channels);

	void ImportChannels(DatalogChannels &channels);
	void ImportChannelTypes(DatalogChannelTypes &channelTypes);
	void ImportDatalogChannelMap(int datalogId, DatalogChannels &datalogChannels);
	void GetDatalogHeaders(DatalogHeaders &headers, wxFFile &file);
	int GetTopDatalogId();


private:

	void SetFilename(wxString name);
	void CreateTables();
	size_t ReadLine(wxString &buffer, wxFFile &file);
	void StripQuotes(wxString &values);
	size_t ExtractValues(wxArrayString &valueList, wxString &line, wxArrayInt *selectedColumns = NULL);

	int InsertDatalogInfo(const DatalogInfo &info);
	void InsertDatalogRow(sqlite3_stmt *query, int id, int timePoint, wxArrayString &values);
	void AddDatalogChannel(int channelId, DatalogChannel &channel);
	bool DatalogColumnExists(wxString &name);
	sqlite3_stmt * CreateDatalogInsertPreparedStatement(DatalogHeaders &headers, wxArrayInt &selectedColumns);
	int GetTopTimePoint();
	int GetTopId(const char *sql);
	int GetTopChannelTypesId();
	int GetTopChannelId();

	bool m_isOpen;
	sqlite3 *m_db;
	wxString m_datastoreName;


};

#endif /* DATALOGSTORE_H_ */
