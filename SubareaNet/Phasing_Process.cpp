//*********************************************************
//	Phasing_Process.cpp - read the phasing plan file
//*********************************************************

#include "SubareaNet.hpp"

#include "Phasing_File.hpp"

//---------------------------------------------------------
//	Phasing_Processing
//---------------------------------------------------------

bool SubareaNet::Phasing_Processing (Db_File *fh)
{
	static Phasing_File *new_file;

	int node;
	Node_Data *node_ptr;

	Phasing_File *file = (Phasing_File *) fh;

	if (First_Record ()) {
		new_file = (Phasing_File *) Network_Db_Base (NEW_PHASING_PLAN);
	}

	//---- read and save the data ----

	node = file->Node ();
	if (node == 0 || new_file == NULL) return (false);

	node_ptr = node_data.Get (node);

	//---- check the node number ----

	if (node_ptr->Type () == 1) {

		//---- copy the fields to the subarea file ----

		new_file->Copy_Fields (file);

		if (!new_file->Write ()) {
			Error ("Writing %s", new_file->File_Type ());
		}
		nphasing++;
	}
	
	//---- don't save the record ----

	return (false);
}
