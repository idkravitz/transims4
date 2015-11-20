//*********************************************************
//	Write_Pattern.cpp - write the person activity patterns
//*********************************************************

#include "ActivityPattern.hpp"

//---------------------------------------------------------
//	Write_Pattern
//---------------------------------------------------------

void ActivityPattern::Write_Pattern (void)
{
	char time [24];
	int previous_time, hhold, person;

	Activity_Data *act_ptr;
	Location_Data *location_ptr;
	Vehicle_Data *vehicle_ptr;

	//---- retrieve each activity ----

	previous_time = hhold = person = 0;

	for (act_ptr = hhold_act_data.First (); act_ptr; act_ptr = hhold_act_data.Next ()) {
		if (hhold != act_ptr->Household () || person != act_ptr->Person ()) {
			if (hhold == act_ptr->Household () && person > act_ptr->Person ()) {
				Activity_Problem (SORT_PROBLEM, act_ptr);
				continue;
			}
			hhold = act_ptr->Household ();
			person = act_ptr->Person ();
			previous_time = 0;
		}
		new_pattern_file.Household (act_ptr->Household ());
		new_pattern_file.Person (act_ptr->Person ());
		new_pattern_file.Activity (act_ptr->Activity ());
		new_pattern_file.Purpose (act_ptr->Purpose ());
		new_pattern_file.Priority (act_ptr->Priority ());

		activity_time.Format_Step (act_ptr->Start_Time (), time);
		if (previous_time > 0 && act_ptr->Start_Time () < previous_time) {
			Activity_Problem (TIME_PROBLEM, act_ptr);
			continue;
		}
		new_pattern_file.Start_Min (time);
		new_pattern_file.Start_Max (time);

		activity_time.Format_Step (act_ptr->End_Time (), time);
		previous_time = act_ptr->End_Time ();

		new_pattern_file.End_Min (time);
		new_pattern_file.End_Max (time);

		activity_time.Format_Duration (act_ptr->Duration (), time);

		new_pattern_file.Time_Min (time);
		new_pattern_file.Time_Max (time);

		new_pattern_file.Mode (act_ptr->Mode ());
		new_pattern_file.Passengers (act_ptr->Passengers ());
		new_pattern_file.Constraint (act_ptr->Constraint ());

		if (act_ptr->Vehicle () > 0) {
			vehicle_ptr = vehicle_data [act_ptr->Vehicle ()];
			if (vehicle_ptr != NULL) {
				new_pattern_file.Vehicle (vehicle_ptr->ID ());
			}
		} else if (act_ptr->Vehicle () < 0) {
			if (act_ptr->Vehicle () == -1) {
				Activity_Problem (ACCESS_PROBLEM, act_ptr);
				continue;
			} else {
				Activity_Problem (PASSENGER_PROBLEM, act_ptr);

				new_pattern_file.Vehicle (0);
				new_pattern_file.Mode (MAGIC_MOVE);
			}
		} else {
			new_pattern_file.Vehicle (act_ptr->Vehicle ());
		}

		if (act_ptr->Location () > 0) {
			location_ptr = location_data [act_ptr->Location ()];

			if (location_ptr != NULL) {
				new_pattern_file.Location (location_ptr->ID ());
			} else {
				new_pattern_file.Location (0);
			}
		} else {
			new_pattern_file.Location (act_ptr->Location ());
		}
		if (!new_pattern_file.Write ()) {
			Error ("Writing Activity File");
		}
		num_activity++;
	}
}
