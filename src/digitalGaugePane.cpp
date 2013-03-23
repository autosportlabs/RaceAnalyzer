/*
 * analogGaugePane.cpp
 *
 *  Created on: 2009-07-20
 *      Author: brent
 */

#define MAX_VALUE_PRECISION 4
static const char * VALUE_FORMAT[MAX_VALUE_PRECISION + 1] = {"%4.0f","%4.1f","%4.2f","%4.3f","%4.4f"};


#include "digitalGaugePane.h"

DigitalGaugePane::DigitalGaugePane(wxWindow *parent,
			ChartParams chartParams,
			wxWindowID id,
			const wxPoint &pos,
			const wxSize &size,
			long style,
			const wxString &name
			)
			: wxPanel(	parent,
						id,
						pos,
						size,
						style,
						name),
						m_chartParams(chartParams)
{
	m_valuePrecision = 0;
	InitComponents();
}

DigitalGaugePane::~DigitalGaugePane(){

}

void DigitalGaugePane::CreateGauge(ViewChannel &channel){

	DatalogChannelType type = m_chartParams.appOptions->GetChannelTypeForChannel(channel);

	AppOptions *options = m_chartParams.appOptions;

	DigitalGaugeTypes &gaugeTypes = options->GetDigitalGaugeTypes();

	if (gaugeTypes.find(type.name) != gaugeTypes.end()){
		DigitalGaugeType &gaugeType = gaugeTypes[type.name];
		m_lcdDisplay->SetNumberDigits(gaugeType.valueDigits + gaugeType.valuePrecision + (type.minValue < 0 ? 1 : 0));
		m_lcdDisplay->SetGrayColour(gaugeType.digitOffColor);
		m_lcdDisplay->SetLightColour(gaugeType.digitOnColor);
		m_valuePrecision = gaugeType.valuePrecision;
	}
	m_channel = channel;
}

void DigitalGaugePane::UpdateValue(ViewChannel &channel, size_t index, double value){
	if (m_channel == channel && DatalogValue::NULL_VALUE != value){
		m_lcdDisplay->SetValue(wxString::Format(VALUE_FORMAT[m_valuePrecision],value));
	}
}

void DigitalGaugePane::InitComponents(){

	wxFlexGridSizer *sizer = new wxFlexGridSizer(1,1,0,0);
	sizer->AddGrowableCol(0);
	sizer->AddGrowableRow(0);

	m_lcdDisplay = new LCDDisplay(this);
	m_lcdDisplay->SetMinSize(wxSize(150,50));
	sizer->Add(m_lcdDisplay,1,wxEXPAND);
	this->SetSizer(sizer);
}


BEGIN_EVENT_TABLE ( DigitalGaugePane, wxPanel )
END_EVENT_TABLE()
