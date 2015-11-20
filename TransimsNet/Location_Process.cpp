//*********************************************************
//	Location_Process.cpp - Read the activity location File
//*********************************************************

#include "TransimsNet.hpp"

#include "Location_File.hpp"

//---------------------------------------------------------
//	Location_Processing
//---------------------------------------------------------

bool TransimsNet::Location_Processing (Db_File *fh)
{
	Location_File *file = (Location_File *) fh;
	
	if (file->Location () > mactivity) mactivity = file->Location ();

	if (update_flag && update_links.In_Range (file->Link ()) ||
		delete_flag && delete_links.In_Range (file->Link ())) {
		xactivity++;
		return (false);
	}
	if (Network_Service::Location_Processing (fh)) {
		Location_File *new_file = (Location_File *) Network_Db_File (NEW_ACTIVITY_LOCATION);

		new_file->Copy_Fields (file);
		if (!new_file->Write ()) {
			Error ("Writing %s", new_file->File_Type ());
		}
		return (true);
	}
	return (false);
}
