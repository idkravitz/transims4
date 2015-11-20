//*********************************************************
//	Vehicle_Process.cpp - Read the Vehicle File
//*********************************************************

#include "PopSyn.hpp"

#include "Vehicle_File.hpp"

//---------------------------------------------------------
//	Vehicle_Processing
//---------------------------------------------------------

bool PopSyn::Vehicle_Processing (Db_File *fh)
{
	int veh;
	Vehicle_File *file;

	file = (Vehicle_File *) fh;

	veh = file->Vehicle ();
	if (veh == 0) return (false);

	if (veh > max_veh) max_veh = veh;
	old_veh++;

	new_veh_file->Copy_Fields (file);

	if (!new_veh_file->Write ()) {
		Error ("Writing %s", new_veh_file->File_Type ());
	}
	return (false);
}
