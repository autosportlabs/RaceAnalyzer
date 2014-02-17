
#include "comm.h"
#include <stdio.h>
#include <wx/log.h>
#include "base64.h"
#include <wx/tokenzr.h>
#include <wx/hashmap.h>
#include "logging.h"

CommException::CommException(int errorStatus, wxString errorDetail){
	_errorStatus = errorStatus;
	_errorDetail = errorDetail;
}

int CommException::GetErrorStatus(){
	return _errorStatus;
}

wxString CommException::GetErrorDetail(){
	return _errorDetail;
}

wxString CommException::GetErrorMessage(){

	switch (_errorStatus){
		case OPEN_PORT_FAILED:
			return "Could not open port";
		case DATA_ERROR:
			return "A data error occured";
		case COMM_TIMEOUT:
			return "A communications timeout occured: (" + _errorDetail + ")";
		default:
			return "An unspecified error occured";
	}
}

RaceAnalyzerComm::RaceAnalyzerComm() : _serialPortNumber(0), _serialPort(NULL) {

	//_logfile = fopen("mjljConfigLog.txt", "w");
	_logfile = 0;
	_logger = new wxLogStderr(_logfile);
	wxLog::SetActiveTarget(_logger);
}

RaceAnalyzerComm::~RaceAnalyzerComm(){
	CloseSerialPort();
}

void RaceAnalyzerComm::CloseSerialPort(){

	wxMutexLocker lock(_commMutex);

	VERBOSE("closing serial port");
	if (_serialPort){
		if (_serialPort->isOpen()) _serialPort->closePort();
		delete _serialPort;
		_serialPort = NULL;
	}
}

CComm* RaceAnalyzerComm::GetSerialPort(){

	if ( _serialPort){
		if ( ! _serialPort->isOpen()){
			delete _serialPort;
			_serialPort = NULL;
		}
	}


	if (! _serialPort){
		_serialPort = OpenSerialPort();
	}

	return _serialPort;
}

CComm* RaceAnalyzerComm::OpenSerialPort(){

	VERBOSE(FMT("Open Serial port %d" , _serialPortNumber + 1));
	CComm* comPort = new CComm();
	if (!comPort->openPort(comPort->getPortName(_serialPortNumber + 1))){
			delete ( comPort );
			throw CommException(CommException::OPEN_PORT_FAILED);
		}
	return comPort;
}

void RaceAnalyzerComm::CheckThrowResult(wxString &result){
	if (GetParam(result,"result") != "ok") throw CommException(CommException::CMD_ERROR,result);
}

void RaceAnalyzerComm::Unescape(wxString &data){

	wxString working = data;
	data = "";
	const char *d = working.ToAscii();
	while (*d){
		if (*d =='\\'){
			switch(*(d + 1)){
				case '_':
					data+=" ";
					break;
				case 'n':
					data+="\n";
					break;
				case 'r':
					data+="\r";
					break;
				case '\\':
					data+="\\";
					break;
				case '"':
					data+="\"";
					break;
				case '\0': //this should *NOT* happen
					return;
			}
			d++;
		}
		else{
			data+=*d;
		}
		d++;
	}
}

void RaceAnalyzerComm::Escape(wxString &data){

	wxString working = data;
	data="";
	const char *d = working.ToAscii();
	while(*d){
		switch(*d){
		case ' ':
			data+="\\_";
			break;
		case '\n':
			data+="\\n";
			break;
		case '\r':
			data+="\\r";
			break;
		case '\"':
			data+="\\\"";
			break;
		default:
			data+=*d;
			break;
		}
		d++;
	}
}


void RaceAnalyzerComm::SetSerialPort(int port){

	wxMutexLocker lock(_commMutex);

	if (_serialPortNumber != port){

		if (_serialPort){
			if (_serialPort->isOpen()) _serialPort->closePort();
			delete _serialPort;
			_serialPort = NULL;
		}
		_serialPortNumber = port;
	}
}

void RaceAnalyzerComm::FlushReceiveBuffer(CComm* comPort){
	comPort->drainInput();
}

wxString RaceAnalyzerComm::SendCommand(CComm *comPort, const wxString &buffer, size_t timeout){

	wxMutexLocker lock(_commMutex);
	wxString response;

	try{
		VERBOSE(FMT("Send Cmd (%d): '%s'",buffer.Len(), buffer.ToAscii()));
		size_t bufferSize = 8192;
		comPort->sendCommand(buffer.ToAscii(),wxStringBuffer(response,bufferSize),bufferSize,timeout,true);
		VERBOSE(FMT("Cmd Response: %s", response.ToAscii()));
	}
	catch(SerialException &e){
		throw CommException(e.GetErrorStatus(), e.GetErrorDetail());
	}
	catch(...){
		throw CommException(-1, "Unknown exception while sending command");
	}
	return response;
}

