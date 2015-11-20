//*********************************************************
//	Read_Activity.cpp - Read the Activity File
//*********************************************************

#include "Relocate.hpp"

//---------------------------------------------------------
//	Read_Activity
//---------------------------------------------------------

void Relocate::Read_Activity (int file_num)
{
	int hhold, person, trip, last_hh, num_out, num_update, num_hhold, location, new_location, last_hhold, last_loc, last_veh, veh;
	char buffer [FIELD_BUFFER];
	bool flag;

	Map_Data *map_ptr, set_rec;

	if (file_num > 0 && new_act_file->Extend ()) {
		if (!new_act_file->Open (file_num)) {
			Error ("Creating %s", new_act_file->File_Type ());
		}
	}

	//---- process each activity ----

	if (activity_file->Extend ()) {
		Show_Message ("Reading %s %s -- Record", activity_file->File_Type (), activity_file->Extension ());
	} else {
		Show_Message ("Reading %s -- Record", activity_file->File_Type ());
	}
	Set_Progress (10000);
	last_hh = num_update = num_out = num_hhold = last_hhold = last_loc = last_veh = veh = 0;

	while (activity_file->Read ()) {
		Show_Progress ();

		hhold = activity_file->Household ();
		if (hhold == 0) continue;
		
		person = activity_file->Person ();
		trip = activity_file->Activity ();

		if (zone_flag && hhold != last_hhold) {
			last_hhold = hhold;
			last_loc = last_veh = 0;
			old_new_map.Clear ();
		}
		flag = false;

		new_act_file->Copy_Fields (activity_file);

		location = activity_file->Location ();
		map_ptr = (Map_Data *) activity_map.Get (&location);

		if (map_ptr != NULL) {
			if (zone_flag) {
				new_location = Zone_Location (hhold, person, trip, location);

				veh = activity_file->Vehicle ();

				if (new_location != location) {
					if (veh > 0 && veh != last_veh && last_loc > 0) {
						map_ptr = (Map_Data *) loc_park.Get (&last_loc);

						if (map_ptr != NULL) {
							if (veh_map.Get_Index (&veh) == 0) {
								set_rec.old_id = veh;
								set_rec.new_id = map_ptr->new_id;
								if (!veh_map.Add (&set_rec)) {
									Error ("Adding Vehicle Record");
								}
								if (change_flag) {
									fprintf (change_file.File (), "%d\t%d\t%d\t%s\t%d\t%d\n", hhold, person, trip, "VEH", veh, map_ptr->new_id);
								}
							}
						}
					}
					last_loc = new_location;
				} else {
					last_loc = 0;
				}
				last_veh = veh;
			} else {
				new_location = map_ptr->new_id;
			}
			new_act_file->Location (new_location);
			flag = (new_location != location);
		}
		if (!new_act_file->Write ()) {
			Error ("Writing %s", new_act_file->File_Type ());
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
	if (activity_file->Extend ()) {
		Print (2, "Number of Activity Records Read from %s = %d", activity_file->Extension (), Progress_Count ());
	} else {
		Print (2, "Number of Activity Records Read = %d", Progress_Count ());
	}
	total_out += num_out;
	if (new_act_file->Extend ()) {
		Print (1, "Number of Activity Records Written to %s = %d", new_act_file->Extension (), num_out);
	} else {
		Print (1, "Number of Activity Records Written = %d", num_out);
	}
	total_update += num_update;
	Print (1, "Number of Activity Records Updated = %d", num_update);

	if (hhlist_flag) {
		total_hhold += num_hhold;
		Print (1, "Number of Updated Households = %d", num_hhold);
	}
}
