/*
 * runtimeReader.h
 *
 *  Created on: Jun 23, 2012
 *      Author: brent
 */

#ifndef DATALOGPLAYER_H_
#define DATALOGPLAYER_H_

#include "wx/wxprec.h"
#include "chartBase.h"
#include "datalogData.h"

class DatalogPlayerListener {

public:
	virtual void OnDatalogTick(int datalogIndex, int tickDuration, size_t maxDatalogSize) = 0;

};

class DatalogPlayer : public wxThread {

public:
	const static int MAX_PLAYBACK_MULTIPLIER = 64;
	DatalogPlayer();
	~DatalogPlayer();
	void PlayFwd(void);
	void PlayRev(void);
	void RequeryAll(void);
	void Requery(int datalogId, DatalogSnapshots &rowCollection);
	void AddView(RaceAnalyzerChannelView *view);
	void InitView(RaceAnalyzerChannelView *view, DatalogSnapshot &snapshot);
	void Pause();
	void SeekAbsPercent(double offset);
	void SkipFwd();
	void SkipRev();
	void SeekFwd();
	void SeekRev();
	void AdjustOffset(int datalogId, int offset);
	void StopPlayback();
	void SetPlaybackMultiplierIndex(int index);
	int GetPlaybackMultiplierIndex();
	void UpdateDataHistory(HistoricalView *view, ViewChannels &channels, size_t fromIndex, size_t toIndex);
	void DatalogSessionsUpdated(void);
	void SetPlayerListener(DatalogPlayerListener *listener);

	void Create(DatalogStore *datalogStore, RaceAnalyzerChannelViews *views);
	void * Entry();

private:
	DatalogSnapshot * GetDatalogSnapshot(int datalogId);
	void Tick(size_t offset);

	void UpdateTimeListener(size_t index);

	int m_sessionsStale;
	int m_datalogIndex;
	int m_multiplierIndex;
	int m_maxSampleRate;
	size_t m_maxDatalogRowCount;

	DatalogStore *m_datalogStore;
	RaceAnalyzerChannelViews * m_views;
	wxSemaphore * m_shouldPlay;
	DatalogPlayerListener * m_playerListener;

	DatalogSnapshots  m_datalogSnapshots;
	wxArrayInt m_datalogIds;

};


#endif /* RUNTIMEREADER_H_ */
