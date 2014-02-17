/*
 * scriptPanel.cpp
 *
 *  Created on: Apr 30, 2009
 *      Author: brent
 */
#include "configuration/scriptPanel.h"

ScriptPanel::ScriptPanel() : BaseConfigPanel()
{
	InitComponents();
}

ScriptPanel::ScriptPanel(wxWindow *parent,
			ConfigPanelParams configParams,
			wxWindowID id,
			const wxPoint &pos,
			const wxSize &size,
			long style,
			const wxString &name
			)
			: BaseConfigPanel(	parent,
						configParams,
						id,
						pos,
						size,
						style,
						name)
{
	InitComponents();
}

ScriptPanel::~ScriptPanel(){

}

enum
{
    MARGIN_LINE_NUMBERS
};

void ScriptPanel::StyleEditor(void){

	wxFont editorFont = wxSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT);
	m_scriptCtrl->StyleSetFont(wxSTC_STYLE_DEFAULT, editorFont);
	m_scriptCtrl->StyleClearAll();
	m_scriptCtrl->SetLexer(wxSTC_LEX_LUA);

	m_scriptCtrl->SetKeyWords(0,"and break do else elseif end for function if local nil not or repeat return then until while");

	wxColor medGreen(0,128,0);
	wxColor grey = wxTheColourDatabase->Find(wxT("GREY"));
	wxColor grey1(75, 75, 75);
	wxColor grey2(220, 220, 220);

	m_scriptCtrl->StyleSetForeground(wxSTC_LUA_DEFAULT, *wxBLACK);
	m_scriptCtrl->StyleSetForeground(wxSTC_LUA_WORD, *wxBLUE);
	m_scriptCtrl->StyleSetForeground(wxSTC_LUA_OPERATOR, *wxRED);
	m_scriptCtrl->StyleSetBold(wxSTC_LUA_OPERATOR, true);
	m_scriptCtrl->StyleSetBold(wxSTC_LUA_IDENTIFIER, true);
	m_scriptCtrl->StyleSetForeground(wxSTC_LUA_NUMBER, *wxBLACK);
	m_scriptCtrl->StyleSetForeground(wxSTC_LUA_STRING, medGreen);
	m_scriptCtrl->StyleSetForeground(wxSTC_LUA_STRING, medGreen);

	m_scriptCtrl->StyleSetForeground(wxSTC_LUA_COMMENT, grey);
	m_scriptCtrl->StyleSetForeground(wxSTC_LUA_COMMENTLINE, grey);
	m_scriptCtrl->StyleSetForeground(wxSTC_LUA_COMMENTDOC, grey);
	m_scriptCtrl->SetTabWidth(4);
	m_scriptCtrl->SetUseTabs(false);
	m_scriptCtrl->SetMarginType(0, wxSTC_MARGIN_NUMBER);

	m_scriptCtrl->SetMarginWidth (MARGIN_LINE_NUMBERS, 15);
	m_scriptCtrl->StyleSetForeground (wxSTC_STYLE_LINENUMBER,  grey1 );
	m_scriptCtrl->StyleSetBackground (wxSTC_STYLE_LINENUMBER, grey2 );
	m_scriptCtrl->SetMarginType (MARGIN_LINE_NUMBERS, wxSTC_MARGIN_NUMBER);
}

void ScriptPanel::InitComponents(){
	wxFlexGridSizer *sizer = new wxFlexGridSizer(2,1,3,3);
	sizer->AddGrowableCol(0);
	sizer->AddGrowableRow(0);



	m_scriptCtrl = new wxStyledTextCtrl(this, ID_SCRIPT_WINDOW);
	StyleEditor();


	sizer->Add(m_scriptCtrl,1,wxEXPAND);

	wxButton *readButton = new wxButton(this,ID_BUTTON_READ,"Read");
	wxButton *writeButton = new wxButton(this, ID_BUTTON_WRITE,"Write");
	wxButton *runButton = new wxButton(this, ID_BUTTON_RUN,"Run");

	wxGridSizer *buttonBarSizer = new wxGridSizer(1,2,3,3);

	wxBoxSizer *leftButtonSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *rightButtonSizer = new wxBoxSizer(wxHORIZONTAL);

	leftButtonSizer->Add(readButton,0,wxALIGN_LEFT | wxALL);
	leftButtonSizer->Add(writeButton,0,wxALIGN_RIGHT | wxALL);
	rightButtonSizer->AddStretchSpacer(1);
	rightButtonSizer->Add(runButton,0,wxALIGN_RIGHT | wxALL);

	buttonBarSizer->Add(leftButtonSizer,1,wxEXPAND | wxALL);
	buttonBarSizer->Add(rightButtonSizer,1,wxEXPAND | wxALL);

	sizer->Add(buttonBarSizer,1,wxEXPAND);
	this->SetSizer(sizer);
}

void ScriptPanel::OnConfigUpdated(){
	wxString script = m_configParams.config->luaScript;
	m_scriptCtrl->ClearAll();
	m_scriptCtrl->SetValue(script);
}

void ScriptPanel::InitOptions(void){

}

void ScriptPanel::OnReadScript(wxCommandEvent &event){
	try{
		wxString script = m_configParams.comm->readScript();
		m_scriptCtrl->ClearAll();
		m_scriptCtrl->SetValue(script);
	}
	catch(CommException &e){
		wxMessageDialog dlg(this, wxString::Format("Error reading script:\n\n%s", e.GetErrorMessage().ToAscii()), "Error Reading Script", wxOK | wxICON_HAND);
		dlg.ShowModal();
	}
}

void ScriptPanel::OnWriteScript(wxCommandEvent &event){

	try{
		wxString script = m_scriptCtrl->GetValue();
		m_configParams.comm->writeScript(script);
	}
	catch(CommException &e){
		wxMessageDialog dlg(this, wxString::Format("Error writing script:\n\n%s", e.GetErrorMessage().ToAscii()), "Error Writing Script", wxOK | wxICON_HAND);
		dlg.ShowModal();
	}
}

void ScriptPanel::OnRunScript(wxCommandEvent &event){
	m_configParams.comm->reloadScript();
}

void ScriptPanel::OnScriptChanged(wxStyledTextEvent &event){
	m_configParams.config->luaScript = m_scriptCtrl->GetValue();
}

BEGIN_EVENT_TABLE ( ScriptPanel, wxPanel )
	EVT_BUTTON(ID_BUTTON_READ,ScriptPanel::OnReadScript)
	EVT_BUTTON(ID_BUTTON_WRITE,ScriptPanel::OnWriteScript)
	EVT_BUTTON(ID_BUTTON_RUN,ScriptPanel::OnRunScript)
	EVT_STC_CHANGE(ID_SCRIPT_WINDOW, ScriptPanel::OnScriptChanged)

END_EVENT_TABLE()
