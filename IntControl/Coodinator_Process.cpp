//*********************************************************
//	Coordinator_Process.cpp - additional signal coordinator processing
//*********************************************************

#include "IntControl.hpp"

//---------------------------------------------------------
//	Coordinator_Processing
//---------------------------------------------------------

bool IntControl::Coordinator_Processing (Db_File *fh)
{
	if (Network_Service::Coordinator_Processing (fh)) {
		Coordinator_Data *coordinator_ptr = coordinator_data.New_Record ();

		if (coordinator_ptr->ID () > max_coord) max_coord = coordinator_ptr->ID ();

		if (coord_flag) {
			coord_db.Copy_Fields ((Db_Base *) fh);

			if (!coord_db.Add_Record ()) {
				Error ("Writing Coordinator Database");
			}
		}
		return (true);
	}
	return (false);
}
