//*********************************************************
//	Sign_Process.cpp - Read the Sign File
//*********************************************************

#include "TransimsNet.hpp"

#include "Sign_File.hpp"

//---------------------------------------------------------
//	Sign_Processing
//---------------------------------------------------------

bool TransimsNet::Sign_Processing (Db_File *fh)
{
	Sign_File *file = (Sign_File *) fh;

	if ((update_flag && update_nodes.In_Range (file->Node ())) ||
		(delete_flag && delete_nodes.In_Range (file->Node ()))) {
		xsign++;
		return (false);
	}
	Sign_File *new_file = (Sign_File *) Network_Db_File (NEW_UNSIGNALIZED_NODE);

	new_file->Copy_Fields (file);
	if (!new_file->Write ()) {
		Error ("Writing %s", new_file->File_Type ());
	}
	return (false);
}
