/*
 * datalogChannelsPanel.h
 *
 *  Created on: Jun 9, 2009
 *      Author: brent
 */

#ifndef DATALOGCHANNELSPANEL_H_
#define DATALOGCHANNELSPANEL_H_

#include "wx/wxprec.h"
#include "controls/treelistctrl.h"
#include "wx/notebook.h"
#include "wx/slider.h"
#include "datalogStore.h"
#include "commonEvents.h"
#include "appOptions.h"
#include "appPrefs.h"
#include "raceCapture/raceCaptureConfig.h"
#include "LCDWindow.h"
#include "datalogPlayer.h"

class ChannelNode : public wxTreeItemData{
public:
	ChannelNode(int datalogId, wxString &channelName) : wxTreeItemData(), datalogId(datalogId), channelName(channelName) {}
	int datalogId;
	wxString channelName;
};

class DatalogChannelsParams{
public:
	DatalogChannelsParams(
			RaceCaptureConfig *config,
			AppPrefs *prefs,
			AppOptions *options,
			DatalogStore *store) :
				raceCaptureConfig(config),
				appPrefs(prefs),
				appOptions(options),
				datalogStore(store)
	{}

	RaceCaptureConfig *raceCaptureConfig;
	AppPrefs *appPrefs;
	AppOptions *appOptions;
	DatalogStore *datalogStore;
};

class DatalogChannelsPanel : public wxPanel, public DatalogPlayerListener{

	public:
		DatalogChannelsPanel(DatalogChannelsParams params,
					wxWindow *parent,
					wxWindowID id = -1,
					const wxPoint &pos = wxDefaultPosition,
					const wxSize &size = wxDefaultSize,
					long style = wxTAB_TRAVERSAL,
					const wxString &name = "panel"
					);

		void DatalogSessionsUpdated();
		void AddDatalogSession(int id);
		void UpdateRuntimeValues();
		DatalogChannelSelectionSet * GetSelectedChannels(void);
		void SetMarkerOffset(size_t offset);
		size_t GetMarkerOffset();

		~DatalogChannelsPanel();
		//event handlers

		//From DatalogPlayerListener
		void OnDatalogTick(int datalogIndex, int tickDuration, size_t maxDatalogSize);
	private:

		wxControl * CreateTimeWidget(wxWindow *parent);
		wxControl * CreateTimeScrollbar(wxWindow *parent);
		wxSizer * CreatePlaybackControls(void);
		void UpdateTimeSlider(double factor);
		void InitComponents();
		void InitOptions();
		wxTreeListCtrl * CreateChannelsList(void);
		void PopulateSelectedChannels(DatalogChannelSelectionSet *selectionSet);
		void OnNewLineChart(wxCommandEvent &event);
		void OnNewAnalogGauge(wxCommandEvent &event);
		void OnNewDigitalGauge(wxCommandEvent &event);
		void OnNewGPSView(wxCommandEvent &event);
		void OnAddChannelView(wxCommandEvent &event);
		void DoGridContextMenu(wxTreeEvent &event);
		void OnTimeScrolled(wxScrollEvent &event);
		void OnTimeScrollRelease(wxScrollEvent &event);
		void OnOffsetChanged(wxTreeEvent &event);
		void OnPlayForward(wxCommandEvent &event);
		void OnPlayReverse(wxCommandEvent &event);
		void OnPause(wxCommandEvent &event);
		void OnSkipForward(wxCommandEvent &event);
		void OnSkipReverse(wxCommandEvent &event);
		void OnSeekForward(wxCommandEvent &event);
		void OnSeekReverse(wxCommandEvent &event);


		wxTreeListCtrl	*m_channelsList;
		size_t 			m_markerOffset;
		DatalogStore 	*m_datalogStore;
		AppOptions		*m_appOptions;
		AppPrefs		*m_appPrefs;
		RaceCaptureConfig *m_raceCaptureConfig;
		wxMenu			*m_gridPopupMenu;
		LCDDisplay		*m_timeDisplay;
		wxSlider		*m_timeSlider;

		bool			m_isScrolling;

	DECLARE_EVENT_TABLE()
};

enum{

	ID_DATALOG_CHANNELS_LIST = wxID_HIGHEST + 1,
	ID_NEW_LINE_CHART,
	ID_NEW_ANALOG_GAUGE,
	ID_NEW_DIGITAL_GAUGE,
	ID_NEW_GPS_VIEW,
	ID_ADD_CHANNEL,
	ID_SKIP_DATALOG_REV,
	ID_SEEK_DATALOG_REV,
	ID_PLAY_DATALOG_REV,
	ID_PAUSE_DATALOG,
	ID_PLAY_DATALOG_FWD,
	ID_SEEK_DATALOG_FWD,
	ID_SKIP_DATALOG_FWD,
	ID_TIME_SCROLLER,
	ID_JUMP_BEGINNING_DATALOG,
	ID_JUMP_END_DATALOG

};



#endif /* DATALOGCHANNELSPANEL_H_ */
