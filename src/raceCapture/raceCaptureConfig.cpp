/*
 * raceCaptureConfig.cpp
 *
 *  Created on: Apr 28, 2012
 *      Author: brent
 */
#include "raceCapture/raceCaptureConfig.h"
#include "appOptions.h"
#include "configuration/defaultConfigJson.h"

#define DEFAULT_TELEMETRY_SERVER "54.245.229.2";

SampleRates ChannelConfig::sampleRates;

ChannelConfig::ChannelConfig(): sampleRate(sample_disabled){}

SampleRates ChannelConfig::GetSampleRates(){

//	const wxString SAMPLE_RATES[] = {"Disabled","1","5","10","20","30","50","100"};
	//TODO make threadsafe or change design
	if (ChannelConfig::sampleRates.Count() == 0){
		ChannelConfig::sampleRates.Add(sample_disabled);
		ChannelConfig::sampleRates.Add(sample_1Hz);
		ChannelConfig::sampleRates.Add(sample_5Hz);
		ChannelConfig::sampleRates.Add(sample_10Hz);
		ChannelConfig::sampleRates.Add(sample_20Hz);
		ChannelConfig::sampleRates.Add(sample_30Hz);
		ChannelConfig::sampleRates.Add(sample_50Hz);
		ChannelConfig::sampleRates.Add(sample_100Hz);
	}
	return sampleRates;
}

RaceCaptureConfig::RaceCaptureConfig(){
	SetDefaults();
}
void RaceCaptureConfig::SetDefaults(){

	Object jsonConfig;
	std::stringstream stream;

	for (size_t i = 0; i < strlen(defaultConfig);i++){
		stream.put(defaultConfig[i]);
	}
	Reader::Read(jsonConfig, stream);
	FromJson(jsonConfig);
}

void RaceCaptureConfig::ChannelConfigFromJson(ChannelConfig &channelConfig, const Object &channelConfigJson){

	SetStringField(channelConfig.label, channelConfigJson, "label");
	SetStringField(channelConfig.units, channelConfigJson, "units");
	channelConfig.sampleRate = (sample_rate_t)((Number)channelConfigJson["sampleRate"]).Value();
}

void RaceCaptureConfig::GpsTargetFromJson(GpsTarget &target, const Object &gpsTargetJson){
	try{
		target.latitude = ((Number)gpsTargetJson["latitude"]).Value();
		target.longitude = ((Number)gpsTargetJson["longitude"]).Value();
		target.targetRadius = ((Number)gpsTargetJson["targetRadius"]).Value();
	}
	catch(json::Exception &e){
		wxLogError("Error parsing GpsTargets: %s", e.what());
	}

}

void RaceCaptureConfig::PopulateGpsConfig(Object &gpsRoot){
	try{
		gpsConfig.gpsInstalled = ((Number)gpsRoot["gpsInstalled"]).Value();

		GpsTargetFromJson(gpsConfig.startFinishTarget,  gpsRoot["startFinishTarget"]);
		GpsTargetFromJson(gpsConfig.splitTarget, gpsRoot["splitTarget"]);
		ChannelConfigFromJson(gpsConfig.latitudeCfg,gpsRoot["latitude"]);
		ChannelConfigFromJson(gpsConfig.longitudeCfg,gpsRoot["longitude"]);
		ChannelConfigFromJson(gpsConfig.speedCfg,gpsRoot["speed"]);
		ChannelConfigFromJson(gpsConfig.timeCfg,gpsRoot["time"]);
		ChannelConfigFromJson(gpsConfig.satellitesCfg,gpsRoot["satellites"]);
		ChannelConfigFromJson(gpsConfig.lapCountCfg,gpsRoot["lapCount"]);
		ChannelConfigFromJson(gpsConfig.lapTimeCfg,gpsRoot["lapTime"]);
		ChannelConfigFromJson(gpsConfig.splitTimeCfg,gpsRoot["splitTime"]);
	}
	catch(json::Exception &e){
		wxLogError("Error parsing GpsConfig: %s", e.what());
	}

}

