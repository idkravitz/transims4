//*********************************************************
//	Read_Plans.cpp - read each plan file
//*********************************************************

#include "PlanTrips.hpp"

//---------------------------------------------------------
//	Read_Plans
//---------------------------------------------------------

void PlanTrips::Read_Plans (void)
{
	int traveler, last_traveler, hhold, last_hhold;
	char buffer [FIELD_BUFFER];

	Plan_Data *plan_data;

	if (plan_file.Extend ()) {
		Show_Message ("Reading Plan File %s -- Plan", plan_file.Extension ());
	} else {
		Show_Message ("Reading Plan File -- Plan");
	}
	Set_Progress (10000);

	last_traveler = last_hhold = 0;

	//---- read each plan record ----

	while (plan_file.Read ()) {
		Show_Progress ();

		traveler = plan_file.Traveler ();

		if (traveler != last_traveler) {
			if (last_traveler > 0) {
				Process_Plans ();

				if (hhold_flag && !create_flag) {
					hhold = last_traveler / Traveler_Scale ();

					if (hhold != last_hhold) {
						str_fmt (buffer, sizeof (buffer), "%d", hhold);

						if (!hhold_file.Write (buffer)) {
							Error ("Writing %s", hhold_file.File_Type ());
						}
						last_hhold = hhold;
					}
				}
			}
			last_traveler = traveler;
		}
		plan_data = plan_file.Plan ();

		if (!plan_list.Add (&plan_data)) {
			Error ("Adding Plan Record");
		}
	}
	End_Progress ();

	if (last_traveler > 0) {
		Process_Plans ();

		if (hhold_flag && !create_flag) {
			hhold = last_traveler / Traveler_Scale ();

			if (hhold != last_hhold) {
				str_fmt (buffer, sizeof (buffer), "%d", hhold);

				if (!hhold_file.Write (buffer)) {
					Error ("Writing %s", hhold_file.File_Type ());
				}
			}
		}
	}
	plan_file.Close ();

	if (plan_flag) {
		new_plan_file.Close ();
	}
	if (hhold_flag) {
		hhold_file.Close ();
	}
}
