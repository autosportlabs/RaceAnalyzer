/*
 * datalogChannelsPanel.cpp
 *
 *  Created on: Jun 9, 2009
 *      Author: brent
 */

#include "datalogChannelsPanel.h"
#include "addChannelWizardDialog.h"
#include "list_add.xpm"
#include "media_play_forward.xpm"
#include "media_play_backward.xpm"
#include "media_stop.xpm"
#include "media_pause.xpm"
#include "media_seek_forward.xpm"
#include "media_seek_backward.xpm"
#include "media_skip_backward.xpm"
#include "media_skip_forward.xpm"



#define UNITS_WIDTH 	"XXXXXXXXXX"
#define TYPE_WIDTH 		"XXXXXXXXXX"
#define MIN_MAX_WIDTH 	"XXXXXXXXXXXX"
#define OFFSET_WIDTH 	"XXXXXX"
#define PICK_RANGE_MIN INT_MIN
#define PICK_RANGE_MAX INT_MAX


DatalogChannelsPanel::DatalogChannelsPanel(DatalogChannelsParams params,
			wxWindow *parent,
			wxWindowID id,
			const wxPoint &pos,
			const wxSize &size,
			long style,
			const wxString &name
			)
			: wxPanel(	parent,
						id,
						pos,
						size,
						style,
						name),
						m_markerOffset(0)
{
	m_datalogStore = params.datalogStore;
	m_appOptions = params.appOptions;
	m_appPrefs = params.appPrefs;
	m_raceCaptureConfig = params.raceCaptureConfig;
	InitComponents();
}

DatalogChannelsPanel::~DatalogChannelsPanel(){

}

void DatalogChannelsPanel::UpdateTimeSlider(double factor){
	double value = (double)m_timeSlider->GetMax() * factor;
	m_timeSlider->SetValue(value);
}

void DatalogChannelsPanel::OnDatalogTick(int datalogIndex, int tickDuration, size_t maxDatalogSize){

	size_t totalMilliseconds = datalogIndex * tickDuration;
	size_t totalSeconds = totalMilliseconds / 1000;
	size_t totalMinutes = totalSeconds / 60;
	size_t totalHours = totalMinutes / 60;

	size_t remainingMinutes = totalMinutes % 60;
	size_t remainingSeconds = totalSeconds % 60;
	size_t remainingMilliseconds = totalMilliseconds % 1000;

	wxString value = wxString::Format("%02lu:%02lu:%02lu.%03lu", totalHours, remainingMinutes, remainingSeconds, remainingMilliseconds);
	m_timeDisplay->SetValue(value);

	UpdateTimeSlider((double)datalogIndex / (double)maxDatalogSize);
}

wxWindow * DatalogChannelsPanel::CreateTimeWidget(wxWindow *parent){

	m_timeDisplay = new LCDDisplay(parent);
	m_timeDisplay->SetGrayColour(wxColor(10,51,56));
	m_timeDisplay->SetLightColour(wxColor(57,232,255));
	m_timeDisplay->SetNumberDigits(11);
	m_timeDisplay->SetValue("00:00:00.000");
	m_timeDisplay->SetMinSize(wxSize(200,40));
	return m_timeDisplay;

}

wxControl * DatalogChannelsPanel::CreateTimeScrollbar(wxWindow *parent){

	//0.01% resolution range on slider
	m_timeSlider = new wxSlider(parent, ID_TIME_SCROLLER, 0, 0, 10000, wxDefaultPosition, wxDefaultSize);//, wxSL_TOP | wxSL_AUTOTICKS);
	m_timeSlider->SetTickFreq(1000);
	return m_timeSlider;
}

