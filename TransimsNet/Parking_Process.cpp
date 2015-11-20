//*********************************************************
//	Parking_Process.cpp - Read the Parking File
//*********************************************************

#include "TransimsNet.hpp"

#include "Parking_File.hpp"

//---------------------------------------------------------
//	Parking_Processing
//---------------------------------------------------------

bool TransimsNet::Parking_Processing (Db_File *fh)
{
	Parking_File *file = (Parking_File *) fh;

	if (file->ID () > mparking) mparking = file->ID ();

	if ((update_flag && update_links.In_Range (file->Link ())) ||
		(delete_flag && delete_links.In_Range (file->Link ()))) {
		xparking++;
		return (false);
	}
	Parking_File *new_file = (Parking_File *) Network_Db_File (NEW_PARKING);

	new_file->Copy_Fields (file);
	if (!new_file->Write ()) {
		Error ("Writing %s", new_file->File_Type ());
	}
	return (false);
}
