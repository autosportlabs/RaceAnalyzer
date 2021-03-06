
#ifndef _CONFIG_DATA_H_
#define _CONFIG_DATA_H_
#include "wx/wxprec.h"
#include <stdio.h>


class ConfigValueException{

	public:
		ConfigValueException();
		ConfigValueException( const wxString &message);
		const wxString GetMessage();

	private:
		wxString _message;
};

class VersionData{

	public:
		VersionData();
		VersionData(int major, int minor, int bugfix, wxString &notes);

		bool IsClientCompatible(const VersionData &clientVersion) const;

		bool IsDefined() const;
		void SetMajor(int major);
		int GetMajor() const;
		void SetMinor(int minor);
		int GetMinor() const;
		void SetBugfix(int bugfix);
		int GetBugfix() const;

		void SetNotes(wxString notes);
		wxString GetNotes() const;

		wxString ToString();

	private:
		int _major;
		int _minor;
		int _bugfix;
		wxString _notes;

};


#endif