void RaceCaptureConfig::PopulateAnalogConfig(Array &analogRoot){

	try{
		Array::const_iterator itAnalog(analogRoot.Begin());
		int i=0;
		for (; itAnalog != analogRoot.End(); ++itAnalog){
			const Object &analog = *itAnalog;
			AnalogConfig &analogConfig = analogConfigs[i++];
			ChannelConfigFromJson(analogConfig.channelConfig,analog["channelConfig"]);
			analogConfig.loggingPrecision = ((Number)analog["loggingPrecision"]);
			analogConfig.linearScaling = ((Number)analog["linearScaling"]);
			analogConfig.scalingMode = (scaling_mode_t)((int)((Number)analog["scalingMode"]));

			Array scalingMapJson = analog["scalingMap"];
			ScalingMap &scalingMap = analogConfig.scalingMap;

			Array::const_iterator itScalingMap(scalingMapJson.Begin());

			int ii=0;
			for (; itScalingMap != scalingMapJson.End();++itScalingMap){
				const Object &scalingMapItem = *itScalingMap;
				scalingMap.rawValues[ii] = ((Number)scalingMapItem["raw"]);
				scalingMap.scaledValue[ii] = ((Number)scalingMapItem["scaled"]);
				ii++;
			}
		}
	}
	catch(json::Exception &e){
		wxLogError("Error parsing AnalogConfig: %s", e.what());
	}

}

void RaceCaptureConfig::PopulatePulseInputConfig(Array &pulseInputRoot){
	try{
		Array::const_iterator itPulseInput(pulseInputRoot.Begin());
		int i=0;
		for (; itPulseInput != pulseInputRoot.End(); ++itPulseInput){
			const Object &pulseInput = *itPulseInput;
			TimerConfig &pulseInputConfig = timerConfigs[i++];
			ChannelConfigFromJson(pulseInputConfig.channelConfig,pulseInput["channelConfig"]);
			pulseInputConfig.loggingPrecision = ((Number)pulseInput["loggingPrecision"]);
			pulseInputConfig.slowTimerEnabled = ((Number)pulseInput["slowTimer"]);
			pulseInputConfig.mode = (timer_mode_t)((int)((Number)pulseInput["mode"]));
			pulseInputConfig.pulsePerRev = ((Number)pulseInput["pulsePerRev"]);
			pulseInputConfig.timerDivider = ((Number)pulseInput["timerDivider"]);
			pulseInputConfig.scaling = ((Number)pulseInput["scaling"]);
		}
	}
	catch(json::Exception &e){
		wxLogError("Error parsing PulseInputConfig: %s", e.what());
	}
}

void RaceCaptureConfig::PopulateAccelConfig(Array &accelRoot){
	try{
		Array::const_iterator itAccel(accelRoot.Begin());
		int i=0;
		for (; itAccel!= accelRoot.End(); ++itAccel){
			const Object &accel = *itAccel;
			AccelConfig &accelConfig = accelConfigs[i++];
			ChannelConfigFromJson(accelConfig.channelConfig,accel["channelConfig"]);
			accelConfig.mode = (accel_mode_t)((int)((Number)accel["mode"]));
			accelConfig.channel = (accel_channel_t)(int)((Number)accel["mapping"]);
			accelConfig.zeroValue = (int)((Number)accel["zeroValue"]);
		}
	}
	catch(json::Exception &e){
		wxLogError("Error parsing AccelConfig: %s", e.what());
	}
}

void RaceCaptureConfig::PopulatePulseOutputConfig(Array &pulseOutputRoot){
	try{
		Array::const_iterator itPulseOutput(pulseOutputRoot.Begin());
		int i=0;
		for (; itPulseOutput != pulseOutputRoot.End(); ++itPulseOutput){
			const Object &pulseOutput = *itPulseOutput;
			PwmConfig &pulseOutputConfig = pwmConfigs[i++];
			ChannelConfigFromJson(pulseOutputConfig.channelConfig, pulseOutput["channelConfig"]);
			pulseOutputConfig.loggingPrecision = Number(pulseOutput["loggingPrecision"]);
			pulseOutputConfig.outputMode =  (pwm_output_mode_t)(int)Number(pulseOutput["outputMode"]);
			pulseOutputConfig.loggingMode = (pwm_logging_mode_t)(int)Number(pulseOutput["loggingMode"]);
			pulseOutputConfig.startupDutyCycle = Number(pulseOutput["startupDutyCycle"]);
			pulseOutputConfig.startupPeriod = Number(pulseOutput["startupPeriod"]);
			pulseOutputConfig.voltageScaling = Number(pulseOutput["voltageScaling"]);
		}
	}
	catch(json::Exception &e){
		wxLogError("Error parsing PulseOutputConfig: %s", e.what());
	}
}