int RaceAnalyzerComm::WriteLine(CComm * comPort, wxString &buffer, int timeout){

	VERBOSE(FMT("writeLine: %s", buffer.ToAscii()));
	char *tempBuff = (char*)malloc(buffer.Len() + 10);
	strcpy(tempBuff,buffer.ToAscii());
	strcat(tempBuff,"\r");
	char *buffPtr = tempBuff;

	size_t len = strlen(buffPtr);
	comPort->writeBuffer(buffPtr,len, timeout);
	free(tempBuff);

	return 0;
}

int RaceAnalyzerComm::ReadLine(CComm * comPort, wxString &buffer, int timeout){
	int count = comPort->readLine(wxStringBuffer(buffer,1024), 1024, timeout);
	buffer.Trim(true).Trim(false);
	return count;
}

void RaceAnalyzerComm::SwapCharsInsideQuotes(wxString &data, char find,char replace){
	bool insideQuotes = false;
	int len = data.Len();
	for (int index = 0; index < len; index++){
		if (data[index] == '"'){
			insideQuotes = !insideQuotes;
		}
		else if (data[index] == find && insideQuotes){
			data[index] = replace;
		}
	}
}

void RaceAnalyzerComm::HideInnerTokens(wxString &data){
	SwapCharsInsideQuotes(data,'=',EQUALS_HIDE_CHAR);
	SwapCharsInsideQuotes(data,';',SEMICOLON_HIDE_CHAR);
}

void RaceAnalyzerComm::UnhideInnerTokens(wxString &data){
	SwapCharsInsideQuotes(data,EQUALS_HIDE_CHAR,'=');
	SwapCharsInsideQuotes(data,SEMICOLON_HIDE_CHAR,';');
}

int RaceAnalyzerComm::GetIntParam(wxString &data, const wxString &name){
	return atoi(GetParam(data,name));
}

float RaceAnalyzerComm::GetFloatParam(wxString &data,const wxString &name){
	return atof(GetParam(data,name));
}
wxString RaceAnalyzerComm::GetParam(wxString &data, const wxString &name, bool hideTokens){

	if (hideTokens) HideInnerTokens(data);
	wxStringTokenizer tokenizer(data,";");

	while (tokenizer.HasMoreTokens()){
		wxString token = tokenizer.GetNextToken();
		wxStringTokenizer subTokenizer(token,"=");
		if (subTokenizer.CountTokens() >= 2){
			wxString paramName = subTokenizer.GetNextToken();
			if (paramName == name){
				wxString value = subTokenizer.GetNextToken();
				UnhideInnerTokens(value);
				if (value.StartsWith("\"") && value.EndsWith("\"")){
					value = value.Left(value.Len() - 1);
					value = value.Right(value.Len() - 1);
				}
				return value;
			}
		}
	}
	return "";
}

void RaceAnalyzerComm::ReadVersion(VersionData &version){
	CComm *serialPort = GetSerialPort();
	if (NULL==serialPort) throw CommException(CommException::OPEN_PORT_FAILED);

	wxString result = SendCommand(serialPort, "version");

	long major = 0;
	long minor = 0;
	long bugfix = 0;

	if (	GetParam(result, "major").ToLong(&major,10) &&
			GetParam(result, "minor").ToLong(&minor, 10) &&
			GetParam(result, "bugfix").ToLong(&bugfix, 10)){

	}
	else{
		throw CommException(CommException::CMD_ERROR,wxString::Format("Could not read version response: %s", result));
	}

	version.SetMajor(major);
	version.SetMinor(minor);
	version.SetBugfix(bugfix);
}

void RaceAnalyzerComm::reloadScript(void){

	CComm *serialPort = GetSerialPort();
	if (NULL==serialPort) throw CommException(CommException::OPEN_PORT_FAILED);

	wxString reloadCmd = "reloadScript";
	wxString result = SendCommand(serialPort, reloadCmd);
	CheckThrowResult(result);
}

