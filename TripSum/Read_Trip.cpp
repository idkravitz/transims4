//*********************************************************
//	Read_Trip.cpp - Read the Trip File
//*********************************************************

#include "TripSum.hpp"

#include "Trip_File.hpp"

//---------------------------------------------------------
//	Read_Trip
//---------------------------------------------------------

bool TripSum::Read_Trip (int file_num)
{
	int hhold, person, purpose, mode, start, end, origin, destination, vehicle;
	int prev_purp, last_hh, last_person, num_out, num_kept, count;

	Trip_File *trip_file, *new_file;
	Survey_Person *person_ptr;
	
	trip_file = (Trip_File *) Demand_Db_Base (TRIP);
		
	if (file_num > 0) {
		if (!split_flag) {
			if (!trip_file->Open (file_num)) return (false);
		} else {
			trip_file->Rewind ();
		}
		if (hhlist_flag && hhlist_file.Extend ()) {
			if (!Household_List (file_num)) {
				if (split_flag) return (false);
				Error ("Opening %s", hhlist_file.File_Type ());
			}
		}
	}
	if (output_flag) {
		new_file = (Trip_File *) Demand_Db_Base (NEW_TRIP);

		if (file_num > 0 && new_file->Extend ()) {
			if (!new_file->Open (file_num)) {
				Error ("Creating %s", new_file->File_Type ());
			}
		}
	}

	//---- process each trip ----

	if (trip_file->Extend ()) {
		Show_Message ("Reading %s %s -- Record", trip_file->File_Type (), trip_file->Extension ());
	} else {
		Show_Message ("Reading %s -- Record", trip_file->File_Type ());
	}
	Set_Progress (10000);
	last_hh = last_person = prev_purp = num_kept = num_out = 0;
	count = 1;

	while (trip_file->Read ()) {
		Show_Progress ();

		hhold = trip_file->Household ();
		if (hhold == 0) continue;

		if (hhlist_flag) {
			if (hhold_list.Get_Index (hhold) == 0) continue;
		}
		person = trip_file->Person ();
		if (hhold != last_hh || person != last_person) {
			if (count_flag) {
				person_ptr = person_count.Get (hhold, person);
				if (person_ptr == NULL) {
					count = 1;
				} else {
					count = person_ptr->Count ();
				}
			}
			last_hh = hhold;
			last_person = person;
			prev_purp = 0;
		}
		purpose = trip_file->Purpose ();
		mode = trip_file->Mode ();
		origin = trip_file->Origin ();
		destination = trip_file->Destination ();
		vehicle = trip_file->Vehicle ();

		start = time_periods.Step (trip_file->Start ());
		if (start < 0) {
			Error ("Converting Start Time %s for Household %d", trip_file->Start (), hhold);
		}

		end = time_periods.Step (trip_file->Arrive ());
		if (end < 0) {
			Error ("Converting Arrival Time %s for Household %d", trip_file->Arrive (), hhold);
		}
		if (Trip_Process (origin, destination, start, end, mode, purpose, prev_purp, vehicle, count)) {
			num_kept++;

			if (output_flag) {
				new_file->Copy_Fields (trip_file);

				if (!new_file->Write ()) {
					Error ("Writing %s", new_file->File_Type ());
				}
				num_out++;
			}
		}
		prev_purp = purpose;
	}
	End_Progress ();

	if (file_num == 0 || !split_flag) {
		total_in += Progress_Count ();
	}
	if (trip_file->Extend ()) {
		Print (2, "Number of Trip Records Read from %s = %d", trip_file->Extension (), Progress_Count ());
	} else {
		Print (2, "Number of Trip Records Read = %d", Progress_Count ());
	}
	if (output_flag) {
		total_out += num_out;
		if (new_file->Extend ()) {
			Print (1, "Number of Trip Records Written to %s = %d", new_file->Extension (), num_out);
		} else {
			Print (1, "Number of Trip Records Written = %d", num_out);
		}
	}
	total_used += num_kept;
	Print (1, "Number of Trips Used for Reports = %d", num_kept);
	return (true);
}