void RaceCaptureConfig::PopulateGpioConfig(Array &gpioConfigRoot){
	try{
		Array::const_iterator itGpio(gpioConfigRoot.Begin());
		int i=0;
		for (; itGpio != gpioConfigRoot.End(); ++itGpio){
			const Object &gpio = *itGpio;
			GpioConfig &gpioConfig = gpioConfigs[i++];
			ChannelConfigFromJson(gpioConfig.channelConfig,gpio["channelConfig"]);
			gpioConfig.mode = (gpio_mode_t)(int)Number(gpio["mode"]);
		}
	}
	catch(json::Exception &e){
		wxLogError("Error parsing GpioCOnfig: %s", e.what());
	}
}

void RaceCaptureConfig::SetStringField(wxString &field, const Object &object, const char * name){
	try{
		const String &val = object[name];
		field = val.Value();
	}
	catch (const Exception& e){}
}

void RaceCaptureConfig::PopulateOutputConfig(Object &outputConfig){
	try{
		connectivityConfig.sdLoggingMode = (sd_logging_mode_t)(int)Number(outputConfig["sdLoggingMode"]);
		connectivityConfig.connectivityMode = (connectivity_mode_t)(int)Number(outputConfig["telemetryMode"]);

		SetStringField(connectivityConfig.telemetryConfig.telemetryServer, outputConfig, "telemetryServer");
		SetStringField(connectivityConfig.telemetryConfig.telemetryDeviceId, outputConfig, "telemetryDeviceId");

		connectivityConfig.p2pConfig.destinationAddrHigh = (unsigned int)Number(outputConfig["p2pDestAddrHigh"]);
		connectivityConfig.p2pConfig.destinationAddrLow = (unsigned int)Number(outputConfig["p2pDestinationAddrLow"]);
	}
	catch(json::Exception &e){
		wxLogError("Error parsing OutputConfig: %s", e.what());
	}
}

void RaceCaptureConfig::PopulateCellConfig(Object &cellConfig){
	try{
		SetStringField(connectivityConfig.cellularConfig.apnHost, cellConfig, "apnHost");
		SetStringField(connectivityConfig.cellularConfig.apnUser, cellConfig, "apnUser");
		SetStringField(connectivityConfig.cellularConfig.apnPass, cellConfig, "apnPass");
	}catch(json::Exception &e){
		wxLogError("Error parsing CellConfig: %s", e.what());
	}
}

void RaceCaptureConfig::PopulateBtConfig(Object &btConfig){
	try{
		SetStringField(connectivityConfig.bluetoothConfig.deviceName, btConfig, "user");
		SetStringField(connectivityConfig.bluetoothConfig.deviceName, btConfig, "pass");
	}catch(json::Exception &e){
		wxLogError("Error parsing ConnectivityConfig: %s", e.what());
	}
}

void RaceCaptureConfig::PopulateConnectivityConfig(Object &connectivityConfig){
	try{
		PopulateCellConfig(connectivityConfig["cellCfg"]);
		PopulateBtConfig(connectivityConfig["btCfg"]);
	}catch(json::Exception &e){
		wxLogError("Error parsing ConnectivityConfig: %s", e.what());
	}
}

void RaceCaptureConfig::PopulateAutomationConfig(Object &automationConfig){
	try{
		SetStringField(luaScript, automationConfig, "script");
	}
	catch(json::Exception &e){
		wxLogError("Error parsing AutomationConfig: %s", e.what());
	}
}

void RaceCaptureConfig::FromJson(Object root){
	try{
		PopulateGpsConfig(root["gpsConfig"]);
		PopulateAnalogConfig(root["analogConfigs"]);
		PopulatePulseInputConfig(root["pulseInputConfigs"]);
		PopulateAccelConfig(root["accelConfigs"]);
		PopulatePulseOutputConfig(root["pulseOutputConfigs"]);
		PopulateGpioConfig(root["gpioConfigs"]);
		PopulateOutputConfig(root["outputConfig"]);
		PopulateConnectivityConfig(root["connectivityConfig"]);

		PopulateAutomationConfig(root["automation"]);
	}
	catch(json::Exception &e){
		wxLogError(wxString::Format("error parsing root of file: %s", e.what()));
	}
}