wxString RaceAnalyzerComm::readScript(){

	wxString script = "";
	int scriptPage = 0;
	int to = 0;
	CComm *serialPort = GetSerialPort();
	if (NULL==serialPort) throw CommException(CommException::OPEN_PORT_FAILED);

	FlushReceiveBuffer(serialPort);
	while(!to){

		//wxString cmd = wxString::Format("println(getScriptPage(%d))",scriptPage++);
		wxString cmd = wxString::Format("readScriptPage %d",scriptPage++);
		wxString buffer = SendCommand(serialPort, cmd);
		buffer.Trim(false);
		buffer.Trim(true);

		wxString scriptFrag = GetParam(buffer,"script", true);

		VERBOSE(FMT("escaped: %s", scriptFrag.ToAscii()));
		Unescape(scriptFrag);
		VERBOSE(FMT("unescaped: %s", scriptFrag.ToAscii()));
		size_t scriptFragmentLen = scriptFrag.Length();

		if (scriptFragmentLen > 0 ) script+=scriptFrag;
		//the last page is a 'partial page'
		if (scriptFragmentLen < SCRIPT_PAGE_LENGTH ) break;
	}
	CloseSerialPort();
	if (to){
		throw CommException(CommException::COMM_TIMEOUT);
	}
	return script;
}

void RaceAnalyzerComm::writeScript(wxString &script){

	size_t index = 0;
	int page,to;
	page = 0;
	to = 0;
	size_t length = script.Length();

	CComm *serialPort = GetSerialPort();
	if (NULL==serialPort) throw CommException(CommException::OPEN_PORT_FAILED);

	while(index < length && page < SCRIPT_PAGES && !to){
		wxString scriptFragment;
		if (index + SCRIPT_PAGE_LENGTH > length){
			scriptFragment = script.Mid(index);
		}else{
			scriptFragment = script.Mid(index, SCRIPT_PAGE_LENGTH);
		}
		Escape(scriptFragment);
		//wxString cmd = wxString::Format("updateScriptPage(%d,\"%s\")", page,data.ToAscii());
		wxString cmd = wxString::Format("writeScriptPage %d %s",page,scriptFragment.ToAscii());
		wxString result = SendCommand(serialPort, cmd);
		CheckThrowResult(result);
		page++;
		index += SCRIPT_PAGE_LENGTH;
	}
	//did we write fewer than the max number of script pages?
	//note we're subtracting script pages by one to account for integer divide truncation
	if ((length / SCRIPT_PAGE_LENGTH) < SCRIPT_PAGES - 1 ){
		//write a null to the next page
		wxString cmd = wxString::Format("writeScriptPage %d",page);
		wxString result = SendCommand(serialPort, cmd);
		CheckThrowResult(result);
	}

	CloseSerialPort();
	if (to){
		throw CommException(CommException::COMM_TIMEOUT);
	}
}

void RaceAnalyzerComm::populateChannelConfig(ChannelConfig &cfg, wxString suffix, wxString &data){
	const char *suf = suffix.ToAscii();
	cfg.label = GetParam(data,wxString::Format("label_%s",suf));
	cfg.units = GetParam(data,wxString::Format("units_%s",suf));
	cfg.sampleRate = (sample_rate_t)GetIntParam(data,wxString::Format("sampleRate_%s",suf));
}

void RaceAnalyzerComm::populateChannelConfig(ChannelConfig &cfg, wxString &data){
	cfg.label = GetParam(data,"label");
	cfg.units = GetParam(data,"units");
	cfg.sampleRate = (sample_rate_t)GetIntParam(data,"sampleRate");
}

void RaceAnalyzerComm::ReadRuntime(RuntimeValues &values){
	try{
		wxDateTime start = wxDateTime::UNow();
		CComm *serialPort = GetSerialPort();
		if (NULL==serialPort) throw CommException(CommException::OPEN_PORT_FAILED);

		wxString rsp = SendCommand(serialPort, "sample");

		wxStringTokenizer tokenizer(rsp,";");
		while (tokenizer.HasMoreTokens()){
			wxString token = tokenizer.GetNextToken();
			wxStringTokenizer subTokenizer(token,"=");
			if (subTokenizer.CountTokens() >= 2){
				wxString name = subTokenizer.GetNextToken();
				wxString value = subTokenizer.GetNextToken();
				UnhideInnerTokens(value);
				if (value.StartsWith("\"") && value.EndsWith("\"")){
					value = value.Left(value.Len() - 1);
					value = value.Right(value.Len() - 1);
				}
				values[name] = atof(value);
			}
		}
		wxTimeSpan dur = wxDateTime::UNow() - start;
	}
	catch(CommException &e){
		CloseSerialPort();
		throw e;
	}
}