wxSizer * DatalogChannelsPanel::CreatePlaybackControls(void){
	wxFlexGridSizer *sizer = new wxFlexGridSizer(2,1,2,2);
	sizer->AddGrowableCol(0);

	//initialize tool bar
	wxToolBar* toolBar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL | wxTB_FLAT | wxTB_NODIVIDER);

	//toolBar->AddTool(ID_ADD_CHANNEL, "", list_add_xpm, wxT("Add Channel View"));

	toolBar->AddTool(ID_SKIP_DATALOG_REV, "", media_skip_backward_xpm, 	"Skip datalog to beginning");
	toolBar->AddTool(ID_SEEK_DATALOG_REV, "", media_seek_backward_xpm, 	"Seek datalog reverse");
	toolBar->AddTool(ID_PLAY_DATALOG_REV, "", media_play_backward_xpm, 	"Play datalog reverse");
	toolBar->AddTool(ID_PAUSE_DATALOG,	  "", media_pause_xpm, 			"Pause datalog");
	toolBar->AddTool(ID_PLAY_DATALOG_FWD, "", media_play_forward_xpm, 	"Play datalog forward");
	toolBar->AddTool(ID_SEEK_DATALOG_FWD, "", media_seek_forward_xpm, 	"Seek datalog forward");
	toolBar->AddTool(ID_SKIP_DATALOG_FWD, "", media_skip_forward_xpm, 	"Skip datalog to end");
	toolBar->Realize();

	wxFlexGridSizer *innerSizer = new wxFlexGridSizer(1,3,2,2);

	innerSizer->Add(toolBar);
	innerSizer->Add(CreateTimeWidget(this));

	sizer->Add(innerSizer, 1, wxEXPAND);
	sizer->Add(CreateTimeScrollbar(this),1,wxEXPAND | wxALIGN_CENTER_VERTICAL);
	return sizer;
}

void DatalogChannelsPanel::InitComponents(){

	wxFlexGridSizer *sizer = new wxFlexGridSizer(2,1,3,3);
	sizer->AddGrowableCol(0);
	sizer->AddGrowableRow(1);


	sizer->Add(CreatePlaybackControls(),1,wxEXPAND);
	m_channelsList = CreateChannelsList();

	sizer->Add(m_channelsList, 1, wxEXPAND);
	this->SetSizer(sizer);

	m_gridPopupMenu = new wxMenu();
	m_gridPopupMenu->Append(ID_NEW_LINE_CHART,"New Line Chart");
	m_gridPopupMenu->Append(ID_NEW_ANALOG_GAUGE,"New Analog Gauge");
	m_gridPopupMenu->Append(ID_NEW_DIGITAL_GAUGE, "New Digital Gauge");
	m_gridPopupMenu->Append(ID_NEW_GPS_VIEW, "New GPS View");
}

wxTreeListCtrl * DatalogChannelsPanel::CreateChannelsList(void){

	wxTreeListCtrl *channelsList = new wxTreeListCtrl(this, ID_DATALOG_CHANNELS_LIST, wxDefaultPosition, wxDefaultSize, wxTR_HIDE_ROOT | wxTR_ROW_LINES | wxTR_HAS_BUTTONS | wxTR_HAS_VARIABLE_ROW_HEIGHT | wxTR_FULL_ROW_HIGHLIGHT | wxTR_MULTIPLE | wxTR_EDIT_LABELS);


	channelsList->AddColumn("Max"); //, channelsList->WidthFor(MIN_MAX_WIDTH), wxALIGN_LEFT, wxCOL_RESIZABLE | wxCOL_SORTABLE);
	channelsList->InsertColumn(0,"Min"); //, channelsList->WidthFor(MIN_MAX_WIDTH), wxALIGN_LEFT, wxCOL_RESIZABLE | wxCOL_SORTABLE);
	channelsList->InsertColumn(0,"Units"); //,channelsList->WidthFor(UNITS_WIDTH), wxALIGN_LEFT, wxCOL_RESIZABLE | wxCOL_SORTABLE);
	channelsList->InsertColumn(0,"Time Offset",DEFAULT_COL_WIDTH,wxALIGN_LEFT, -1, true, true ); //, channelsList->WidthFor(OFFSET_WIDTH), wxALIGN_LEFT, wxCOL_RESIZABLE | wxCOL_SORTABLE);
	channelsList->InsertColumn(0,"Type"); //, channelsList->WidthFor(TYPE_WIDTH), wxALIGN_LEFT, wxCOL_RESIZABLE | wxCOL_SORTABLE);
	channelsList->SetColumnPickType(1,wxTR_COLUMN_SPIN);
	channelsList->SetColumnPickRange(1, PICK_RANGE_MIN, PICK_RANGE_MAX);
	return channelsList;
}

