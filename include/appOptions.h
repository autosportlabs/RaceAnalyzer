#ifndef APPOPTIONS_H_
#define APPOPTIONS_H_
#include "wx/wxprec.h"
#include "datalogData.h"
#include "wx/hashmap.h"
#include "raceAnalyzerConfigBase.h"


#define CHANNEL_TYPE_VALUE "Value"
#define CHANNEL_TYPE_RAW "Raw"
#define CHANNEL_TYPE_GFORCE "GForce"
#define CHANNEL_TYPE_ROTATION "Rotation"
#define CHANNEL_TYPE_TIMEDATE "TimeDate"
#define CHANNEL_TYPE_COUNT "Count"
#define CHANNEL_TYPE_LATITUDE "Latitude"
#define CHANNEL_TYPE_LONGITUDE "Longitude"
#define CHANNEL_TYPE_SPEED "Speed"
#define CHANNEL_TYPE_VOLTS "Volts"
#define CHANNEL_TYPE_PRESSURE "Pressure"
#define CHANNEL_TYPE_TEMPERATURE "Temperature"
#define CHANNEL_TYPE_FREQUENCY "Frequency"
#define CHANNEL_TYPE_RPM "RPM"
#define CHANNEL_TYPE_DURATION "Duration"
#define CHANNEL_TYPE_PERCENT "Percent"
#define CHANNEL_TYPE_DIGITAL "Digital"
#define CHANNEL_TYPE_TIME "Time"
#define CHANNEL_TYPE_DISTANCE "Distance"
#define CHANNEL_TYPE_ANGLE "Angle"

#define DEFAULT_CHANNEL_TYPE CHANNEL_TYPE_VALUE

#define CHANNEL_TIME "Time"
#define CHANNEL_GPS_SATS "GpsSats"
#define CHANNEL_LATITUDE "Latitude"
#define CHANNEL_LONGITUDE "Longitude"
#define CHANNEL_SPEED "Speed"
#define CHANNEL_LAPCOUNT "LapCount"
#define CHANNEL_LAPTIME "LapTime"
#define CHANNEL_SPLITTIME "SplitTime"
#define CHANNEL_FUEL_LEVEL "FuelLevel"
#define CHANNEL_COOLANT "Coolant"
#define CHANNEL_OIL_PRESSURE "OilPress"
#define CHANNEL_OIL_TEMP "OilTemp"
#define CHANNEL_AFR "AFR"
#define CHANNEL_IAR "IAR"
#define CHANNEL_MAP "MAP"
#define CHANNEL_EGT "EGT"
#define CHANNEL_TPS "TPS"
#define CHANNEL_BATTERY "Battery"
#define CHANNEL_FUEL_PRESSURE "FuelPressure"
#define CHANNEL_STEERING "Steering"
#define CHANNEL_BRAKE "Brake"
#define CHANNEL_LF_HEIGHT "LF_Height"
#define CHANNEL_RF_HEIGHT "RF_Height"
#define CHANNEL_LR_HEIGHT "LR_Height"
#define CHANNEL_RR_HEIGHT "RR_Height"
#define CHANNEL_ANALOG1 "Analog1"
#define CHANNEL_ANALOG2 "Analog2"
#define CHANNEL_ANALOG3 "Analog3"
#define CHANNEL_ANALOG4 "Analog4"
#define CHANNEL_ANALOG5 "Analog5"
#define CHANNEL_ANALOG6 "Analog6"
#define CHANNEL_ANALOG7 "Analog7"
#define CHANNEL_RPM "RPM"
#define CHANNEL_WHEEL "Wheel"
#define CHANNEL_INJECTOR "Injector"
#define CHANNEL_FREQ1 "Freq1"
#define CHANNEL_FREQ2 "Freq2"
#define CHANNEL_FREQ3 "Freq3"
#define CHANNEL_VOUT1 "Vout1"
#define CHANNEL_VOUT2 "Vout2"
#define CHANNEL_VOUT3 "Vout3"
#define CHANNEL_VOUT4 "Vout4"
#define CHANNEL_FREQOUT1 "FreqOut1"
#define CHANNEL_FREQOUT2 "FreqOut2"
#define CHANNEL_FREQOUT3 "FreqOut3"
#define CHANNEL_FREQOUT4 "FreqOut4"
#define CHANNEL_GPI1 "GPI1"
#define CHANNEL_GPI2 "GPI2"
#define CHANNEL_GPI3 "GPI3"
#define CHANNEL_GPO1 "GPO1"
#define CHANNEL_GPO2 "GPO2"
#define CHANNEL_GPO3 "GPO3"

