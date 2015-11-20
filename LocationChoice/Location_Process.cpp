//*********************************************************
//	Location_Process.cpp - Read the Activity Location File
//*********************************************************

#include "LocationChoice.hpp"

#include "Location_File.hpp"

//---------------------------------------------------------
//	Location_Processing
//---------------------------------------------------------

bool LocationChoice::Location_Processing (Db_File *fh)
{
	Location_File *file;

	if (Network_Service::Location_Processing (fh)) {

		file = (Location_File *) fh;

		if (First_Record ()) {
			if (!location_db.Max_Records (location_data.Max_Records ())) {
				Error ("Insufficient Memory for Activity Location Database");
			}
		}
		location_db.Copy_Fields (file);

		if (!location_db.Add_Record ()) {
			Error ("Writing Activity Location Database");
		}

		//---- add the record to the zone list -----

		if (zone_flag) {
			Zone_List *zone_ptr;
			Choice_Data *choice_ptr;

			choice_ptr = location_data.New_Record ();

			if (choice_ptr != NULL) {
				int zone, location;

				zone = choice_ptr->Zone ();
				location = choice_ptr->ID ();

				zone_ptr = zone_data.Get (zone);

				if (zone_ptr == NULL) {
					if (zone != 0) {
						Warning ("Location %d Zone %d was Not Found", location, zone);
					} else {
						zero_zone++;
					}
				} else {
					choice_ptr->List (zone_ptr->Location ());
					zone_ptr->Location (location_data.Num_Records () + 1);

					if (zone_ptr->Min_X () == 0 || choice_ptr->X () < zone_ptr->Min_X ()) {
						zone_ptr->Min_X (choice_ptr->X ());
					}
					if (zone_ptr->Min_Y () == 0 || choice_ptr->Y () < zone_ptr->Min_Y ()) {
						zone_ptr->Min_Y (choice_ptr->Y ());
					}
					if (zone_ptr->Max_X () == 0 || choice_ptr->X () > zone_ptr->Max_X ()) {
						zone_ptr->Max_X (choice_ptr->X ());
					}
					if (zone_ptr->Max_Y () == 0 || choice_ptr->Y () > zone_ptr->Max_Y ()) {
						zone_ptr->Max_Y (choice_ptr->Y ());
					}
				}
			}
		}
		return (true);
	}
	return (false);
}
