
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

DatalogChannel AppOptions::CreateGenericChannel(wxString &name, int sampleRate){
	return DatalogChannel(name, DEFAULT_CHANNEL_TYPE, "", sampleRate, true);
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
	return m_standardChannelTypes[CHANNEL_TYPE_VALUE];
}

DatalogChannelType AppOptions::GetChannelTypeForChannel(ViewChannel &channel){

	DatalogChannels::iterator it = m_allStandardChannels.find(channel.channelName);
	if (it != m_allStandardChannels.end()){
		DatalogChannelTypes::iterator it2 = m_standardChannelTypes.find(it->second.type);
		if (it2 != m_standardChannelTypes.end()){
			return it2->second;
		}
	}
	return GetDefaultUnknownChannelType(channel.channelName);
}


void AppOptions::LoadStandardChannelTypes(DatalogChannelTypes &types){
	types[CHANNEL_TYPE_VALUE] = DatalogChannelType(CHANNEL_TYPE_VALUE, "Number", 0, -1000, 1000, 0);
	types[CHANNEL_TYPE_RAW] = DatalogChannelType(CHANNEL_TYPE_RAW,"Number", 0, 0, 1024, 0);
	types[CHANNEL_TYPE_GFORCE] = DatalogChannelType(CHANNEL_TYPE_GFORCE, "G", 5, -2.0, 2.0, 2);
	types[CHANNEL_TYPE_ROTATION] = DatalogChannelType(CHANNEL_TYPE_ROTATION, "Deg/Sec", 5, -300.0, 300.0, 1);
	types[CHANNEL_TYPE_TIMEDATE] = DatalogChannelType(CHANNEL_TYPE_TIMEDATE, "UTC", 0, 0, 0, 2);
	types[CHANNEL_TYPE_COUNT] = DatalogChannelType(CHANNEL_TYPE_COUNT, "Count", 0,0, 1000.0, 0);
	types[CHANNEL_TYPE_LATITUDE] = DatalogChannelSystemTypes::GetLatitudeChannelType();
	types[CHANNEL_TYPE_LONGITUDE] = DatalogChannelSystemTypes::GetLongitudeChannelType();
	types[CHANNEL_TYPE_SPEED] = DatalogChannelType(CHANNEL_TYPE_SPEED, "MPH", 0, 0, 240.0, 0);
	types[CHANNEL_TYPE_VOLTS] = DatalogChannelType(CHANNEL_TYPE_VOLTS, "Volts", 0, 0, 25.0, 2);
	types[CHANNEL_TYPE_PRESSURE] = DatalogChannelType(CHANNEL_TYPE_PRESSURE, "PSI", 0, 0, 300.0, 2);
	types[CHANNEL_TYPE_TEMPERATURE] = DatalogChannelType(CHANNEL_TYPE_TEMPERATURE, "F", 0, 0, 300.0, 0);
	types[CHANNEL_TYPE_FREQUENCY] = DatalogChannelType(CHANNEL_TYPE_FREQUENCY, "Hz", 0, 0, 2000.0, 0);
	types[CHANNEL_TYPE_RPM] = DatalogChannelType(CHANNEL_TYPE_RPM, "RPM", 0, 0, 10000.0, 0);
	types[CHANNEL_TYPE_DURATION] = DatalogChannelType(CHANNEL_TYPE_DURATION, "Ms.", 0, 0, 100.0, 0);
	types[CHANNEL_TYPE_PERCENT] = DatalogChannelType(CHANNEL_TYPE_PERCENT, "%", 0, 0, 100.0, 2);
	types[CHANNEL_TYPE_DIGITAL] = DatalogChannelType(CHANNEL_TYPE_DIGITAL, "Off/On", 0, 0, 1, 0);
	types[CHANNEL_TYPE_TIME] = DatalogChannelType(CHANNEL_TYPE_TIME, "Seconds", 0, 0, 1000, 2);
	types[CHANNEL_TYPE_DISTANCE] = DatalogChannelType(CHANNEL_TYPE_DISTANCE, "mm", 0, 0, 1000, 2);
	types[CHANNEL_TYPE_ANGLE] = DatalogChannelType(CHANNEL_TYPE_ANGLE, "deg", 0, 0, 1000, 2);
}

#define CHANNEL_ACCELX "AccelX"
#define CHANNEL_ACCELY "AccelY"
#define CHANNEL_ACCELZ "AccelZ"
#define CHANNEL_YAW "Yaw"

