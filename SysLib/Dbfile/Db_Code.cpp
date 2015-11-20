//*********************************************************
//	Db_Code.cpp - network codes
//*********************************************************

#include "Db_Code.hpp"

#include "Utility.hpp"

//---- format type codes ----

char * Db_Code::format_text [] = {
	"TEXT", "UNFORMATED", "BINARY", "FIXED_COLUMN", "COMMA_DELIMITED", 
	"SPACE_DELIMITED", "TAB_DELIMITED", "CSV_DELIMITED", "DBASE", "VERSION3", "LANL", 
	"SQLITE3", NULL
};

Db_Code::Format_Type  Db_Code::format_code [] = {
	UNFORMATED, UNFORMATED, BINARY, FIXED_COLUMN, COMMA_DELIMITED, 
	SPACE_DELIMITED, TAB_DELIMITED, CSV_DELIMITED, DBASE, VERSION3, VERSION3,
	SQLITE3
};

//---- field type codes ----

char * Db_Code::field_text [] = {
	"INTEGER", "UNSIGNED", "DOUBLE", "FIXED", "STRING", 
	"CHAR", "DATE", "TIME", "DATE_TIME", "DAY_TIME", NULL
};

Db_Code::Field_Type  Db_Code::field_code [] = {
	INTEGER, UNSIGNED, DOUBLE, FIXED, STRING, 
	CHAR, DATE, TIME, DATE_TIME, DAY_TIME
};

//---------------------------------------------------------
//	Db_Code constructor
//---------------------------------------------------------

Db_Code::Db_Code (void) : Static_Service ()
{
}

//---------------------------------------------------------
//	Format_Code
//---------------------------------------------------------

Db_Code::Format_Type  Db_Code::Format_Code (char *text)
{
	for (int i=0; format_text [i] != NULL; i++) {
		if (str_cmp (text, format_text [i]) == 0) {
			return (format_code [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Format Type %s", text);
	}
	return (UNFORMATED);
}

char * Db_Code::Format_Code (Db_Code::Format_Type code)
{
	for (int i=0; format_text [i] != NULL; i++) {
		if (code == format_code [i]) {
			return (format_text [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Format Code %d", code);
	}
	return (NULL);
}

//---------------------------------------------------------
//	Field_Code
//---------------------------------------------------------

Db_Code::Field_Type  Db_Code::Field_Code (char *text)
{
	for (int i=0; field_text [i] != NULL; i++) {
		if (str_cmp (text, field_text [i]) == 0) {
			return (field_code [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Field Type %s", text);
	}
	return (INTEGER);
}

char * Db_Code::Field_Code (Db_Code::Field_Type code)
{
	for (int i=0; field_text [i] != NULL; i++) {
		if (code == field_code [i]) {
			return (field_text [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Field Code %d", code);
	}
	return (NULL);
}
