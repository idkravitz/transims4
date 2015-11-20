//*********************************************************
//	Access_Process.cpp - Read the process link File
//*********************************************************

#include "TransimsNet.hpp"

#include "Access_File.hpp"

//---------------------------------------------------------
//	Access_Processing
//---------------------------------------------------------

bool TransimsNet::Access_Processing (Db_File *fh)
{
	Location_Data *location_ptr;
	Access_Data *access_ptr;

	if (Network_Service::Access_Processing (fh)) {
		access_ptr = access_data.New_Record ();

		if (access_ptr->ID () > mprocess) mprocess = access_ptr->ID ();

		if (update_flag || delete_flag) {
			if (access_ptr->From_Type () == LOCATION_ID) {
				location_ptr = location_data.Get (access_ptr->From_ID ());
				if (location_ptr == NULL) {
					xprocess++;
					return (false);
				}
			} else if (access_ptr->To_Type () == LOCATION_ID) {
				location_ptr = location_data.Get (access_ptr->To_ID ());
				if (location_ptr == NULL) {
					xprocess++;
					return (false);
				}
			}
		}
		Access_File *file = (Access_File *) fh;
		Access_File *new_file = (Access_File *) Network_Db_File (NEW_PROCESS_LINK);

		new_file->Copy_Fields (file);
		if (!new_file->Write ()) {
			Error ("Writing %s", new_file->File_Type ());
		}
		return (true);
	}
	return (false);
}