void AppOptions::LoadStandardAccelChannels(DatalogChannels &channels){
	//Accelerometer inputs
	channels[CHANNEL_ACCELX] = DatalogChannel(CHANNEL_ACCELX, CHANNEL_TYPE_GFORCE, "Accelerometer X Axis");
	channels[CHANNEL_ACCELY] = DatalogChannel(CHANNEL_ACCELY, CHANNEL_TYPE_GFORCE, "Accelerometer Y Axis");
	channels[CHANNEL_ACCELZ] = DatalogChannel(CHANNEL_ACCELZ, CHANNEL_TYPE_GFORCE, "Accelerometer Z Axis");
	channels[CHANNEL_YAW] = DatalogChannel(CHANNEL_YAW, CHANNEL_TYPE_ROTATION, "Accelerometer Z Axis Rotation");
}

void AppOptions::LoadStandardGpsChannels(DatalogChannels &channels){
	//GPS inputs
	channels[CHANNEL_TIME] = DatalogChannel(CHANNEL_TIME, CHANNEL_TYPE_TIMEDATE, "GPS Time in UTC");
	channels[CHANNEL_GPS_SATS] =  DatalogChannel(CHANNEL_GPS_SATS, CHANNEL_TYPE_COUNT, "Number of Active Satellites");
	channels[CHANNEL_LATITUDE] = DatalogChannel(CHANNEL_LATITUDE, CHANNEL_TYPE_LATITUDE, "GPS Latitude in Degrees");
	channels[CHANNEL_LONGITUDE] = DatalogChannel(CHANNEL_LONGITUDE, CHANNEL_TYPE_LONGITUDE, "GPS Longitude in Degrees");
	channels[CHANNEL_SPEED] =  DatalogChannel(CHANNEL_SPEED, CHANNEL_TYPE_SPEED, "GPS Speed");
	channels[CHANNEL_LAPCOUNT] = DatalogChannel(CHANNEL_LAPCOUNT, CHANNEL_TYPE_COUNT, "Lap Count" );
	channels[CHANNEL_LAPTIME] = DatalogChannel(CHANNEL_LAPTIME, CHANNEL_TYPE_TIME, "Lap Time" );
	channels[CHANNEL_SPLITTIME] = DatalogChannel(CHANNEL_SPLITTIME, CHANNEL_TYPE_TIME, "Split Time" );
}

void AppOptions::LoadStandardAnalogChannels(DatalogChannels &channels){
	//Analog inputs
	channels[CHANNEL_FUEL_LEVEL] = DatalogChannel(CHANNEL_FUEL_LEVEL, CHANNEL_TYPE_PERCENT, "Fuel Level");
	channels[CHANNEL_COOLANT] = DatalogChannel(CHANNEL_COOLANT, CHANNEL_TYPE_TEMPERATURE, "Engine Coolant Temperature");
	channels[CHANNEL_OIL_PRESSURE] = DatalogChannel(CHANNEL_OIL_PRESSURE, CHANNEL_TYPE_PRESSURE, "Oil Pressure");
	channels[CHANNEL_OIL_TEMP] = DatalogChannel(CHANNEL_OIL_TEMP, CHANNEL_TYPE_TEMPERATURE, "Engine Oil Temperature");
	channels[CHANNEL_AFR] = DatalogChannel(CHANNEL_AFR, CHANNEL_TYPE_RAW, "Air/Fuel Ratio");
	channels[CHANNEL_IAR] = DatalogChannel(CHANNEL_IAR, CHANNEL_TYPE_TEMPERATURE, "Inlet Air Temperature");
	channels[CHANNEL_MAP] = DatalogChannel(CHANNEL_MAP, CHANNEL_TYPE_PRESSURE, "Manifold Air Pressure");
	channels[CHANNEL_EGT] = DatalogChannel(CHANNEL_EGT, CHANNEL_TYPE_TEMPERATURE, "Exhaust Gas Temperature");
	channels[CHANNEL_TPS] = DatalogChannel(CHANNEL_TPS, CHANNEL_TYPE_TEMPERATURE, "Throttle Position");
	channels[CHANNEL_BATTERY] = DatalogChannel(CHANNEL_BATTERY, CHANNEL_TYPE_VOLTS, "Battery Voltage");
	channels[CHANNEL_FUEL_PRESSURE] = DatalogChannel(CHANNEL_FUEL_PRESSURE, CHANNEL_TYPE_PRESSURE, "Fuel Pressure");
	channels[CHANNEL_STEERING] = DatalogChannel(CHANNEL_STEERING, CHANNEL_TYPE_ANGLE, "Steering angle");
	channels[CHANNEL_BRAKE] = DatalogChannel(CHANNEL_BRAKE, CHANNEL_TYPE_PRESSURE, "Brake Pressure");
	channels[CHANNEL_LF_HEIGHT] = DatalogChannel(CHANNEL_LF_HEIGHT, CHANNEL_TYPE_DISTANCE, "Left Front suspension height");
	channels[CHANNEL_RF_HEIGHT] =  DatalogChannel(CHANNEL_RF_HEIGHT, CHANNEL_TYPE_DISTANCE, "Right Front suspension height");
	channels[CHANNEL_LR_HEIGHT] =  DatalogChannel(CHANNEL_LR_HEIGHT, CHANNEL_TYPE_DISTANCE, "Left Rear suspension height");
	channels[CHANNEL_RR_HEIGHT] =  DatalogChannel(CHANNEL_RR_HEIGHT, CHANNEL_TYPE_DISTANCE, "Right Rear suspension height");
	channels[CHANNEL_ANALOG1] = DatalogChannel(CHANNEL_ANALOG1, CHANNEL_TYPE_VOLTS, "Analog Input 1");
	channels[CHANNEL_ANALOG2] =  DatalogChannel(CHANNEL_ANALOG2, CHANNEL_TYPE_VOLTS, "Analog Input 2");
	channels[CHANNEL_ANALOG3] =  DatalogChannel(CHANNEL_ANALOG3, CHANNEL_TYPE_VOLTS, "Analog Input 3");
	channels[CHANNEL_ANALOG4] =  DatalogChannel(CHANNEL_ANALOG4, CHANNEL_TYPE_VOLTS, "Analog Input 4");
	channels[CHANNEL_ANALOG5] =  DatalogChannel(CHANNEL_ANALOG5, CHANNEL_TYPE_VOLTS, "Analog Input 5");
	channels[CHANNEL_ANALOG6] =  DatalogChannel(CHANNEL_ANALOG6, CHANNEL_TYPE_VOLTS, "Analog Input 6");
	channels[CHANNEL_ANALOG7] =  DatalogChannel(CHANNEL_ANALOG7, CHANNEL_TYPE_VOLTS, "Analog Input 7");
}


