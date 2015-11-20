//*********************************************************
//	Process_Plans.cpp - read process the traveler plans
//*********************************************************

#include "PlanTrips.hpp"

//---------------------------------------------------------
//	Process_Plans
//---------------------------------------------------------

void PlanTrips::Process_Plans (void)
{
	int traveler, trip, last_trip, start_time, end_time, act_time, shift, hhold;
	bool prev_flag, shift_flag, skip_flag;

	Plan_Data **ptr, *plan_ptr;

	//---- adjust the leg times ----

	last_trip = act_time = 0;
	prev_flag = skip_flag = false;

	for (ptr = (Plan_Data **) plan_list.First (); ptr; ptr = (Plan_Data **) plan_list.Next ()) {
		plan_ptr = *ptr;

		traveler = plan_ptr->key1;
		trip = plan_ptr->trip;
		start_time = plan_ptr->key2;
		end_time = plan_ptr->stop_time;
		shift_flag = false;

		if (last_trip != trip) {
			if (last_trip > 0) {
				if (prev_flag || plan_ptr->mode == ACTIVITY_MODE) {
					shift_flag = (start_time != act_time);
				} else {
					act_time += 60;		//---- minimum time between trips (60 seconds) ----
					shift_flag = (start_time < act_time);
				}
			} else if (hhlist_flag) {
				hhold = traveler / Traveler_Scale ();
				skip_flag = (hhlist_data.Get_Index (hhold) == 0);
			}
			last_trip = trip;
		} else {
			shift_flag = (start_time != act_time);
 		}
		if (!skip_flag) {
			if (shift_flag) {
				shift = act_time - start_time;
				plan_ptr->stop_time = end_time += shift;
				plan_ptr->key2 = start_time = act_time;
				tot_adjust += abs (shift);
				num_adjust++;
			} else {
				shift = 0;
			}
			if (!trip_flag && !act_flag) {

				//---- report summary ----

				if (shift == 0) {
					shift = 50;
				} else {
					shift = (shift + 15000) / 300;
					if (shift < 0) {
						shift = 0;
					} else if (shift >= 50) {
						shift++;
						if (shift > 100) shift = 100;
					}
				}
				start_offset [shift]++;
				mid_offset [shift]++;
				end_offset [shift]++;
			}
			if (create_flag) {
			}
		}
		act_time = end_time;
		prev_flag = (plan_ptr->mode == ACTIVITY_MODE);
	}

	//---- process the adjusted results ----

	if (trip_flag || act_flag) {

		//---- locate the traveler ----

		Trip_Data *trip_ptr = NULL;
		Activity_Data *act_ptr = NULL;

		if (trip_flag) {
			trip_ptr = trip_data.Get_GE (traveler, 0);

			if (trip_ptr != NULL && trip_ptr->Traveler () != traveler) {
				trip_ptr = NULL;
			}
		}
		if (act_flag && trip_ptr == NULL) {
			act_ptr = activity_data.Get_GE (traveler, 0);

			if (act_ptr != NULL && act_ptr->Traveler () != traveler) {
				act_ptr = NULL;
			}
		}

		//---- insert a new traveler ----

		if (trip_ptr == NULL && act_ptr == NULL) {
			if (trip_flag) {
				Add_Trips ();
			} else {
				Add_Activity ();
			}
		} else if (trip_ptr != NULL) {
			Adjust_Trips (traveler);
		} else {
			Adjust_Activity (traveler);
		}
	} else if (new_trip_flag) {
		New_Trips ();
	} else if (new_act_flag) {
		New_Activity ();
	}

	//---- save the plan record ----

	if (plan_flag) {
		for (ptr = (Plan_Data **) plan_list.First (); ptr; ptr = (Plan_Data **) plan_list.Next ()) {
			plan_ptr = *ptr;

			new_plan_file.Plan (plan_ptr);

			if (!create_flag || plan_ptr->mode != ACTIVITY_MODE) {
				if (!new_plan_file.Write ()) {
					Error ("Writing New Plan File");
				}
			}
			free (*ptr);
		}
		plan_list.Reset ();
	} else {
		for (ptr = (Plan_Data **) plan_list.First (); ptr; ptr = (Plan_Data **) plan_list.Next ()) {
			free (*ptr);
		}
		plan_list.Reset ();
	}
}
