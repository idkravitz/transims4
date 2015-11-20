//*********************************************************
//	Parking_Process.cpp - save subarea parking data 
//*********************************************************

#include "SubareaNet.hpp"

#include "Parking_File.hpp"

//---------------------------------------------------------
//	Parking_Processing
//---------------------------------------------------------

bool SubareaNet::Parking_Processing (Db_File *fh)
{
	static Parking_File *new_file;

	int id;
	Parking_File *file = (Parking_File *) fh;
	Link_Data *link_ptr;

	if (First_Record ()) {
		new_file = (Parking_File *) Network_Db_Base (NEW_PARKING);
	}

	id = file->ID ();
	if (id == 0) return (false);

	if (id > max_parking) max_parking = id;

	link_ptr = link_data.Get (file->Link ());

	if (link_ptr != NULL && link_ptr->Type () > 0) {

		//---- copy the fields to the subarea file ----

		new_file->Copy_Fields (file);

		if (!new_file->Write ()) {
			Error ("Writing %s", new_file->File_Type ());
		}
		nparking++;
	}
	
	//---- don't save the record ----

	return (false);
}
