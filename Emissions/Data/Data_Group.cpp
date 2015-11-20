//*********************************************************
//	Data_Group.cpp - speed bin data group
//*********************************************************

#include "Data_Group.hpp"

//---------------------------------------------------------
//	Data_Group constructor
//---------------------------------------------------------

Data_Group::Data_Group (void)
{
	group = 0;
	factor = 1.0;
	file = NULL;
}

//---- Open_File ----

bool Data_Group::Open_File (char *name, char *format)
{
	char *type_ptr = "Speed Bin File";

	file = new Speed_Bin_File ();

	if (group > 0) {
		char buffer [FIELD_BUFFER];
		str_fmt (buffer, sizeof (buffer), "%s #%d", type_ptr, group);
		file->File_Type (buffer);
	} else {
		file->File_Type (type_ptr);
	}
	if (format != NULL) {
		file->Dbase_Format (format);
	}
	return (file->Open (name));
}

//---- Clear ----

void Data_Group::Clear (void)
{
	if (file != NULL) {
		delete file;
		file = NULL;
	}
}

//---------------------------------------------------------
//	Data_Group_Array constructor
//---------------------------------------------------------

Data_Group_Array::Data_Group_Array (int max_records) : 
	Data_Array (sizeof (Data_Group), max_records)
{
}

Data_Group_Array::~Data_Group_Array (void)
{
	for (Data_Group *group = First (); group != NULL; group = Next ()) {
		group->Clear ();
	}
}
