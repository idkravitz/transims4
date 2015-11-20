//*********************************************************
//	New_Location.cpp - write the new activity location records
//*********************************************************

#include "VISSIMNet.hpp"

#include "Location_File.hpp"
#include "Utility.hpp"

//---------------------------------------------------------
//	New_Location
//---------------------------------------------------------

void VISSIMNet::New_Location (void)
{
	int i, link, offset, off_low, off_high;

	Link_Splits *split_ptr;
	Link_Name *link_ptr;
	Location_Data *location_ptr, *new_location_ptr;
	Access_Data *access_ptr;
	Parking_Data *parking_ptr;
	Location_File *location_file;

	//---- copy and renumber the locations ----

	for (location_ptr = location_data.First_Key (); location_ptr; location_ptr = location_data.Next_Key ()) {

		//---- find a parking lot ----

		parking_ptr = NULL;

		for (access_ptr = access_data.First (); access_ptr; access_ptr = access_data.Next ()) {
			if (access_ptr->From_Type () == LOCATION_ID && access_ptr->From_ID () == location_ptr->Location ()) {
				if (access_ptr->To_Type () == PARKING_ID) {
					parking_ptr = parking_data.Get (access_ptr->To_ID ());
					break;
				}
			}
		}
		if (parking_ptr == NULL) continue;

		//---- save the activity location ----

		new_location_ptr = new_location_data.New_Record (true);

		memcpy (new_location_ptr, location_ptr, sizeof (Location_Data));

		if (parking_ptr->Type () == BOUNDARY) {
			link = parking_ptr->Link ();

			link_ptr = link_data.Get (link);

			if (parking_ptr->Offset () > link_ptr->Length () / 2) {
				split_ptr = (Link_Splits *) link_splits.Get (&link);

				link = link * MAX_SPLITS + split_ptr->num_splits;

				offset = link_ptr->Length () - parking_ptr->Offset ();

				link_ptr = new_link_data.Get (link);

				offset = link_ptr->Length () - offset;
				if (offset <= 0) offset = 1;
			} else {
				link = link * MAX_SPLITS;
				offset = parking_ptr->Offset ();
			}
			new_location_ptr->Link (link);
			new_location_ptr->Dir (parking_ptr->Dir ());
			new_location_ptr->Offset (offset);

		} else {

			link = parking_ptr->Link ();

			split_ptr = (Link_Splits *) link_splits.Get (&link);

			off_low = parking_ptr->Offset () - OFFSET_RANGE;
			off_high = parking_ptr->Offset () + OFFSET_RANGE;

			for (i=0; i < split_ptr->num_splits; i++) {
				if (off_low < split_ptr->offset [i] && off_high > split_ptr->offset [i]) {
					if (location_ptr->Dir () == 0) {
						link = split_ptr->link_ab [i];
					} else {
						link = split_ptr->link_ba [i];
					}
					break;
				}
			}
			new_location_ptr->Link (link);
			new_location_ptr->Dir (0);
			new_location_ptr->Offset (Round (PARKING_OFFSET * 3 / 4));
		}

		if (!new_location_data.Add ()) {
			Error ("Adding New Activity Location Data");
		}
	}

	//---- write the new location file ----

	if (!Network_File_Flag (NEW_ACTIVITY_LOCATION)) return;

	location_file = (Location_File *) Network_Db_Base (NEW_ACTIVITY_LOCATION);

	for (location_ptr = new_location_data.First (); location_ptr; location_ptr = new_location_data.Next ()) {
		link_ptr = new_link_data.Get (location_ptr->Link ());

		location_file->Location (location_ptr->Location ());
		location_file->Link (location_ptr->Link ());

		if (location_ptr->Dir () == 0) {
			location_file->Node (link_ptr->Bnode ());
			location_file->Offset (UnRound (link_ptr->Length () - location_ptr->Offset ()));
		} else {
			location_file->Node (link_ptr->Anode ());
			location_file->Offset (UnRound (location_ptr->Offset ()));
		}
		location_file->X (UnRound (location_ptr->X ()));
		location_file->Y (UnRound (location_ptr->Y ()));
		location_file->Zone (location_ptr->Zone ());

		if (!location_file->Write ()) {
			Error ("Writing %s", location_file->File_Type ());
		}
	}
	location_file->Close ();

	Print (2, "Number of New Activity Location Records = %d", new_location_data.Num_Records ());
}
