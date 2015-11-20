//*********************************************************
//	Set_Trip.cpp - Define Trip Details
//*********************************************************

#include "ConvertTrips.hpp"

#include "math.h"

//---------------------------------------------------------
//	Set_Trips
//---------------------------------------------------------

int ConvertTrips::Set_Trips (File_Group *group, int org, int des, int trips, int num, int t1, int t2, int duration)
{
	int t, loc, trp, time, time1, ttim, num_out, org_loc, org_park, des_loc, skim;
	int last, veh, start, arrive, x, y, mode, ntrips;
	double org_wt, des_wt, prob, dist, cum_wt, dx, dy, share, share1, total;
	char start_str [20], arrive_str [20];
	bool return_flag, org_flag, des_flag;
	int retry_count;
	Integer_List *org_ptr, *des_ptr;

	Convert_Location *loc_ptr;
	Diurnal_Target *diurnal_ptr;

	num_out = skim = 0;
	return_flag = (duration > 0);

	if (zone_loc_flag) {
		org_ptr = zone_loc_map.Locations (org);
		des_ptr = zone_loc_map.Locations (des);
	} else {
		org_ptr = des_ptr = 0;
	}

	//---- set the vehicle access flags ----

	mode = group->Mode ();

	org_flag = (mode != DRIVE_ALONE && mode != PNR_OUT && mode != CARPOOL2 &&
				mode != CARPOOL3 && mode != CARPOOL4 && mode != KNR_OUT);
	des_flag = (mode != DRIVE_ALONE && mode != PNR_IN && mode != CARPOOL2 &&
				mode != CARPOOL3 && mode != CARPOOL4 && mode != KNR_IN);

	//---- calculate the origin weight ----

	org_wt = 0.0;

	for (loc = zone_loc [org]; loc; loc = loc_ptr->Zone_List ()) {
		loc_ptr = (Convert_Location *) location_data [loc];	

		if (org_flag || loc_ptr->Org_Parking () != 0) {
			org_wt += loc_ptr->Weight (group->Org_Wt ());
		}
	}
	if (org_ptr != 0) {
		for (loc = org_ptr->First (); loc != 0; loc = org_ptr->Next ()) {
			loc_ptr = (Convert_Location *) location_data [loc];	

			if (org_flag || loc_ptr->Org_Parking () > 0) {
				org_wt += loc_ptr->Weight (group->Org_Wt ());
			}
		}
	}
	if (org_wt == 0.0) return (trips);

	//---- check the destination weight ----
	
	des_wt = 0.0;

	for (loc = zone_loc [des]; loc; loc = loc_ptr->Zone_List ()) {
		loc_ptr = (Convert_Location *) location_data [loc];	

		if (des_flag || loc_ptr->Des_Parking () != 0) {
			des_wt += loc_ptr->Weight (group->Des_Wt ());
		}
	}
	if (des_ptr != 0) {
		for (loc = des_ptr->First (); loc != 0; loc = des_ptr->Next ()) {
			loc_ptr = (Convert_Location *) location_data [loc];	

			if (des_flag || loc_ptr->Des_Parking () > 0) {
				des_wt += loc_ptr->Weight (group->Des_Wt ());
			}
		}
	}
	if (des_wt == 0.0) return (trips);

	if (return_flag) {
		ntrips = (trips + 1) / 2;
	} else {
		ntrips = trips;
	}

	//---- distribute the trips to the time targets ----

	total = 0.0;

	for (t=t1; t <= t2; t++) {
		diurnal_ptr = diurnal_data [t];
		total += diurnal_ptr->Share (num);
	}
	if (total == 0.0) return (trips);

	for (t=t1; t <= t2; t++) {
		diurnal_ptr = diurnal_data [t];
		share = diurnal_ptr->Share (num) / total;
		diurnal_ptr->Add_Target (num, share * ntrips);
	}

	//---- get the travel time ----

	if (skim_ptr != NULL) {
		skim = skim_ptr->Data ();
	} else {
		skim = 0;
	}

	//---- located each trip ----

	for (trp=0; trp < ntrips; trp++) {

		//---- locate the trip origin ----

		prob = org_wt * random_org.Probability ();

		last = 0;
		cum_wt = 0.0;

		for (loc = zone_loc [org]; loc; loc = loc_ptr->Zone_List ()) {
			loc_ptr = (Convert_Location *) location_data [loc];	

			if (org_flag || loc_ptr->Org_Parking () != 0) {
				share = loc_ptr->Weight (group->Org_Wt ());

				if (share != 0.0) {
					cum_wt += share;
					if (prob < cum_wt) break;
					last = loc;
				}
			}
		}
		if (loc == 0 && org_ptr != 0) {
			for (loc = org_ptr->First (); loc != 0; loc = org_ptr->Next ()) {
				loc_ptr = (Convert_Location *) location_data [loc];	

				if (org_flag || loc_ptr->Org_Parking () != 0) {
					share = loc_ptr->Weight (group->Org_Wt ());

					if (share != 0.0) {
						cum_wt += share;
						if (prob < cum_wt) break;
						last = loc;
					}
				}
			}
		}
		if (loc == 0) {
			if (last == 0) continue;
			loc_ptr = (Convert_Location *) location_data [last];
		}
		org_loc = loc_ptr->ID ();
		org_park = loc_ptr->Org_Parking ();
		x = loc_ptr->X ();
		y = loc_ptr->Y ();

		//---- calculate the destination weight ----

		des_wt = 0.0;

		for (loc = zone_loc [des]; loc; loc = loc_ptr->Zone_List ()) {
			loc_ptr = (Convert_Location *) location_data [loc];	

			if (loc_ptr->ID () != org_loc && (des_flag || loc_ptr->Des_Parking () != 0)) {
				share = loc_ptr->Weight (group->Des_Wt ());

				if (share != 0.0) {

					if (group->Dist_Wt ()) {

						//--- apply the distance weight ----

						dx = loc_ptr->X () - x;
						dy = loc_ptr->Y () - y;

						dist = sqrt (dx * dx + dy * dy);
						if (dist == 0.0) dist = 0.01;
					
						des_wt += share * dist;
					} else {
						des_wt += share;
					}
				}
			}
		}
		if (des_ptr != 0) {
			for (loc = des_ptr->First (); loc != 0; loc = des_ptr->Next ()) {
				loc_ptr = (Convert_Location *) location_data [loc];	

				if (loc_ptr->ID () != org_loc && (des_flag || loc_ptr->Des_Parking () != 0)) {
					share = loc_ptr->Weight (group->Des_Wt ());

					if (share != 0.0) {

						if (group->Dist_Wt ()) {

							//--- apply the distance weight ----

							dx = loc_ptr->X () - x;
							dy = loc_ptr->Y () - y;

							dist = sqrt (dx * dx + dy * dy);
							if (dist == 0.0) dist = 0.01;
						
							des_wt += share * dist;
						} else {
							des_wt += share;
						}
					}
				}
			}
		}
		if (des_wt == 0.0) continue;

		//---- locate the destination ----

		prob = des_wt * random_des.Probability ();
		
		last = 0;
		cum_wt = 0.0;

		for (loc = zone_loc [des]; loc; loc = loc_ptr->Zone_List ()) {
			loc_ptr = (Convert_Location *) location_data [loc];	

			if (loc_ptr->ID () != org_loc && (des_flag || loc_ptr->Des_Parking () != 0)) {
				share = loc_ptr->Weight (group->Des_Wt ());

				if (share != 0.0) {
					if (group->Dist_Wt ()) {

						//--- apply the distance weight ----

						dx = loc_ptr->X () - x;
						dy = loc_ptr->Y () - y;

						dist = sqrt (dx * dx + dy * dy);
						if (dist == 0.0) dist = 0.01;
					
						cum_wt += share * dist;
					} else {
						cum_wt += share;
					}
					if (prob < cum_wt) break;
					last = loc;
				}
			}
		}
		if (loc == 0 && des_ptr != 0) {
			for (loc = des_ptr->First (); loc != 0; loc = des_ptr->Next ()) {
				loc_ptr = (Convert_Location *) location_data [loc];	

				if (loc_ptr->ID () != org_loc && (des_flag || loc_ptr->Des_Parking () != 0)) {
					share = loc_ptr->Weight (group->Des_Wt ());

					if (share != 0.0) {
						if (group->Dist_Wt ()) {

							//--- apply the distance weight ----

							dx = loc_ptr->X () - x;
							dy = loc_ptr->Y () - y;

							dist = sqrt (dx * dx + dy * dy);
							if (dist == 0.0) dist = 0.01;
						
							cum_wt += share * dist;
						} else {
							cum_wt += share;
						}
						if (prob < cum_wt) break;
						last = loc;
					}
				}
			}
		}
		if (loc == 0) {
			if (last == 0) continue;
			loc_ptr = (Convert_Location *) location_data [last];
		}
		dx = loc_ptr->X () - x;
		dy = loc_ptr->Y () - y;

		dist = sqrt (dx * dx + dy * dy);

		des_loc = loc_ptr->ID ();

		if (org_loc == des_loc || des_loc == 0) continue;

		//--- estimate the travel time ----

		if (skim != 0) {
			ttim = skim + additional_time;
		} else {
			ttim = (int) ((dist / group->Speed ()) + additional_time + 0.5);
		}
		retry_count = 0;

retry:
		prob = random_tod.Probability ();
		
		time = time1 = 0;
		share = share1 = 0.0;

		for (t=t1; t <= t2; t++) {
			diurnal_ptr = diurnal_data [t];

			share1 = share + diurnal_ptr->Share (num) / total;

			if (prob < share1) {
				if (diurnal_ptr->Target (num) < diurnal_ptr->Trips (num) + 0.5) {
					if (retry_count < 10) {
						retry_count++;
						goto retry;
					}
				}
				time = diurnal_ptr->Start_Time ();
				time1 = diurnal_ptr->End_Time ();
				diurnal_ptr->Add_Trips (num);
				break;
			}
			share = share1;
		}
		if (share != share1) {
			time += (int) ((time1 - time) * (prob - share) / (share1 - share) + 0.5);
		}

		//---- time methods ----

		if (group->Method () == 0) {
			start = time;
		} else if (group->Method () == 1) {
			start = time - ttim;
			if (start < 0) {
				if (return_flag) {
					start = 1;
				} else {
					start += MIDNIGHT;
				}
			}
		} else {
			start = time - ttim / 2;
			if (start < 0) {
				if (return_flag) {
					start = 1;
				} else {
					start += MIDNIGHT;
				}
			}
		}
		arrive = start + ttim;

		//---- convert time to a string ----

		if (trip_time.Format_Step (start, start_str) == NULL ||
			trip_time.Format_Step (arrive, arrive_str) == NULL) {
			Error ("Time of Day Formating");
		}

		//---- store the results ----
	
		mode = group->Mode ();

		if (!org_flag || !des_flag) {
			veh = vehicle_id++;
		} else {
			veh = 0;
		}

		//---- trip data ----

		trip_file->Household (hhold_id);
		trip_file->Person (1);
		trip_file->Trip (1);
		trip_file->Purpose (group->Purpose ());
		trip_file->Mode (mode);
		trip_file->Vehicle (veh);
		trip_file->Start (start_str);
		trip_file->Origin (org_loc);
		trip_file->Arrive (arrive_str);
		trip_file->Destination (des_loc);
		trip_file->Constraint (group->Method ());

		if (!trip_file->Write ()) {
			Error ("Writing Trip File");
		}

		//---- household data ----

		household_file->Household (hhold_id);
		household_file->Location (org_loc);
		household_file->Persons (1);
		household_file->Workers (1);
		household_file->Vehicles (1);

		if (!household_file->Write ()) {
			Error ("Writing Household File");
		}

		//---- population data ---- 

		population_file->Household (hhold_id);
		population_file->Person (1);
		population_file->Age (25);
		population_file->Gender (1);
		population_file->Work (1);
		population_file->Drive (1);

		if (!population_file->Write ()) {
			Error ("Writing Population File");
		}

		//---- vehicle data ----

		if (veh != 0) {
			vehicle_file->Vehicle (veh);
			vehicle_file->Household (hhold_id);
			vehicle_file->Location (org_park);
			vehicle_file->Type (group->Type ());
			vehicle_file->Sub_Type (group->SubType ());

			if (!vehicle_file->Write ()) {
				Error ("Writing Vehicle File");
			}
		}
		hhold_id++;
		nperson++;
		num_trip++;
		num_out++;

		//---- check for return trips ----

		if (!return_flag || num_out == trips) continue;

		if (mode == PNR_OUT) mode = PNR_IN;
		if (mode == KNR_OUT) mode = KNR_IN;

		//---- set start and end times ----

		start = arrive + duration;

		if (start > MIDNIGHT) {
			start = MIDNIGHT - 1;
		}
		arrive = start + ttim;

		//---- convert time to a string ----

		if (trip_time.Format_Step (start, start_str) == NULL ||
			trip_time.Format_Step (arrive, arrive_str) == NULL) {
			Error ("Time of Day Formating");
		}

		//---- return trip data ----

		trip_file->Trip (2);
		trip_file->Mode (mode);
		trip_file->Start (start_str);
		trip_file->Origin (des_loc);
		trip_file->Arrive (arrive_str);
		trip_file->Destination (org_loc);
		trip_file->Constraint (0);

		if (!trip_file->Write ()) {
			Error ("Writing Trip File");
		}
		num_out++;
		num_trip++;
	}
	return (trips - num_out);
}
