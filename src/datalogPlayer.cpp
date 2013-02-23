/*
 * runtimeReader.cpp
 *
 *  Created on: Jun 23, 2012
 *      Author: brent
 */
#include "datalogPlayer.h"
#include "raceCapture/raceCaptureConfig.h"

DatalogPlayer::DatalogPlayer() : m_shouldReloadSessions(false), m_offset(0), m_multiplier(1), m_maxSampleRate(sample_1Hz), m_maxDatalogRowCount(0), m_datalogStore(NULL), m_views(NULL){
	m_shouldPlay = new wxSemaphore(0,1);
}

DatalogPlayer::~DatalogPlayer(){
	delete(m_shouldPlay);
}

void DatalogPlayer::DatalogSessionsUpdated(void){
	m_shouldReloadSessions = true;
}

void DatalogPlayer::PlayFwd(void){
	if (m_shouldReloadSessions) RequeryAll();
	SetPlaybackMultiplier(1);
	m_shouldPlay->Post();
}

void DatalogPlayer::PlayRev(void){
	if (m_shouldReloadSessions) RequeryAll();
	SetPlaybackMultiplier(-1);
	m_shouldPlay->Post();
}

void DatalogPlayer::Pause(){
	m_shouldPlay->TryWait();
}

void DatalogPlayer::SkipFwd(){
	m_offset = 0;
	Tick(m_offset);
}

void DatalogPlayer::SkipRev(){

	m_offset = m_maxDatalogRowCount -1;
	Tick(m_offset);
}

void DatalogPlayer::SeekFwd(){
	int playbackMultiplier = GetPlaybackMultiplier();
	if (playbackMultiplier < MAX_PLAYBACK_MULTIPLIER){
		SetPlaybackMultiplier(playbackMultiplier + 1);
		m_shouldPlay->Post();
	}
}

void DatalogPlayer::SeekRev(){
	int playbackMultiplier = GetPlaybackMultiplier();
	if (playbackMultiplier > -MAX_PLAYBACK_MULTIPLIER){
		SetPlaybackMultiplier(playbackMultiplier - 1);
		m_shouldPlay->Post();
	}
}

void DatalogPlayer::SetPlaybackMultiplier(int multiplier)
{
	m_multiplier = multiplier;
}

int DatalogPlayer::GetPlaybackMultiplier(){
	return m_multiplier;
}

void DatalogPlayer::Create(DatalogStore *datalogStore, RaceAnalyzerChannelViews *views )  {
	m_datalogStore = datalogStore;
	m_views = views;
  	wxThread::Create();
}

void DatalogPlayer::StopPlayback(){
	SetPlaybackMultiplier(0);
	Pause();
}

void * DatalogPlayer::Entry(){
	while (! TestDestroy()){
		m_shouldPlay->Wait();
		Tick(m_offset);
		m_shouldPlay->Post();
		wxThread::Sleep(1000 / m_maxSampleRate);

		m_offset += m_multiplier;

		int ubound = m_maxDatalogRowCount - 1;
		if (m_offset < 0 ){
			StopPlayback();
			m_offset = 0;
		}
		if (m_offset > ubound ){
			StopPlayback();
			m_offset = ubound;
		}
	}
	return NULL;
}

void DatalogPlayer::RequeryAll(void){
	m_datalogSnapshots.Clear();
	m_maxSampleRate = sample_1Hz;
	m_maxDatalogRowCount = 0;

	if (m_datalogStore->IsOpen()){
		m_datalogIds.Clear();
		m_datalogStore->ReadDatalogIds(m_datalogIds);

		for (wxArrayInt::iterator it = m_datalogIds.begin(); it != m_datalogIds.end(); ++it){
			Requery(*it, m_datalogSnapshots);
		}

		//update all views
		for (size_t i = 0; i < m_views->Count(); i++){
			for (size_t ii = 0; ii < m_datalogSnapshots.Count(); ii++){
				DatalogSnapshot &snapshot = m_datalogSnapshots[ii];
				RaceAnalyzerChannelView *view = m_views->Item(i);
				InitView(view, snapshot);
			}
		}
	}
}

