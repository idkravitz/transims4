//*********************************************************
//	Write_Signs.cpp - Create the Unsignalized Node File
//*********************************************************

#include "IntControl.hpp"

//---------------------------------------------------------
//	Write_Signs
//---------------------------------------------------------

void IntControl::Write_Signs (void)
{
	int dir, type, sign, index;

	char *notes [] = {
		"IntControl Generated",
		"User-Provided Control"
	};

	Node_List *node_ptr;
	Dir_Connect *dir_ptr;
	Sign_Data *sign_ptr;

	Sign_File *sign_file = (Sign_File *) Network_Db_Base (NEW_UNSIGNALIZED_NODE);

	//---- write the unsignalized node file ----

	Show_Message ("Writing %s -- Record", sign_file->File_Type ());
	Set_Progress ();

	//---- save the original records ----

	if (input_sign_flag) {
		sign_db.Rewind ();

		while (sign_db.Read_Record ()) {
			Show_Progress ();

			sign_file->Copy_Fields (&sign_db);

			sign_db.Get_Field (sign_index, &index);

			sign_ptr = sign_data [index];

			if (sign_ptr != NULL) {
				dir = sign_ptr->Link_Dir ();

				sign_ptr = input_sign.Get (dir);
				if (sign_ptr != NULL) continue;
			} else if (!no_control_flag) {
				nsign++;
				continue;
			}
			if (!sign_file->Write ()) {
				Error ("Writing %s", sign_file->File_Type ());
			}
		}
		Set_Progress ();
	}

	//---- add new records ----

	for (node_ptr = node_data.First_Key (); node_ptr; node_ptr = node_data.Next_Key ()) {
		Show_Progress ();

		if (node_ptr->Type () != 0) continue;

		for (dir = node_ptr->To_Dir (); dir; dir = dir_ptr->To_List ()) {
			dir_ptr = dir_data [dir];

			if (input_sign_flag) {
				sign_ptr = input_sign.Get (dir_ptr->Link_Dir ());
			} else {
				sign_ptr = sign_data.Get (dir_ptr->Link_Dir ());
			}
			if (sign_ptr == NULL) {
				if (input_sign_flag) {
					sign_ptr = sign_data.Get (dir_ptr->Link_Dir ());
					if (sign_ptr != NULL) continue;
				}
				if (!no_control_flag) {
					if (warning_flag) {
						Warning ("A Control Record is Missing on Link %d at Node %d", 
							dir_ptr->Link (), node_ptr->Node ());
					}
					continue;
				}
				type = 0;
				sign = NO_SIGN;
			} else {
				type = 1;
				sign = sign_ptr->Sign ();
			}
			nsign++;
			sign_file->Node (node_ptr->Node ());
			sign_file->Link (dir_ptr->Link ());
			sign_file->Sign (Sign_Code ((Sign_Type) sign));
			sign_file->Notes (notes [type]);

			if (!sign_file->Write ()) {
				Error ("Writing %s", sign_file->File_Type ());
			}
		}
	}
	End_Progress ();

	sign_file->Close ();
}
