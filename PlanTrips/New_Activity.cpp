//*********************************************************
//	New_Activiy.cpp - write traveler activity records
//*********************************************************

#include "PlanTrips.hpp"

//---------------------------------------------------------
//	New_Activity
//---------------------------------------------------------

void PlanTrips::New_Activity (void)
{
	int num, trip, last_trip, start_time, end_time, act_time;
	int origin, destination, vehicle, mode;
	bool drive_flag, transit_flag, bike_flag, drive_in_flag, drive_out_flag;
	char buffer [FIELD_BUFFER];

	Location_Access *access_ptr;
	Plan_Data **ptr, *plan_ptr;

	last_trip = num = act_time = 0;

	for (ptr = (Plan_Data **) plan_list.First (); ptr; ptr = (Plan_Data **) plan_list.Next ()) {
		plan_ptr = *ptr;

		trip = plan_ptr->trip;

		if (last_trip != trip) {
			if (last_trip > 0 && origin != destination) {

				if (num == 0) {
					act_file->Household (plan_ptr->key1 / Traveler_Scale ());
					act_file->Person (plan_ptr->key1 % Traveler_Scale ());
					act_file->Activity (++num);
					act_file->Purpose (0);
					act_file->Priority (0);

					trip_time.Format_Step (0, buffer);
					act_file->Start_Min (buffer);
					act_file->Start_Max (buffer);
					
					trip_time.Format_Step (start_time, buffer);
					act_file->End_Min (buffer);
					act_file->End_Max (buffer);
					
					trip_time.Format_Duration (start_time, buffer);
					act_file->Time_Min (buffer);
					act_file->Time_Max (buffer);

					act_time = end_time;

					act_file->Mode (1);
					act_file->Vehicle (0);
					act_file->Location (origin);
					act_file->Passengers (0);
					act_file->Constraint (0);

					if (!act_file->Write ()) {
						Error ("Writing %s", act_file->File_Type ());
					}
					num_act++;
				} else {
					trip_time.Format_Step (start_time, buffer);
					act_file->End_Min (buffer);
					act_file->End_Max (buffer);
					
					trip_time.Format_Duration (start_time - act_time, buffer);
					act_file->Time_Min (buffer);
					act_file->Time_Max (buffer);

					if (!act_file->Write ()) {
						Error ("Writing %s", act_file->File_Type ());
					}
					num_act++;
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

				act_file->Activity (++num);

				trip_time.Format_Step (end_time, buffer);
				act_file->Start_Min (buffer);
				act_file->Start_Max (buffer);

				act_time = end_time;

				act_file->Mode (mode);
				act_file->Vehicle (vehicle);
				act_file->Location (destination);
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
			act_file->Household (plan_ptr->key1 / Traveler_Scale ());
			act_file->Person (plan_ptr->key1 % Traveler_Scale ());
			act_file->Activity (++num);
			act_file->Purpose (0);
			act_file->Priority (0);

			trip_time.Format_Step (0, buffer);
			act_file->Start_Min (buffer);
			act_file->Start_Max (buffer);

			if (origin == destination) {
				start_time = end_time;
			}
			trip_time.Format_Step (start_time, buffer);
			act_file->End_Min (buffer);
			act_file->End_Max (buffer);
			
			trip_time.Format_Duration (start_time, buffer);
			act_file->Time_Min (buffer);
			act_file->Time_Max (buffer);

			act_time = end_time;

			act_file->Mode (1);
			act_file->Vehicle (0);
			act_file->Location (origin);
			act_file->Passengers (0);
			act_file->Constraint (0);

			if (!act_file->Write ()) {
				Error ("Writing %s", act_file->File_Type ());
			}
			num_act++;
		} else {
			if (origin == destination) {
				start_time = end_time;
			}
			trip_time.Format_Step (start_time, buffer);
			act_file->End_Min (buffer);
			act_file->End_Max (buffer);
			
			trip_time.Format_Duration (start_time - act_time, buffer);
			act_file->Time_Min (buffer);
			act_file->Time_Max (buffer);

			if (!act_file->Write ()) {
				Error ("Writing %s", act_file->File_Type ());
			}
			num_act++;
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

			act_file->Activity (++num);

			trip_time.Format_Step (end_time, buffer);
			act_file->Start_Min (buffer);
			act_file->Start_Max (buffer);

			act_time = MIDNIGHT;
			if (act_time < end_time) act_time = end_time + 60;

			trip_time.Format_Step (act_time, buffer);
			act_file->End_Min (buffer);
			act_file->End_Max (buffer);
			
			trip_time.Format_Duration (act_time - end_time, buffer);
			act_file->Time_Min (buffer);
			act_file->Time_Max (buffer);

			act_file->Mode (mode);
			act_file->Vehicle (vehicle);
			act_file->Location (destination);

			if (!act_file->Write ()) {
				Error ("Writing %s", act_file->File_Type ());
			}
			num_act++;
		}
	}
}
