//*********************************************************
//	Location_Process.cpp - Read the Activity Location File
//*********************************************************

#include "TransitNet.hpp"

#include "Location_File.hpp"

//---------------------------------------------------------
//	Location_Processing
//---------------------------------------------------------

bool TransitNet::Location_Processing (Db_File *fh)
{
	int zone;

	if (Network_Service::Location_Processing (fh)) {
		Location_File *file = (Location_File *) fh;

		int id = file->Location ();
		if (id > max_location) max_location = id;

		zone = file->Zone ();

		if (zone > 0) {
			Location_Data *location_ptr = location_data.New_Record ();

			Dir_Path_Data *dir_ptr = dir_data.Get (location_ptr->Link_Dir ());

			if (dir_ptr != NULL) {
				dir_ptr->Zone (zone);
			}
		}

		Location_File *new_file = (Location_File *) Network_Db_Base (NEW_ACTIVITY_LOCATION);

		new_file->Copy_Fields (file);

		if (!new_file->Write ()) {
			Error ("Writing %s", new_file->File_Type ());
		}
		nlocation++;
		return (true);
	}
	return (false);
}
