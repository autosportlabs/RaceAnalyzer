/*
 * rcpLogFileDropTarget.h
 *
 *  Created on: May 15, 2013
 *      Author: brent
 */

#ifndef RCPLOGFILEDROPTARGET_H_
#define RCPLOGFILEDROPTARGET_H_
#include "wx/wxprec.h"
#include "wx/dnd.h"

class LogFileDropTarget : public wxFileDropTarget{

public:
		bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString & filenames);
};
#endif /* RCPLOGFILEDROPTARGET_H_ */
