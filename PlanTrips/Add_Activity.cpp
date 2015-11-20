//*********************************************************
//	Add_Activiy.cpp - add activity records to activity data
//*********************************************************

#include "PlanTrips.hpp"

//---------------------------------------------------------
//	Add_Activity
//---------------------------------------------------------

void PlanTrips::Add_Activity (void)
{
	int num, trip, last_trip, start_time, end_time, act_time;
	int origin, destination, vehicle, mode;
	bool drive_flag, transit_flag, bike_flag, drive_in_flag, drive_out_flag;

	Location_Access *access_ptr;
	Plan_Data **ptr, *plan_ptr;
	Activity_Data act_rec;

	last_trip = num = act_time = 0;

	for (ptr = (Plan_Data **) plan_list.First (); ptr; ptr = (Plan_Data **) plan_list.Next ()) {
		plan_ptr = *ptr;

		trip = plan_ptr->trip;

		if (last_trip != trip) {
			if (last_trip > 0 && origin != destination) {

				if (num == 0) {
					act_rec.Traveler (plan_ptr->key1);
					act_rec.Activity (++num);
					act_rec.Purpose (0);
					act_rec.Priority (0);

					act_rec.Start_Time (0);
					act_rec.End_Time (start_time);
					act_rec.Duration (start_time);

					act_time = end_time;

					act_rec.Mode (1);
					act_rec.Vehicle (0);
					act_rec.Location (origin);
					act_rec.Passengers (0);
					act_rec.Constraint (0);

					if (!activity_data.Add (&act_rec)) {
						Error ("Adding Activity Data");
					}
				} else {
					act_rec.End_Time (start_time);
					act_rec.Duration (start_time - act_time);

					if (!activity_data.Add (&act_rec)) {
						Error ("Adding Activity Data");
					}
				}

				//---- set the mode ----

				if (drive_in_flag) {
					mode = PNR_IN;
				} else if (drive_out_flag) {
					mode = PNR_OUT;
				} else if (drive_flag) {
					mode = DRIVE_ALONE;
				} else if (transit_flag) {
					mode = TRANSIT;
				} else if (bike_flag) {
					mode = BICYCLE;
				} else {
					mode = ALL_WALK;
				}

				act_rec.Activity (++num);

				act_rec.Start_Time (end_time);

				act_time = end_time;

				act_rec.Mode (mode);
				act_rec.Vehicle (vehicle);
				act_rec.Location (destination);
			}
			last_trip = trip;

			//---- start a new trip ----

			vehicle = 0;
			drive_flag = transit_flag = bike_flag = drive_in_flag = drive_out_flag = 0;

			start_time = plan_ptr->key2;
			origin = plan_ptr->start_id;

			if (plan_ptr->start_type == PARKING_ID) {
				access_ptr = (Location_Access *) parking_access.Get (&origin);
				if (access_ptr == NULL) {
					Warning ("No Activity Location to access Parking Lot %d", origin);
				} else {
					start_time -= access_ptr->from_time;
					if (start_time < 0) start_time = 0;
					origin = access_ptr->from_loc;
				}
			} else if (plan_ptr->start_type == STOP_ID) {
				access_ptr = (Location_Access *) stop_access.Get (&origin);
				if (access_ptr == NULL) {
					Warning ("No Activity Location to access Transit Stop %d", origin);
				} else {
					start_time -= access_ptr->from_time;
					if (start_time < 0) start_time = 0;
					origin = access_ptr->from_loc;
				}
			}
		}

		//---- process the travel leg ----

		destination = plan_ptr->end_id;
		end_time = plan_ptr->stop_time;

		if (plan_ptr->mode == AUTO_MODE) {
			vehicle = plan_ptr->data [0];
		}

		if (plan_ptr->end_type == PARKING_ID) {
			access_ptr = (Location_Access *) parking_access.Get (&destination);
			if (access_ptr == NULL) {
				Warning ("No Activity Location for leaving Parking Lot %d", destination);
			} else {
				end_time += access_ptr->to_time;
				destination = access_ptr->to_loc;
			}
			drive_flag = true;
			if (transit_flag) {
				drive_in_flag = true;
			}
		} else if (plan_ptr->end_type == STOP_ID) {
			access_ptr = (Location_Access *) stop_access.Get (&destination);
			if (access_ptr == NULL) {
				Warning ("No Activity Location for leaving Transit Stop %d", destination);
			} else {
				end_time += access_ptr->to_time;
				destination = access_ptr->to_loc;
			}
			transit_flag = true;
			if (drive_flag) {
				drive_out_flag = true;
			}
		} else if (plan_ptr->mode == BIKE_MODE) {
			bike_flag = true;
		}
	}

	//---- save the last trip ----

	if (last_trip > 0) {
		if (num == 0) {
			act_rec.Traveler (plan_ptr->key1);
			act_rec.Activity (++num);
			act_rec.Purpose (0);
			act_rec.Priority (0);

			act_rec.Start_Time (0);

			if (origin == destination) {
				start_time = end_time;
			}
			act_rec.End_Time (start_time);
			act_rec.Duration (start_time);

			act_time = end_time;

			act_rec.Mode (1);
			act_rec.Vehicle (0);
			act_rec.Location (origin);
			act_rec.Passengers (0);
			act_rec.Constraint (0);

			if (!activity_data.Add (&act_rec)) {
				Error ("Adding Activity Data");
			}
		} else {
			if (origin == destination) {
				start_time = end_time;
			}
			act_rec.End_Time (start_time);
			act_rec.Duration (start_time - act_time);

			if (!activity_data.Add (&act_rec)) {
				Error ("Adding Activity Data");
			}
		}

		if (origin != destination) {

			//---- set the mode ----

			if (drive_in_flag) {
				mode = PNR_IN;
			} else if (drive_out_flag) {
				mode = PNR_OUT;
			} else if (drive_flag) {
				mode = DRIVE_ALONE;
			} else if (transit_flag) {
				mode = TRANSIT;
			} else if (bike_flag) {
				mode = BICYCLE;
			} else {
				mode = ALL_WALK;
			}

			//---- save the activity record ----

			act_rec.Activity (++num);

			act_rec.Start_Time (end_time);

			act_time = MIDNIGHT;
			if (act_time < end_time) act_time = end_time + 60;

			act_rec.End_Time (act_time);
			act_rec.Duration (act_time - end_time);

			act_rec.Mode (mode);
			act_rec.Vehicle (vehicle);
			act_rec.Location (destination);

			if (!activity_data.Add (&act_rec)) {
				Error ("Adding Activity Data");
			}
		}
	}
}
