
#include "appPrefs.h"
#include "wx/config.h"
#include "wx/stdpaths.h"
#include "logging.h"

AppPrefs::AppPrefs() {}


void AppPrefs::SaveAppPrefs(){
	wxConfig config(RACE_ANALYZER_APP_NAME);

	config.Write(CONFIG_KEY_LAST_RACEEVENT_DIRECTORY, m_lastRaceEventDirectory);
	config.Write(CONFIG_KEY_LAST_CONFIG_DIRECTORY,m_lastConfigFileDirectory);
	config.Write(CONFIG_KEY_LAST_DATALOG_DIRECTORY, m_lastDatalogDirectory);
	config.Write(CONFIG_KEY_ACTIVE_PERSPECTIVE, m_currentPerspectiveName);
}

void AppPrefs::LoadAppPrefs(){
	wxConfig config(RACE_ANALYZER_APP_NAME);
	wxStandardPaths standardPaths;
	m_lastConfigFileDirectory = config.Read(CONFIG_KEY_LAST_CONFIG_DIRECTORY, standardPaths.GetUserConfigDir());
	m_lastDatalogDirectory = config.Read(CONFIG_KEY_LAST_DATALOG_DIRECTORY, standardPaths.GetUserConfigDir());
	m_lastRaceEventDirectory = config.Read(CONFIG_KEY_LAST_RACEEVENT_DIRECTORY, standardPaths.GetUserConfigDir());
	m_currentPerspectiveName = config.Read(CONFIG_KEY_ACTIVE_PERSPECTIVE, DEFAULT_PERSPECTIVE);
}

void AppPrefs::ResetDefaults(){
	SaveAppPrefs();
}

void AppPrefs::SetLastConfigFileDirectory(wxString lastConfigFileDirectory){
	m_lastConfigFileDirectory = lastConfigFileDirectory;
}

wxString AppPrefs::GetLastRaceEventDirectory(){
	return m_lastRaceEventDirectory;
}

void AppPrefs::SetLastRaceEventDirectory(wxString lastRaceEventDirectory){
	m_lastRaceEventDirectory = lastRaceEventDirectory;
}

wxString AppPrefs::GetLastConfigFileDirectory(){
	return m_lastConfigFileDirectory;
}

void AppPrefs::SetLastDatalogDirectory(wxString lastDatalogDirectory){
	m_lastDatalogDirectory = lastDatalogDirectory;
}

wxString AppPrefs::GetLastDatalogDirectory(){
	return m_lastDatalogDirectory;
}

wxString AppPrefs::GetCurrentPerspectiveName(){
	return m_currentPerspectiveName;
}

void AppPrefs::SetCurrentPerspectiveName(wxString perspectiveName){
	m_currentPerspectiveName = perspectiveName;
}

void AppPrefs::SavePerspectiveConfig(wxString perspectiveName, wxString perspectiveConfig){
	wxConfig config(RACE_ANALYZER_APP_NAME);
	config.Write(wxString::Format("perspective_%s", perspectiveName.ToAscii()), perspectiveConfig);
	config.Flush();
}

wxString AppPrefs::ReadPerspectiveConfig(wxString perspectiveName){
	wxConfig config(RACE_ANALYZER_APP_NAME);
	wxString perspectiveConfig = config.Read(wxString::Format("perspective_%s", perspectiveName.ToAscii()),"");
	VERBOSE(FMT("reading config %s: %s", perspectiveName.ToAscii(), perspectiveConfig.ToAscii()));
	return perspectiveConfig;
}
