//*********************************************************
//	New_Parking.cpp - write the new parking records
//*********************************************************

#include "VISSIMNet.hpp"

#include "Parking_File.hpp"
#include "Utility.hpp"

//---------------------------------------------------------
//	New_Parking
//---------------------------------------------------------

void VISSIMNet::New_Parking (void)
{
	int i, link, offset, off_low, off_high;

	Link_Splits *split_ptr;
	Link_Name *link_ptr;
	Parking_Data *parking_ptr, *new_parking_ptr;
	Parking_File *parking_file;

	//---- copy and renumber the parkings ----

	for (parking_ptr = parking_data.First_Key (); parking_ptr; parking_ptr = parking_data.Next_Key ()) {

		new_parking_ptr = new_parking_data.New_Record (true);

		memcpy (new_parking_ptr, parking_ptr, sizeof (Parking_Data));

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

				new_parking_ptr->Offset (offset);	
			} else {
				link = link * MAX_SPLITS;
			}
			new_parking_ptr->Link (link);
		
		} else {

			link = parking_ptr->Link ();

			split_ptr = (Link_Splits *) link_splits.Get (&link);

			off_low = parking_ptr->Offset () - OFFSET_RANGE;
			off_high = parking_ptr->Offset () + OFFSET_RANGE;

			for (i=0; i < split_ptr->num_splits; i++) {
				if (off_low < split_ptr->offset [i] && off_high > split_ptr->offset [i]) {
					if (parking_ptr->Dir () == 0) {
						link = split_ptr->link_ab [i];
					} else {
						link = split_ptr->link_ba [i];
					}
					break;
				}
			}
			new_parking_ptr->Link (link);
			new_parking_ptr->Dir (0);
			new_parking_ptr->Offset (Round (PARKING_OFFSET * 3 / 4));
		}

		if (!new_parking_data.Add ()) {
			Error ("Adding New Parking Data");
		}
	}

	//---- write the new parking file ----

	if (!Network_File_Flag (NEW_PARKING)) return;

	parking_file = (Parking_File *) Network_Db_Base (NEW_PARKING);

	for (parking_ptr = new_parking_data.First (); parking_ptr; parking_ptr = new_parking_data.Next ()) {
		link_ptr = new_link_data.Get (parking_ptr->Link ());

		parking_file->ID (parking_ptr->ID ());
		parking_file->Link (parking_ptr->Link ());

		if (parking_ptr->Dir () == 0) {
			parking_file->Node (link_ptr->Bnode ());
			parking_file->Offset (UnRound (link_ptr->Length () - parking_ptr->Offset ()));
		} else {
			parking_file->Node (link_ptr->Anode ());
			parking_file->Offset (UnRound (parking_ptr->Offset ()));
		}
		parking_file->Type (Parking_Code (BOUNDARY));
		parking_file->Use (Use_Code ((Use_Type) parking_ptr->Use ()));
		parking_file->Space (parking_ptr->Space ());
		parking_file->Start (time_range.Format_Step (parking_ptr->Start ()));
		parking_file->End (time_range.Format_Step (parking_ptr->End ()));
		parking_file->Hourly (parking_ptr->Hourly ());
		parking_file->Daily (parking_ptr->Daily ());

		if (!parking_file->Write ()) {
			Error ("Writing %s", parking_file->File_Type ());
		}
	}
	parking_file->Close ();

	Print (2, "Number of New Parking Records = %d", new_parking_data.Num_Records ());
}
