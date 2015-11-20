//*********************************************************
//	Timing_Process.cpp - read the timing plan file
//*********************************************************

#include "ArcNet.hpp"

#include "Timing_File.hpp"

//---------------------------------------------------------
//	Timing_Processing
//---------------------------------------------------------

bool ArcNet::Timing_Processing (Db_File *fh)
{
	Timing_File *file;

	if (Network_Service::Timing_Processing (fh)) {

		file = (Timing_File *) fh;

		if (First_Record ()) {
			if (!timing_db.Max_Records (timing_data.Max_Records ())) {
				Error ("Insufficient Memory for Timing Plan Database");
			}
		}
		timing_db.Copy_Fields (file);

		if (!timing_db.Add_Record ()) {
			Error ("Writing Timing Plan Database");
		}
	}
	return (false);
}
