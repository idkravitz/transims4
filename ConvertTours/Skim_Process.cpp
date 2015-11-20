//*********************************************************
//	Skim_Process.cpp - read the travel time skim
//*********************************************************

#include "ConvertTours.hpp"

//---------------------------------------------------------
//	Skim_Processing
//---------------------------------------------------------

void ConvertTours::Skim_Processing (void)
{
	int org, des, period, ttime;
	Matrix_Data skim_rec;

	//---- read the travel time skim into memory ----

	Show_Message ("Reading %s -- Record", skim_file.File_Type ());
	Set_Progress (100000);

	while (skim_file.Read ()) {
		Show_Progress ();

		org = skim_file.Origin ();
		if (org == 0) continue;

		des = skim_file.Destination ();

		period = skim_file.Period ();
		if (period == 0) period = 1;

		ttime = skim_file.Data ();

		if (ttime == 0) {
			ttime = (int) (skim_file.Time () + 0.5);

			if (ttime == 0) {
				ttime = skim_file.Drive ();
			}
		}
		skim_rec.ID (ttime_skim.Key (org, des, period));
		skim_rec.Data (ttime);

		if (!ttime_skim.Add (&skim_rec)) {
			Error ("Adding Travel Time Data");
		}
	}
	End_Progress ();

	skim_file.Close ();
}
