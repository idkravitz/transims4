//*********************************************************
//	Write_Volume.cpp - Write the Output Volume File
//*********************************************************

#include "Validate.hpp"

//---------------------------------------------------------
//	Write_Volume
//---------------------------------------------------------

void Validate::Write_Volume (void)
{
	int i, num, volume, nvol;
	bool save;

	TTime_Data *ttime_ptr;
	Link_Data *link_ptr;
	Range_Data *range_ptr;

	nvol = 0;

	Show_Message ("Writing Output Volume File -- Record");
	Set_Progress ();

	//---- add range data and save the header ----

	for (range_ptr = time_periods.First (); range_ptr; range_ptr = time_periods.Next ()) {
		if (!output_file.Add (range_ptr)) {
			Error ("Adding Range to Output Volume File");
		}
	}
	if (!output_file.Create_Fields ()) {
		Error ("Creating Output Volume Fields");
	}
	if (!output_file.Write_Header ()) {
		Error ("Creating Output Volume Header");
	}

	//---- process each link in sorted order ----

    for (link_ptr = link_data.First_Key (); link_ptr != NULL; link_ptr = link_data.Next_Key ()) {
		Show_Progress ();

		save = false;
		for (i=1; i <= output_file.Num_Fields (); i++) {
			output_file.Put_Field (i, 0);
		}

		//---- process the AB direction ----

		ttime_ptr = ttime_data [link_ptr->AB_Dir ()];

		if (ttime_ptr != NULL) {
			num = ttime_ptr->Periods ();

			for (i=1; i <= num; i++) {
				volume = ttime_ptr->Volume (i);

				if (volume > 0) {
					output_file.Volume_AB (i, volume);
					save = true;
				}
			}
		}

		//---- process the BA direction ----

		ttime_ptr = ttime_data [link_ptr->BA_Dir ()];

		if (ttime_ptr != NULL) {
			num = ttime_ptr->Periods ();

			for (i=1; i <= num; i++) {
				volume = ttime_ptr->Volume (i);
				
				if (volume > 0) {
					output_file.Volume_BA (i, volume);
					save = true;
				}
			}
		}

		//---- save non-zero records ----

		if (save) {
			nvol++;

			output_file.Link (link_ptr->Link ());
			output_file.Anode (link_ptr->Anode ());
			output_file.Bnode (link_ptr->Bnode ());

			if (!output_file.Write ()) {
				Error ("Writing the Output Volume File");
			}
		}
	}
	End_Progress ();

	Print (2, "Number of Output Volume Records = %d", nvol);

	output_file.Close ();
}
