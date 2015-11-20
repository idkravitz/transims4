//*********************************************************
//	Read_Activity.cpp - process household activities
//*********************************************************

#include "LocationChoice.hpp"

//---------------------------------------------------------
//	Read_Activity
//---------------------------------------------------------

void LocationChoice::Read_Activity (void)
{
	int last_hh, lower, upper, location;
	bool stat, keep;

	Activity_Data act_rec;

	Show_Message ("Reading Activity -- Household");
	if (hhlist_flag) {
		Set_Progress (100);
	} else {
		Set_Progress (10);
	}

	last_hh = 0;
	keep = true;

	for (;;) {
		stat = pattern_file.Read ();

		if (!stat || pattern_file.Household () != last_hh) {

			//---- process the household activities ----

			if (keep && last_hh > 0) {

				if (dump_list_flag) {
					dump_flag = (dump_list.Get_Index (last_hh) > 0);
				}

				//---- locate activities ----

				Locate_Activity ();

				//---- estimate travel times ----

				Travel_Time ();

				//---- set activities times ----

				Schedule_Activity ();

				//---- write the activities ----

				Write_Activity ();
			}
			if (!stat) break;

			//---- initialize the next household ----

			Show_Progress ();

			last_hh = pattern_file.Household ();

			if (hhlist_flag) {
				keep = (hhold_list.Get_Index (last_hh) > 0);
			}
			if (keep) {
				hhold_act_data.Reset ();

				act_rec.Household (last_hh);
			}
		}
		num_act_gen++;

		if (!keep) continue;

		//---- add the activity to the array ----

		act_rec.Person (pattern_file.Person ());
		act_rec.Activity (pattern_file.Activity ());
		act_rec.Purpose (pattern_file.Purpose ());
		act_rec.Priority (pattern_file.Priority ());
		act_rec.Mode (pattern_file.Mode ());
		act_rec.Vehicle (pattern_file.Vehicle ());
		act_rec.Passengers (pattern_file.Passengers ());
		act_rec.Constraint (pattern_file.Constraint ());
		act_rec.Problem (0);

		//---- convert the activity location ID ----

		location = pattern_file.Location ();

		if (location <= 0) {
			act_rec.Location (location);
		} else {
			act_rec.Location (location_data.Get_Index (location));

			if (act_rec.Location () == 0) {
				Error ("Activity Location %s was Not Found", location);
			}
			act_rec.Location (location);
		}

		//---- convert the time fields ----

		lower = activity_time.Step (pattern_file.Start_Min ());
		upper = activity_time.Step (pattern_file.Start_Max ());

		act_rec.Start_Time ((lower + upper) / 2);

		lower = activity_time.Step (pattern_file.End_Min ());
		upper = activity_time.Step (pattern_file.End_Max ());

		act_rec.End_Time ((lower + upper + 1) / 2);

		act_rec.Duration (act_rec.End_Time () - act_rec.Start_Time ());

		if (!hhold_act_data.Add (&act_rec)) {
			Error ("Saving Activity Data");
		}
	}
	End_Progress ();

	if (warning_count > 1) {
		Write (1, "Number of Exponetial Warning Messages = %d", warning_count);
	}
	if (num_problem > 0 || num_act_gen != num_activity) {
		Print (2, "Number of Activities Read= %d", num_act_gen);

		if (num_problem > 0) {
			Print (1, "Number of Activities with Problems = %d (%.1lf%%)", num_problem,
				(100.0 * num_problem / num_act_gen));
		}
		Print (1, "Number of Activities Written = %d (%.1lf%%)", num_activity,
			(100.0 * num_activity / num_act_gen));
	} else {
		Print (2, "Number of Activities = %d", num_activity);
	}
}

