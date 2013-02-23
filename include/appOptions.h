#ifndef APPOPTIONS_H_
#define APPOPTIONS_H_
#include "wx/wxprec.h"
#include "datalogData.h"
#include "wx/hashmap.h"
#include "raceAnalyzerConfigBase.h"


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
