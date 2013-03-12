
#include "appOptions.h"
#include "wx/config.h"
#include "wx/tokenzr.h"

#define DEFAULT_COM_PORT 						1
#define DEFAULT_AUTO_LOAD_CONFIG 				true
#define DEFAULT_DATALOG_CHANNEL_COUNT 			-1
#define DEFAULT_DATALOG_CHANNEL_TYPES_COUNT 	-1
#define DEFAULT_ANALOG_GAUGE_TYPE_COUNT			-1
#define DEFAULT_DIGITAL_GAUGE_TYPE_COUNT		-1


#define DEFAULT_CHART_COLORS					"0x003AC1,0xFF5D00,0xFFC700,0x607DC1,0xFFAE7F,0xFFE37F,0x919FC1,0xFFD6BF,0xFFF1BF"
#define CHART_COLOR_STRING_DELIMITER			","

#define CONFIG_KEY_COM_PORT 					"COM_Port"
#define CONFIG_KEY_AUTO_LOAD					"AutoLoadConfig"
#define CONFIG_KEY_DATALOG_CHANNEL				"DatalogChannel_"
#define CONFIG_KEY_DATALOG_CHANNEL_TYPE			"DatalogChannelType_"
#define CONFIG_KEY_DATALOG_CHANNEL_COUNT		"DatalogChannelCount"
#define CONFIG_KEY_DATALOG_CHANNEL_TYPE_COUNT	"DatalogChannelTypeCount"
#define CONFIG_KEY_CHART_COLORS					"ChartColors"

#define CONFIG_KEY_ANALOG_GAUGE_TYPE_COUNT		"AnalogGaugeTypeCount"
#define	CONFIG_KEY_ANALOG_GAUGE_TYPE			"AnalogGaugeType_"
#define CONFIG_KEY_ANALOG_GAUGE_MINOR_TICK		"AnalogGaugeTypeMinorTick_"
#define	CONFIG_KEY_ANALOG_GAUGE_MAJOR_TICK		"AnalogGaugeTypeMajorTick_"

#define CONFIG_KEY_DIGITAL_GAUGE_TYPE_COUNT		"DigitalGaugeTypeCount"
#define CONFIG_KEY_DIGITAL_GAUGE_TYPE			"DigitalGaugeType_"
#define CONFIG_KEY_DIGITAL_GAUGE_DIGITS			"DigitalGaugeTypeDigits_"
#define CONFIG_KEY_DIGITAL_GAUGE_PRECISION		"DigitalGaugeTypePrecision_"



#include <wx/arrimpl.cpp> // this is a magic incantation which must be done!
WX_DEFINE_OBJARRAY(ChartColors);

AppOptions::AppOptions()
	: 	m_serialPort(DEFAULT_COM_PORT) ,
		m_autoLoadConfig(DEFAULT_AUTO_LOAD_CONFIG)
{}

int AppOptions::GetSerialPort(){
	return m_serialPort;
}

void AppOptions::SetSerialPort(int comPort){
	m_serialPort = comPort;
}

bool & AppOptions::GetAutoLoadConfig(){
	return m_autoLoadConfig;
}

void AppOptions::SetAutoLoadConfig(bool autoConnect){
	m_autoLoadConfig = autoConnect;
}

ChartColors & AppOptions::GetChartColors(){
	return m_chartColors;
}

void AppOptions::SaveAppOptions(){
	wxConfig config(RACE_ANALYZER_APP_NAME);

	config.Write(CONFIG_KEY_COM_PORT,m_serialPort);
	config.Write(CONFIG_KEY_AUTO_LOAD, m_autoLoadConfig);
}