void DatalogChannelsPanel::InitOptions(){

}

void DatalogChannelsPanel::AddDatalogSession(int datalogId){

	wxTreeItemId root = m_channelsList->GetRootItem();

	DatalogInfo datalogInfo;
	m_datalogStore->ReadDatalogInfo(datalogId, datalogInfo);

	wxTreeItemId session = m_channelsList->AppendItem(root, datalogInfo.name);

	m_channelsList->SetItemText(session, 1, wxString::Format("%d", datalogInfo.timeOffset));
	m_channelsList->SetItemBold(session, true);
	DatalogChannels channels;
	m_datalogStore->GetChannels(datalogId, channels);

	DatalogChannelTypes channelTypes;
	m_datalogStore->GetChannelTypes(channelTypes);

	wxArrayString sortedChannelNames;
	for (DatalogChannels::iterator it = channels.begin(); it != channels.end(); ++it){
		sortedChannelNames.Add(it->second.name);
	}
	sortedChannelNames.Sort();

	for (size_t i = 0; i < sortedChannelNames.Count(); i++){
		DatalogChannel &channel = channels[sortedChannelNames[i]];
		wxTreeItemId channelItem = m_channelsList->AppendItem(session, channel.name);
		m_channelsList->SetItemData(session, new ChannelNode(datalogId, channel.name));

		DatalogChannelTypes::iterator it2 = channelTypes.find(channel.type);

		if (it2 != channelTypes.end()){
			DatalogChannelType &type = it2->second;
			m_channelsList->SetItemData(channelItem, new ChannelNode(datalogId, channel.name));
			m_channelsList->SetItemText(channelItem, 2, type.unitsLabel);
			m_channelsList->SetItemText(channelItem, 3, wxString::Format("%.2f", type.minValue));
			m_channelsList->SetItemText(channelItem, 4, wxString::Format("%.2f", type.maxValue));
		}
	}
	UpdateRuntimeValues();
}


void DatalogChannelsPanel::SetMarkerOffset(size_t offset){
	m_markerOffset = offset;
	UpdateRuntimeValues();
}


size_t DatalogChannelsPanel::GetMarkerOffset(){
	return m_markerOffset;
}

void DatalogChannelsPanel::UpdateRuntimeValues(){

}

void DatalogChannelsPanel::DatalogSessionsUpdated(){

	if (m_datalogStore->IsOpen()){
		m_channelsList->DeleteRoot();
		m_channelsList->AddRoot("root");
		wxArrayInt datalogIds;
		m_datalogStore->ReadDatalogIds(datalogIds);

		for (size_t i = 0; i < datalogIds.size(); i++){
			AddDatalogSession(datalogIds[i]);
		}
	}
}

DatalogChannelSelectionSet * DatalogChannelsPanel::GetSelectedChannels(void){
	DatalogChannelSelectionSet *selectionSet = new DatalogChannelSelectionSet();
	PopulateSelectedChannels(selectionSet);
	return selectionSet;
}

void DatalogChannelsPanel::PopulateSelectedChannels(DatalogChannelSelectionSet *selectionSet){

	wxArrayTreeItemIds selections;
	m_channelsList->GetSelections(selections);

	for(wxArrayTreeItemIds::iterator it = selections.begin(); it != selections.end(); it++){
		wxTreeItemId item = *it;
		wxTreeItemData *data = m_channelsList->GetItemData(item);
		ChannelNode *node = dynamic_cast<ChannelNode *>(data);
		if (NULL != node){
			int datalogId = node->datalogId;
			DatalogInfo datalogInfo;
			m_datalogStore->ReadDatalogInfo(datalogId, datalogInfo);
			DatalogChannelSelection sel(datalogId, datalogInfo.name, node->channelName);
			selectionSet->Add(sel);
		}
	}
}

