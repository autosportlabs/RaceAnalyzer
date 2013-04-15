#include "configData.h"

ConfigValueException::ConfigValueException(){
	_message = "";
}

ConfigValueException::ConfigValueException( const wxString &message){
	wxLogError("Config Value Exception: %s", message.ToAscii());
	_message = message;
}

const wxString ConfigValueException::GetMessage(){
	return _message;
}

VersionData::VersionData() : _major(0), _minor(0), _bugfix(0) {}

VersionData::VersionData(int major, int minor, int bugfix, wxString &notes) :
		_major(major),
		_minor(minor),
		_bugfix(bugfix),
		_notes(notes) {}

bool VersionData::IsDefined() const{
	return !(!_major && !_minor && !_bugfix);
}

void VersionData::SetMajor(int major){
	_major = major;
}

int VersionData::GetMajor() const{
	return _major;
}

void VersionData::SetMinor(int minor){
	_minor = minor;
}

int VersionData::GetMinor() const{
	return _minor;
}

int VersionData::GetBugfix() const{
	return _bugfix;
}

void VersionData::SetBugfix(int bugfix){
	_bugfix = bugfix;
}

wxString VersionData::GetNotes() const{
	return _notes;
}

void VersionData::SetNotes(wxString notes){
	_notes = notes;
}

wxString VersionData::ToString(){
	return wxString::Format("%d.%d.%d %s", _major, _minor, _bugfix, _notes);
}
