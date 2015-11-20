//*********************************************************
//	Access_Process.cpp - save subarea access data 
//*********************************************************

#include "SubareaNet.hpp"

#include "Access_File.hpp"

//---------------------------------------------------------
//	Access_Processing
//---------------------------------------------------------

bool SubareaNet::Access_Processing (Db_File *fh)
{
	static Access_File *new_file;

	int id, location, stop;
	Access_File *file = (Access_File *) fh;

	if (First_Record ()) {
		new_file = (Access_File *) Network_Db_Base (NEW_PROCESS_LINK);
	}

    id = file->ID ();
	if (id == 0) return (false);

	if (id > max_access) max_access = id;

	//---- get the location id ----
	
	if (End_Code (file->From_Type ()) == LOCATION_ID) {
		location = file->From_ID ();
	} else if (End_Code (file->To_Type ()) == LOCATION_ID) {
		location = file->To_ID ();
	} else {
		return (false);
	}

	if (location_data.Get (location)) {

		//---- check transit stops ----

		if (End_Code (file->From_Type ()) == STOP_ID) {
			stop = file->From_ID ();
		} else if (End_Code (file->To_Type ()) == STOP_ID) {
			stop = file->To_ID ();
		} else {
			stop = 0;
		}
		if (stop > 0) {
			Stop_Data *stop_ptr = stop_data.Get (stop);
			if (stop_ptr == NULL) return (false);

			Link_Data *link_ptr = link_data.Get (stop_ptr->Link ());
			if (link_ptr == NULL || link_ptr->Type () == 0) return (false);
		}

		//---- copy the fields to the subarea file ----

		new_file->Copy_Fields (file);

		if (!new_file->Write ()) {
			Error ("Writing %s", new_file->File_Type ());
		}
		naccess++;
	}

	//---- don't save the record ----

	return (false);
}