void DatalogChannelsPanel::OnNewLineChart(wxCommandEvent &event){

	DatalogChannelSelectionSet *selectionSet = new DatalogChannelSelectionSet();
	PopulateSelectedChannels(selectionSet);
	wxCommandEvent addEvent( ADD_NEW_LINE_CHART_EVENT, ADD_NEW_LINE_CHART );
	addEvent.SetClientData(selectionSet);
	GetParent()->GetEventHandler()->AddPendingEvent(addEvent);
}

void DatalogChannelsPanel::OnNewAnalogGauge(wxCommandEvent &event){

	DatalogChannelSelectionSet *selectionSet = new DatalogChannelSelectionSet();
	PopulateSelectedChannels(selectionSet);
	wxCommandEvent addEvent( ADD_NEW_ANALOG_GAUGE_EVENT, ADD_NEW_ANALOG_GAUGE );
	addEvent.SetClientData(selectionSet);
	GetParent()->GetEventHandler()->AddPendingEvent(addEvent);
}

void DatalogChannelsPanel::OnNewDigitalGauge(wxCommandEvent &event){

	DatalogChannelSelectionSet *selectionSet = new DatalogChannelSelectionSet();
	PopulateSelectedChannels(selectionSet);
	wxCommandEvent addEvent( ADD_NEW_DIGITAL_GAUGE_EVENT, ADD_NEW_DIGITAL_GAUGE );
	addEvent.SetClientData(selectionSet);
	GetParent()->GetEventHandler()->AddPendingEvent(addEvent);
}

void DatalogChannelsPanel::OnNewGPSView(wxCommandEvent &event){
	DatalogChannelSelectionSet *selectionSet = new DatalogChannelSelectionSet();
	PopulateSelectedChannels(selectionSet);
	wxCommandEvent addEvent( ADD_NEW_GPS_VIEW_EVENT, ADD_NEW_GPS_VIEW );
	addEvent.SetClientData(selectionSet);
	GetParent()->GetEventHandler()->AddPendingEvent(addEvent);
}

void DatalogChannelsPanel::OnAddChannelView(wxCommandEvent &event){

	AddChannelWizard *wiz = new AddChannelWizard(GetParent(),AddChannelWizardParams(m_appPrefs,m_appOptions,m_datalogStore, m_raceCaptureConfig));
	wiz->ShowPage(wiz->GetFirstPage());
	wiz->Show(true);
}

void DatalogChannelsPanel::OnPlayForward(wxCommandEvent &event){
	wxCommandEvent playEvent( PLAY_FWD_DATALOG_EVENT, PLAY_FWD_DATALOG);
	GetParent()->GetEventHandler()->AddPendingEvent(playEvent);
}

void DatalogChannelsPanel::OnPause(wxCommandEvent &event){
	wxCommandEvent pauseEvent( PAUSE_DATALOG_EVENT, PAUSE_DATALOG);
	GetParent()->GetEventHandler()->AddPendingEvent(pauseEvent);
}

void DatalogChannelsPanel::OnPlayReverse(wxCommandEvent &event){
	wxCommandEvent playEvent( PLAY_REV_DATALOG_EVENT, PLAY_REV_DATALOG);
	GetParent()->GetEventHandler()->AddPendingEvent(playEvent);
}

void DatalogChannelsPanel::OnSkipForward(wxCommandEvent &event){
	wxCommandEvent evt( JUMP_END_DATALOG_EVENT, JUMP_END_DATALOG);
	GetParent()->GetEventHandler()->AddPendingEvent(evt);
}

void DatalogChannelsPanel::OnSkipReverse(wxCommandEvent &event){
	wxCommandEvent evt( JUMP_BEGINNING_DATALOG_EVENT, JUMP_BEGINNING_DATALOG);
	GetParent()->GetEventHandler()->AddPendingEvent(evt);
}