Object RaceCaptureConfig::ChannelConfigToJson(ChannelConfig &channelConfig){
	Object cfg;
	cfg["label"] = String(channelConfig.label.ToAscii());
	cfg["units"] = String(channelConfig.units.ToAscii());
	cfg["sampleRate"] = Number(channelConfig.sampleRate);
	return cfg;
}

Object RaceCaptureConfig::GpsTargetToJson(GpsTarget &gpsTarget){
	Object cfg;
	cfg["latitude"] = Number(gpsTarget.latitude);
	cfg["longitude"] = Number(gpsTarget.longitude);
	cfg["targetRadius"] = Number(gpsTarget.targetRadius);
	return cfg;
}

Object RaceCaptureConfig::GpsConfigToJson(){
	Object cfg;
	cfg["gpsInstalled"] = Number(gpsConfig.gpsInstalled);
	cfg["startFinishTarget"] = GpsTargetToJson(gpsConfig.startFinishTarget);
	cfg["splitTarget"] = GpsTargetToJson(gpsConfig.splitTarget);
	cfg["latitude"] = ChannelConfigToJson(gpsConfig.latitudeCfg);
	cfg["longitude"] = ChannelConfigToJson(gpsConfig.longitudeCfg);
	cfg["speed"] = ChannelConfigToJson(gpsConfig.speedCfg);
	cfg["time"]  = ChannelConfigToJson(gpsConfig.timeCfg);
	cfg["satellites"] = ChannelConfigToJson(gpsConfig.satellitesCfg);
	cfg["lapCount"] = ChannelConfigToJson(gpsConfig.lapCountCfg);
	cfg["lapTime"] = ChannelConfigToJson(gpsConfig.lapTimeCfg);
	cfg["splitTime"] = ChannelConfigToJson(gpsConfig.splitTimeCfg);
	return cfg;
}

Array RaceCaptureConfig::AnalogConfigToJson(){
	Array analogConfigs;
	for (int i=0; i < CONFIG_ANALOG_CHANNELS; i++){
		Object cfg;
		AnalogConfig &analogConfig = this->analogConfigs[i];
		cfg["channelConfig"] = ChannelConfigToJson(analogConfig.channelConfig);
		cfg["loggingPrecision"] = Number(analogConfig.loggingPrecision);
		cfg["linearScaling"] = Number(analogConfig.linearScaling);
		cfg["scalingMode"] = Number(analogConfig.scalingMode);

		Array scalingMap;
		for (int m = 0; m < CONFIG_ANALOG_SCALING_BINS; m++ ){
			Object mapItem;
			mapItem["raw"] = Number(analogConfig.scalingMap.rawValues[m]);
			mapItem["scaled"] = Number(analogConfig.scalingMap.scaledValue[m]);
			scalingMap.Insert(mapItem);
		}
		cfg["scalingMap"] = scalingMap;
		analogConfigs.Insert(cfg);
	}
	return analogConfigs;
}

Array RaceCaptureConfig::PulseInputConfigToJson(){
	Array pulseInputConfigs;
	for (int i=0; i < CONFIG_TIMER_CHANNELS; i++){
		Object cfg;
		TimerConfig &timerConfig = this->timerConfigs[i];
		cfg["channelConfig"] = ChannelConfigToJson(timerConfig.channelConfig);
		cfg["loggingPrecision"] = Number(timerConfig.loggingPrecision);
		cfg["slowTimer"] = Number(timerConfig.slowTimerEnabled);
		cfg["mode"] = Number(timerConfig.mode);
		cfg["pulsePerRev"] = Number(timerConfig.pulsePerRev);
		cfg["timerDivider"] = Number(timerConfig.timerDivider);
		cfg["scaling"] = Number(timerConfig.scaling);
		pulseInputConfigs.Insert(cfg);
	}
	return pulseInputConfigs;
}

Array RaceCaptureConfig::AccelConfigToJson(){
	Array accelConfigs;
	for (int i=0; i < CONFIG_ACCEL_CHANNELS; i++){
		Object cfg;
		AccelConfig &accelConfig = this->accelConfigs[i];
		cfg["channelConfig"] = ChannelConfigToJson(accelConfig.channelConfig);
		cfg["mode"] = Number(accelConfig.mode);
		cfg["mapping"] = Number(accelConfig.channel);
		cfg["zeroValue"] = Number(accelConfig.zeroValue);
		accelConfigs.Insert(cfg);
	}
	return accelConfigs;
}

