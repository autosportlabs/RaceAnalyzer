#ifndef RACEANALYZERCONFIGBASE_H_
#define RACEANALYZERCONFIGBASE_H_

#define		RACE_ANALYZER_VERSION			"0.0.3"
#define 	RACE_ANALYZER_APP_NAME 			"Autosport_Labs_RaceAnalyzer_0.0.3"


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