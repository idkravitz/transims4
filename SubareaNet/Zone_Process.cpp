//*********************************************************
//	Zone_Process.cpp - read the zone file
//*********************************************************

#include "SubareaNet.hpp"

#include "Zone_File.hpp"
#include "In_Polygon.hpp"

//---------------------------------------------------------
//	Zone_Processing
//---------------------------------------------------------

bool SubareaNet::Zone_Processing (Db_File *fh)
{
	static Zone_File *new_file;

	double x, y;

	Zone_File *file = (Zone_File *) fh;

	if (First_Record ()) {
		new_file = (Zone_File *) Network_Db_Base (NEW_ZONE);
	}

	//---- check the ID and output file ----

	if (file->Zone () == 0 || new_file == NULL) return (false);

	//---- check the coordinates ----

	x = file->X ();
	y = file->Y ();

	if (In_Polygon (x, y, &subarea_boundary.points)) {

		//---- copy the fields to the subarea file ----

		new_file->Copy_Fields (file);

		if (!new_file->Write ()) {
			Error ("Writing %s", new_file->File_Type ());
		}
		nzone++;
	}
	
	//---- don't save the record ----

	return (false);
}
