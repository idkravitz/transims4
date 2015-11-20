//*********************************************************
//	Read_Trip.cpp - Read the Trip File
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Read_Trip
//---------------------------------------------------------

void Router::Read_Trip (void)
{
	int last_hhold, last_person, last_trip, hhold, person, trip;
	int start, arrive, type, mode, origin, destination, purpose;
	bool skip, warning_flag;

	Household_Type *hh_ptr;
	Trip_Data trip_rec;

	//---- process each trip ----

	type = 1;
	last_hhold = last_person = last_trip = 0;
	skip = warning_flag = false;

	Show_Message ("Reading %s -- Record", trip_file->File_Type ());
	Set_Progress (1000);

	while (trip_file->Read ()) {
		Show_Progress ();
		nrecord++;

		//---- check the household list ----

		hhold = trip_file->Household ();
		person = trip_file->Person ();
		trip = trip_file->Trip ();

		if (hhold != last_hhold || person != last_person || trip != last_trip) {
			if (hhlist_flag || hhrec_flag || hhold_flag) {
				hh_ptr = household_type.Get_LE (hhold, person, trip);

				if (hh_ptr != NULL && 
					(hh_ptr->Household () != hhold ||
					(hh_ptr->Person () != 0 && hh_ptr->Person () != person) ||
					(hh_ptr->Record () != 0 && hh_ptr->Record () != trip))) {

					hh_ptr = NULL;
				}
				if (hh_ptr == NULL) {
					if (hhlist_flag || hhrec_flag) {
						if (nhh_proc >= nhh_list) break;
					} else {
						Warning ("Household %d was Not Found in the Household File", hhold);
					}
					skip = true;
					continue;
				} else {
					veh_flag = (hh_ptr->Record () > 0);
				}
				if (hhold_flag) {
					type = hh_ptr->Type ();
				}
			}
			if (hhold != last_hhold) {
				nhh_proc++;
			} else if (person != last_person && last_person > 1) {
				reset_veh_flag = true;
			}
			last_hhold = hhold;
			last_person = person;
			last_trip = trip;
			skip = false;
		} else {
			if (skip) continue;
		}

		//---- check the selection criteria ----

		mode = trip_file->Mode ();
		if (mode < MAX_MODE && !select_mode [mode]) continue;

		purpose = trip_file->Purpose ();
		if (purpose_flag && !purpose_range.In_Range (purpose)) continue;

		origin = trip_file->Origin ();
		destination = trip_file->Destination ();

		if (origin == destination) continue;
		if (select_org && !org_range.In_Range (origin)) continue;
		if (select_des && !des_range.In_Range (destination)) continue;

		//---- check the traveler ID ----

		if (person >= Traveler_Scale () && !warning_flag) {
			Warning ("Person ID %d is Out of Range (1..%d)", person, Traveler_Scale ());
			warning_flag = true;
		}

		//---- save the trip data ----

		trip_rec.Household (hhold);
		trip_rec.Person (person);
		trip_rec.Trip (trip);
		trip_rec.Purpose (purpose);
		trip_rec.Mode (mode);
		trip_rec.Vehicle (trip_file->Vehicle ());
		trip_rec.Origin (origin);
		trip_rec.Destination (destination);

		//---- get and check the start time ----

		start = trip_time.Step (trip_file->Start ());
		if (start < 0 || start > MIDNIGHT * 3) {
			Warning ("Converting Start Time %s for Household %d", trip_file->Start (), hhold);
			continue;
		}
		trip_rec.Start_Time (start);

		if (select_time && !time_range.In_Range (start)) continue;

		//---- get and check the arrival time ----
		
		arrive = trip_time.Step (trip_file->Arrive ());
		if (arrive < 0 || arrive > MIDNIGHT * 3) {
			Warning ("Converting Arrival Time %s for Household %d", trip_file->Arrive (), hhold);
			continue;
		}
		trip_rec.End_Time (arrive);

		//---- build the path ----

		Plan_Build (&trip_rec);
	}
	End_Progress ();

	trip_file->Close ();
}
