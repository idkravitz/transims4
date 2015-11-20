//*********************************************************
//	Write_Activity.cpp - write the person activity
//*********************************************************

#include "LocationChoice.hpp"

//---------------------------------------------------------
//	Write_Activity
//---------------------------------------------------------

void LocationChoice::Write_Activity (void)
{
	char time [24];
	int previous_time, hhold, person, group;

	Activity_Data *act_ptr;
	Location_Data *location_ptr;

	Activity_File *act_file = (Activity_File *) Demand_Db_Base (NEW_ACTIVITY);

	//---- retrieve each activity ----

	previous_time = hhold = person = 0;

	for (act_ptr = hhold_act_data.First (); act_ptr; act_ptr = hhold_act_data.Next ()) {

		if (act_problem_flag) {
			group = (act_ptr->Mode () << 16) + act_ptr->Purpose ();
		}

		if (hhold != act_ptr->Household () || person != act_ptr->Person ()) {
			if (hhold == act_ptr->Household () && person > act_ptr->Person ()) {
				Activity_Problem (SORT_PROBLEM, act_ptr);
				continue;
			}
			hhold = act_ptr->Household ();
			person = act_ptr->Person ();
			previous_time = 0;
		}
		act_file->Household (act_ptr->Household ());
		act_file->Person (act_ptr->Person ());
		act_file->Activity (act_ptr->Activity ());
		act_file->Purpose (act_ptr->Purpose ());
		act_file->Priority (act_ptr->Priority ());

		activity_time.Format_Step (act_ptr->Start_Time (), time);
		if (previous_time > 0 && act_ptr->Start_Time () < previous_time) {
			Activity_Problem (TIME_PROBLEM, act_ptr);
			if (act_problem_flag) act_problem_data.Add_Problem (group, 1, 1);
			continue;
		}
		act_file->Start_Min (time);
		act_file->Start_Max (time);

		activity_time.Format_Step (act_ptr->End_Time (), time);
		previous_time = act_ptr->End_Time ();

		act_file->End_Min (time);
		act_file->End_Max (time);

		activity_time.Format_Duration (act_ptr->Duration (), time);

		act_file->Time_Min (time);
		act_file->Time_Max (time);

		act_file->Mode (act_ptr->Mode ());
		act_file->Passengers (act_ptr->Passengers ());
		act_file->Constraint (act_ptr->Constraint ());

		if (act_ptr->Vehicle () < 0) {
			Activity_Problem (ACCESS_PROBLEM, act_ptr);
			if (act_problem_flag) act_problem_data.Add_Problem (group, 1, 0, 1);
			continue;
		} else {
			act_file->Vehicle (act_ptr->Vehicle ());
		}

		if (act_ptr->Location () > 0) {
			location_ptr = location_data [act_ptr->Location ()];

			if (location_ptr != NULL) {
				act_file->Location (location_ptr->ID ());
			} else {
				act_file->Location (0);
			}
		} else {
			Activity_Problem (LOCATION_PROBLEM, act_ptr);
			if (act_problem_flag) act_problem_data.Add_Problem (group, 1, 0, 0, 1);
			continue;
		}
		if (!act_file->Write ()) {
			Error ("Writing Activity File");
		}
		if (act_problem_flag) act_problem_data.Add_Problem (group);
		num_activity++;
	}
}
