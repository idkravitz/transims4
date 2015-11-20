//*********************************************************
//	Read_Activity.cpp - Read the Activity File
//*********************************************************

#include "TripSum.hpp"

#include "Activity_File.hpp"
#include "Trip_File.hpp"
#include "Vehicle_File.hpp"

//---------------------------------------------------------
//	Read_Activity
//---------------------------------------------------------

bool TripSum::Read_Activity (int file_num)
{
	int last_hhold, last_person, hhold, person, purpose, mode, start, end, origin, destination;
	int time1, time2, prev_purp, num_kept, num_out, count, vehicle, *parking, nparking, pass;
	bool flag;

	Activity_File *activity_file, *new_file;
	Trip_File *trip_file;
	Vehicle_File *veh_file;
	Survey_Person *person_ptr;
	Vehicle_Data *veh_ptr;
	Location_Data *loc_ptr;
	Access_Data *acc_ptr;

	activity_file = (Activity_File *) Demand_Db_Base (ACTIVITY);

	if (file_num > 0) {
		if (!split_flag) {
			if (!activity_file->Open (file_num)) return (false);
		} else {
			activity_file->Rewind ();
		}
		if (hhlist_flag && hhlist_file.Extend ()) {
			if (!Household_List (file_num)) {
				if (split_flag) return (false);
				Error ("Opening %s", hhlist_file.File_Type ());
			}
		}
	}
	if (output_flag) {
		new_file = (Activity_File *) Demand_Db_Base (NEW_ACTIVITY);

		if (file_num > 0 && new_file->Extend ()) {
			if (!new_file->Open (file_num)) {
				Error ("Creating %s", new_file->File_Type ());
			}
		}
	}
	if (create_flag) {
		trip_file = (Trip_File *) Demand_Db_Base (NEW_TRIP);

		if (file_num > 0 && trip_file->Extend ()) {
			if (!trip_file->Open (file_num)) {
				Error ("Creating %s", trip_file->File_Type ());
			}
		}
		veh_file = (Vehicle_File *) Demand_Db_Base (NEW_VEHICLE);

		if (file_num > 0) {
			if (newhh_flag && newhh_file.Extend ()) {
				if (!newhh_file.Open (file_num)) {
					Error ("Opening %s", hhlist_file.File_Type ());
				}
				fprintf (newhh_file.File (), "HHOLD\tOLDHH\n");
			}
		} else if (newhh_flag) {
			fprintf (newhh_file.File (), "HHOLD\tOLDHH\n");
		}

		//---- build the parking map ----

		loc_ptr = location_data.Last ();
		nparking = loc_ptr->Location () + 1;

		parking = new int [nparking];
		memset (parking, '\0', nparking * sizeof (int));

		for (acc_ptr = access_data.First (); acc_ptr; acc_ptr = access_data.Next ()) {
			if (acc_ptr->From_Type () == LOCATION_ID && acc_ptr->To_Type () == PARKING_ID) {
				parking [acc_ptr->From_ID ()] = acc_ptr->To_ID ();
			}
		}
	}

	//---- process each trip ----

	last_hhold = last_person = prev_purp = num_kept = num_out = 0;
	count = 1;
	flag = true;

	if (activity_file->Extend ()) {
		Show_Message ("Reading %s %s -- Record", activity_file->File_Type (), activity_file->Extension ());
	} else {
		Show_Message ("Reading %s -- Record", activity_file->File_Type ());
	}
	Set_Progress (10000);

	while (activity_file->Read ()) {
		Show_Progress ();

		hhold = activity_file->Household ();
		if (hhold == 0) continue;

		if (hhlist_flag) {
			if (hhold_list.Get_Index (hhold) == 0) continue;
		}
		person = activity_file->Person ();
		
		purpose = activity_file->Purpose ();
		mode = activity_file->Mode ();
		vehicle = activity_file->Vehicle ();

		destination = activity_file->Location ();
		time1 = time_periods.Step (activity_file->Start_Min ());
		time2 = time_periods.Step (activity_file->Start_Max ());

		if (time1 < 0 || time2 < 0) {
			Error ("Converting Start Time for Household %d", hhold);
		}
		end = (time1 + time2 + 1) / 2;

		if (hhold == last_hhold && person == last_person) {
			flag = Trip_Process (origin, destination, start, end, mode, purpose, prev_purp, vehicle, count);
			if (flag) {
				num_kept++;

				if (create_flag) {
					trip_file->Household (hhold_id);
					trip_file->Person (person);
					trip_file->Trip (activity_file->Activity ());
					trip_file->Purpose (purpose);

					pass = activity_file->Passengers ();

					if (mode == DRIVE_ALONE) {
						if (pass > 2) {
							mode = CARPOOL4;
						} else if (pass > 1) {
							mode = CARPOOL3;
						} else if (pass > 0) {
							mode = CARPOOL2;
						}
					} else if (mode == CARPOOL2 || mode == CARPOOL3 || mode == CARPOOL4) {
						mode = MAGIC_MOVE;
					}
					trip_file->Mode (mode);

					trip_file->Start (time_periods.Format_Step (start));
					trip_file->Origin (origin);
					trip_file->Arrive (time_periods.Format_Step (end));
					trip_file->Destination (destination);
					trip_file->Constraint (activity_file->Constraint ());

					if (vehicle > 0) {
						trip_file->Vehicle (veh_id);

						veh_ptr = vehicle_data.Get (vehicle);

						if (veh_ptr != NULL && origin < nparking) {
							veh_file->Vehicle (veh_id);
							veh_file->Household (hhold_id);
							veh_file->Location (parking [origin]);
							veh_file->Type (veh_ptr->Type ());
							veh_file->Sub_Type (veh_ptr->Sub_Type ());

							if (!veh_file->Write ()) {
								Error ("Writing %s", veh_file->File_Type ());
							}
						}
						veh_id++;
					} else {
						trip_file->Vehicle (vehicle);
					}
					if (!trip_file->Write ()) {
						Error ("Writing %s", trip_file->File_Type ());
					}
					if (newhh_flag) {
						fprintf (newhh_file.File (), "%d\t%d\n", hhold_id, hhold);
					}
					hhold_id++;
				}
			}
		} else {
			flag = true;
			if (count_flag) {
				person_ptr = person_count.Get (hhold, person);
				if (person_ptr == NULL) {
					count = 1;
				} else {
					count = person_ptr->Count ();
				}
			}
		}
		origin = destination;

		time1 = time_periods.Step (activity_file->End_Min ());
		time2 = time_periods.Step (activity_file->End_Max ());

		if (time1 < 0 || time2 < 0) {
			Error ("Converting End Time for Household %d", hhold);
		}
		start = (time1 + time2 + 1) / 2;

		last_hhold = hhold;
		last_person = person;
		prev_purp = purpose;

		if (flag && output_flag) {
			new_file->Copy_Fields (activity_file);

			if (!new_file->Write ()) {
				Error ("Writing %s", new_file->File_Type ());
			}
			num_out++;
		}
	}
	End_Progress ();

	if (file_num == 0 || !split_flag) {
		total_in += Progress_Count ();
	}
	if (activity_file->Extend ()) {
		Print (2, "Number of Activity Records Read from %s = %d", activity_file->Extension (), Progress_Count ());
	} else {
		Print (2, "Number of Activity Records Read = %d", Progress_Count ());
	}
	if (output_flag) {
		total_out += num_out;
		if (new_file->Extend ()) {
			Print (1, "Number of Activity Records Written to %s = %d", new_file->Extension (), num_out);
		} else {
			Print (1, "Number of Activity Records Written = %d", num_out);
		}
	}
	total_used += num_kept;
	Print (1, "Number of Trips Used for Reports = %d", num_kept);

	return (true);
}
