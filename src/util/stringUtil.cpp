#include "util/stringUtil.h"


#define STRING_DELIMETER "\""
#define STRING_DELIMITER_ESCAPE "\"\""


void StringUtil::StripQuotes(wxString &value){
	size_t len = value.Length();
	if (len > 0 && value[0] == '"' && value[len - 1] == '"'){
		value.Remove(0,1);
		value.RemoveLast(1);
		value.Replace(STRING_DELIMITER_ESCAPE, STRING_DELIMETER, true);
	}
}
