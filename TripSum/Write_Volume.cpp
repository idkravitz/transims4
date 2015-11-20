//*********************************************************
//	Write_Volume.cpp - create link trip end file
//*********************************************************

#include "TripSum.hpp"

//---------------------------------------------------------
//	Write_Volume
//---------------------------------------------------------

void TripSum::Write_Volume (void)
{
	Link_Data *link_ptr;
	TTime_Data *ttime_ptr;

	int i, dir, volume, nvol;
	bool vol_flag;

	Show_Message ("Writing %s -- Record", volume_file.File_Type ());
	Set_Progress (1000);

	//---- write each link ----

	nvol = 0;

	for (link_ptr = link_data.First_Key (); link_ptr; link_ptr = link_data.Next_Key ()) {
		Show_Progress ();

		//---- write the link ids ----

		volume_file.Link (link_ptr->Link ());
		volume_file.Anode (link_ptr->Anode ());
		volume_file.Bnode (link_ptr->Bnode ());

		vol_flag = false;

		//---- process the A->B direction ----

		dir = link_ptr->AB_Dir ();

		if (dir > 0) {
			ttime_ptr = ttime_data [dir];

			if (ttime_ptr->Periods () > 0) {
				for (i=1; i <= num_inc; i++) {
					volume = ttime_ptr->Volume (i);
					if (volume > 0) vol_flag = true;
					volume_file.Volume_AB (i, volume);
				}
			} else {
				for (i=1; i <= num_inc; i++) {
					volume_file.Volume_AB (i, 0);
				}
			}
		} else {
			for (i=1; i <= num_inc; i++) {
				volume_file.Volume_AB (i, 0);
			}
		}

		//---- process the B->A direction ----

		dir = link_ptr->BA_Dir ();

		if (dir > 0) {
			ttime_ptr = ttime_data [dir];

			if (ttime_ptr->Periods () > 0) {
				for (i=1; i <= num_inc; i++) {
					volume = ttime_ptr->Volume (i);
					if (volume > 0) vol_flag = true;
					volume_file.Volume_BA (i, volume);
				}
			} else {
				for (i=1; i <= num_inc; i++) {
					volume_file.Volume_BA (i, 0);
				}
			}
		} else {
			for (i=1; i <= num_inc; i++) {
				volume_file.Volume_BA (i, 0);
			}
		}
	
		//---- save the record ----

		if (vol_flag) {
			volume_file.Write ();
			nvol++;
		}
	}
	End_Progress ();

	Print (2, "Number of %s Records = %d", volume_file.File_Type (), nvol);

	volume_file.Close ();
}