void RaceAnalyzerComm::readAnalogConfig(AnalogConfig *config){
	CComm *serialPort = GetSerialPort();
	if (NULL==serialPort) throw CommException(CommException::OPEN_PORT_FAILED);

	for (int i = 0; i < CONFIG_ANALOG_CHANNELS; i++){
		AnalogConfig &analogConfig = (config[i]);
		wxString cmd = wxString::Format("getAnalogCfg %d",i);
		wxString rsp = SendCommand(serialPort,cmd);
		populateChannelConfig(analogConfig.channelConfig,rsp);
		analogConfig.loggingPrecision = GetIntParam(rsp,"loggingPrecision");
		analogConfig.scalingMode = (scaling_mode_t)GetIntParam(rsp,"scalingMode");
		analogConfig.linearScaling = GetFloatParam(rsp,"scaling");
		for (int ii=0; ii < CONFIG_ANALOG_SCALING_BINS;ii++){
			analogConfig.scalingMap.rawValues[ii]=GetIntParam(rsp,wxString::Format("mapRaw_%d",ii));
		}
		for (int ii=0; ii < CONFIG_ANALOG_SCALING_BINS;ii++){
			analogConfig.scalingMap.scaledValue[ii]=GetFloatParam(rsp,wxString::Format("mapScaled_%d",ii));
		}
	}
}

void RaceAnalyzerComm::readTimerConfig(TimerConfig *config){
	CComm *serialPort = GetSerialPort();
	if (NULL==serialPort) throw CommException(CommException::OPEN_PORT_FAILED);

	for (int i = 0; i < CONFIG_TIMER_CHANNELS; i++){
		TimerConfig &timerConfig = config[i];
		wxString cmd = wxString::Format("getTimerCfg %d",i);
		wxString rsp = SendCommand(serialPort,cmd);
		populateChannelConfig(timerConfig.channelConfig,rsp);
		timerConfig.loggingPrecision = GetIntParam(rsp,"loggingPrecision");
		timerConfig.slowTimerEnabled = GetIntParam(rsp,"slowTimer") == 1;
		timerConfig.mode = (timer_mode_t)GetIntParam(rsp,"mode");
		timerConfig.pulsePerRev = GetIntParam(rsp,"pulsePerRev");
		timerConfig.timerDivider = GetIntParam(rsp,"divider");
		timerConfig.scaling = GetIntParam(rsp,"scaling");
	}
}


void RaceAnalyzerComm::readAccelConfig(AccelConfig *config){
	CComm *serialPort = GetSerialPort();
	if (NULL==serialPort) throw CommException(CommException::OPEN_PORT_FAILED);

	for (int i = 0; i < CONFIG_ACCEL_CHANNELS; i++){
		AccelConfig &accelConfig = (config[i]);
		wxString cmd = wxString::Format("getAccelCfg %d",i);
		wxString rsp = SendCommand(serialPort,cmd);
		populateChannelConfig(accelConfig.channelConfig,rsp);
		accelConfig.mode = (accel_mode_t)GetIntParam(rsp,"mode");
		accelConfig.channel = (accel_channel_t)GetIntParam(rsp,"channel");
		accelConfig.zeroValue = GetIntParam(rsp,"zeroValue");
	}
}

void RaceAnalyzerComm::readAnalogPulseConfig(PwmConfig *config){
	CComm *serialPort = GetSerialPort();
	if (NULL==serialPort) throw CommException(CommException::OPEN_PORT_FAILED);

	for (int i = 0; i < CONFIG_ANALOG_PULSE_CHANNELS; i++){
		PwmConfig &pwmConfig = (config[i]);
		wxString cmd = wxString::Format("getPwmCfg %d",i);
		wxString rsp = SendCommand(serialPort,cmd);
		populateChannelConfig(pwmConfig.channelConfig,rsp);
		pwmConfig.loggingPrecision = GetIntParam(rsp,"loggingPrecision");
		pwmConfig.outputMode = (pwm_output_mode_t)GetIntParam(rsp,"outputMode");
		pwmConfig.loggingMode = (pwm_logging_mode_t)GetIntParam(rsp,"loggingMode");
		pwmConfig.startupDutyCycle = GetIntParam(rsp,"startupDutyCycle");
		pwmConfig.startupPeriod = GetIntParam(rsp,"startupPeriod");
		pwmConfig.voltageScaling = GetFloatParam(rsp,"voltageScaling");
	}
}