void DatalogPlayer::Requery(int datalogId, DatalogSnapshots &rowsCollection){
	DatalogSnapshot newDataView;
	rowsCollection.Add(newDataView);

	DatalogSnapshot &snapshot = rowsCollection.Item(rowsCollection.Count() - 1);

	m_datalogStore->GetChannels(datalogId, snapshot.channels);
	m_datalogStore->ReadDatalogInfo(datalogId, snapshot.datalogInfo);
	snapshot.datalogId = datalogId;

	DatalogChannels &channels = snapshot.channels;
	wxArrayString channelNames;
	for (size_t i = 0; i < snapshot.channels.Count(); i++){
		channelNames.Add( channels[i].name );
	}
	m_datalogStore->ReadDatalog(snapshot.rows, datalogId, channelNames, 0);

	size_t rowCount = snapshot.rows.Count();
	if (rowCount > m_maxDatalogRowCount) m_maxDatalogRowCount = rowCount;

	int maxSampleRate = snapshot.datalogInfo.maxSampleRate;
	if (maxSampleRate > m_maxSampleRate) m_maxSampleRate = maxSampleRate;
}

void DatalogPlayer::AddView(RaceAnalyzerChannelView *view){
	for (size_t i = 0; i < m_datalogSnapshots.Count(); i++){
		DatalogSnapshot &snapshot = m_datalogSnapshots[i];
		InitView(view, snapshot);
	}
}

void DatalogPlayer::InitView(RaceAnalyzerChannelView *view, DatalogSnapshot &snapshot){

	ViewChannels channels;
	DatalogChannels &dlChannels = snapshot.channels;
	for (size_t ii = 0; ii < dlChannels.Count(); ii++){
		ViewChannel channel(snapshot.datalogId, dlChannels[ii].name);
		channels.Add(channel);
	}

	HistoricalView *hv = dynamic_cast<HistoricalView *>(view);
	if (NULL != hv) hv->SetBufferSize(channels, snapshot.rows.Count());

}

DatalogSnapshot * DatalogPlayer::GetDatalogSnapshot(int datalogId){
	DatalogSnapshot * snapshot = NULL;
	for (int i = 0; i < m_datalogSnapshots.Count();i++){
		DatalogSnapshot &testSnapshot = m_datalogSnapshots[i];
		if (testSnapshot.datalogId == datalogId){
			snapshot = &testSnapshot;
			break;
		}
	}
	return snapshot;
}

void DatalogPlayer::UpdateDataHistory(HistoricalView *view, ViewChannels &channels, size_t fromIndex, size_t toIndex){

	ViewDataHistoryArray viewDataHistoryArray;

	for (size_t viewIndex = 0; viewIndex < channels.Count(); viewIndex++){
		ChartValues values;
		values.Alloc(toIndex - fromIndex);
		ViewChannel &viewChannel = channels[viewIndex];
		DatalogSnapshot * snapshot = GetDatalogSnapshot(viewChannel.datalogId);
		if (NULL != snapshot){
			DatalogChannels &channels = snapshot->channels;
			int channelId = DatalogChannelUtil::FindChannelIdByName(channels, viewChannel.channelName);
			if ( channelId >= 0){
				for (size_t i = fromIndex; i < toIndex; i++){
					DatastoreRow row = snapshot->rows[i - fromIndex];
					double value = row.values[channelId];
					values.Add(value);
				}
				ViewDataHistory history(viewChannel, values);
				viewDataHistoryArray.Add(history);
			}
		}
	}
	if (NULL != view) view->UpdateValueRange(viewDataHistoryArray, fromIndex, toIndex);
}

void DatalogPlayer::Tick(size_t index){
	for (size_t i = 0; i < m_datalogSnapshots.Count(); i++){
		DatalogSnapshot &storeSnapshot = m_datalogSnapshots[i];
		DatalogStoreRows &rows = storeSnapshot.rows;
		if (index < rows.Count()){
			DatastoreRow &row = rows[index];
			RowValues &values = row.values;
			size_t valuesCount = values.Count();
			DatalogChannels &channels = storeSnapshot.channels;
			for (size_t i = 0; i < m_views->Count(); i++){
				for (size_t ii = 0; ii < valuesCount; ii++){
					wxString &channel = channels[ii].name;
					double value = row.values[ii];
					ViewChannel vc(storeSnapshot.datalogId, channel);
					(*m_views)[i]->UpdateValue(vc, index, value);
				}
			}
		}
	}
}




