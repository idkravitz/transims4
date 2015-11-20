//*********************************************************
//	Zone_Process.cpp - Read the Zone File
//*********************************************************

#include "ModeChoice.hpp"

#include "Zone_File.hpp"

//---------------------------------------------------------
//	Zone_Processing
//---------------------------------------------------------

bool ModeChoice::Zone_Processing (Db_File *fh)
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
		return (true);
	}
	return (false);
}
