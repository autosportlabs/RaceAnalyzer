
#include "appPrefs.h"
#include "wx/config.h"
#include "wx/stdpaths.h"

AppPrefs::AppPrefs() : m_activePerspective(0){}


void AppPrefs::SaveAppPrefs(){
	wxConfig config(RACE_ANALYZER_APP_NAME);

	config.Write(CONFIG_KEY_LAST_RACEEVENT_DIRECTORY, m_lastRaceEventDirectory);
	config.Write(CONFIG_KEY_LAST_CONFIG_DIRECTORY,m_lastConfigFileDirectory);
	config.Write(CONFIG_KEY_LAST_DATALOG_DIRECTORY, m_lastDatalogDirectory);

	int count = m_perspectives.Count();
	config.Write(CONFIG_KEY_PERSPECTIVE_COUNT, count);
	config.Write(CONFIG_KEY_ACTIVE_PERSPECTIVE, m_activePerspective);
	for (int i = 0; i < count; i++){
		config.Write(wxString::Format("%s%d",CONFIG_KEY_PERSPECTIVE,i), m_perspectives[i]);
		config.Write(wxString::Format("%s%d",CONFIG_KEY_PERSPECTIVE_NAME,i), m_perspectiveNames[i]);
	}
}

void AppPrefs::LoadAppPrefs(){
	wxConfig config(RACE_ANALYZER_APP_NAME);

	wxStandardPaths standardPaths;

	m_lastConfigFileDirectory = config.Read(CONFIG_KEY_LAST_CONFIG_DIRECTORY, standardPaths.GetUserConfigDir());
	m_lastDatalogDirectory = config.Read(CONFIG_KEY_LAST_DATALOG_DIRECTORY, standardPaths.GetUserConfigDir());
	m_lastRaceEventDirectory = config.Read(CONFIG_KEY_LAST_RACEEVENT_DIRECTORY, standardPaths.GetUserConfigDir());

	int count;
	config.Read(CONFIG_KEY_PERSPECTIVE_COUNT, &count,0);
	config.Read(CONFIG_KEY_ACTIVE_PERSPECTIVE, &m_activePerspective,0);
	//if we can't load all perspectives correctly we fail-back to defaults
	for (int i = 0; i < count; i++){
		wxString perspectiveConfig;
		if (config.Read(wxString::Format("%s%d",CONFIG_KEY_PERSPECTIVE,i), &perspectiveConfig)){
			m_perspectives.Add(perspectiveConfig);
		}
		else{
			ResetDefaults();
			break;
		}
		wxString perspectiveName;
		if (config.Read(wxString::Format("%s%d", CONFIG_KEY_PERSPECTIVE_NAME,i), &perspectiveName)){
			m_perspectiveNames.Add(perspectiveName);
		}
		else{
			ResetDefaults();
			break;
		}
	}
}

void AppPrefs::ResetDefaults(){
	m_perspectives.Empty();
	m_activePerspective = 0;
	SaveAppPrefs();
}

wxArrayString & AppPrefs::GetPerspectives(){
	return m_perspectives;
}

wxArrayString & AppPrefs::GetPerspectiveNames(){
	return m_perspectiveNames;
}

int & AppPrefs::GetActivePerspective(){
	return m_activePerspective;
}

void AppPrefs::SetActivePerspective(int activePerspective){
	m_activePerspective = activePerspective;
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
