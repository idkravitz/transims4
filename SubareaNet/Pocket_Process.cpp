//*********************************************************
//	Pocket_Process.cpp - read the pocket lane file
//*********************************************************

#include "SubareaNet.hpp"

#include "Pocket_File.hpp"

//---------------------------------------------------------
//	Pocket_Processing
//---------------------------------------------------------

bool SubareaNet::Pocket_Processing (Db_File *fh)
{
	static Pocket_File *new_file;

	int id;
	Pocket_File *file = (Pocket_File *) fh;
	Link_Data *link_ptr;

	if (First_Record ()) {
		new_file = (Pocket_File *) Network_Db_Base (NEW_POCKET_LANE);
	}

	//---- read and save the data ----

	id = file->ID ();
	if (id == 0) return (false);

	//---- check the link number ----

	link_ptr = link_data.Get (file->Link ());

	if (link_ptr != NULL && link_ptr->Type () > 0) {

		//---- copy the fields to the subarea file ----

		new_file->Copy_Fields (file);

		if (!new_file->Write ()) {
			Error ("Writing %s", new_file->File_Type ());
		}
		npocket++;
	}
	
	//---- don't save the record ----

	return (false);
}
