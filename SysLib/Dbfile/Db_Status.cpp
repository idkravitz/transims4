//*********************************************************
//	Db_Status.cpp - database status code
//*********************************************************

#include "Db_Status.hpp"

#include "Utility.hpp"

#include <string.h>

char * Db_Status::default_type = "Db_Record";
char * Db_Status::default_id = "Db_File";

//---------------------------------------------------------
//	Db_Status constructors
//---------------------------------------------------------

Db_Status::Db_Status (void) : Db_Code ()
{
	file_type = NULL;
	file_id = NULL;

	Status (OK);
}

//---------------------------------------------------------
//	Db_Status destructor
//---------------------------------------------------------

Db_Status::~Db_Status (void)
{
	if (file_type != NULL) {
		delete [] file_type;
		file_type = NULL;
	}
	if (file_id != NULL) {
		delete [] file_id;
		file_id = NULL;
	}
}

//---------------------------------------------------------
//	Status
//---------------------------------------------------------

bool Db_Status::Status (Status_Code stat, bool message)
{
	status = stat;

	if (status == OK) return (true);

	if (message && Send_Messages ()) {
		exe->Error ("%s %s", File_Type (), Status_Message ());
	}
	return (false);
}

//---------------------------------------------------------
//	File_Type
//---------------------------------------------------------

bool Db_Status::File_Type (char *label)
{
	if (file_type != NULL) {
		delete [] file_type;
		file_type = NULL;
	}
	if (label != NULL) {
		int len = (int) strlen (label) + 1;
		file_type = new char [len];
		if (file_type == NULL) return (Status (MEMORY));
		str_cpy (file_type, len, label);
		return (true);
	}
	return (false);
}

//---------------------------------------------------------
//	File_ID
//---------------------------------------------------------

bool Db_Status::File_ID (char *_file_id)
{
	if (file_id != NULL) {
		delete [] file_id;
		file_id = NULL;
	}
	if (_file_id != NULL) {
		int len = (int) strlen (_file_id) + 1;
		file_id = new char [len];
		if (file_id == NULL) return (Status (MEMORY));
		str_cpy (file_id, len, _file_id);
		return (true);
	}
	return (false);
}

//---------------------------------------------------------
//	Status_Message
//---------------------------------------------------------

char * Db_Status::Status_Message (void)
{
	switch (Get_Status ()) {

		//---- Db_Record ----

		case OK:
			return ("OK");
		case ERROR:
			return ("Software Error");
		case MEMORY:
			return ("Insufficient Memory");
		case RECORD_SIZE:
			return ("Insufficient Record Size");

		//---- Db_File ----

		case NOT_OPEN:
			return ("Not Open");
		case NULL_POINTER:
			return ("Null Data Pointer");
		case FILE_IO:
			return ("I/O Error");
		case CLOSING:
			return ("Closing");
		case POSITIONING:
			return ("Positioning");

		//---- Db_Base ----

		case FIELD_LIST:
			return ("Field List");
		case NO_FIELD:
			return ("Field Not Found");
		case FIELD_BYTES:
			return ("Incompatible Field Size");
		case NULL_NAME:
			return ("Field Name is NULL");

		//---- Ext_File ----

		case EXTENSION:
			return ("Extension Number");

		//---- Plan_IO ----

		case PLAN_FIELDS:
			return ("Token Fields don't Match");

		//---- Csv_File ----

		case FILE_HEADER:
			return ("Header");
		case RECORD_SYNTAX:
			return ("Record Syntax");
		case FIELD_TYPE:
			return ("Field Type");

		//---- Db_Header ----

		case OPEN_DEF:
			return ("Opening Field Definition File");
		case DEF_FORMAT:
			return ("Field Definition Format");
		case DEF_NEST:
			return ("Nested Definition Fields");

		//---- Btree_Index ----

		case HEADER_MEMORY:
			return ("Insufficient Memory for Header Record");
		case INDEX_OPEN:
			return ("Opening");
		case HEADER_READ:
			return ("Reading Header");
		case HEADER_VERSION:
			return ("Incompatible Index Version");
		case NODE_MEMORY:
			return ("Insufficient Node Memory");
		case NODE_READ:
			return ("Reading Node Records");
		case PATH_NAME:
			return ("Original Data Path Name was Not Found");
		case HEADER_WRITE:
			return ("Writing Header");
		case TOO_MANY_FILES:
			return ("Too Many Data File Names");
		case NODE_WRITE:
			return ("Writing Node Records");
	}
	return ("Unknown Error");
}