void AppOptions::LoadAppOptions(){
	wxConfig config(RACE_ANALYZER_APP_NAME);

	config.Read(CONFIG_KEY_COM_PORT,&m_serialPort, DEFAULT_COM_PORT);
	config.Read(CONFIG_KEY_AUTO_LOAD, &m_autoLoadConfig, DEFAULT_AUTO_LOAD_CONFIG);

	int standardChannelCount = 0;
	config.Read(CONFIG_KEY_DATALOG_CHANNEL_COUNT, &standardChannelCount, DEFAULT_DATALOG_CHANNEL_COUNT );

	if (DEFAULT_DATALOG_CHANNEL_COUNT == standardChannelCount){
		LoadAllStandardChannels(m_allStandardChannels);
		LoadStandardAccelChannels(m_standardAccelChannels);
		LoadStandardGpsChannels(m_standardGpsChannels);
		LoadStandardAnalogChannels(m_standardAnalogChannels);
		LoadStandardFrequencyChannels(m_standardPulseInputChannels);
		LoadStandardPwmChannels(m_standardAnalogPwmOutputChannels);
		LoadStandardGpioChannels(m_standardGpioChannels);
	}

	int standardChannelTypesCount = 0;
	config.Read(CONFIG_KEY_DATALOG_CHANNEL_TYPE_COUNT, &standardChannelTypesCount, DEFAULT_DATALOG_CHANNEL_TYPES_COUNT );

	if (DEFAULT_DATALOG_CHANNEL_TYPES_COUNT == standardChannelTypesCount){
		LoadStandardChannelTypes(m_standardChannelTypes);
	}

	wxString chartColorString;
	config.Read(CONFIG_KEY_CHART_COLORS, &chartColorString, DEFAULT_CHART_COLORS);
	wxStringTokenizer colorSplitter(chartColorString,CHART_COLOR_STRING_DELIMITER);
	while (colorSplitter.HasMoreTokens()){
		wxString tok = colorSplitter.GetNextToken();
		long int color;
		tok.ToLong(&color,16);
		wxColour chartColor((color & 0xff0000) >> 16, (color & 0xff00) >> 8, color & 0xff);
		m_chartColors.Add(chartColor);
	}

	int analogGaugeTypeCount = 0;
	config.Read(CONFIG_KEY_ANALOG_GAUGE_TYPE_COUNT, &analogGaugeTypeCount, DEFAULT_ANALOG_GAUGE_TYPE_COUNT);
	if (DEFAULT_ANALOG_GAUGE_TYPE_COUNT == analogGaugeTypeCount){
		LoadDefaultAnalogGaugeTypes(m_analogGaugeTypes);
	}

	int digitalGaugeTypeCount = 0;
	config.Read(CONFIG_KEY_DIGITAL_GAUGE_TYPE_COUNT, &digitalGaugeTypeCount, DEFAULT_DIGITAL_GAUGE_TYPE_COUNT);
	if (DEFAULT_DIGITAL_GAUGE_TYPE_COUNT == digitalGaugeTypeCount){
		LoadDefaultDigitalGaugeTypes(m_digitalGaugeTypes);
	}

}

DatalogChannels & AppOptions::GetStandardAccelChannels(){
	return m_standardAccelChannels;
}

DatalogChannels & AppOptions::GetStandardGpsChannels(){
	return m_standardGpsChannels;
}

DatalogChannels & AppOptions::GetStandardAnalogChannels(){
	return m_standardAnalogChannels;
}

DatalogChannels & AppOptions::GetStandardPulseInputChannels(){
	return m_standardPulseInputChannels;
}

DatalogChannels & AppOptions::GetStandardAnalogPwmOutputChannels(){
	return m_standardAnalogPwmOutputChannels;
}

DatalogChannels & AppOptions::GetStandardGpioChannels(){
	return m_standardGpioChannels;
}

DatalogChannels & AppOptions::GetAllStandardChannels(){
	return m_allStandardChannels;
}

DatalogChannelTypes & AppOptions::GetStandardChannelTypes(){
	return m_standardChannelTypes;
}

AnalogGaugeTypes & AppOptions::GetAnalogGaugeTypes(){
	return m_analogGaugeTypes;
}

DigitalGaugeTypes & AppOptions::GetDigitalGaugeTypes(){
	return m_digitalGaugeTypes;
}

DatalogChannelType AppOptions::GetDefaultUnknownChannelType(wxString name){
	DatalogChannelType channelType(name, "", 0, 0, 10000, 2);
	return channelType;
}

DatalogChannelType AppOptions::GetChannelTypeForChannel(ViewChannel &channel){
	//search through this. maybe change this to a wxHashMap for better efficiency
	size_t count = m_allStandardChannels.Count();
	for (size_t i = 0; i < count; i++){
		DatalogChannel &datalogChannel = m_allStandardChannels[i];
		if (datalogChannel.name == channel.channelName){
			int typeId = datalogChannel.typeId;
			return m_standardChannelTypes[typeId];
		}
	}
	return GetDefaultUnknownChannelType(channel.channelName);
}

void AppOptions::LoadStandardChannelTypes(DatalogChannelTypes &types){

	types.Add( DatalogChannelType("Raw","Number", 0, 0, 1024, 0) );
	types.Add( DatalogChannelType("GForce", "G", 5, -2.0, 2.0, 2) );
	types.Add( DatalogChannelType("Rotation", "Deg/Sec", 5, -300.0, 300.0, 1) );
	types.Add( DatalogChannelType("TimeDate", "UTC", 0, 0, 0, 2) );
	types.Add( DatalogChannelType("Count", "Count", 0,0, 1000.0, 0) );
	types.Add( DatalogChannelSystemTypes::GetLatitudeChannelType());
	types.Add( DatalogChannelSystemTypes::GetLongitudeChannelType());
	types.Add( DatalogChannelType("Speed", "MPH", 0, 0, 240.0, 0) );
	types.Add( DatalogChannelType("Volts", "Volts", 0, 0, 25.0, 2) );
	types.Add( DatalogChannelType("Pressure", "PSI", 0, 0, 300.0, 2) );
	types.Add( DatalogChannelType("Temperature", "F", 0, 0, 300.0, 0) );
	types.Add( DatalogChannelType("Frequency", "Hz", 0, 0, 2000.0, 0) );
	types.Add( DatalogChannelType("RPM", "RPM", 0, 0, 10000.0, 0) );
	types.Add( DatalogChannelType("Duration", "Ms.", 0, 0, 100.0, 0) );
	types.Add( DatalogChannelType("Percent", "%", 0, 0, 100.0, 2) );
	types.Add( DatalogChannelType("Digital", "Off/On", 0, 0, 1, 0) );
	types.Add( DatalogChannelType("Time", "Seconds", 0, 0, 1000, 2));
	types.Add( DatalogChannelType("Distance", "mm", 0, 0, 1000, 2));
}