void AppOptions::LoadStandardFrequencyChannels(DatalogChannels &channels){
	//Frequency Inputs
	channels[CHANNEL_RPM] =  DatalogChannel(CHANNEL_RPM, CHANNEL_TYPE_RPM, "Engine Speed in RPM");
	channels[CHANNEL_WHEEL] =  DatalogChannel(CHANNEL_WHEEL, CHANNEL_TYPE_RPM, "Wheel Speed in RPM");
	channels[CHANNEL_INJECTOR] =  DatalogChannel(CHANNEL_INJECTOR, CHANNEL_TYPE_DURATION, "Injector pulse width in Ms.");
	channels[CHANNEL_FREQ1] =  DatalogChannel(CHANNEL_FREQ1, CHANNEL_TYPE_FREQUENCY, "Frequency Input 1");
	channels[CHANNEL_FREQ2] =  DatalogChannel(CHANNEL_FREQ2, CHANNEL_TYPE_FREQUENCY, "Frequency Input 2");
	channels[CHANNEL_FREQ3] =  DatalogChannel(CHANNEL_FREQ3, CHANNEL_TYPE_FREQUENCY, "Frequency Input 3");
}


void AppOptions::LoadStandardPwmChannels(DatalogChannels &channels){
	//Analog Outputs
	channels[CHANNEL_VOUT1] =  DatalogChannel(CHANNEL_VOUT1, CHANNEL_TYPE_VOLTS, "Analog Output 1");
	channels[CHANNEL_VOUT2] =  DatalogChannel(CHANNEL_VOUT2, CHANNEL_TYPE_VOLTS, "Analog Output 2");
	channels[CHANNEL_VOUT3] =  DatalogChannel(CHANNEL_VOUT3, CHANNEL_TYPE_VOLTS, "Analog Output 3");
	channels[CHANNEL_VOUT4] =  DatalogChannel(CHANNEL_VOUT4, CHANNEL_TYPE_VOLTS, "Analog Output 4");

	//Frequency Outputs
	channels[CHANNEL_FREQOUT1] =  DatalogChannel(CHANNEL_FREQOUT1, CHANNEL_TYPE_FREQUENCY, "Frequency Output 1");
	channels[CHANNEL_FREQOUT2] =  DatalogChannel(CHANNEL_FREQOUT2, CHANNEL_TYPE_FREQUENCY, "Frequency Output 2");
	channels[CHANNEL_FREQOUT3] =  DatalogChannel(CHANNEL_FREQOUT3, CHANNEL_TYPE_FREQUENCY, "Frequency Output 3");
	channels[CHANNEL_FREQOUT4] =  DatalogChannel(CHANNEL_FREQOUT4, CHANNEL_TYPE_FREQUENCY, "Frequency Output 4");
}


