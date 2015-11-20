//*********************************************************
//	New_Connect.cpp - write the new lane connectivity
//*********************************************************

#include "VISSIMNet.hpp"

#include "Connect_File.hpp"
#include "Utility.hpp"

//---------------------------------------------------------
//	New_Connect
//---------------------------------------------------------

void VISSIMNet::New_Connect (void)
{
	int i, j, link, link2, lane, type;

	Link_Splits *split_ptr;
	Link_Name *link_ptr;
	Dir_List *dir_ab_ptr, *dir_ba_ptr;
	Connect_Data *connect_ptr, *new_connect_ptr;
	Connect_File *connect_file;

	//---- copy and renumber the lane connections ----

	for (connect_ptr = connect_data.First_Key (); connect_ptr; connect_ptr = connect_data.Next_Key ()) {

		new_connect_ptr = new_connect_data.New_Record (true);

		memcpy (new_connect_ptr, connect_ptr, sizeof (Connect_Data));

		if (connect_ptr->In_Dir () == 1) {
			new_connect_ptr->In_Link (connect_ptr->In_Link () * MAX_SPLITS);
		} else {
			link = connect_ptr->In_Link ();

			split_ptr = (Link_Splits *) link_splits.Get (&link);

			new_connect_ptr->In_Link (connect_ptr->In_Link () * MAX_SPLITS + split_ptr->num_splits);
		}

		if (connect_ptr->Out_Dir () == 0) {
			new_connect_ptr->Out_Link (connect_ptr->Out_Link () * MAX_SPLITS);
		} else {
			link = connect_ptr->Out_Link ();

			split_ptr = (Link_Splits *) link_splits.Get (&link);

			new_connect_ptr->Out_Link (connect_ptr->Out_Link () * MAX_SPLITS + split_ptr->num_splits);
		}

		if (!new_connect_data.Add ()) {
			Error ("Adding New Lane Connectivity Data");
		}
	}

	//---- add connections for each parking link ----


	//---- add parking connection links ----

	for (split_ptr = (Link_Splits *) link_splits.First_Key (); split_ptr; split_ptr = (Link_Splits *) link_splits.Next_Key ()) {
		if (split_ptr->num_splits == 0) continue;

		link_ptr = link_data [split_ptr->record];
		dir_ab_ptr = dir_data [link_ptr->AB_Dir ()];
		dir_ba_ptr = dir_data [link_ptr->BA_Dir ()];

		for (i=0; i < split_ptr->num_splits; i++) {

			//---- add thru lanes ----

			if (dir_ab_ptr != NULL) {
				new_connect_ptr = new_connect_data.New_Record (true);

				link = link_ptr->Link () * MAX_SPLITS + i;

				new_connect_ptr->In_Link (link);
				new_connect_ptr->In_Dir (0);

				new_connect_ptr->Out_Link (link + 1);
				new_connect_ptr->Out_Dir (0);

				lane = dir_ab_ptr->Left () + 1;

				new_connect_ptr->In_Lane_Low (lane);
				new_connect_ptr->Out_Lane_Low (lane);

				lane = dir_ab_ptr->Left () + dir_ab_ptr->Thru ();

				new_connect_ptr->In_Lane_High (lane);
				new_connect_ptr->Out_Lane_High (lane);

				new_connect_ptr->Type (THRU);

				if (!new_connect_data.Add ()) {
					Error ("Adding Parking Connections");
				}
			}

			if (dir_ba_ptr != NULL) {
				new_connect_ptr = new_connect_data.New_Record (true);

				link = link_ptr->Link () * MAX_SPLITS + i + 1;

				new_connect_ptr->In_Link (link);
				new_connect_ptr->In_Dir (1);

				new_connect_ptr->Out_Link (link - 1);
				new_connect_ptr->Out_Dir (1);

				lane = dir_ba_ptr->Left () + 1;

				new_connect_ptr->In_Lane_Low (lane);
				new_connect_ptr->Out_Lane_Low (lane);

				lane = dir_ba_ptr->Left () + dir_ba_ptr->Thru ();

				new_connect_ptr->In_Lane_High (lane);
				new_connect_ptr->Out_Lane_High (lane);

				new_connect_ptr->Type (THRU);

				if (!new_connect_data.Add ()) {
					Error ("Adding Parking Connections");
				}
			}

			//---- add parking turns ----

			for (j=0; j < 2; j++) {
				if (j == 0) {
					link = split_ptr->link_ab [i];
					link2 = split_ptr->link_ba [i];
				} else {
					link = split_ptr->link_ba [i];
					link2 = split_ptr->link_ab [i];
				}
				if (link <= 0) continue;

				//---- add thru lane ----

				if (link2 > 0) {
					new_connect_ptr = new_connect_data.New_Record (true);

					new_connect_ptr->In_Link (link);
					new_connect_ptr->In_Dir (1);
					new_connect_ptr->In_Lane_Low (1);
					new_connect_ptr->In_Lane_High (1);

					new_connect_ptr->Out_Link (link2);
					new_connect_ptr->Out_Dir (0);
					new_connect_ptr->Out_Lane_Low (1);
					new_connect_ptr->Out_Lane_High (1);
					new_connect_ptr->Type (THRU);

					if (!new_connect_data.Add ()) {
						Error ("Adding Parking Connections");
					}
				}

				//---- add turning movements ----

				if (dir_ab_ptr != NULL) {
					new_connect_ptr = new_connect_data.New_Record (true);

					new_connect_ptr->In_Link (link);
					new_connect_ptr->In_Dir (1);
					new_connect_ptr->In_Lane_Low (1);
					new_connect_ptr->In_Lane_High (1);

					new_connect_ptr->Out_Link (link_ptr->Link () * MAX_SPLITS + i + 1);
					new_connect_ptr->Out_Dir (0);
					if (j == 0) {
						lane = dir_ab_ptr->Left () + dir_ab_ptr->Thru ();
						type = RIGHT;
					} else {
						lane = dir_ab_ptr->Left () + 1;
						type = LEFT;
					}
					new_connect_ptr->Out_Lane_Low (lane);
					new_connect_ptr->Out_Lane_High (lane);
					new_connect_ptr->Type (type);

					if (!new_connect_data.Add ()) {
						Error ("Adding Parking Connections");
					}

					new_connect_ptr = new_connect_data.New_Record (true);

					new_connect_ptr->Out_Link (link);
					new_connect_ptr->Out_Dir (0);
					new_connect_ptr->Out_Lane_Low (1);
					new_connect_ptr->Out_Lane_High (1);

					new_connect_ptr->In_Link (link_ptr->Link () * MAX_SPLITS + i);
					new_connect_ptr->In_Dir (0);
					if (j == 0) {
						lane = dir_ab_ptr->Left () + dir_ab_ptr->Thru ();
						type = RIGHT;
					} else {
						lane = dir_ab_ptr->Left () + 1;
						type = LEFT;
					}
					new_connect_ptr->In_Lane_Low (lane);
					new_connect_ptr->In_Lane_High (lane);
					new_connect_ptr->Type (type);

					if (!new_connect_data.Add ()) {
						Error ("Adding Parking Connections");
					}
				}
				if (dir_ba_ptr != NULL) {
					new_connect_ptr = new_connect_data.New_Record (true);

					new_connect_ptr->In_Link (link);
					new_connect_ptr->In_Dir (1);
					new_connect_ptr->In_Lane_Low (1);
					new_connect_ptr->In_Lane_High (1);

					new_connect_ptr->Out_Link (link_ptr->Link () * MAX_SPLITS + i);
					new_connect_ptr->Out_Dir (1);
					if (j == 0) {
						lane = dir_ba_ptr->Left () + 1;
						type = LEFT;
					} else {
						lane = dir_ba_ptr->Left () + dir_ba_ptr->Thru ();
						type = RIGHT;
					}
					new_connect_ptr->Out_Lane_Low (lane);
					new_connect_ptr->Out_Lane_High (lane);
					new_connect_ptr->Type (type);

					if (!new_connect_data.Add ()) {
						Error ("Adding Parking Connections");
					}
					new_connect_ptr = new_connect_data.New_Record (true);

					new_connect_ptr->Out_Link (link);
					new_connect_ptr->Out_Dir (0);
					new_connect_ptr->Out_Lane_Low (1);
					new_connect_ptr->Out_Lane_High (1);

					new_connect_ptr->In_Link (link_ptr->Link () * MAX_SPLITS + i + 1);
					new_connect_ptr->In_Dir (1);
					if (j == 0) {
						lane = dir_ba_ptr->Left () + 1;
						type = LEFT;
					} else {
						lane = dir_ba_ptr->Left () + dir_ba_ptr->Thru ();
						type = RIGHT;
					}
					new_connect_ptr->In_Lane_Low (lane);
					new_connect_ptr->In_Lane_High (lane);
					new_connect_ptr->Type (type);

					if (!new_connect_data.Add ()) {
						Error ("Adding Parking Connections");
					}
				}
			}
		}
	}

	//---- write the new lane connectivity file ----

	if (!Network_File_Flag (NEW_LANE_CONNECTIVITY)) return;

	connect_file = (Connect_File *) Network_Db_Base (NEW_LANE_CONNECTIVITY);

	for (connect_ptr = new_connect_data.First (); connect_ptr; connect_ptr = new_connect_data.Next ()) {
		link_ptr = new_link_data.Get (connect_ptr->In_Link ());

		if (connect_ptr->In_Dir () == 0) {
			connect_file->Node (link_ptr->Bnode ());
		} else {
			connect_file->Node (link_ptr->Anode ());
		}
		connect_file->In_Link (connect_ptr->In_Link ());
		connect_file->Out_Link (connect_ptr->Out_Link ());
		connect_file->In_Low (connect_ptr->In_Lane_Low ());
		connect_file->In_High (connect_ptr->In_Lane_High ());
		connect_file->Out_Low (connect_ptr->Out_Lane_Low ());
		connect_file->Out_High (connect_ptr->Out_Lane_High ());
		connect_file->Type (Connect_Code ((Connect_Type) connect_ptr->Type ()));
		connect_file->Penalty (connect_ptr->Penalty ());
		connect_file->Speed (Resolve (connect_ptr->Speed ()));
		connect_file->Capacity (connect_ptr->Capacity ());

		if (!connect_file->Write ()) {
			Error ("Writing %s", connect_file->File_Type ());
		}
	}
	connect_file->Close ();

	Print (2, "Number of New Lane Connectivity Records = %d", new_connect_data.Num_Records ());
}