void RaceAnalyzerComm::readGpioConfig(GpioConfig *config){
	CComm *serialPort = GetSerialPort();
	if (NULL==serialPort) throw CommException(CommException::OPEN_PORT_FAILED);

	for (int i = 0; i < CONFIG_GPIO_CHANNELS; i++){
		GpioConfig &gpioConfig = (config[i]);
		wxString cmd = wxString::Format("getGpioCfg %d",i);
		wxString rsp = SendCommand(serialPort,cmd);
		populateChannelConfig(gpioConfig.channelConfig,rsp);
		gpioConfig.mode = (gpio_mode_t)GetIntParam(rsp,"mode");
	}
}

void RaceAnalyzerComm::readGpsConfig(GpsConfig *config){
	CComm *serialPort = GetSerialPort();
	if (NULL==serialPort) throw CommException(CommException::OPEN_PORT_FAILED);

	wxString cmd = "getGpsCfg";
	wxString rsp = SendCommand(serialPort,cmd);
	config->gpsInstalled = GetIntParam(rsp,"installed") != 0;
	populateChannelConfig(config->latitudeCfg,"lat",rsp);
	populateChannelConfig(config->longitudeCfg,"long",rsp);
	populateChannelConfig(config->speedCfg,"vel",rsp);
	populateChannelConfig(config->timeCfg,"time",rsp);
	populateChannelConfig(config->satellitesCfg,"sats",rsp);
}

void RaceAnalyzerComm::readGpsTargetConfig(GpsConfig *config){
	CComm *serialPort = GetSerialPort();
	if (NULL==serialPort) throw CommException(CommException::OPEN_PORT_FAILED);

	wxString cmd = "getStartFinishCfg";
	wxString rsp = SendCommand(serialPort,cmd);
	populateChannelConfig(config->lapCountCfg,"lapCount",rsp);
	populateChannelConfig(config->lapTimeCfg,"lapTime",rsp);
	populateChannelConfig(config->splitTimeCfg, "splitTime", rsp);
	config->startFinishTarget.latitude = GetFloatParam(rsp,"startFinishLat");
	config->startFinishTarget.longitude = GetFloatParam(rsp,"startFinishLong");
	config->startFinishTarget.targetRadius = GetFloatParam(rsp,"startFinishRadius");
	config->splitTarget.latitude = GetFloatParam(rsp, "splitLat");
	config->splitTarget.longitude = GetFloatParam(rsp, "splitLong");
	config->splitTarget.targetRadius = GetFloatParam(rsp, "splitRadius");
}

void RaceAnalyzerComm::readConnectivityConfig(ConnectivityConfig *config){
	CComm *serialPort = GetSerialPort();
	if (NULL==serialPort) throw CommException(CommException::OPEN_PORT_FAILED);

	wxString cmd = "getOutputCfg";
	wxString rsp = SendCommand(serialPort,cmd);
	config->sdLoggingMode = (sd_logging_mode_t)GetIntParam(rsp,"sdLoggingMode");
	config->connectivityMode = (connectivity_mode_t)GetIntParam(rsp,"telemetryMode");
	config->p2pConfig.destinationAddrHigh = GetIntParam(rsp,"p2pDestAddrHigh");
	config->p2pConfig.destinationAddrLow = GetIntParam(rsp,"p2pDestAddrLow");
	config->telemetryConfig.telemetryServer = GetParam(rsp, "telemetryServerHost");
	config->telemetryConfig.telemetryDeviceId = GetParam(rsp, "telemetryDeviceId");

	rsp = SendCommand(serialPort, "getCellCfg");
	config->cellularConfig.apnHost = GetParam(rsp, "apnHost");
	config->cellularConfig.apnUser = GetParam(rsp, "apnUser");
	config->cellularConfig.apnPass = GetParam(rsp, "apnPass");
}