void AppOptions::LoadStandardGpioChannels(DatalogChannels &channels){
	//Digital Inputs
	channels[CHANNEL_GPI1] = DatalogChannel(CHANNEL_GPI1, CHANNEL_TYPE_DIGITAL, "Digital Input 1");
	channels[CHANNEL_GPI2] = DatalogChannel(CHANNEL_GPI2, CHANNEL_TYPE_DIGITAL, "Digital Input 2");
	channels[CHANNEL_GPI3] = DatalogChannel(CHANNEL_GPI3, CHANNEL_TYPE_DIGITAL, "Digital Input 3");

	//Digital Outputs
	channels[CHANNEL_GPO1] = DatalogChannel(CHANNEL_GPO1, CHANNEL_TYPE_DIGITAL, "Digital Output 1");
	channels[CHANNEL_GPO2] = DatalogChannel(CHANNEL_GPO2, CHANNEL_TYPE_DIGITAL, "Digital Output 2");
	channels[CHANNEL_GPO3] = DatalogChannel(CHANNEL_GPO3, CHANNEL_TYPE_DIGITAL, "Digital Output 3");
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

	analogGaugeTypes[CHANNEL_TYPE_RAW] = AnalogGaugeType(100, 50, 0);
	analogGaugeTypes[CHANNEL_TYPE_GFORCE] = AnalogGaugeType(0.5, 0.1,2);
	analogGaugeTypes[CHANNEL_TYPE_ROTATION] = AnalogGaugeType(100.0, 50.0, 1);
	analogGaugeTypes[CHANNEL_TYPE_COUNT] = AnalogGaugeType(10.0, 5.0, 1);
	analogGaugeTypes[CHANNEL_TYPE_SPEED] = AnalogGaugeType(20.0, 10.0, 0);
	analogGaugeTypes[CHANNEL_TYPE_VOLTS] = AnalogGaugeType(5.0, 1.0, 2);
	analogGaugeTypes[CHANNEL_TYPE_PRESSURE] = AnalogGaugeType(10.0, 5.0, 0);
	analogGaugeTypes[CHANNEL_TYPE_TEMPERATURE] = AnalogGaugeType(10.0, 5.0, 0);
	analogGaugeTypes[CHANNEL_TYPE_FREQUENCY] = AnalogGaugeType(100.0, 50.0, 0);
	analogGaugeTypes[CHANNEL_TYPE_RPM] = AnalogGaugeType(100.0, 50.0, 0);
	analogGaugeTypes[CHANNEL_TYPE_DURATION] = AnalogGaugeType(10.0, 5.0, 0);
	analogGaugeTypes[CHANNEL_TYPE_PERCENT] = AnalogGaugeType(10.0, 5.0, 0);
	analogGaugeTypes[CHANNEL_TYPE_TIME] = AnalogGaugeType(1,0,0);
}

void AppOptions::LoadDefaultDigitalGaugeTypes(DigitalGaugeTypes &digitalGaugeTypes){

	digitalGaugeTypes[CHANNEL_TYPE_VALUE] = DigitalGaugeType(4,0);
	digitalGaugeTypes[CHANNEL_TYPE_RAW] = DigitalGaugeType(3,0);
	digitalGaugeTypes[CHANNEL_TYPE_GFORCE] = DigitalGaugeType(1,2);
	digitalGaugeTypes[CHANNEL_TYPE_ROTATION] = DigitalGaugeType(2,2);
	digitalGaugeTypes[CHANNEL_TYPE_COUNT] = DigitalGaugeType(3,0);
	digitalGaugeTypes[CHANNEL_TYPE_SPEED] = DigitalGaugeType(3,0);
	digitalGaugeTypes[CHANNEL_TYPE_VOLTS] = DigitalGaugeType(2,2);
	digitalGaugeTypes[CHANNEL_TYPE_PRESSURE] = DigitalGaugeType(3,0);
	digitalGaugeTypes[CHANNEL_TYPE_TEMPERATURE] = DigitalGaugeType(3,0);
	digitalGaugeTypes[CHANNEL_TYPE_FREQUENCY] = DigitalGaugeType(4,0);
	digitalGaugeTypes[CHANNEL_TYPE_RPM] = DigitalGaugeType(4,0);
	digitalGaugeTypes[CHANNEL_TYPE_DURATION] = DigitalGaugeType(3,1);
	digitalGaugeTypes[CHANNEL_TYPE_PERCENT] = DigitalGaugeType(2,0);
	digitalGaugeTypes[CHANNEL_TYPE_TIME] = DigitalGaugeType(2,3);
}
