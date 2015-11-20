//*********************************************************
//	Coordinator_Process.cpp - additional signal processing
//*********************************************************

#include "ArcNet.hpp"

#include "Coordinator_File.hpp"

//---------------------------------------------------------
//	Coordinator_Processing
//---------------------------------------------------------

bool ArcNet::Coordinator_Processing (Db_File *fh)
{
	Coordinator_File *file;

	if (Network_Service::Coordinator_Processing (fh)) {
		file = (Coordinator_File *) fh;

		if (First_Record ()) {
			if (!coordinator_db.Max_Records (coordinator_data.Max_Records ())) {
				Error ("Insufficient Memory for Signal Coordinator Database");
			}
		}
		coordinator_db.Copy_Fields (file);

		if (!coordinator_db.Add_Record ()) {
			Error ("Writing Signal Coordinator Database");
		}
	}
	return (false);
}
