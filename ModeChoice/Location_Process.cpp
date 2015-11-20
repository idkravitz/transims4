//*********************************************************
//	Location_Process.cpp - Read the Activity Location File
//*********************************************************

#include "ModeChoice.hpp"

#include "Location_File.hpp"

//---------------------------------------------------------
//	Location_Processing
//---------------------------------------------------------

bool ModeChoice::Location_Processing (Db_File *fh)
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
		return (true);
	}
	return (false);
}