void DatalogChannelsPanel::OnSeekForward(wxCommandEvent &event){
	wxCommandEvent evt( SEEK_FWD_DATALOG_EVENT, SEEK_FWD_DATALOG);
	GetParent()->GetEventHandler()->AddPendingEvent(evt);
}

void DatalogChannelsPanel::OnSeekReverse(wxCommandEvent &event){
	wxCommandEvent evt( SEEK_REV_DATALOG_EVENT, SEEK_REV_DATALOG);
	GetParent()->GetEventHandler()->AddPendingEvent(evt);
}

void DatalogChannelsPanel::DoGridContextMenu(wxTreeEvent &event){
	PopupMenu(m_gridPopupMenu);
}

void DatalogChannelsPanel::OnTimeScrolled(wxScrollEvent &event){
	m_isScrolling = true;
	wxCommandEvent evt( SEEK_ABS_DATALOG_EVENT, SEEK_ABS_DATALOG);
	evt.SetInt(m_timeSlider->GetValue());
	GetParent()->GetEventHandler()->AddPendingEvent(evt);
}

void DatalogChannelsPanel::OnTimeScrollRelease(wxScrollEvent &event){
	m_isScrolling = false;
}

void DatalogChannelsPanel::OnOffsetChanged(wxTreeEvent &event){
	wxTreeItemId item = event.GetItem();
	long offset = 0;
	if (m_channelsList->GetItemText(item,1).ToLong(&offset,10)){
		wxTreeItemData *data = m_channelsList->GetItemData(item);
		ChannelNode *node = dynamic_cast<ChannelNode *>(data);
		if (NULL != node){
			int datalogId = node->datalogId;
			wxCommandEvent event( TIME_OFFSET_CHANGED_EVENT, TIME_OFFSET_CHANGED );
			event.SetClientData(new TimeOffsetChange(datalogId, (int)offset));
			GetParent()->GetEventHandler()->AddPendingEvent(event);
		}
	}
}

BEGIN_EVENT_TABLE ( DatalogChannelsPanel, wxPanel )
	EVT_MENU(ID_NEW_LINE_CHART,DatalogChannelsPanel::OnNewLineChart)
	EVT_MENU(ID_NEW_ANALOG_GAUGE, DatalogChannelsPanel::OnNewAnalogGauge)
	EVT_MENU(ID_NEW_DIGITAL_GAUGE, DatalogChannelsPanel::OnNewDigitalGauge)
	EVT_MENU(ID_NEW_GPS_VIEW, DatalogChannelsPanel::OnNewGPSView)
	EVT_MENU(ID_ADD_CHANNEL, DatalogChannelsPanel::OnAddChannelView)

	EVT_MENU(ID_SKIP_DATALOG_REV, DatalogChannelsPanel::OnSkipReverse)
	EVT_MENU(ID_SEEK_DATALOG_REV, DatalogChannelsPanel::OnSeekReverse)
	EVT_MENU(ID_PLAY_DATALOG_REV, DatalogChannelsPanel::OnPlayReverse)
	EVT_MENU(ID_PAUSE_DATALOG, DatalogChannelsPanel::OnPause)
	EVT_MENU(ID_PLAY_DATALOG_FWD, DatalogChannelsPanel::OnPlayForward)
	EVT_MENU(ID_SEEK_DATALOG_FWD, DatalogChannelsPanel::OnSeekForward)
	EVT_MENU(ID_SKIP_DATALOG_FWD, DatalogChannelsPanel::OnSkipForward)
	EVT_COMMAND_SCROLL(ID_TIME_SCROLLER, DatalogChannelsPanel::OnTimeScrolled)
	EVT_COMMAND_SCROLL_THUMBRELEASE(ID_TIME_SCROLLER, DatalogChannelsPanel::OnTimeScrollRelease)
	EVT_TREE_END_LABEL_EDIT(ID_DATALOG_CHANNELS_LIST, DatalogChannelsPanel::OnOffsetChanged)

	EVT_TREE_ITEM_RIGHT_CLICK(ID_DATALOG_CHANNELS_LIST, DatalogChannelsPanel::DoGridContextMenu)
END_EVENT_TABLE()
