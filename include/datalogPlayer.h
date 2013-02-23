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

class DatalogPlayer : public wxThread {

public:
	const static int MAX_PLAYBACK_MULTIPLIER = 25;
	DatalogPlayer();
	~DatalogPlayer();
	void PlayFwd(void);
	void PlayRev(void);
	void RequeryAll(void);
	void Requery(int datalogId, DatalogSnapshots &rowCollection);
	void AddView(RaceAnalyzerChannelView *view);
	void InitView(RaceAnalyzerChannelView *view, DatalogSnapshot &snapshot);
	void Pause();
	void SkipFwd();
	void SkipRev();
	void SeekFwd();
	void SeekRev();
	void StopPlayback();
	void SetPlaybackMultiplier(int multiplier);
	int GetPlaybackMultiplier();
	void UpdateDataHistory(HistoricalView *view, ViewChannels &channels, size_t fromIndex, size_t toIndex);
	void DatalogSessionsUpdated(void);

	void Create(DatalogStore *datalogStore, RaceAnalyzerChannelViews *views);
	void * Entry();

private:
	DatalogSnapshot * GetDatalogSnapshot(int datalogId);
	void Tick(size_t offset);

	int m_shouldReloadSessions;
	int m_offset;
	int m_multiplier;
	int m_maxSampleRate;
	size_t m_maxDatalogRowCount;

	wxArrayInt m_datalogIds;
	DatalogStore *m_datalogStore;
	DatalogSnapshots  m_datalogSnapshots;
	RaceAnalyzerChannelViews * m_views;
	wxSemaphore * m_shouldPlay;


};


#endif /* RUNTIMEREADER_H_ */