class AnalogGaugeType{

public:
	AnalogGaugeType() : majorTickInterval(0), minorTickInterval(0), valuePrecision(0)
	{}
	AnalogGaugeType(double majorTick, double minorTick, unsigned int prec) :
		majorTickInterval(majorTick), minorTickInterval(minorTick), valuePrecision(prec)
		{
		labelFont.SetPointSize(16);
		valueFont.SetPointSize(18);
		majorTickFont.SetPointSize(12);
		}

	double majorTickInterval;
	double minorTickInterval;
	unsigned int valuePrecision;
	wxFont labelFont;
	wxFont valueFont;
	wxFont majorTickFont;
};

class DigitalGaugeType{


public:
	DigitalGaugeType() :
		valueDigits(4),
		valuePrecision(0),
		digitOnColor(wxColour(0,255,0)),
		digitOffColor(wxColor(0,40,0))
		{}

	DigitalGaugeType(unsigned int dig, unsigned int prec) :
		valueDigits(dig),
		valuePrecision(prec),
		digitOnColor(wxColour(0,255,0)),
		digitOffColor(wxColor(0,40,0))
	{}

	unsigned int valueDigits;
	unsigned int valuePrecision;
	wxColour digitOnColor;
	wxColour digitOffColor;
};

WX_DECLARE_OBJARRAY(wxColor,ChartColors);
WX_DECLARE_STRING_HASH_MAP(AnalogGaugeType, AnalogGaugeTypes);
WX_DECLARE_STRING_HASH_MAP(DigitalGaugeType, DigitalGaugeTypes);

class AppOptions{

	public:

		AppOptions();
		int GetSerialPort();
		void SetSerialPort(int serialPort);

		bool & GetAutoLoadConfig();
		void SetAutoLoadConfig(bool autoConnect);

		void SaveAppOptions();
		void LoadAppOptions();

		DatalogChannel CreateGenericChannel(wxString &name, int sampleRate);
		DatalogChannels & GetAllStandardChannels();
		DatalogChannels & GetStandardAccelChannels();
		DatalogChannels & GetStandardGpsChannels();
		DatalogChannels & GetStandardAnalogChannels();
		DatalogChannels & GetStandardPulseInputChannels();
		DatalogChannels & GetStandardAnalogPwmOutputChannels();
		DatalogChannels & GetStandardGpioChannels();

		DatalogChannelTypes & GetStandardChannelTypes();
		AnalogGaugeTypes & GetAnalogGaugeTypes();

		DigitalGaugeTypes & GetDigitalGaugeTypes();

		ChartColors & GetChartColors();

		void LoadStandardAccelChannels(DatalogChannels &channels);
		void LoadStandardGpsChannels(DatalogChannels &channels);
		void LoadStandardAnalogChannels(DatalogChannels &channels);
		void LoadStandardFrequencyChannels(DatalogChannels &channels);
		void LoadStandardPwmChannels(DatalogChannels &channels);
		void LoadStandardGpioChannels(DatalogChannels &channels);
		void LoadAllStandardChannels(DatalogChannels &channels);

		DatalogChannelType GetGenericChannelType();
		void LoadStandardChannelTypes(DatalogChannelTypes &channelTypes);
		void LoadDefaultAnalogGaugeTypes(AnalogGaugeTypes &analogGaugeTypes);
		void LoadDefaultDigitalGaugeTypes(DigitalGaugeTypes &digitalGaugeTypes);
		DatalogChannelType GetChannelTypeForChannel(ViewChannel &channel);
		DatalogChannelType GetDefaultUnknownChannelType(wxString name);

	private:
		int 				m_serialPort;
		bool 				m_autoLoadConfig;

		DatalogChannels 	m_allStandardChannels;
		DatalogChannels 	m_standardAccelChannels;
		DatalogChannels		m_standardGpsChannels;
		DatalogChannels 	m_standardAnalogChannels;
		DatalogChannels		m_standardPulseInputChannels;
		DatalogChannels		m_standardAnalogPwmOutputChannels;
		DatalogChannels		m_standardGpioChannels;

		DatalogChannelTypes	m_standardChannelTypes;
		ChartColors			m_chartColors;
		AnalogGaugeTypes	m_analogGaugeTypes;
		DigitalGaugeTypes	m_digitalGaugeTypes;


};


#endif /*APPOPTIONS_H_*/
