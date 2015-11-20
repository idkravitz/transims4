//*********************************************************
//	Stop_Process.cpp - save subarea stop data 
//*********************************************************

#include "SubareaNet.hpp"

#include "Stop_File.hpp"

//---------------------------------------------------------
//	Stop_Processing
//---------------------------------------------------------

bool SubareaNet::Stop_Processing (Db_File *fh)
{
	static Stop_File *new_file;

	int id;
	Stop_File *file = (Stop_File *) fh;
	Link_Data *link_ptr;

	if (First_Record ()) {
		new_file = (Stop_File *) Network_Db_Base (NEW_TRANSIT_STOP);
	}

	if (Network_Service::Stop_Processing (fh)) {

		id = file->ID ();
		if (id > max_stop) max_stop = id;

		link_ptr = link_data.Get (file->Link ());

		if (link_ptr != NULL && link_ptr->Type () > 0) {

			//---- copy the fields to the subarea file ----

			new_file->Copy_Fields (file);

			if (!new_file->Write ()) {
				Error ("Writing %s", new_file->File_Type ());
			}
			nstop++;
		}
		return (true);
	}
	return (false);
}
