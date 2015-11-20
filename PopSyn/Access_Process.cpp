//*********************************************************
//	Access_Process.cpp - Read the Activity Location File
//*********************************************************

#include "PopSyn.hpp"

#include "Access_File.hpp"

//---------------------------------------------------------
//	Access_Processing
//---------------------------------------------------------

bool PopSyn::Access_Processing (Db_File *fh)
{
	int from_type, to_type;

	Location_List *location_ptr;

	Access_File *file = (Access_File *) fh;

	if (file->ID () == 0) return (false);

	//---- set the end type ----
	
	from_type = End_Code (file->From_Type ());

	if (from_type != LOCATION_ID) return (false);

	to_type = End_Code (file->To_Type ());

	if (to_type != PARKING_ID) return (false);

	//---- get the location list record ----

	location_ptr = location_list.Get (file->From_ID ());

	if (location_ptr == NULL) return (false);

	//---- save the parking lot ID ----

	location_ptr->Parking (file->To_ID ());

	return (false);
}