Array RaceCaptureConfig::PulseOutputConfigToJson(){
	Array pulseOutputConfigs;
	for (int i=0; i < CONFIG_ANALOG_PULSE_CHANNELS; i++){
		Object cfg;
		PwmConfig &pwmConfig = this->pwmConfigs[i];
		cfg["channelConfig"] = ChannelConfigToJson(pwmConfig.channelConfig);
		cfg["loggingPrecision"] = Number(pwmConfig.loggingPrecision);
		cfg["outputMode"] = Number(pwmConfig.outputMode);
		cfg["loggingMode"] = Number(pwmConfig.loggingMode);
		cfg["startupDutyCycle"] = Number(pwmConfig.startupDutyCycle);
		cfg["startupPeriod"] = Number(pwmConfig.startupPeriod);
		cfg["voltageScaling"] = Number(pwmConfig.voltageScaling);
		pulseOutputConfigs.Insert(cfg);
	}
	return pulseOutputConfigs;
}

Array RaceCaptureConfig::GpioConfigToJson(){
	Array gpioConfigs;
	for (int i=0; i < CONFIG_GPIO_CHANNELS; i++){
		Object cfg;
		GpioConfig &gpioConfig = this->gpioConfigs[i];
		cfg["channelConfig"] = ChannelConfigToJson(gpioConfig.channelConfig);
		cfg["mode"] = Number(gpioConfig.mode);
		gpioConfigs.Insert(cfg);
	}
	return gpioConfigs;
}

Object RaceCaptureConfig::OutputConfigToJson(){
	Object cfg;
	cfg["sdLoggingMode"] = Number(connectivityConfig.sdLoggingMode);
	cfg["telemetryMode"] = Number(connectivityConfig.connectivityMode);
	cfg["telemetryServer"] = String(connectivityConfig.telemetryConfig.telemetryServer.ToAscii());
	cfg["telemetryDeviceId"] = String(connectivityConfig.telemetryConfig.telemetryDeviceId.ToAscii());
	cfg["p2pDestAddrHigh"] = Number(connectivityConfig.p2pConfig.destinationAddrHigh);
	cfg["p2pDestAddrLow"] = Number(connectivityConfig.p2pConfig.destinationAddrLow);
	return cfg;
}



Object RaceCaptureConfig::ConnectivityConfigToJson(){
	Object cfg;
	cfg["cellCfg"] = CellularConfigToJson();
	cfg["btCfg"] = BluetoothConfigToJson();
	return cfg;
}

Object RaceCaptureConfig::BluetoothConfigToJson(){
	Object cfg;
	cfg["name"] = String(connectivityConfig.bluetoothConfig.deviceName.ToAscii());
	cfg["pass"] = String(connectivityConfig.bluetoothConfig.passcode.ToAscii());
	return cfg;
}

Object RaceCaptureConfig::CellularConfigToJson(){
	Object cfg;
	cfg["apnHost"] = String(connectivityConfig.cellularConfig.apnHost.ToAscii());
	cfg["apnUser"] = String(connectivityConfig.cellularConfig.apnUser.ToAscii());
	cfg["apnPass"] = String(connectivityConfig.cellularConfig.apnPass.ToAscii());
	return cfg;
}

Object RaceCaptureConfig::ScriptToJson(){
	Object cfg;
	cfg["script"] = String(luaScript.ToAscii());
	return cfg;
}

Object RaceCaptureConfig::ToJson(void){

	Object objRoot;
	objRoot["configVersion"] = String(CONFIG_VERSION_1);
	objRoot["gpsConfig"] = GpsConfigToJson();
	objRoot["analogConfigs"] = AnalogConfigToJson();
	objRoot["pulseInputConfigs"] = PulseInputConfigToJson();
	objRoot["accelConfigs"] = AccelConfigToJson();
	objRoot["pulseOutputConfigs"] = PulseOutputConfigToJson();
	objRoot["gpioConfigs"] = GpioConfigToJson();
	objRoot["outputConfig"] = OutputConfigToJson();
	objRoot["connectivityConfig"] = ConnectivityConfigToJson();

	objRoot["automation"] = ScriptToJson();
	return objRoot;
}
