//*********************************************************
//	Connect_Process.cpp - Read the lane connectivity File
//*********************************************************

#include "TransimsNet.hpp"

#include "Connect_File.hpp"

//---------------------------------------------------------
//	Connectivity_Processing
//---------------------------------------------------------

bool TransimsNet::Connectivity_Processing (Db_File *fh)
{
	Connect_File *file = (Connect_File *) fh;

	if ((update_flag || delete_flag) && 
		(update_nodes.In_Range (file->Node ()) ||
		delete_nodes.In_Range (file->Node ()) ||
		repair_nodes.In_Range (file->Node ()))) {
		xconnect++;
		return (false);
	}

	Connect_File *new_file = (Connect_File *) Network_Db_File (NEW_LANE_CONNECTIVITY);

	new_file->Copy_Fields (file);
	if (!new_file->Write ()) {
		Error ("Writing %s", new_file->File_Type ());
	}
	return (false);
}
