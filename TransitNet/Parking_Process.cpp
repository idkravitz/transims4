//*********************************************************
//	Parking_Process.cpp - Read the Parking File
//*********************************************************

#include "TransitNet.hpp"

#include "Parking_File.hpp"

//---------------------------------------------------------
//	Parking_Processing
//---------------------------------------------------------

bool TransitNet::Parking_Processing (Db_File *fh)
{
	int link, dir, offset;

	Link_Data *link_ptr;
	Dir_Path_Data *dir_ptr;
	Parking_File *file = (Parking_File *) fh;

	int id = file->ID ();
	if (id == 0) return (false);

	if (id > max_parking) max_parking = id;

	Parking_File *new_file = (Parking_File *) Network_Db_Base (NEW_PARKING);

	new_file->Copy_Fields (file);

	if (!new_file->Write ()) {
		Error ("Writing %s", new_file->File_Type ());
	}
	nparking++;

	if (!parkride_flag) return (false);

	//---- get the directional link ----

	link = file->Link ();
	dir = file->Node ();

	if (file->LinkDir_Type () == LINK_SIGN) {
		if (link < 0) {
			link = -link;
			dir = 1;
		}
	}
	link_ptr = link_data.Get (link);
	if (link_ptr == NULL) return (false);

	if (file->LinkDir_Type () == LINK_NODE) {
		if (dir == link_ptr->Anode ()) {
			dir = 1;
		} else if (dir == link_ptr->Bnode ()) {
			dir = 0;
		}
	}
	if (dir > 0) {
		dir = link_ptr->BA_Dir ();
	} else {
		dir = link_ptr->AB_Dir ();
	}
	
	dir_ptr = dir_data [dir];
	if (dir_ptr == NULL) return (false);

	offset = Round (file->Offset ());

	if (dir_ptr->Offset () == 0 || dir_ptr->Offset () > offset) {
		dir_ptr->Offset (offset);
	}
	return (false);
}
