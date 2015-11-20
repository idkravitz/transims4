//*********************************************************
//	Pocket_Process.cpp - read the pocket lane file
//*********************************************************

#include "ArcDelay.hpp"

//---------------------------------------------------------
//	Pocket_Processing
//---------------------------------------------------------

bool ArcDelay::Pocket_Processing (Db_File *fh)
{
	Pocket_Data *pocket_ptr;
	Pocket_Index index_rec;

	if (Network_Service::Pocket_Processing (fh)) {

		pocket_ptr = pocket_data.New_Record ();

		index_rec.link_dir = pocket_ptr->Link_Dir ();
		index_rec.pocket_id = pocket_ptr->ID ();

		if (!pocket_index.Add (&index_rec)) {
			Error ("Adding Pocket Lane Index");
		}
		return (true);
	}
	return (false);
}
