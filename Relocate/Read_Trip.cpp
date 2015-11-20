//*********************************************************
//	Read_Trip.cpp - Read the Trip File
//*********************************************************

#include "Relocate.hpp"

//---------------------------------------------------------
//	Read_Trip
//---------------------------------------------------------

void Relocate::Read_Trip (int file_num)
{
	int hhold, person, trip, last_hh, num_out, num_update, num_hhold, location, last_hhold, new_location;
	char buffer [FIELD_BUFFER];
	bool flag;

	Map_Data *map_ptr;

	if (file_num > 0 && new_trip_file->Extend ()) {
		if (!new_trip_file->Open (file_num)) {
			Error ("Creating %s", new_trip_file->File_Type ());
		}
	}

	//---- process each trip ----

	if (trip_file->Extend ()) {
		Show_Message ("Reading %s %s -- Record", trip_file->File_Type (), trip_file->Extension ());
	} else {
		Show_Message ("Reading %s -- Record", trip_file->File_Type ());
	}
	Set_Progress (10000);

	last_hhold = last_hh = num_update = num_out = num_hhold = 0;

	while (trip_file->Read ()) {
		Show_Progress ();

		hhold = trip_file->Household ();
		if (hhold == 0) continue;

		person = trip_file->Person ();
		trip = trip_file->Trip ();

		if (zone_flag && hhold != last_hhold) {
			last_hhold = hhold;
			old_new_map.Clear ();
		}
		flag = false;

		new_trip_file->Copy_Fields (trip_file);

		location = new_trip_file->Origin ();

		map_ptr = (Map_Data *) activity_map.Get (&location);

		if (map_ptr != NULL) {
			if (zone_flag) {
				new_location = Zone_Location (hhold, person, trip, location, trip_file->Vehicle (), true);
			} else {
				new_location = map_ptr->new_id;
			}
			new_trip_file->Origin (new_location);
			flag = (new_location != location);
		}
		location = new_trip_file->Destination ();

		map_ptr = (Map_Data *) activity_map.Get (&location);

		if (map_ptr != NULL) {
			if (zone_flag) {
				new_location = Zone_Location (hhold, person, trip, location);
			} else {
				new_location = map_ptr->new_id;
			}
			new_trip_file->Destination (new_location);
			flag = flag || (new_location != location);
		}
		if (!new_trip_file->Write ()) {
			Error ("Writing %s", new_trip_file->File_Type ());
		}
		num_out++;

		if (flag) {
			num_update++;

			if (hhold != last_hh) {
				last_hh = hhold;
				num_hhold++;

				if (hhlist_flag) {
					str_fmt (buffer, sizeof (buffer), "%d", hhold);
					if (!hhlist_file.Write (buffer)) {
						Error ("Writing %s", hhlist_file.File_Type ());
					}
				}
			}
		}
	}
	End_Progress ();

	total_in += Progress_Count ();
	if (trip_file->Extend ()) {
		Print (2, "Number of Trip Records Read from %s = %d", trip_file->Extension (), Progress_Count ());
	} else {
		Print (2, "Number of Trip Records Read = %d", Progress_Count ());
	}
	total_out += num_out;
	if (new_trip_file->Extend ()) {
		Print (1, "Number of Trip Records Written to %s = %d", new_trip_file->Extension (), num_out);
	} else {
		Print (1, "Number of Trip Records Written = %d", num_out);
	}
	total_update += num_update;
	Print (1, "Number of Trip Records Updated = %d", num_update);

	if (hhlist_flag) {
		total_hhold += num_hhold;
		Print (1, "Number of Updated Households = %d", num_hhold);
	}
}
