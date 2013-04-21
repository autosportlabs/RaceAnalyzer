#ifndef APPPEFS_H_
#define APPPEFS_H_
#include "wx/wxprec.h"

#include "raceAnalyzerConfigBase.h"

#define CONFIG_KEY_LAST_RACEEVENT_DIRECTORY "lastRaceEventDir"
#define CONFIG_KEY_LAST_CONFIG_DIRECTORY "lastConfigDir"
#define CONFIG_KEY_LAST_DATALOG_DIRECTORY "lastDatalogDir"
#define CONFIG_KEY_PERSPECTIVE 	"perspective_"
#define CONFIG_KEY_ACTIVE_PERSPECTIVE "activePerspective"
#define DEFAULT_PERSPECTIVE ""


class AppPrefs{

	public:


		AppPrefs();

		wxArrayString & GetPerspectiveNames();
		wxString GetLastRaceEventDirectory();
		void SetLastRaceEventDirectory(wxString lastRaceEventDirectory);
		wxString GetLastConfigFileDirectory();
		void SetLastConfigFileDirectory(wxString lastconfigFileDirectory);
		wxString GetLastDatalogDirectory();
		void SetLastDatalogDirectory(wxString lastDatalogDirectory);
		wxString GetCurrentPerspectiveName();
		void SetCurrentPerspectiveName(wxString perspectiveName);
		wxString ReadPerspectiveConfig(wxString perspectiveName);
		void SavePerspectiveConfig(wxString perspectiveName, wxString perspectiveConfig);



		void SaveAppPrefs();
		void LoadAppPrefs();
		void ResetDefaults();

	private:
		wxString		m_currentPerspectiveName;

		wxString		m_lastRaceEventDirectory;
		wxString		m_lastConfigFileDirectory;
		wxString		m_lastDatalogDirectory;

};
#endif /*APPOPTIONS_H_*/
