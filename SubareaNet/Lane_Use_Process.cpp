//*********************************************************
//	Lane_Use_Process.cpp - read the lane use file
//*********************************************************

#include "SubareaNet.hpp"

#include "Lane_Use_File.hpp"

//---------------------------------------------------------
//	Lane_Use_Processing
//---------------------------------------------------------

bool SubareaNet::Lane_Use_Processing (Db_File *fh)
{
	static Lane_Use_File *new_file;

	int link;
	Lane_Use_File *file = (Lane_Use_File *) fh;
	Link_Data *link_ptr;

	if (First_Record ()) {
		new_file = (Lane_Use_File *) Network_Db_Base (NEW_LANE_USE);
	}

	//---- read and save the data ----

	link = file->Link ();
	if (link == 0 || new_file == NULL) return (false);

	//---- check the link number ----

	link_ptr = link_data.Get (link);

	if (link_ptr != NULL && link_ptr->Type () > 0) {

		//---- copy the fields to the subarea file ----

		new_file->Copy_Fields (file);

		if (!new_file->Write ()) {
			Error ("Writing %s", new_file->File_Type ());
		}
		nlane_use++;
	}
	
	//---- don't save the record ----

	return (false);
}
