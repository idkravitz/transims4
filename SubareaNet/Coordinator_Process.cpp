//*********************************************************
//	Coordinator_Process.cpp - read the coordinator file
//*********************************************************

#include "SubareaNet.hpp"

#include "Coordinator_File.hpp"

//---------------------------------------------------------
//	Coordinator_Processing
//---------------------------------------------------------

bool SubareaNet::Coordinator_Processing (Db_File *fh)
{
	Coordinator_File *file = (Coordinator_File *) fh;

	//---- reserve memory ----

	if (First_Record ()) {
		if (!coordinator_db.Max_Records (file->Num_Records ())) {
			Error ("Insufficient Memory for Signal Coordinator Database");
		}
	}
	if (file->ID () == 0) return (false);

	coordinator_db.Copy_Fields (file);

	if (!coordinator_db.Add_Record ()) {
		Error ("Writing Signal Coordinator Database");
	}
	return (false);
}
