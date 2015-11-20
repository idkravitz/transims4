//*********************************************************
//	Read_Factor.cpp - select probability file
//*********************************************************

#include "TripPrep.hpp"

//---------------------------------------------------------
//	Read_Factor
//---------------------------------------------------------

void TripPrep::Read_Factor (void)
{
	int org, des, period;

	Factor_Data *factor_ptr;

	//---- read the trip adjustment factors into memory ----

	if (!factor_data.Set_Key (time_equiv.Num_Periods ())) {
		Error ("Factor Time Periods %d are Out of Range", time_equiv.Num_Periods ());
	}

	Show_Message ("Reading %s -- Record", factor_file.File_Type ());
	Set_Progress ();

	while (factor_file.Read ()) {
		Show_Progress ();

		org = factor_file.Origin ();
		if (org == 0) continue;

		des = factor_file.Destination ();

		period = factor_file.Period ();
		if (period == 0) period = 1;

		factor_ptr = factor_data.New_Record ();

		factor_ptr->Factor (factor_file.Factor ());

		if (factor_ptr->Factor () < 1.0) {
			factor_ptr->ID (factor_data.Key (org, des, period));
			factor_ptr->Bucket (random.Probability ());

			if (!factor_data.Add ()) {
				Error ("Adding Trip Adjustment Factor");
			}
		}
	}
	End_Progress ();

	factor_file.Close ();
	return;
}
