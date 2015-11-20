//*********************************************************
//	Timing_Process.cpp - read the timing plan file
//*********************************************************

#include "SubareaNet.hpp"

#include "Timing_File.hpp"

//---------------------------------------------------------
//	Timing_Processing
//---------------------------------------------------------

bool SubareaNet::Timing_Processing (Db_File *fh)
{
	Timing_File *file = (Timing_File *) fh;

	//---- reserve memory ----

	if (First_Record ()) {
		if (!timing_db.Max_Records (file->Num_Records ())) {
			Error ("Insufficient Memory for Timing Plan Database");
		}
	}
	if (file->Timing () == 0) return (false);

	timing_db.Copy_Fields (file);

	if (!timing_db.Add_Record ()) {
		Error ("Writing Timing Plan Database");
	}
	return (false);
}
