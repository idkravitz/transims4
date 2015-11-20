//*********************************************************
//	Factor_Process.cpp - Trip Adjustment Factor Processing
//*********************************************************

#include "ConvertTrips.hpp"

//---------------------------------------------------------
//	Factor_Processing
//---------------------------------------------------------

void ConvertTrips::Factor_Processing (File_Group *group)
{
	int dist, org, des, period;

	Factor_File *file;
	Factor_Data *factor_ptr;

	//---- read the trip adjustment factors into memory ----

	file = group->Trip_Factor ();
	factor_data.Reset ();

	if (!factor_data.Set_Key (group->Factor_Periods ())) {
		Error ("Factor Time Periods %d are Out of Range", group->Factor_Periods ());
	}
	Show_Message ("Reading %s -- Record", file->File_Type ());
	Set_Progress ();

	while (file->Read ()) {
		Show_Progress ();

		org = dist = file->Origin ();
		if (org == 0) continue;

		if (equiv_flag) {
			if (zone_equiv.Get (&dist) == NULL) goto dist_error;
		}
		des = dist = file->Destination ();

		if (equiv_flag) {
			if (zone_equiv.Get (&dist) == NULL) goto dist_error;
		}
		period = file->Period ();
		if (period == 0) period = 1;

		factor_ptr = factor_data.New_Record ();

		factor_ptr->Factor (file->Factor ());

		if (factor_ptr->Factor () != 1.0) {
			factor_ptr->ID (factor_data.Key (org, des, period));
			factor_ptr->Bucket (0.45);

			if (!factor_data.Add ()) {
				Error ("Adding Trip Adjustment Factor");
			}
		}
	}
	End_Progress ();

	file->Close ();
	return;

dist_error:
	Error ("District %d is Not in the Zone Equivalence", dist);
}