void AppOptions::LoadStandardAccelChannels(DatalogChannels &channels){
	//Accelerometer inputs
	channels.Add( DatalogChannel("AccelX", 1, "Accelerometer X Axis") );
	channels.Add( DatalogChannel("AccelY", 1, "Accelerometer Y Axis") );
	channels.Add( DatalogChannel("AccelZ", 1, "Accelerometer Z Axis") );
	channels.Add( DatalogChannel("Yaw", 2, "Accelerometer Z Axis Rotation") );
}

void AppOptions::LoadStandardGpsChannels(DatalogChannels &channels){
	//GPS inputs
	channels.Add( DatalogChannel("Time", 3, "GPS Time in UTC") );
	channels.Add( DatalogChannel("GpsSats", 4, "Number of Active Satellites") );
	channels.Add( DatalogChannel("Latitude", 5, "GPS Latitude in Degrees") );
	channels.Add( DatalogChannel("Longitude", 6,"GPS Longitude in Degrees") );
	channels.Add( DatalogChannel("Speed", 7, "GPS Speed") );
	channels.Add( DatalogChannel("LapCount",4, "Lap Count" ));
	channels.Add( DatalogChannel("LapTime", 16, "Lap Time" ));
	channels.Add( DatalogChannel("SplitTime", 16, "Split Time" ));
}

void AppOptions::LoadStandardAnalogChannels(DatalogChannels &channels){
	//Analog inputs
	channels.Add( DatalogChannel("FuelLevel", 9, "Fuel Level") );
	channels.Add( DatalogChannel("Coolant", 10, "Engine Coolant Temperature") );
	channels.Add( DatalogChannel("OilPress", 9, "Oil Pressure") );
	channels.Add( DatalogChannel("OilTemp", 10, "Engine Oil Temperature") );
	channels.Add( DatalogChannel("AFR", 0, "Air/Fuel Ratio") );
	channels.Add( DatalogChannel("IAR", 10, "Inlet Air Temperature") );
	channels.Add( DatalogChannel("MAP", 10, "Manifold Air Pressure") );
	channels.Add( DatalogChannel("EGT", 10, "Exhaust Gas Temperature") );
	channels.Add( DatalogChannel("TPS", 14, "Throttle Position") );
	channels.Add( DatalogChannel("Battery", 8, "Battery Voltage") );
	channels.Add( DatalogChannel("FuelPress", 9, "Fuel Pressure") );
	channels.Add( DatalogChannel("Steering", 15, "Steering angle") );
	channels.Add( DatalogChannel("Brake", 9, "Brake Pressure") );
	channels.Add( DatalogChannel("LF_Height", 17, "Left Front suspension height") );
	channels.Add( DatalogChannel("RF_Height", 17, "Right Front suspension height") );
	channels.Add( DatalogChannel("LR_Height", 17, "Left Rear suspension height") );
	channels.Add( DatalogChannel("RR_Height", 17, "Right Rear suspension height") );
	channels.Add( DatalogChannel("Analog1", 8, "Analog Input 1") );
	channels.Add( DatalogChannel("Analog2", 8, "Analog Input 2") );
	channels.Add( DatalogChannel("Analog3", 8, "Analog Input 3") );
	channels.Add( DatalogChannel("Analog4", 8, "Analog Input 4") );
	channels.Add( DatalogChannel("Analog5", 8, "Analog Input 5") );
	channels.Add( DatalogChannel("Analog6", 8, "Analog Input 6") );
	channels.Add( DatalogChannel("Analog7", 8, "Analog Input 7") );
}

void AppOptions::LoadStandardFrequencyChannels(DatalogChannels &channels){
	//Frequency Inputs
	channels.Add( DatalogChannel("RPM", 12, "Engine Speed in RPM") );
	channels.Add( DatalogChannel("Wheel", 12, "Wheel Speed in RPM") );
	channels.Add( DatalogChannel("Injector", 13, "Injector pulse width in Ms.") );
	channels.Add( DatalogChannel("Freq1", 11, "Frequency Input 1") );
	channels.Add( DatalogChannel("Freq2", 11, "Frequency Input 2") );
	channels.Add( DatalogChannel("Freq3", 11, "Frequency Input 3") );

}

