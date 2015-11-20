//*********************************************************
//	Signal_Process.cpp - Read the Signal File
//*********************************************************

#include "TransimsNet.hpp"

#include "Signal_File.hpp"

//---------------------------------------------------------
//	Signal_Processing
//---------------------------------------------------------

bool TransimsNet::Signal_Processing (Db_File *fh)
{
	Signal_File *file = (Signal_File *) fh;

	if ((update_flag && update_nodes.In_Range (file->Node ())) ||
		(delete_flag && delete_nodes.In_Range (file->Node ()))) {
		xsignal++;
		return (false);
	}

	Signal_File *new_file = (Signal_File *) Network_Db_File (NEW_SIGNALIZED_NODE);

	new_file->Copy_Fields (file);
	if (!new_file->Write ()) {
		Error ("Writing %s", new_file->File_Type ());
	}
	return (false);
}
