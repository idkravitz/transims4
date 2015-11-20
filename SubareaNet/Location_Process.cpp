//*********************************************************
//	Location_Process.cpp - save subarea location data 
//*********************************************************

#include "SubareaNet.hpp"

#include "Location_File.hpp"

//---------------------------------------------------------
//	Location_Processing
//---------------------------------------------------------

bool SubareaNet::Location_Processing (Db_File *fh)
{
	static Location_File *new_file;

	int id;
	Location_Data *location_ptr;
	Link_Data *link_ptr;

	Location_File *file = (Location_File *) fh;

	if (First_Record ()) {
		new_file = (Location_File *) Network_Db_Base (NEW_ACTIVITY_LOCATION);

		if (!location_data.Max_Records (file->Num_Records ())) goto mem_error;
	}

	id = file->Location ();
	if (id == 0) return (false);

	location_ptr = location_data.New_Record ();
	if (location_ptr == NULL) goto mem_error;

	location_ptr->ID (id);

	if (id > max_location) max_location = id;

	id = file->Zone ();

	if (id > max_zone) max_zone = id;

	link_ptr = link_data.Get (file->Link ());

	if (link_ptr != NULL && link_ptr->Type () > 0) {

		//---- copy the fields to the subarea file ----

		new_file->Copy_Fields (file);

		if (!new_file->Write ()) {
			Error ("Writing %s", new_file->File_Type ());
		}
		nlocation++;
		return (true);
	}
	return (false);

mem_error:
	Error ("Insufficient Memory for Activity Location Data");
	return (false);
}
