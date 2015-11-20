//*********************************************************
//	Sign_Process.cpp - read the sign file
//*********************************************************

#include "SubareaNet.hpp"

#include "Sign_File.hpp"

//---------------------------------------------------------
//	Sign_Processing
//---------------------------------------------------------

bool SubareaNet::Sign_Processing (Db_File *fh)
{
	static Sign_File *new_file;

	int link;
	Sign_File *file = (Sign_File *) fh;
	Link_Data *link_ptr;

	if (First_Record ()) {
		new_file = (Sign_File *) Network_Db_Base (NEW_UNSIGNALIZED_NODE);
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
		nsign++;
	}
	
	//---- don't save the record ----

	return (false);
}
