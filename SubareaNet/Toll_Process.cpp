//*********************************************************
//	Toll_Process.cpp - read the toll file
//*********************************************************

#include "SubareaNet.hpp"

#include "Toll_File.hpp"

//---------------------------------------------------------
//	Toll_Processing
//---------------------------------------------------------

bool SubareaNet::Toll_Processing (Db_File *fh)
{
	static Toll_File *new_file;

	int link;
	Toll_File *file = (Toll_File *) fh;
	Link_Data *link_ptr;

	if (First_Record ()) {
		new_file = (Toll_File *) Network_Db_Base (NEW_TOLL);
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
		ntoll++;
	}
	
	//---- don't save the record ----

	return (false);
}
