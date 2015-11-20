//*********************************************************
//	New_Sign.cpp - write the new sign records
//*********************************************************

#include "VISSIMNet.hpp"

#include "Sign_File.hpp"
#include "Utility.hpp"

//---------------------------------------------------------
//	New_Sign
//---------------------------------------------------------

void VISSIMNet::New_Sign (void)
{
	int i, j, link, node;

	Link_Splits *split_ptr;
	Link_Name *link_ptr;
	Sign_Data *sign_ptr, *new_sign_ptr;
	Sign_File *sign_file;

	//---- copy and renumber the signs ----

	for (sign_ptr = sign_data.First_Key (); sign_ptr; sign_ptr = sign_data.Next_Key ()) {

		new_sign_ptr = new_sign_data.New_Record (true);

		memcpy (new_sign_ptr, sign_ptr, sizeof (Sign_Data));

		if (sign_ptr->Dir () == 1) {
			new_sign_ptr->Link (sign_ptr->Link () * MAX_SPLITS);
		} else {
			link = sign_ptr->Link ();

			split_ptr = (Link_Splits *) link_splits.Get (&link);

			new_sign_ptr->Link (sign_ptr->Link () * MAX_SPLITS + split_ptr->num_splits);
		}
		if (!new_sign_data.Add ()) {
			Error ("Adding New Sign Data");
		}
	}

	//---- add stops to parking connection links ----


	for (split_ptr = (Link_Splits *) link_splits.First_Key (); split_ptr; split_ptr = (Link_Splits *) link_splits.Next_Key ()) {
		if (split_ptr->num_splits == 0) continue;

		for (i=0; i < split_ptr->num_splits; i++) {
			node = split_ptr->node [i];

			for (j=0; j < 2; j++) {
				if (j == 0) {
					link = split_ptr->link_ab [i];
					if (link <= 0) continue;
				} else {
					link = split_ptr->link_ba [i];
					if (link <= 0) continue;
				}
				new_sign_ptr = new_sign_data.New_Record (true);

				new_sign_ptr->Link (link);
				new_sign_ptr->Dir (1);
				new_sign_ptr->Sign (STOP_SIGN);

				if (!new_sign_data.Add ()) {
					Error ("Adding Parking Stop Sign");
				}
			}
		}
	}

	//---- write the new sign file ----

	if (!Network_File_Flag (NEW_UNSIGNALIZED_NODE)) return;

	sign_file = (Sign_File *) Network_Db_Base (NEW_UNSIGNALIZED_NODE);

	for (sign_ptr = new_sign_data.First (); sign_ptr; sign_ptr = new_sign_data.Next ()) {
		link_ptr = new_link_data.Get (sign_ptr->Link ());

		if (sign_ptr->Dir () == 0) {
			sign_file->Node (link_ptr->Bnode ());
		} else {
			sign_file->Node (link_ptr->Anode ());
		}
		sign_file->Link (sign_ptr->Link ());
		sign_file->Sign (Sign_Code ((Sign_Type) sign_ptr->Sign ()));

		if (!sign_file->Write ()) {
			Error ("Writing %s", sign_file->File_Type ());
		}
	}
	sign_file->Close ();

	Print (2, "Number of New Sign Records = %d", new_sign_data.Num_Records ());
}
