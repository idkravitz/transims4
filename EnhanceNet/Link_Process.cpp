//*********************************************************
//	Link_Process.cpp - additonal link processing
//*********************************************************

#include "EnhanceNet.hpp"

#include "Link_File.hpp"

//---------------------------------------------------------
//	Link_Processing
//---------------------------------------------------------

bool EnhanceNet::Link_Processing (Db_File *fh)
{
	static Link_File *new_file;
	static bool new_flag;

	Link_File *file = (Link_File *) fh;

	if (First_Record ()) {
		new_file = (Link_File *) Network_Db_File (NEW_LINK);
		new_flag = (new_file != NULL);
	}

	if (Network_Service::Link_Processing (file)) {
		if (new_flag) {
			new_file->Copy_Fields (file);

			if (!file->Bearing_Flag () || file->Bearing_A () == 0) {
				Dir_Data *dir_ptr = dir_data.New_Record ();

				if (dir_ptr->Dir () == 1) {
					new_file->Bearing_A (compass.Flip (dir_ptr->Out_Bearing ()));
					new_file->Bearing_B (compass.Flip (dir_ptr->In_Bearing ()));
				} else {
					new_file->Bearing_A (dir_ptr->In_Bearing ());
					new_file->Bearing_B (dir_ptr->Out_Bearing ());
				}
			}
			if (!new_file->Write ()) {
				Error ("Writing New Link File");
			}
		}
		return (true);
	}
	return (false);
}
