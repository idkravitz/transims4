//*********************************************************
//	Read_Activity.cpp - Read the Activity File
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Read_Activity
//---------------------------------------------------------

void Router::Read_Activity (void)
{
	int last_hhold, last_person, hhold, person, duration, trip, type, vehicle;
	int lower_start, upper_start, lower_time, upper_time, lower_end, upper_end;
	int mode, start, arrive, origin, destination, purpose;
	bool skip_hhold, skip_person, warning_flag;

	Household_Type *hh_ptr;
	Trip_Data trip_rec;

	walk_active = false;
	warning_flag = false;
	skip_hhold = skip_person = true;

	type = 1;
	last_hhold = last_person = trip = start = origin = person = 0;

	//---- process each activity ----

	Show_Message ("Reading %s -- Record", activity_file->File_Type ());
	Set_Progress (2000);

	while (activity_file->Read ()) {
		Show_Progress ();
		nrecord++;

		//---- check the household list ----

		hhold = activity_file->Household ();

		if (hhold != last_hhold) {
			if (hhlist_flag || hhrec_flag || hhold_flag) {
				hh_ptr = household_type.Get_GE (hhold);

				if (hh_ptr == NULL || hh_ptr->Household () != hhold) {
					if (hhlist_flag || hhrec_flag) {
						if (nhh_proc >= nhh_list) break;
					} else {
						Warning ("Household %d was Not Found in the Household File", hhold);
					}
					skip_hhold = true;
					continue;
				}
				if (hhold_flag) {
					type = hh_ptr->Type ();
				}
			}
			if (hhold != last_hhold) {
				nhh_proc++;
			}
			skip_hhold = false;
			last_hhold = hhold;
			last_person = 0;
		} else {
			if (skip_hhold) continue;
		}

		//---- gather data ----

		lower_start = trip_time.Step (activity_file->Start_Min ());
		upper_start = trip_time.Step (activity_file->Start_Max ());
		lower_time = trip_time.Step (activity_file->Time_Min ());
		upper_time = trip_time.Step (activity_file->Time_Max ());

		if (lower_start < 0 || lower_start > MIDNIGHT * 3 ||
			upper_start < 0 || upper_start > MIDNIGHT * 3) {
			Warning ("Converting Start Time %s for Household %d", activity_file->Start_Min (), hhold);
			skip_hhold = true;
			continue;
		}
		if (lower_time < 0 || lower_time > MIDNIGHT * 3 ||
			upper_time < 0 || upper_time > MIDNIGHT * 3) {
			Warning ("Converting Duration Time %s for Household %d", activity_file->Time_Min (), hhold);
			skip_hhold = true;
			continue;
		}
		arrive = (lower_start + upper_start) / 2;
		duration = (lower_time + upper_time) / 2;
		destination = activity_file->Location ();
		mode = activity_file->Mode ();
		purpose = activity_file->Purpose ();
		vehicle = activity_file->Vehicle ();

		if (trip == 0 && lower_start == 0) arrive = 0;

		//---- convert vehicle drivers and passengers ----

		if (mode == DRIVE_ALONE) {
			if (activity_file->Passengers () > 2) {
				mode = CARPOOL4;
			} else if (activity_file->Passengers () > 1) {
				mode = CARPOOL3;
			} else if (activity_file->Passengers () > 0) {
				mode = CARPOOL2;
			}
		//} else if ((mode == CARPOOL2 || mode == CARPOOL3 || mode == CARPOOL4) && vehicle <= 0) {
		} else if (mode == CARPOOL2 || mode == CARPOOL3 || mode == CARPOOL4) {
			mode = MAGIC_MOVE;
		}

		//---- check for a new person ----

		person = activity_file->Person ();

		if (person != last_person) {
			if (hhrec_flag) {
				hh_ptr = household_type.Get_LE (hhold, person);

				if (hh_ptr != NULL && 
					(hh_ptr->Household () != hhold ||
					(hh_ptr->Person () != 0 && hh_ptr->Person () != person))) {

					hh_ptr = NULL;
				}
				if (hh_ptr == NULL) {
					hh_ptr = household_type.Get_GE (hhold, person);

					if (hh_ptr == NULL || hh_ptr->Household () != hhold || hh_ptr->Person () != person) {
						skip_person = true;
						continue;
					}
				}
			}
			skip_person = false;
			if (last_person > 0 && person > 1) {
				reset_veh_flag = true;
			}
			last_person = person;
			trip = 0;
			nprocess++;

			//---- check the traveler ID ----

			if (person >= Traveler_Scale () && !warning_flag) {
				Write (1, "Person ID %d is Out of Range (1..%d)", person, Traveler_Scale ());
				warning_flag = true;
			}
			trip_rec.Household (hhold);
			trip_rec.Person (person);

			//---- check the first activity ----

			if (hhrec_flag) {
				hh_ptr = household_type.Get_LE (hhold, person);

				if (hh_ptr != NULL && 
					(hh_ptr->Household () != hhold ||
					(hh_ptr->Person () != 0 && hh_ptr->Person () != person))) {

					hh_ptr = NULL;
				}
				if (hh_ptr == NULL) {
					trip++;
					start = arrive + duration;
					origin = destination;
					continue;
				}
			}
		} else {

			if (skip_person) continue;

			if (hhrec_flag) {
				hh_ptr = household_type.Get_LE (hhold, person, trip);

				if (hh_ptr != NULL && 
					(hh_ptr->Household () != hhold ||
					(hh_ptr->Person () != 0 && hh_ptr->Person () != person) ||
					(hh_ptr->Record () != 0 && hh_ptr->Record () != trip))) {

					hh_ptr = NULL;
				}
				if (hh_ptr == NULL) {
					trip += 2;
					start = arrive + duration;
					origin = destination;
					continue;
				} else {
					veh_flag = (hh_ptr->Record () > 0);
				}
			}

			//---- save the trip data ----

			trip_rec.Household (hhold);
			trip_rec.Person (person);
			trip_rec.Trip (trip++);
			trip_rec.Purpose (purpose);
			trip_rec.Mode (mode);
			trip_rec.Vehicle (vehicle);
			trip_rec.Origin (origin);
			trip_rec.Start_Time (start);
			trip_rec.Destination (destination);
			trip_rec.End_Time (arrive);

			//---- check the selection criteria ----

			if (origin != destination &&
				(mode > MAX_MODE || select_mode [mode]) &&
				(!purpose_flag || purpose_range.In_Range (purpose)) &&
				(!select_org || org_range.In_Range (origin)) &&
				(!select_des || des_range.In_Range (destination)) &&
				(!select_time || time_range.In_Range (start))) {

				Plan_Build (&trip_rec, type, duration);

				arrive = Resolve (trip_des.TOD ());
			}
		}

		//---- check the artivity time constraints ----

		if (arrive < lower_start && !adjust_flag) arrive = lower_start;

		lower_end = trip_time.Step (activity_file->End_Min ());
		upper_end = trip_time.Step (activity_file->End_Max ());

		start = arrive + duration;

		if (duration_flag) {
			if (start > upper_end) {
				duration = upper_end - arrive;
				if (duration < 60) duration = 60;
				start = arrive + duration;
			}
		} else if (!adjust_flag) {
			if (start < lower_end) {
				start = lower_end;
				duration = start - arrive;
			} else if (start > upper_end) {
				duration = upper_end - arrive;
				if (duration < lower_time) duration = lower_time;
				start = arrive + duration;
			}
		}
		origin = destination;

		//---- output the activity ----

		if (((!select_org || org_range.In_Range (origin)) || 
			(!select_des || des_range.In_Range (destination))) &&
			(!select_time || time_range.In_Range (arrive) || time_range.In_Range (start))) {

			//---- copy old plans ----

			if (old_plan_flag) {
				while (old_plan_file.Traveler () < trip_rec.Traveler () ||
					(old_plan_file.Traveler () == trip_rec.Traveler () && 
					old_plan_file.Trip () < trip)) {

					if (old_plan_file.Traveler () > 0) {
						if (!skim_flag || (skim_flag && Update_Plan ())) {
							if (!plan_file.Write (old_plan_file.Plan ())) {
								Error ("Writing Plan File");
							}
						}
					}
					if (!old_plan_file.Read ()) {
						old_plan_flag = false;
						old_plan_file.Close ();
						break;
					}
				}
				if (old_plan_flag) {
					while (old_plan_file.Traveler () == trip_rec.Traveler () &&
						old_plan_file.Trip () == trip) {

						if (!old_plan_file.Read ()) {
							old_plan_flag = false;
							old_plan_file.Close ();
							break;
						}
					}
				}
			}
			if (!duration_flag) {
				plan_file.Traveler (trip_rec.Traveler ());
				plan_file.Trip (trip);
				plan_file.Leg (1);
				plan_file.Time (arrive);
				plan_file.Start_ID (destination);
				plan_file.Start_Type (LOCATION_ID);
				plan_file.End_ID (destination);
				plan_file.End_Type (LOCATION_ID);
				plan_file.Duration (duration);
				plan_file.Stop_Time (start);
				plan_file.Cost (0);
				plan_file.GCF (0);
				plan_file.Driver_Flag (0);
				plan_file.Mode (Plan_File::ACTIVITY_MODE);
				plan_file.Tokens (0);

				if (save_plans && !plan_file.Write ()) {
					Error ("Writing Plan File");
				}
			}
		}
		trip++;
	}
	End_Progress ();

	activity_file->Close ();
}
