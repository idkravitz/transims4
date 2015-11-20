//*********************************************************
//	Adjust_Trips.cpp - adjust the trip travel schedule
//*********************************************************

#include "PlanTrips.hpp"

//---------------------------------------------------------
//	Adjust_Trips
//---------------------------------------------------------

void PlanTrips::Adjust_Trips (int traveler)
{
	int i, j, trip, start_time, end_time, shift, ntravel, *offset, type;
	bool shift_flag;

	Trip_Data *trip_ptr;
	Plan_Data **ptr, *plan_ptr;
	Travel_Activity *travel_ptr;

	//---- collect travel activities ----

	ntravel = trip = 0;

	memset (travel, '\0', sizeof (travel));
	travel_ptr = travel;

	for (ptr = (Plan_Data **) plan_list.First (); ptr; ptr = (Plan_Data **) plan_list.Next ()) {
		plan_ptr = *ptr;

		if (plan_ptr->mode == ACTIVITY_MODE) {
			if (ntravel == 0) continue;
			travel_ptr->duration += plan_ptr->duration;
			travel_ptr->index2 = plan_list.Record_Index ();
		} else {
			if (trip != plan_ptr->trip) {
				if (++ntravel == MAX_TRAVEL) break;
				if (trip > 0) travel_ptr++;
				travel_ptr->rec_id = trip = plan_ptr->trip;
				travel_ptr->index1 = plan_list.Record_Index ();
				travel_ptr->start = plan_ptr->key2;
			}
			travel_ptr->index2 = plan_list.Record_Index ();
			travel_ptr->end = plan_ptr->stop_time;
		}
	}

	//---- assign trip file constraints ----

	for (trip_ptr = trip_data.Get_GE (traveler, 0); trip_ptr; trip_ptr = trip_data.Next_Key ()) {
		if (trip_ptr->Traveler () != traveler) break;
		trip = trip_ptr->Trip ();

		if (trip_ptr->Mode () != ACTIVITY_MODE) {
			for (i=0; i < ntravel; i++) {
				travel_ptr = travel + i;

				if (travel_ptr->rec_id == trip) {
					travel_ptr->type = type = trip_ptr->Constraint ();
					if (type == 0) {
						travel_ptr->constraint = trip_ptr->Start_Time ();
					} else if (type == 1) {
						travel_ptr->constraint = trip_ptr->End_Time ();
					} else if (type == 2) {
						travel_ptr->constraint = (trip_ptr->Start_Time () + trip_ptr->End_Time ()) / 2;
					}
					break;
				}
			}
		}
	}

	//---- reconcile the constraints with the travel activities ----

	for (i=0; i < ntravel; i++) {
		travel_ptr = travel + i;

		if (travel_ptr->type == 1) {
			shift = travel_ptr->constraint - travel_ptr->end;
		} else if (travel_ptr->type == 2) {
			shift = travel_ptr->constraint - ((travel_ptr->start + travel_ptr->end) / 2);
		} else {
			shift = 0;
		}

		//---- apply the shift ----

		if (shift != 0) {
			start_time = travel_ptr->end + travel_ptr->duration + shift;
			shift_flag = (travel_ptr->duration  > 0);
			end_time = shift;

			//---- cascade forward ----

			for (j=i+1; j < ntravel; j++) {
				travel_ptr = travel + j;

				if (travel_ptr->type > 0) break;
				if (!shift_flag) {
					if (travel_ptr->start > start_time) break;
					shift = start_time - travel_ptr->start + 60;
				}
				travel_ptr->shift = shift;
				travel_ptr->start += shift;
				travel_ptr->end += shift;
				start_time = travel_ptr->end + travel_ptr->duration;
				shift_flag = (travel_ptr->duration > 0);
			}

			//---- cascade back ----

			shift = end_time;

			for (j=i; j >= 0; j--) {
				travel_ptr = travel + j;

				if (i != j) {
					if (travel_ptr->type > 0) break;
					if (travel_ptr->duration == 0) {
						if (travel_ptr->end < start_time) break;
						shift = start_time - travel_ptr->end - 60;
					}
				}
				travel_ptr->shift = shift;
				start_time = travel_ptr->start += shift;
				travel_ptr->end += shift;
			}
		}
	}

	//---- apply the time adjustments to the trip data ----

	for (trip_ptr = trip_data.Get_GE (traveler, 0); trip_ptr; trip_ptr = trip_data.Next_Key ()) {
		if (trip_ptr->Traveler () != traveler) break;
		trip = trip_ptr->Trip ();

		for (i=0; i < ntravel; i++) {
			travel_ptr = travel + i;

			if (travel_ptr->rec_id == trip || 
				(trip_ptr->Mode () == ACTIVITY_MODE && travel_ptr->rec_id == trip - 1)) {

				start_time = travel_ptr->start;
				if (start_time < 0) {
					start_time = 0;
				} else if (start_time >= MIDNIGHT) {
					start_time = MIDNIGHT - 1;
				}
				end_time = travel_ptr->end;
				if (end_time < 0) end_time = 0;

				if (travel_ptr->type == 1) {
					shift = start_time - trip_ptr->Start_Time ();
					offset = start_offset;
				} else if (travel_ptr->type == 2) {
					shift = start_time - trip_ptr->Start_Time ();
					offset = mid_offset;
				} else {
					shift = end_time - trip_ptr->End_Time ();
					offset = end_offset;
				}
				trip_ptr->Start_Time (start_time);
				trip_ptr->End_Time (end_time);

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

				plan_ptr->stop_time += travel_ptr->shift;
				tot_adjust += abs (travel_ptr->shift);
				num_adjust++;
			}
		}
	}
}
