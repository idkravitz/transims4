//*********************************************************
//	Write_Links.cpp - Create Link Event File
//*********************************************************

#include "EventSum.hpp"

//---------------------------------------------------------
//	Write_Links
//---------------------------------------------------------

void EventSum::Write_Links (void)
{
	Link_Data *link_ptr;
	TTime_Data *ttime_ptr;

	int i, dir;

	Show_Message ("Writing %s -- Record", link_file.File_Type ());
	Set_Progress (1000);

	//---- write each link ----

	for (link_ptr = link_data.First_Key (); link_ptr; link_ptr = link_data.Next_Key ()) {
		Show_Progress ();

		//---- write the link ids ----

		link_file.Link (link_ptr->Link ());
		link_file.Anode (link_ptr->Anode ());
		link_file.Bnode (link_ptr->Bnode ());

		//---- process the A->B direction ----

		dir = link_ptr->AB_Dir ();

		if (dir > 0) {
			ttime_ptr = ttime_data [dir];

			if (ttime_ptr->Periods () > 0) {
				for (i=1; i <= num_inc; i++) {
					link_file.Volume_AB (i, ttime_ptr->Data (i, 1));
				}
			} else {
				for (i=1; i <= num_inc; i++) {
					link_file.Volume_AB (i, 0);
				}
			}
		} else {
			for (i=1; i <= num_inc; i++) {
				link_file.Volume_AB (i, 0);
			}
		}

		//---- process the B->A direction ----

		dir = link_ptr->BA_Dir ();

		if (dir > 0) {
			ttime_ptr = ttime_data [dir];

			if (ttime_ptr->Periods () > 0) {
				for (i=1; i <= num_inc; i++) {
					link_file.Volume_BA (i, ttime_ptr->Data (i, 1));
				}
			} else {
				for (i=1; i <= num_inc; i++) {
					link_file.Volume_BA (i, 0);
				}
			}
		} else {
			for (i=1; i <= num_inc; i++) {
				link_file.Volume_BA (i, 0);
			}
		}
	
		//---- save the record ----

		link_file.Write ();
	}
	End_Progress ();

	Print (2, "Number of %s Records = %d", link_file.File_Type (), Progress_Count ());

	link_file.Close ();
}
