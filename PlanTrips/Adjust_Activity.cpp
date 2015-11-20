//*********************************************************
//	Adjust_Activity.cpp - adjust the activity travel schedule
//*********************************************************

#include "PlanTrips.hpp"

//---------------------------------------------------------
//	Adjust_Activity
//---------------------------------------------------------

void PlanTrips::Adjust_Activity (int traveler)
{
	int i, j, trip, start_time, end_time, shift, ntravel, *offset, type;

	Activity_Data *act_ptr;
	Plan_Data **ptr, *plan_ptr;
	Travel_Activity *travel_ptr;

	//---- collect travel activities ----

	ntravel = trip = 0;

	memset (travel, '\0', sizeof (travel));
	travel_ptr = travel;

	for (ptr = (Plan_Data **) plan_list.First (); ptr; ptr = (Plan_Data **) plan_list.Next ()) {
		plan_ptr = *ptr;

		if (plan_ptr->mode == ACTIVITY_MODE) {
			if (++ntravel == MAX_TRAVEL) break;

			travel_ptr->rec_id = ntravel;
			travel_ptr->duration = plan_ptr->duration;
			travel_ptr->index2 = plan_list.Record_Index ();

			if (ntravel == 1) {
				travel_ptr->index1 = 1;
				travel_ptr->start = 0;
				travel_ptr->end = 0;
			}
			travel_ptr++;

			travel_ptr->index1 = plan_list.Record_Index () + 1;
			travel_ptr->start = plan_ptr->stop_time;
			travel_ptr->end = plan_ptr->stop_time;
		} else {
			if (trip != plan_ptr->trip) {
				trip = plan_ptr->trip;
				travel_ptr->index1 = plan_list.Record_Index ();
				travel_ptr->start = plan_ptr->key2;
			}
			travel_ptr->index2 = plan_list.Record_Index ();
			travel_ptr->end = plan_ptr->stop_time;
		}
	}

	//---- assign activity file constraints ----

	for (act_ptr = activity_data.Get_GE (traveler, 0); act_ptr; act_ptr = activity_data.Next_Key ()) {
		if (act_ptr->Traveler () != traveler) break;
		trip = act_ptr->Activity ();

		if (act_ptr->Mode () != ACTIVITY_MODE) {
			for (i=0; i < ntravel; i++) {
				travel_ptr = travel + i;

				if (travel_ptr->rec_id == trip) {
					travel_ptr->type = type = act_ptr->Constraint ();
					if (type == END_TIME) {
						travel_ptr->constraint = act_ptr->End_Time ();
					} else if (type == DURATION) {
						travel_ptr->constraint = act_ptr->End_Time () - act_ptr->Start_Time ();
					} else {
						travel_ptr->constraint = act_ptr->Start_Time ();
					}
					break;
				}
			}
		}
	}

	//---- reconcile the constraints with the travel activities ----

	for (i=0; i < ntravel; i++) {
		travel_ptr = travel + i;

		if (travel_ptr->type == START_TIME || travel_ptr->type == FIXED_TIME) {
			shift = travel_ptr->constraint - travel_ptr->end;
		} else if (travel_ptr->type == END_TIME) {
			shift = travel_ptr->constraint - (travel_ptr->end + travel_ptr->duration);
		} else {
			shift = 0;
		}

		//---- apply the shift ----

		if (shift != 0) {
			start_time = travel_ptr->end + travel_ptr->duration + shift;
			end_time = shift;

			//---- cascade forward ----

			for (j=i+1; j < ntravel; j++) {
				travel_ptr = travel + j;

				if (travel_ptr->type > 0) break;

				travel_ptr->shift = shift;
				travel_ptr->start += shift;
				travel_ptr->end += shift;
				start_time = travel_ptr->end + travel_ptr->duration;
			}

			//---- cascade back ----

			shift = end_time;

			for (j=i; j >= 0; j--) {
				travel_ptr = travel + j;

				if (i != j && travel_ptr->type > 0) break;

				travel_ptr->shift = shift;
				start_time = travel_ptr->start += shift;
				travel_ptr->end += shift;
			}
		}
	}

	//---- apply the time adjustments to the activity data ----

	for (act_ptr = activity_data.Get_GE (traveler, 0); act_ptr; act_ptr = activity_data.Next_Key ()) {
		if (act_ptr->Traveler () != traveler) break;
		trip = act_ptr->Activity ();

		for (i=0; i < ntravel; i++) {
			travel_ptr = travel + i;

			if (travel_ptr->rec_id == trip) {

				start_time = travel_ptr->end;
				end_time = travel_ptr->end + travel_ptr->duration;

				if (start_time < 0) start_time = 0;
				if (i == 0 && start_time > 0) start_time = 0;

				if (travel_ptr->type == END_TIME) {
					shift = start_time - act_ptr->Start_Time ();
					offset = start_offset;
				} else if (travel_ptr->type == DURATION) {
					shift = start_time - act_ptr->Start_Time ();
					offset = mid_offset;
				} else {
					shift = end_time - act_ptr->End_Time ();
					offset = end_offset;
				}
				act_ptr->Start_Time (start_time);
				act_ptr->End_Time (end_time);
				act_ptr->Duration (end_time - start_time);

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
				offset [shift]++;
				break;
			}
		}
	}

	//---- update the plan records ----

	for (i=0; i < ntravel; i++) {
		travel_ptr = travel + i;

		for (j=travel_ptr->index1; j <= travel_ptr->index2; j++) {
			if (travel_ptr->shift != 0) {
				plan_ptr = *((Plan_Data **) plan_list [j]);

				plan_ptr->key2 += travel_ptr->shift;
				if (plan_ptr->key2 < 0) plan_ptr->key2 = 0;
				if (i == 0 && j == travel_ptr->index1 && plan_ptr->key2 > 0) plan_ptr->key2 = 0;

				plan_ptr->stop_time += travel_ptr->shift;
				plan_ptr->duration = plan_ptr->stop_time - plan_ptr->key2;
				tot_adjust += abs (travel_ptr->shift);
				num_adjust++;
			}
		}
	}
}
