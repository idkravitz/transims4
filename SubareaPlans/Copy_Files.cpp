//*********************************************************
//	Copy_Files.cpp - copies files to subarea 
//*********************************************************

#include "SubareaPlans.hpp"

//---------------------------------------------------------
//	Access_Processing
//---------------------------------------------------------

bool SubareaPlans::Access_Processing (Db_File *fh)
{
	int id;
	Access_File *file = (Access_File *) fh;

    id = file->ID ();

	if (id == 0) return (false);

	if (id > max_access) max_access = id;

    if (access_flag) {

		//---- save the record to the subarea file ----

		access_file->Copy_Fields (file);

		if (!access_file->Write ()) {
			Error ("Writing %s", access_file->File_Type ());
		}
	}

	//---- don't save the record ----

	return (false);
}

//---------------------------------------------------------
//	Location_Processing
//---------------------------------------------------------

bool SubareaPlans::Location_Processing (Db_File *fh)
{
	if (Network_Service::Location_Processing (fh)) {
		int id;
		Location_File *file = (Location_File *) fh;

		id = file->Location ();

		if (id == 0) return (false);

		if (id > max_location) max_location = id;

		id = file->Zone ();

		if (id > max_zone) max_zone = id;

		if (location_flag) {

			//---- save the record to the subarea file ----

			location_file->Copy_Fields (file);

			if (!location_file->Write ()) {
				Error ("Writing %s", location_file->File_Type ());
			}
		}
		return (true);
	}
	return (false);
}

//---------------------------------------------------------
//	Parking_Processing
//---------------------------------------------------------

bool SubareaPlans::Parking_Processing (Db_File *fh)
{
	if (Network_Service::Parking_Processing (fh)) {
		int id;
		Parking_File *file = (Parking_File *) fh;

		id = file->ID ();

		if (id == 0) return (false);

		if (id > max_parking) max_parking = id;

		if (parking_flag) {

			//---- save the record to the subarea file ----

			parking_file->Copy_Fields (file);

			if (!parking_file->Write ()) {
				Error ("Writing %s", parking_file->File_Type ());
			}
		}
		return (true);
	}
	return (false);
}

//---------------------------------------------------------
//	Stop_Processing
//---------------------------------------------------------

bool SubareaPlans::Stop_Processing (Db_File *fh)
{
	if (Network_Service::Stop_Processing (fh)) {
		int id;
		Stop_File *file = (Stop_File *) fh;

		id = file->ID ();
		if (id > max_stop) max_stop = id;

		if (output_flag) {

			//---- save the records to the subarea file ----

			stop_file->Copy_Fields (file);

			if (!stop_file->Write ()) {
				Error ("Writing %s", stop_file->File_Type ());
			}
		}
		return (true);
	}
	return (false);
}