void RaceAnalyzerComm::readConfig(RaceCaptureConfig *config,RaceAnalyzerCommCallback *callback){
	try{
		wxDateTime start = wxDateTime::UNow();
		int updateCount = 0;
		CComm *serialPort = GetSerialPort();
		if (NULL==serialPort) throw CommException(CommException::OPEN_PORT_FAILED);

		updateWriteConfigPct(updateCount, callback);

		readGpsConfig(&config->gpsConfig);
		updateWriteConfigPct(++updateCount, callback);

		readGpsTargetConfig(&config->gpsConfig);
		updateWriteConfigPct(++updateCount, callback);

		readAnalogConfig(config->analogConfigs);
		updateCount += CONFIG_ANALOG_CHANNELS;
		updateWriteConfigPct(updateCount, callback);

		readTimerConfig(config->timerConfigs);
		updateCount += CONFIG_TIMER_CHANNELS;
		updateWriteConfigPct(updateCount, callback);

		readAccelConfig(config->accelConfigs);
		updateCount += CONFIG_ACCEL_CHANNELS;
		updateWriteConfigPct(updateCount, callback);

		readAnalogPulseConfig(config->pwmConfigs);
		updateCount += CONFIG_ANALOG_PULSE_CHANNELS;
		updateWriteConfigPct(updateCount, callback);

		readGpioConfig(config->gpioConfigs);
		updateCount += CONFIG_GPIO_CHANNELS;
		updateWriteConfigPct(updateCount, callback);

		readConnectivityConfig(&config->connectivityConfig);
		updateWriteConfigPct(++updateCount,callback);

		config->luaScript = readScript();
		updateWriteConfigPct(++updateCount,callback);

		wxTimeSpan dur = wxDateTime::UNow() - start;
		VERBOSE(FMT("get config in %f",dur.GetMilliseconds().ToDouble()));
		callback->ReadConfigComplete(true,"");
	}
	catch(CommException &e){
		callback->ReadConfigComplete(false, e.GetErrorMessage());
	}
	CloseSerialPort();
}

void RaceAnalyzerComm::updateWriteConfigPct(int count,RaceAnalyzerCommCallback *callback){
	int total = CONFIG_ANALOG_CHANNELS +
			CONFIG_TIMER_CHANNELS +
			CONFIG_ACCEL_CHANNELS +
			CONFIG_ANALOG_PULSE_CHANNELS +
			CONFIG_GPIO_CHANNELS + 4; // 4 extra = gps, start/finish, logger output config , scripting

	int pct =  (count * 100) / total;
	callback->OnProgress(pct);

}

