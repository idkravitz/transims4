//*********************************************************
//	New_Network.cpp - build a new parking link network
//*********************************************************

#include "VISSIMNet.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	New_Network
//---------------------------------------------------------

void VISSIMNet::New_Network (void)
{
	int i, j, link, off_low, off_high, nsplits;

	Link_Splits *split_ptr;
	Link_Name *link_ptr;
	Parking_Data *park_ptr;

	//---- initialize the link splits ----

	for (link_ptr = link_data.First_Key (); link_ptr; link_ptr = link_data.Next_Key ()) {
		split_ptr = (Link_Splits *) link_splits.New_Record (true);

		split_ptr->link = link_ptr->Link ();
		split_ptr->record = link_data.Record_Index ();

		if (!link_splits.Add ()) {
			Error ("Adding Link Splits");
		}
	}

	//---- set the parking offsets ----

	for (park_ptr = parking_data.First (); park_ptr; park_ptr = parking_data.Next ()) {
		if (park_ptr->Type () == BOUNDARY) continue;

		link = park_ptr->Link ();

		split_ptr = (Link_Splits *) link_splits.Get (&link);

		if (split_ptr->num_splits == 0) {
			split_ptr->num_splits = 1;
			split_ptr->offset [0] = park_ptr->Offset ();

			if (park_ptr->Dir () == 0) {
				split_ptr->park_ab [0] = -1;
			} else {
				split_ptr->park_ba [0] = -1;
			}
		} else {
			off_low = park_ptr->Offset () - OFFSET_RANGE;
			off_high = park_ptr->Offset () + OFFSET_RANGE;

			for (i=0; i < split_ptr->num_splits; i++) {
				if (off_low < split_ptr->offset [i] && off_high > split_ptr->offset [i]) {
					if (park_ptr->Dir () == 0) {
						split_ptr->park_ab [i] = -1;
					} else {
						split_ptr->park_ba [i] = -1;
					}
					break;
				}
			}
			if (i == split_ptr->num_splits && i < MAX_SPLITS) {
				split_ptr->num_splits++;
				split_ptr->offset [i] = park_ptr->Offset ();
				if (park_ptr->Dir () == 0) {
					split_ptr->park_ab [i] = -1;
				} else {
					split_ptr->park_ba [i] = -1;
				}
			}
		}
	}

	//---- sort the split offsets ----

	nsplits = link = 0;

	for (split_ptr = (Link_Splits *) link_splits.First (); split_ptr; split_ptr = (Link_Splits *) link_splits.Next ()) {
		if (split_ptr->num_splits == 0) continue;

		nsplits += split_ptr->num_splits + 1;
		link++;

		if (split_ptr->num_splits > 1) {
			for (i=0; i < split_ptr->num_splits-1; i++) {
				for (j=i+1; j < split_ptr->num_splits; j++) {
					if (split_ptr->offset [i] > split_ptr->offset [j]) {
						off_low = split_ptr->offset [j];
						split_ptr->offset [j] = split_ptr->offset [i];
						split_ptr->offset [i] = off_low;

						off_low = split_ptr->park_ab [j];
						split_ptr->park_ab [j] = split_ptr->park_ab [i];
						split_ptr->park_ab [i] = off_low;

						off_low = split_ptr->park_ba [j];
						split_ptr->park_ba [j] = split_ptr->park_ba [i];
						split_ptr->park_ba [i] = off_low;
					}
				}
			}
		}
	}
	Print (2, "Number of Split Links = %d", link);
	Print (1, "Number of Link Splits = %d", nsplits);

	//---- copy and add data records ----

	New_Node ();
	New_Link ();
	New_Parking ();
	New_Location ();
	New_Access ();
	New_Connect ();
	New_Pocket ();
	New_Sign ();
	New_Signal ();

	//---- reset the pointers to the network network ----

	node_array = (Node_Array *) &new_node_data;
	link_array = (Link_Array *) &new_link_data;
	dir_array = (Dir_Array *) &new_dir_data;
	shape_array = &new_shape_data;
	pocket_array = &new_pocket_data;
	connect_array = &new_connect_data;
	parking_array = &new_parking_data;
	location_array = &new_location_data;
	access_array = &new_access_data;
	signal_array = &new_signal_data;
	sign_array = &new_sign_data;
	timing_array = &new_timing_data;
	phasing_array = &new_phasing_data;
	detector_array = &new_detector_data;
	coordinator_array = &new_coordinator_data;
}