void AppOptions::LoadStandardPwmChannels(DatalogChannels &channels){
	//Analog Outputs
	channels.Add( DatalogChannel("Vout1", 8, "Analog Output 1") );
	channels.Add( DatalogChannel("Vout2", 8, "Analog Output 2") );
	channels.Add( DatalogChannel("Vout3", 8, "Analog Output 3") );
	channels.Add( DatalogChannel("Vout4", 8, "Analog Output 4") );

	//Frequency Outputs
	channels.Add( DatalogChannel("FreqOut1", 11, "Frequency Output 1") );
	channels.Add( DatalogChannel("FreqOut2", 11, "Frequency Output 2") );
	channels.Add( DatalogChannel("FreqOut3", 11, "Frequency Output 3") );
	channels.Add( DatalogChannel("FreqOut4", 11, "Frequency Output 4") );
}

void AppOptions::LoadStandardGpioChannels(DatalogChannels &channels){
	//Digital Inputs
	channels.Add(DatalogChannel("GPI1", 15, "Digital Input 1") );
	channels.Add(DatalogChannel("GPI2", 15, "Digital Input 2") );
	channels.Add(DatalogChannel("GPI3", 15, "Digital Input 3") );

	//Digital Outputs
	channels.Add(DatalogChannel("GPO1", 15, "Digital Output 1") );
	channels.Add(DatalogChannel("GPO2", 15, "Digital Output 2") );
	channels.Add(DatalogChannel("GPO3", 15, "Digital Output 3") );
}

void AppOptions::LoadAllStandardChannels(DatalogChannels &channels){

	LoadStandardAccelChannels(channels);
	LoadStandardGpsChannels(channels);
	LoadStandardAnalogChannels(channels);
	LoadStandardFrequencyChannels(channels);
	LoadStandardPwmChannels(channels);
	LoadStandardGpioChannels(channels);
}

void AppOptions::LoadDefaultAnalogGaugeTypes(AnalogGaugeTypes &analogGaugeTypes){

	analogGaugeTypes["Raw"] = AnalogGaugeType(100, 50, 0);
	analogGaugeTypes["GForce"] = AnalogGaugeType(0.5, 0.1,2);
	analogGaugeTypes["Rotation"] = AnalogGaugeType(100.0, 50.0, 1);
	analogGaugeTypes["Count"] = AnalogGaugeType(10.0, 5.0, 1);
	analogGaugeTypes["Speed"] = AnalogGaugeType(20.0, 10.0, 0);
	analogGaugeTypes["Volts"] = AnalogGaugeType(5.0, 1.0, 2);
	analogGaugeTypes["Pressure"] = AnalogGaugeType(10.0, 5.0, 0);
	analogGaugeTypes["Temperature"] = AnalogGaugeType(10.0, 5.0, 0);
	analogGaugeTypes["Frequency"] = AnalogGaugeType(100.0, 50.0, 0);
	analogGaugeTypes["RPM"] = AnalogGaugeType(100.0, 50.0, 0);
	analogGaugeTypes["Duration"] = AnalogGaugeType(10.0, 5.0, 0);
	analogGaugeTypes["Percent"] = AnalogGaugeType(10.0, 5.0, 0);
	analogGaugeTypes["Time"] = AnalogGaugeType(1,0,0);
}

void AppOptions::LoadDefaultDigitalGaugeTypes(DigitalGaugeTypes &digitalGaugeTypes){

	digitalGaugeTypes["Raw"] = DigitalGaugeType(3,0);
	digitalGaugeTypes["GForce"] = DigitalGaugeType(1,2);
	digitalGaugeTypes["Rotation"] = DigitalGaugeType(2,2);
	digitalGaugeTypes["Count"] = DigitalGaugeType(3,0);
	digitalGaugeTypes["Speed"] = DigitalGaugeType(3,0);
	digitalGaugeTypes["Volts"] = DigitalGaugeType(2,2);
	digitalGaugeTypes["Pressure"] = DigitalGaugeType(3,0);
	digitalGaugeTypes["Temperature"] = DigitalGaugeType(3,0);
	digitalGaugeTypes["Frequency"] = DigitalGaugeType(4,0);
	digitalGaugeTypes["RPM"] = DigitalGaugeType(4,0);
	digitalGaugeTypes["Duration"] = DigitalGaugeType(3,1);
	digitalGaugeTypes["Percent"] = DigitalGaugeType(2,0);
	digitalGaugeTypes["Time"] = DigitalGaugeType(2,3);
}
