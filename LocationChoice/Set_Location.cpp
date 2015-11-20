//*********************************************************
//	Set_Location.cpp - select activity locations
//*********************************************************

#include "LocationChoice.hpp"

//---------------------------------------------------------
//	Set_Location
//---------------------------------------------------------

void LocationChoice::Set_Location (int start, int arrive, int leave, int end)
{
	int i, loc, loc1, loc2, budget1, budget2, time;
	int start_time, end_time, duration, mode, purpose, group; 
	int zone1, zone2, location1, location2, purpose1, purpose2;
	bool drive_flag;

	Purpose_Group *group_ptr;
	Choice_Data *location_ptr;
	Activity_Data *act_ptr;
	
	//---- get the start and end locations ----

	act_ptr = hhold_act_data [start];
	loc1 = act_ptr->Location ();

	if (loc1 < 0) {
		if (dump_flag) {
			Print (1, "Start Location %d is Undefined", loc1);
		}
		Activity_Problem (LOCATION_PROBLEM, act_ptr);
		return;
	}
	location_ptr = location_data [loc1];
	if (location_ptr != NULL) {
		zone1 = location_ptr->Zone ();
		location1 = location_ptr->Location ();
	} else {
		zone1 = location1 = 0;
	}
	purpose1 = act_ptr->Purpose ();
	start_time = time = act_ptr->End_Time ();
	budget1 = 0;

	for (i=start+1; i <= arrive; i++) {
		act_ptr = hhold_act_data [i];
		budget1 += act_ptr->Start_Time () - time;
		time = act_ptr->End_Time ();
	}
	act_ptr = hhold_act_data [leave];
	end_time = time = act_ptr->End_Time ();
	budget2 = 0;

	for (i=leave+1; i <= end; i++) {
		act_ptr = hhold_act_data [i];
		budget2 += act_ptr->Start_Time () - time;
		time = act_ptr->End_Time ();
	}
	loc2 = act_ptr->Location ();

	if (loc2 < 0) {
		loc2 = loc1;
	}
	location_ptr = location_data [loc2];
	if (location_ptr != NULL) {
		zone2 = location_ptr->Zone ();
		location2 = location_ptr->Location ();
	} else {
		zone2 = location2 = 0;
	}
	purpose2 = act_ptr->Purpose ();

	//---- get the purpose group ----

	act_ptr = hhold_act_data [arrive];
	duration = end_time - act_ptr->Start_Time ();

	mode = act_ptr->Mode ();
	if (mode < ALL_WALK || mode >= MAX_MODE) mode = DRIVE_ALONE;

	purpose = act_ptr->Purpose ();
	if (purpose < 1) {
		if (dump_flag) {
			Print (1, "Trip Purpose %d is Undefined", purpose);
		}
		return;
	}

	if (purpose > max_purpose) {
		if (purpose >= MAX_PURPOSE) {
			if (dump_flag) {
				Print (1, "Trip Purpose %d is Out of Range", purpose);
			}
			purpose = MAX_PURPOSE - 1;
		}
		max_purpose = purpose;
		purpose_map [purpose] = -1;
		if (dump_flag) {
			Print (1, "Trip Purpose %d is Undefined", purpose);
		}
		return;
	}
	group = purpose_map [purpose];
	if (group < 1) {
		if (group == 0) {
			if (dump_flag) {
				Print (1, "Trip Purpose %d Group is Undefined", purpose);
			}
			purpose_map [purpose] = -1;
		}
		return;
	}
	group_ptr = purpose_group [group];
	if (group_ptr == NULL) return;

	//---- initialize the model data -----

	if (group_ptr->Model ()) {
		tour_info.Put_Field (start_field, start_time);
		tour_info.Put_Field (end_field, end_time);
		tour_info.Put_Field (duration_field, duration);
		tour_info.Put_Field (purpose_field, act_ptr->Purpose ());
		tour_info.Put_Field (stops1_field, (arrive - start - 1));
		tour_info.Put_Field (stops2_field, (end - leave - 1));
		tour_info.Put_Field (subtours_field, (leave - arrive));
		tour_info.Put_Field (budget1_field, budget1);
		tour_info.Put_Field (budget2_field, budget2);
		tour_info.Put_Field (zone1_field, zone1);
		tour_info.Put_Field (zone2_field, zone2);
		tour_info.Put_Field (loc1_field, location1);
		tour_info.Put_Field (loc2_field, location2);
		tour_info.Put_Field (purp1_field, purpose1);
		tour_info.Put_Field (purp2_field, purpose2);
	}

	//---- implement the appropriate method -----

	if (group_ptr->Method ()) {
		loc = Zone_Method (group_ptr, loc1, start_time, budget1, loc2, end_time, budget2, mode);
	} else {
		loc = Location_Method (group_ptr, loc1, start_time, budget1, loc2, end_time, budget2, mode);
	}
	if (loc == 0) {
		if (dump_flag) {
			Print (1, "A Location was Not Found");
		}
		Activity_Problem (LOCATION_PROBLEM, act_ptr);
		return;
	} else if (dump_flag) {
		Print (1, "Location %d was Selected", loc);
	}

	//---- save the location data ----

	loc1 = act_ptr->Location ();
	loc2 = 1;

	location_ptr = location_data [loc];

	mode = location_ptr->Mode ();
	drive_flag = (mode == DRIVE_ALONE);

	act_ptr->Location (loc);
	act_ptr->Mode (mode);

	//---- set other locations with the same code ----

	for (act_ptr = hhold_act_data.First (); act_ptr; act_ptr = hhold_act_data.Next ()) {
		if (act_ptr->Location () == loc1) {
			act_ptr->Location (loc);
		}
		if ((loc2 > start && loc2 < arrive) || (loc2 > leave && loc2 <= end)) {
			if (!drive_flag && act_ptr->Mode () == DRIVE_ALONE) {
				act_ptr->Mode (mode);
			}
		}
		loc2++;
	}
}
