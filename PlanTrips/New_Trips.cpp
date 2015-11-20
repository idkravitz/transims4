//*********************************************************
//	New_Trips.cpp - write traveler trip records
//*********************************************************

#include "PlanTrips.hpp"

//---------------------------------------------------------
//	New_Trips
//---------------------------------------------------------

void PlanTrips::New_Trips (void)
{
	int num, trip, last_trip, start_time, end_time;
	int origin, destination, vehicle, mode, hhold, person;
	bool drive_flag, transit_flag, bike_flag, drive_in_flag, drive_out_flag;
	char buffer [FIELD_BUFFER];

	Location_Access *access_ptr;
	Plan_Data **ptr, *plan_ptr;
	Vehicle_Data *veh_ptr;

	//---- calculate the trip times ----

	last_trip = num = hhold = 0;
	person = 1;

	for (ptr = (Plan_Data **) plan_list.First (); ptr; ptr = (Plan_Data **) plan_list.Next ()) {
		plan_ptr = *ptr;

		trip = plan_ptr->trip;

		if (last_trip != trip) {
			if (last_trip > 0 && origin != destination) {

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

				//---- save the trip record ----

				trip_file->Household (hhold);
				trip_file->Person (person);
				trip_file->Trip (((create_flag) ? 1 : ++num));
				trip_file->Purpose (0);
				trip_file->Mode (mode);
				trip_file->Vehicle (vehicle);
				trip_file->Origin (origin);
				trip_file->Destination (destination);
				trip_file->Start (trip_time.Format_Time (start_time));
				trip_file->Arrive (trip_time.Format_Time (end_time));
				trip_file->Constraint (0);

				if (!trip_file->Write ()) {
					Error ("Writing Trip File");
				}
				num_trips++;
			}
			last_trip = trip;

			//---- start a new trip ----

			vehicle = 0;
			drive_flag = transit_flag = bike_flag = drive_in_flag = drive_out_flag = 0;

			if (create_flag) {
				hhold = ++hhold_id;

				if (hhold_flag) {
					str_fmt (buffer, sizeof (buffer), "%d", hhold);

					if (!hhold_file.Write (buffer)) {
						Error ("Writing %s", hhold_file.File_Type ());
					}
				}
			} else {
				hhold = plan_ptr->key1 / Traveler_Scale ();
				person = plan_ptr->key1 % Traveler_Scale ();
			}
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

			if (create_flag) {
				veh_ptr = vehicle_data.Get (vehicle);
				vehicle = ++veh_id;

				veh_file->Vehicle (vehicle);
				veh_file->Household (hhold);
				veh_file->Location (plan_ptr->start_id);
				veh_file->Type (veh_ptr->Type ());
				veh_file->Sub_Type (veh_ptr->Sub_Type ());

				if (!veh_file->Write ()) {
					Error ("Writing %s", veh_file->File_Type ());
				}
				plan_ptr->data [0] = vehicle;
			}
		}
		if (create_flag) {
			plan_ptr->key1 = hhold * Traveler_Scale () + 1;
			plan_ptr->trip = 1;
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

	if (last_trip > 0 && origin != destination) {

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

		//---- save the trip record ----

		trip_file->Household (hhold);
		trip_file->Person (person);
		trip_file->Trip (((create_flag) ? 1 : ++num));
		trip_file->Purpose (0);
		trip_file->Mode (mode);
		trip_file->Vehicle (vehicle);
		trip_file->Origin (origin);
		trip_file->Destination (destination);
		trip_file->Start (trip_time.Format_Time (start_time));
		trip_file->Arrive (trip_time.Format_Time (end_time));
		trip_file->Constraint (0);

		if (!trip_file->Write ()) {
			Error ("Writing Trip File");
		}
		num_trips++;
	}
}
