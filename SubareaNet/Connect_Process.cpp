//*********************************************************
//	Connect_Process.cpp - read the lane connectivity file
//*********************************************************

#include "SubareaNet.hpp"

#include "Connect_File.hpp"

//---------------------------------------------------------
//	Connectivity_Processing
//---------------------------------------------------------

bool SubareaNet::Connectivity_Processing (Db_File *fh)
{
	static Connect_File *new_file;

	int node;
	Node_Data *node_ptr;

	Connect_File *file = (Connect_File *) fh;
	
	if (First_Record ()) {
		new_file = (Connect_File *) Network_Db_Base (NEW_LANE_CONNECTIVITY);
	}

	//---- read and save the data ----

	node = file->Node ();
	if (node == 0) return (false);

	node_ptr = node_data.Get (node);
	if (node_ptr == NULL) {
		Warning ("Connection Node %d was Not Found", node);
		return (false);
	}

	//---- check the node number ----

	if (node_ptr->Type () == 1) {

		//---- copy the fields to the subarea file ----

		new_file->Copy_Fields (file);

		if (!new_file->Write ()) {
			Error ("Writing %s", new_file->File_Type ());
		}
		nconnect++;
	}
	
	//---- don't save the record ----

	return (false);
}