void RaceAnalyzerComm::writeConfig(RaceCaptureConfig *config, RaceAnalyzerCommCallback * callback){
	try{
		wxDateTime start = wxDateTime::UNow();
		CComm *serialPort = GetSerialPort();
		if (NULL==serialPort) throw CommException(CommException::OPEN_PORT_FAILED);

		int updateCount = 0;
		for (int i = 0; i < CONFIG_ANALOG_CHANNELS;i++){
			AnalogConfig &cfg = config->analogConfigs[i];
			wxString cmd = "setAnalogCfg";
			cmd = AppendIntParam(cmd, i);
			cmd = AppendChannelConfig(cmd, cfg.channelConfig);
			cmd = AppendIntParam(cmd, cfg.loggingPrecision);
			cmd = AppendIntParam(cmd, cfg.scalingMode);
			cmd = AppendFloatParam(cmd, cfg.linearScaling);
			ScalingMap &map = (cfg.scalingMap);
			for (int m=0; m < CONFIG_ANALOG_SCALING_BINS;m++){
				cmd = AppendIntParam(cmd, map.rawValues[m]);
			}
			for (int m=0; m < CONFIG_ANALOG_SCALING_BINS;m++){
				cmd = AppendFloatParam(cmd, map.scaledValue[m]);
			}
			wxString result = SendCommand(serialPort, cmd);
			CheckThrowResult(result);
			updateWriteConfigPct(++updateCount,callback);
		}
		for (int i = 0; i < CONFIG_TIMER_CHANNELS; i++){
			TimerConfig &cfg = config->timerConfigs[i];
			wxString cmd = "setTimerCfg";
			cmd = AppendIntParam(cmd, i);
			AppendChannelConfig(cmd, cfg.channelConfig);
			cmd = AppendIntParam(cmd, cfg.loggingPrecision);
			cmd = AppendIntParam(cmd, cfg.slowTimerEnabled);
			cmd = AppendIntParam(cmd, cfg.mode);
			cmd = AppendIntParam(cmd, cfg.pulsePerRev);
			cmd = AppendIntParam(cmd, cfg.timerDivider);
			cmd = AppendIntParam(cmd, cfg.scaling);
			wxString result = SendCommand(serialPort, cmd);
			CheckThrowResult(result);
			updateWriteConfigPct(++updateCount,callback);
		}
		for (int i = 0; i < CONFIG_ACCEL_CHANNELS; i++){
			AccelConfig &cfg = config->accelConfigs[i];
			wxString cmd = "setAccelCfg";
			cmd = AppendIntParam(cmd, i);
			cmd = AppendChannelConfig(cmd, cfg.channelConfig);
			cmd = AppendIntParam(cmd, cfg.mode);
			cmd = AppendIntParam(cmd, cfg.channel);
			cmd = AppendIntParam(cmd, cfg.zeroValue);
			wxString result = SendCommand(serialPort, cmd);
			CheckThrowResult(result);
			updateWriteConfigPct(++updateCount,callback);
		}
		for (int i = 0; i < CONFIG_ANALOG_PULSE_CHANNELS; i++){
			PwmConfig &cfg = config->pwmConfigs[i];
			wxString cmd = "setPwmCfg";
			cmd = AppendIntParam(cmd, i);
			cmd = AppendChannelConfig(cmd, cfg.channelConfig);
			cmd = AppendIntParam(cmd, cfg.loggingPrecision);
			cmd = AppendIntParam(cmd, cfg.outputMode);
			cmd = AppendIntParam(cmd, cfg.loggingMode);
			cmd = AppendIntParam(cmd, cfg.startupDutyCycle);
			cmd = AppendIntParam(cmd, cfg.startupPeriod);
			cmd = AppendFloatParam(cmd, cfg.voltageScaling);
			wxString result = SendCommand(serialPort, cmd);
			CheckThrowResult(result);
			updateWriteConfigPct(++updateCount,callback);
		}
		for (int i = 0; i < CONFIG_GPIO_CHANNELS; i++){
			GpioConfig &cfg = config->gpioConfigs[i];
			wxString cmd = "setGpioCfg";
			cmd = AppendIntParam(cmd,i);
			cmd = AppendChannelConfig(cmd, cfg.channelConfig);
			cmd = AppendIntParam(cmd, cfg.mode);
			wxString result = SendCommand(serialPort, cmd);
			CheckThrowResult(result);
			updateWriteConfigPct(++updateCount,callback);
		}
		{
			GpsConfig &cfg = config->gpsConfig;
			{
				wxString cmd = "setGpsCfg";
				cmd = AppendIntParam(cmd, cfg.gpsInstalled);
				cmd = AppendChannelConfig(cmd, cfg.latitudeCfg);
				cmd = AppendChannelConfig(cmd, cfg.longitudeCfg);
				cmd = AppendChannelConfig(cmd, cfg.speedCfg);
				cmd = AppendChannelConfig(cmd, cfg.timeCfg);
				cmd = AppendChannelConfig(cmd, cfg.satellitesCfg);
				wxString result = SendCommand(serialPort, cmd);
				CheckThrowResult(result);
				updateWriteConfigPct(++updateCount,callback);
			}
			{
				wxString cmd = "setStartFinishCfg";
				cmd = AppendChannelConfig(cmd, cfg.lapCountCfg);
				cmd = AppendChannelConfig(cmd, cfg.lapTimeCfg);
				cmd = AppendChannelConfig(cmd, cfg.splitTimeCfg);
				cmd = AppendFloatParam(cmd, cfg.startFinishTarget.latitude);
				cmd = AppendFloatParam(cmd, cfg.startFinishTarget.longitude);
				cmd = AppendFloatParam(cmd, cfg.startFinishTarget.targetRadius);
				cmd = AppendFloatParam(cmd, cfg.splitTarget.latitude);
				cmd = AppendFloatParam(cmd, cfg.splitTarget.longitude);
				cmd = AppendFloatParam(cmd, cfg.splitTarget.targetRadius);
				wxString result = SendCommand(serialPort, cmd);
				CheckThrowResult(result);
				updateWriteConfigPct(++updateCount,callback);
			}
		}
		{
			ConnectivityConfig &cfg = config->connectivityConfig;
			wxString cmd = "setOutputCfg";
			cmd = AppendIntParam(cmd, cfg.sdLoggingMode);
			cmd = AppendIntParam(cmd, cfg.connectivityMode);
			cmd = AppendUIntParam(cmd, cfg.p2pConfig.destinationAddrHigh);
			cmd = AppendUIntParam(cmd, cfg.p2pConfig.destinationAddrLow);
			cmd = AppendStringParam(cmd, cfg.telemetryConfig.telemetryServer);
			cmd = AppendStringParam(cmd, cfg.telemetryConfig.telemetryDeviceId);

			wxString result = SendCommand(serialPort, cmd);
			CheckThrowResult(result);

			cmd = "setCellCfg";
			cmd = AppendStringParam(cmd, cfg.cellularConfig.apnHost);
			cmd = AppendStringParam(cmd, cfg.cellularConfig.apnUser);
			cmd = AppendStringParam(cmd, cfg.cellularConfig.apnPass);
			result = SendCommand(serialPort, cmd);
			CheckThrowResult(result);

			updateWriteConfigPct(++updateCount,callback);
		}
		{
			writeScript(config->luaScript);
			updateWriteConfigPct(++updateCount,callback);
		}
		wxTimeSpan dur = wxDateTime::UNow() - start;
		VERBOSE(FMT("write config in %f",dur.GetMilliseconds().ToDouble()));
		callback->WriteConfigComplete(true,"");
	}
	catch(CommException &e){
		callback->WriteConfigComplete(false, e.GetErrorMessage());
	}
	CloseSerialPort();
}

