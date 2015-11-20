//*********************************************************
//	Write_Locations.cpp - write location trip end file
//*********************************************************

#include "TripSum.hpp"

//---------------------------------------------------------
//	Write_Locations
//---------------------------------------------------------

void TripSum::Write_Locations (void)
{
	int i, nrec;

	Point_Time_Data *end_ptr;

	Show_Message ("Writing %s -- Record", loc_end_file.File_Type ());
	Set_Progress (1000);

	//---- write each location ----

	nrec = 0;

	for (end_ptr = loc_end_data.First_Key (); end_ptr; end_ptr = loc_end_data.Next_Key ()) {
		Show_Progress ();

		if (end_ptr->Periods () > 0) {
			loc_end_file.ID (end_ptr->ID ());

			for (i=1; i <= num_inc; i++) {
				loc_end_file.Total_In (i, end_ptr->In (i));
				loc_end_file.Total_Out (i, end_ptr->Out (i));
			}
			if (!loc_end_file.Write ()) {
				Error ("Writing %s", loc_end_file.File_Type ());
			}
			nrec++;
		}
	}
	End_Progress ();

	Print (2, "Number of %s Records = %d", loc_end_file.File_Type (), nrec);

	loc_end_file.Close ();
}
