#ifndef _COMM_H_
#define _COMM_H_

#include "configData.h"
#include "serialComm.h"
#include "raceCapture/raceCaptureConfig.h"
#include "raceCapture/raceCaptureRuntime.h"

#include "commonEvents.h"

class RaceAnalyzerCommCallback : public ProgressReceiver{
public:
	virtual void ReadConfigComplete(bool success, wxString msg) = 0;
	virtual void WriteConfigComplete(bool success, wxString msg) = 0;
	virtual void FlashConfigComplete(bool success, wxString msg) = 0;

	virtual ~RaceAnalyzerCommCallback(){}
};

class RaceAnalyzerComm {

	public:
		RaceAnalyzerComm();
		~RaceAnalyzerComm();
		void SetSerialPort(int port);
		void CloseSerialPort();
		wxString readScript();
		void writeScript(wxString &script);
		void populateChannelConfig(ChannelConfig &cfg, wxString suffix, wxString &data);
		void populateChannelConfig(ChannelConfig &cfg, wxString &data);
		void flashCurrentConfig();
		void calibrateAccelZero();

		void ReadVersion(VersionData &version);
		void reloadScript(void);
		void ReadRuntime(RuntimeValues &values);
		Object ParseJSON(wxString &json);
		wxString GetLogfile();

		void readConfig(RaceCaptureConfig *config, RaceAnalyzerCommCallback *callback);

		void readGpioConfig(GpioConfig *config);
		void readAnalogConfig(AnalogConfig *config);
		void readTimerConfig(TimerConfig *config);
		void readAccelConfig(AccelConfig *config);
		void readAnalogPulseConfig(PwmConfig *config);
		void readGpsConfig(GpsConfig *config);
		void readGpsTargetConfig(GpsConfig *config);
		void readConnectivityConfig(ConnectivityConfig *config);

		void writeConfig(RaceCaptureConfig *config, RaceAnalyzerCommCallback *callback);
		void updateWriteConfigPct(int count, RaceAnalyzerCommCallback *callback);

	private:

		static const int DEFAULT_TIMEOUT = 500;
		wxLogStderr *_logger;
		FILE *_logfile;

		static const size_t SCRIPT_PAGE_LENGTH = 256;
		static const int SCRIPT_PAGES = 40;
		static const char EQUALS_HIDE_CHAR = 1;
		static const char SEMICOLON_HIDE_CHAR = 2;

		CComm* OpenSerialPort();
		CComm* GetSerialPort();
		void CheckThrowResult(wxString &result);
		void FlushReceiveBuffer(CComm * comPort);
		int ReadLine(CComm * comPort, wxString &buffer, int timeout);
		int WriteLine(CComm * comPort, wxString &buffer, int timeout);
		wxString SendCommand(CComm *comPort, const wxString &buffer, size_t timeout = DEFAULT_TIMEOUT);
		void Unescape(wxString &data);
		void Escape(wxString &data);
		int GetIntParam(wxString &data, const wxString &name);
		float GetFloatParam(wxString &data,const wxString &name);
		wxString GetParam(wxString &data, const wxString &name, bool hideTokens = true);
		void HideInnerTokens(wxString &data);
		void UnhideInnerTokens(wxString &data);
		void SwapCharsInsideQuotes(wxString &data, char find,char replace);

		wxString AppendStringParam(wxString &cmd, wxString param);
		wxString AppendFloatParam(wxString &cmd, float param);
		wxString AppendIntParam(wxString &cmd, int param);
		wxString AppendUIntParam(wxString &cmd, unsigned int param);
		wxString AppendChannelConfig(wxString &cmd, ChannelConfig &channelConfig);

		void CheckThrowIncompatibleVersion();

		int 			_serialPortNumber;
		CComm* 			_serialPort;

		wxMutex			_commMutex;
};

class AsyncRaceAnalyzerComm : public wxThread{

public:

	static const int ACTION_NONE = -1;
	static const int ACTION_READ_CONFIG = 0;
	static const int ACTION_WRITE_CONFIG = 1;
	static const int ACTION_FLASH_CONFIG = 2;

	AsyncRaceAnalyzerComm(RaceAnalyzerComm *comm, RaceCaptureConfig *config, RaceAnalyzerCommCallback *progress);
	void RunReadConfig(void);
	void RunWriteConfig(void);
	void RunFlashConfig(void);
	void Run(int action);
	void * Entry();

private:
	void WriteConfig();

	int m_action;
	RaceAnalyzerComm *m_comm;
	RaceCaptureConfig *m_config;
	RaceAnalyzerCommCallback *m_callback;
};

class CommException{

	public:
		static const int OPEN_PORT_FAILED = 0;
		static const int DATA_ERROR = 1;
		static const int COMM_TIMEOUT = 2;
		static const int CMD_ERROR = 3;

		CommException(int errorStatus, wxString errorDetail = "");
		int GetErrorStatus();
		wxString GetErrorDetail();
		wxString GetErrorMessage();

	private:
		int _errorStatus;
		wxString _errorDetail;
};

#endif
