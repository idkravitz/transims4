//*********************************************************
//	Pocket_Process.cpp - Read the Pocket File
//*********************************************************

#include "TransimsNet.hpp"

#include "Pocket_File.hpp"

//---------------------------------------------------------
//	Pocket_Processing
//---------------------------------------------------------

bool TransimsNet::Pocket_Processing (Db_File *fh)
{
	int link;
	Pocket_File *file = (Pocket_File *) fh;	

	if (file->ID () > mpocket) mpocket = file->ID ();

	link = file->Link ();

	if ((update_flag && update_links.In_Range (link)) ||
		(delete_flag && delete_links.In_Range (link))){
		xpocket++;
		return (false);
	}

	if (Network_Service::Pocket_Processing (fh)) {
		Pocket_Data *pocket_ptr = pocket_data.New_Record ();

		Pocket_File *new_file = (Pocket_File *) Network_Db_File (NEW_POCKET_LANE);

		new_file->Copy_Fields (file);
		if (!new_file->Write ()) {
			Error ("Writing %s", new_file->File_Type ());
		}

		//---- update the link data ----

		Dir_Connect *dir_ptr;
		Link_Data *link_ptr = link_data.Get (link);

		if (pocket_ptr->Dir () == 0) {
			dir_ptr = dir_data [link_ptr->AB_Dir ()];
		} else {
			dir_ptr = dir_data [link_ptr->BA_Dir ()];
		}

		if (dir_ptr != NULL) {
			int lane = pocket_ptr->Lane ();

			if (pocket_ptr->Type () == TURN_LANE) {
				if (lane <= dir_ptr->Left ()) {
					lane = dir_ptr->Left () - lane + 1;
					if (lane > dir_ptr->Left_Turn ()) {
						dir_ptr->Left_Turn (lane);
					}
				} else {
					lane = lane - dir_ptr->Left () - dir_ptr->Thru ();
					if (lane > dir_ptr->Right_Turn ()) {
						dir_ptr->Right_Turn (lane);
					}
				}
			} else if (pocket_ptr->Type () == MERGE_LANE) {
				if (lane <= dir_ptr->Left ()) {
					lane = dir_ptr->Left () - lane + 1;
					if (lane > dir_ptr->Left_Merge ()) {
						dir_ptr->Left_Merge (lane);
					}
				} else {
					lane = lane - dir_ptr->Left () - dir_ptr->Thru ();
					if (lane > dir_ptr->Right_Merge ()) {
						dir_ptr->Right_Merge (lane);
					}
				}
			}
		}
	}
	return (false);
}
