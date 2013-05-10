#ifndef RACEANALYZERCONFIGBASE_H_
#define RACEANALYZERCONFIGBASE_H_

#ifndef RACE_ANALYZER_VERSION
	#define	RACE_ANALYZER_VERSION			"1.1.9"
#endif

#define 	RACE_ANALYZER_APP_NAME 			"Autosport_Labs_RaceAnalyzer_" RACE_ANALYZER_VERSION


class InvalidStateException{


	public:
		InvalidStateException(){}
		InvalidStateException(wxString message){
			_message = message;
		}

		wxString GetMessage(){
			return _message;
		}

		wxString _message;
};
#endif
