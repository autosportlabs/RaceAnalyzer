/*
 * analogGaugePane.cpp
 *
 *  Created on: 2009-07-20
 *      Author: brent
 */

#include "analogGaugePane.h"

AnalogGaugePane::AnalogGaugePane(wxWindow *parent,
			ChartParams chartParams,
			wxWindowID id,
			const wxPoint &pos,
			const wxSize &size,
			long style,
			const wxString &name
			)
			:  wxPanel(	parent,
						id,
						pos,
						size,
						style,
						name),
				m_chartParams(chartParams)
{
	InitComponents();
}

AnalogGaugePane::~AnalogGaugePane(){

}

void AnalogGaugePane::CreateGauge(ViewChannel &channel){

	m_channel = channel;
	DatalogChannelType type = m_chartParams.appOptions->GetChannelTypeForChannel(channel);

	m_angularMeter->SetRange(type.minValue, type.maxValue);
	AppOptions *options = m_chartParams.appOptions;
	AnalogGaugeTypes &gaugeTypes = options->GetAnalogGaugeTypes();

	if (gaugeTypes.find(type.name) != gaugeTypes.end()){
		AnalogGaugeType &gaugeType = gaugeTypes[type.name];
		m_angularMeter->AddTicks(gaugeType.majorTickInterval, gaugeType.minorTickInterval);
		m_angularMeter->SetLabelFont(gaugeType.labelFont);
		m_angularMeter->SetMajorTickFont(gaugeType.majorTickFont);
		m_angularMeter->SetValueFont(gaugeType.valueFont);
		m_angularMeter->SetValuePrecision(gaugeType.valuePrecision);
		m_angularMeter->SetLabel(type.unitsLabel);
		m_angularMeter->SetMajorTickTextColor(wxColor(0xFF,0xC7,0x00));
	}
}


void AnalogGaugePane::UpdateValue(ViewChannel &channel, size_t index, double value){
	if (m_channel == channel && DatalogValue::NULL_VALUE != value) m_angularMeter->SetValue(value);
}

void AnalogGaugePane::InitComponents(){

	wxFlexGridSizer *sizer = new wxFlexGridSizer(1,1,0,0);
	sizer->AddGrowableCol(0);
	sizer->AddGrowableRow(0);
	m_angularMeter = new AngularMeter(this,-1);
	m_angularMeter->SetMinSize(wxSize(150,150));
	m_angularMeter->SetBackColour(*wxBLACK);
	sizer->Add(m_angularMeter,1,wxEXPAND);
	this->SetSizer(sizer);
}


BEGIN_EVENT_TABLE ( AnalogGaugePane, wxPanel )
END_EVENT_TABLE()
