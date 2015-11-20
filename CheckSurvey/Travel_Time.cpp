//*********************************************************
//	Travel_Time.cpp - estimate travel time
//*********************************************************

#include "CheckSurvey.hpp"

#include <math.h>

//---------------------------------------------------------
//	Travel_Time
//---------------------------------------------------------

void CheckSurvey::Travel_Time (int weight)
{
	int i, j, nact, person, time, ttim, mode, distance, purpose, start;
	int tour_distance, tour_time, tour_purp, home_loc, loc;
	int purp_code, prev_purp, stops;
	bool home_flag;

	Activity_Data *act_ptr, *loc_ptr;

	nact = hhold_act_data.Num_Records ();

	//---- calculate travel times ----

	person = home_loc = loc = 0;
	tour_purp = tour_distance = tour_time = prev_purp = stops = 0;
	home_flag = false;

	for (act_ptr = hhold_act_data.First (); act_ptr; act_ptr = hhold_act_data.Next ()) {

		//---- get the current location ----

		purpose = act_ptr->Purpose ();

		if (purpose == 0) {
			if (!home_flag) {
				home_loc = act_ptr->Location ();
				home_flag = true;
			}

			//---- sum the tour length data ----

			if (tour_flag && tour_purp > 0) {
				if (!tour_length.Add_Trip (tour_purp, tour_distance, tour_time, 0, weight)) {
					Error ("Adding Tour Length Data");
				}
			}
			if (tour_purpose_flag && tour_purp > 0) {
				purp_code = (tour_purp << 16) + stops;

				if (!tour_purpose.Add_Trip (purp_code, tour_distance, tour_time, 0, weight)) {
					Error ("Adding Tour Purpose Data");
				}
			}
			loc = home_loc;
			tour_distance = tour_time = tour_purp = stops = 0;
		} else {
			loc = act_ptr->Location ();
			stops++;
		}

		//---- check for a new person or a passenger ----

		if (person != act_ptr->Person () || act_ptr->Mode () >= CARPOOL2) {
			person = act_ptr->Person ();
			act_ptr->Duration (0);
			start = 0;
			time = act_ptr->End_Time ();
			prev_purp = purpose;
			continue;
		}

		//---- get the distance and travel time ----

		mode = act_ptr->Mode ();
		if (mode < ALL_WALK || mode >= MAX_MODE) mode = DRIVE_ALONE;

		ttim = act_ptr->Start_Time () - time;

		if (ttim <= 0 && prev_purp == 0) {
			ttim = (time - start) / 2;
			if (ttim > 1800) {
				ttim = 1800;
			}
			time -= ttim;
			ttim = act_ptr->Start_Time () - time;
		}
		if (ttim < 0) ttim = 0;
		act_ptr->Duration (ttim);

		if (add_time [mode] < ttim) {
			ttim -= add_time [mode];
		}
		distance = Resolve (ttim * avg_speed [mode]);

		time = act_ptr->End_Time ();

		//---- sum the trip length data ----

		if (length_flag && purpose > 0) {
			if (!trip_length.Add_Trip (purpose, distance, ttim, 0, weight)) {
				Error ("Adding Trip Length Data");
			}
		}
		if (tour_flag || tour_purpose_flag) {
			if (act_ptr->Priority () == PRIMARY_ANCHOR) {
				tour_purp = purpose;
			}
			tour_distance += distance;
			tour_time += ttim;
		}
		if (trip_purpose_flag) {
			purp_code = (prev_purp << 16) + purpose;

			if (!trip_purpose.Add_Trip (purp_code, distance, ttim, 0, weight)) {
				Error ("Adding Trip Purpose Data");
			}
		}
		if (mode_flag) {
			if (!mode_length.Add_Trip (mode, distance, ttim, 0, weight)) {
				Error ("Adding Mode Length Data");
			}
		}
		if (mode_purpose_flag) {
			purp_code = (mode << 16) + purpose;

			if (!mode_purpose.Add_Trip (purp_code, distance, ttim, 0, weight)) {
				Error ("Adding Mode Purpose Data");
			}
		}
		prev_purp = purpose;
	}

	//---- set passenger travel times ----

	for (i=1; i <= nact; i++) {
		act_ptr = hhold_act_data [i];

		if (act_ptr->Mode () >= CARPOOL2) {
			for (j=1; j <= nact; j++) {
				loc_ptr = hhold_act_data [j];

				if (loc_ptr->Mode () == DRIVE_ALONE &&
					loc_ptr->Vehicle () == act_ptr->Vehicle () &&
					loc_ptr->Start_Time () == act_ptr->Start_Time ()) {

					act_ptr->Duration (loc_ptr->Duration ());
					break;
				}
			}
		}
	}
}
