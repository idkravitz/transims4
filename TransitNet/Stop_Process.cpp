//*********************************************************
//	Stop_Process.cpp - Read the Transit Stop File
//*********************************************************

#include "TransitNet.hpp"

#include "Stop_File.hpp"

//---------------------------------------------------------
//	Stop_Processing
//---------------------------------------------------------

bool TransitNet::Stop_Processing (Db_File *fh)
{
	int id;
	Stop_File *file, *new_file;
	Stop_Data *stop_ptr;
	Dir_Path_Data *dir_ptr;

	if (Network_Service::Stop_Processing (fh)) {

		//---- update the stop list ----

		stop_ptr = stop_data.New_Record ();

		dir_ptr = dir_data.Get (stop_ptr->Link_Dir ());
		if (dir_ptr == NULL) return (false);

		stop_ptr->Traveler (dir_ptr->Stop_List ());
		dir_ptr->Stop_List (stop_data.Num_Records () + 1);

		//---- copy the record to the output file ----

		file = (Stop_File *) fh;

		id = file->ID ();
		if (id > max_stop) max_stop = id;
		nstop++;

		new_file = (Stop_File *) Network_Db_Base (NEW_TRANSIT_STOP);

		new_file->Copy_Fields (file);

		if (!new_file->Write ()) {
			Error ("Writing %s", new_file->File_Type ());
		}
		return (true);
	}
	return (false);
}
