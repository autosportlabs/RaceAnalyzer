#ifndef APPPEFS_H_
#define APPPEFS_H_
#include "wx/wxprec.h"

#include "raceAnalyzerConfigBase.h"

#define CONFIG_KEY_LAST_RACEEVENT_DIRECTORY "lastRaceEventDir"
#define CONFIG_KEY_LAST_CONFIG_DIRECTORY "lastConfigDir"
#define CONFIG_KEY_LAST_DATALOG_DIRECTORY "lastDatalogDir"
#define CONFIG_KEY_PERSPECTIVE 	"perspective_"
#define CONFIG_KEY_PERSPECTIVE_COUNT "perspectiveCount"
#define CONFIG_KEY_ACTIVE_PERSPECTIVE "activePerspective"
#define CONFIG_KEY_PERSPECTIVE_NAME "perspectiveName_"


class AppPrefs{

	public:


		AppPrefs();

		wxArrayString & GetPerspectives();
		int & GetActivePerspective();
		void SetActivePerspective(int activePerspective);
		wxArrayString & GetPerspectiveNames();
		wxString GetLastRaceEventDirectory();
		void SetLastRaceEventDirectory(wxString lastRaceEventDirectory);
		wxString GetLastConfigFileDirectory();
		void SetLastConfigFileDirectory(wxString lastconfigFileDirectory);
		wxString GetLastDatalogDirectory();
		void SetLastDatalogDirectory(wxString lastDatalogDirectory);


		void SaveAppPrefs();
		void LoadAppPrefs();
		void ResetDefaults();

	private:
		int				m_activePerspective;
		wxArrayString 	m_perspectives;
		wxArrayString	m_perspectiveNames;

		wxString		m_lastRaceEventDirectory;
		wxString		m_lastConfigFileDirectory;
		wxString		m_lastDatalogDirectory;

};
#endif /*APPOPTIONS_H_*/
