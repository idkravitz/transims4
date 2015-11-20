//*********************************************************
//	Write_Zones.cpp - write zone trip end file
//*********************************************************

#include "TripSum.hpp"

//---------------------------------------------------------
//	Write_Zones
//---------------------------------------------------------

void TripSum::Write_Zones (void)
{
	int i, nrec;

	Point_Time_Data *end_ptr;

	Show_Message ("Writing %s -- Record", zone_end_file.File_Type ());
	Set_Progress (1000);

	//---- write each zone ----

	nrec = 0;

	for (end_ptr = zone_end_data.First_Key (); end_ptr; end_ptr = zone_end_data.Next_Key ()) {
		Show_Progress ();

		if (end_ptr->Periods () > 0) {
			zone_end_file.ID (end_ptr->ID ());

			for (i=1; i <= num_inc; i++) {
				zone_end_file.Total_In (i, end_ptr->In (i));
				zone_end_file.Total_Out (i, end_ptr->Out (i));
			}
			if (!zone_end_file.Write ()) {
				Error ("Writing %s", zone_end_file.File_Type ());
			}
			nrec++;
		}
	}
	End_Progress ();

	Print (2, "Number of %s Records = %d", zone_end_file.File_Type (), nrec);

	zone_end_file.Close ();
}
