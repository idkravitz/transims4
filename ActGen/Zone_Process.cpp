//*********************************************************
//	Zone_Process.cpp - Read the Zone File
//*********************************************************

#include "ActGen.hpp"

#include "Zone_File.hpp"

//---------------------------------------------------------
//	Zone_Processing
//---------------------------------------------------------

bool ActGen::Zone_Processing (Db_File *fh)
{
	Zone_File *file;

	if (Network_Service::Zone_Processing (fh)) {

		file = (Zone_File *) fh;

		if (First_Record ()) {
			if (!zone_db.Max_Records (zone_data.Max_Records ())) {
				Error ("Insufficient Memory for Zone Database");
			}
		}
		zone_db.Copy_Fields (file);

		if (!zone_db.Add_Record ()) {
			Error ("Writing Zone Database");
		}

		//---- initialize the location list ----

		Zone_List *zone_ptr = zone_data.New_Record ();

		if (zone_ptr != NULL) {
			zone_ptr->Location (0);
			zone_ptr->Min_X (Round (file->Min_X ()));
			zone_ptr->Min_Y (Round (file->Min_Y ()));
			zone_ptr->Max_X (Round (file->Max_X ()));
			zone_ptr->Max_Y (Round (file->Max_Y ()));
		}
		return (true);
	}
	return (false);
}
