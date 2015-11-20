//*********************************************************
//	Turn_Process.cpp - read the turn prohibition file
//*********************************************************

#include "SubareaNet.hpp"

#include "Turn_File.hpp"

//---------------------------------------------------------
//	Turn_Processing
//---------------------------------------------------------

bool SubareaNet::Turn_Processing (Db_File *fh)
{
	static Turn_File *new_file;

	int node;
	Node_Data *node_ptr;

	Turn_File *file = (Turn_File *) fh;
	
	if (First_Record ()) {
		new_file = (Turn_File *) Network_Db_Base (NEW_TURN_PROHIBITION);
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
		nturn++;
	}
	
	//---- don't save the record ----

	return (false);
}
