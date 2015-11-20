//*********************************************************
//	Access_Process.cpp - Read the Process Link File
//*********************************************************

#include "TransitNet.hpp"

#include "Access_File.hpp"

//---------------------------------------------------------
//	Access_Processing
//---------------------------------------------------------

bool TransitNet::Access_Processing (Db_File *fh)
{
	Access_File *file = (Access_File *) fh;

	int id = file->ID ();
	if (id <= 0) return (false);

	if (id > max_access) max_access = id;

	if (End_Code (file->From_Type ()) == STOP_ID) {
		if (stop_data.Get (file->From_ID ()) == NULL) return (false);
	} else if (End_Code (file->To_Type ()) == STOP_ID) {
		if (stop_data.Get (file->To_ID ()) == NULL) return (false);
	}
	Access_File *new_file = (Access_File *) Network_Db_Base (NEW_PROCESS_LINK);

	new_file->Copy_Fields (file);

	if (!new_file->Write ()) {
		Error ("Writing %s", new_file->File_Type ());
	}
	naccess++;
	return (false);
}
