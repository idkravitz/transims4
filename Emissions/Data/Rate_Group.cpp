//*********************************************************
//	Rate_Group.cpp - emission rate group
//*********************************************************

#include "Rate_Group.hpp"

//---------------------------------------------------------
//	Rate_Group constructor
//---------------------------------------------------------

Rate_Group::Rate_Group (void)
{
	group = year = month = period = region = area_type = facility = road_type = 0;
	vehicle = pollutant = speed_bin = rate = units = 0;
	file = NULL;
}

//---- Open_File ----

bool Rate_Group::Open_File (char *name, char *format)
{
	char *type_ptr = "Emission Rate File";

	file = new Db_Header ();

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

void Rate_Group::Clear (void)
{
	if (file != NULL) {
		delete file;
		file = NULL;
	}
}

//---------------------------------------------------------
//	Rate_Group_Array constructor
//---------------------------------------------------------

Rate_Group_Array::Rate_Group_Array (int max_records) : 
	Data_Array (sizeof (Rate_Group), max_records)
{
}

Rate_Group_Array::~Rate_Group_Array (void)
{
	for (Rate_Group *group = First (); group != NULL; group = Next ()) {
		group->Clear ();
	}
}