void RaceAnalyzerComm::flashCurrentConfig(){
	try{
		wxDateTime start = wxDateTime::UNow();
		CComm *serialPort = GetSerialPort();
		if (NULL==serialPort) throw CommException(CommException::OPEN_PORT_FAILED);
		wxString cmd = "flashLoggerCfg";

		wxString result = SendCommand(serialPort, cmd);
		CheckThrowResult(result);
		wxTimeSpan dur = wxDateTime::UNow() - start;
		VERBOSE(FMT("flash config in %f",dur.GetMilliseconds().ToDouble()));
	}
	catch(CommException &e){
		VERBOSE(FMT("Error during flash: %s", e.GetErrorMessage()));
	}
	CloseSerialPort();
}

void RaceAnalyzerComm::calibrateAccelZero(){
	try{
		wxDateTime start = wxDateTime::UNow();
		CComm *serialPort = GetSerialPort();
		if (NULL==serialPort) throw CommException(CommException::OPEN_PORT_FAILED);
		wxString cmd = "calibrateAccelZero";

		wxString result = SendCommand(serialPort, cmd);
		CheckThrowResult(result);
		wxTimeSpan dur = wxDateTime::UNow() - start;
		VERBOSE(FMT("calibrateAccelZero in %f",dur.GetMilliseconds().ToDouble()));
	}
	catch(CommException &e){
		VERBOSE(FMT("Error during calibrateAccelZero: %s", e.GetErrorMessage()));
		throw e;
	}
	CloseSerialPort();
}

wxString RaceAnalyzerComm::AppendStringParam(wxString &cmd, wxString param){
	param.Replace(" ","_",true);
	return cmd + wxString::Format(" \"%s\"", param.ToAscii());
}

wxString RaceAnalyzerComm::AppendFloatParam(wxString &cmd, float param){
	return cmd + wxString::Format(" %f", param);
}

wxString RaceAnalyzerComm::AppendIntParam(wxString &cmd, int param){
	return cmd + wxString::Format(" %d", param);
}

wxString RaceAnalyzerComm::AppendUIntParam(wxString &cmd, unsigned int param){
	return cmd + wxString::Format(" %u", param);
}

wxString RaceAnalyzerComm::AppendChannelConfig(wxString &cmd, ChannelConfig &channelConfig){
	cmd = AppendStringParam(cmd, channelConfig.label);
	cmd = AppendStringParam(cmd, channelConfig.units);
	cmd = AppendIntParam(cmd, channelConfig.sampleRate);
	return cmd;
}

AsyncRaceAnalyzerComm::AsyncRaceAnalyzerComm(RaceAnalyzerComm *comm, RaceCaptureConfig *config, RaceAnalyzerCommCallback *callback) :
		m_comm(comm),
		m_config(config),
		m_callback(callback),
		m_action(ACTION_NONE)
{
	wxThread::Create();
}

void AsyncRaceAnalyzerComm::Run(int action){
	m_action = action;
	wxThread::Run();
}

void AsyncRaceAnalyzerComm::RunReadConfig(void){
	Run(ACTION_READ_CONFIG);
}

void AsyncRaceAnalyzerComm::RunWriteConfig(void){
	Run(ACTION_WRITE_CONFIG);
}

void AsyncRaceAnalyzerComm::RunFlashConfig(void){
	Run(ACTION_FLASH_CONFIG);
}

void * AsyncRaceAnalyzerComm::Entry(){
	switch(m_action){
	case ACTION_READ_CONFIG:
		m_comm->readConfig(m_config,m_callback);
		break;
	case ACTION_WRITE_CONFIG:
		m_comm->writeConfig(m_config,m_callback);
		break;
	case ACTION_FLASH_CONFIG:
		m_comm->flashCurrentConfig();
		break;
	default:
		break;
	}
	return NULL;
}
