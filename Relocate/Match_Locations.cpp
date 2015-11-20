//*********************************************************
//	Match_Locations.cpp - map activity locations
//*********************************************************

#include "Relocate.hpp"

//---------------------------------------------------------
//	Match_Locations
//---------------------------------------------------------

void Relocate::Match_Locations (void)
{
	int location, x, y, best_loc, xmin, xmax, ymin, ymax, type, zone;
	double dx, dy, best_dist;

	Map_Data map_rec, *map_ptr, park_rec;

	Location_Data *loc_ptr;
	Access_Data *acc_ptr;

	if (!activity_map.Max_Records (old_loc_file.Num_Records ())) {
		Error ("Insufficient Memory for Old Activity Locations");
	}
	Show_Message ("Reading %s -- Record", old_loc_file.File_Type ());
	Set_Progress (1000);	

	while (old_loc_file.Read ()) {
		Show_Progress ();

		map_rec.old_id = location = old_loc_file.Location ();
		zone = old_loc_file.Zone ();
		x = Round (old_loc_file.X ());
		y = Round (old_loc_file.Y ());
		
		xmin = x - MIN_DIFF;
		xmax = x + MIN_DIFF;
		ymin = y - MIN_DIFF;
		ymax = y + MIN_DIFF;

		loc_ptr = location_data.Get (location);

		if (loc_ptr != NULL && loc_ptr->X () <= xmax && loc_ptr->X () >= xmin && 
			loc_ptr->Y () <= ymax && loc_ptr->Y () >= ymin) {

			map_rec.new_id = location;
		} else {
			best_loc = 0;
			best_dist = 0.0;

			for (loc_ptr = location_data.First (); loc_ptr; loc_ptr = location_data.Next ()) {
				if (zone > 0 && loc_ptr->Zone () == 0) continue;

				dx = (double) loc_ptr->X () - x;
				dy = (double) loc_ptr->Y () - y;

				dx = dx * dx + dy * dy;

				if (best_loc == 0 || best_dist > dx) {
					best_loc = loc_ptr->Location ();
					best_dist = dx;
					if (best_dist == 0.0) break;
				}
			}
			map_rec.new_id = best_loc;
		}
		if (!activity_map.Add (&map_rec) || !loc_zone.Add (zone)) {
			Error ("Adding Activity Map Record");
		}
	}
	End_Progress ();

	Print (2, "Number of %s Records = %d", old_loc_file.File_Type (), Progress_Count ());

	if (loc_map_flag) {
		for (map_ptr = (Map_Data *) activity_map.First_Key (); map_ptr; map_ptr = (Map_Data *) activity_map.Next_Key ()) {
			new_loc_file.Put_Field (1, map_ptr->old_id);
			new_loc_file.Put_Field (2, map_ptr->new_id);

			if (!new_loc_file.Write ()) {
				Error ("Writing %s", new_loc_file.File_Type ());
			}
		}
		new_loc_file.Close ();
	}

	if (!vehicle_flag && !plan_flag) return;

	//---- match parking lots ----

	if (!parking_map.Max_Records (old_acc_file.Num_Records () / 2)) {
		Error ("Insufficient Memory for Old Parking Locations");
	}

	Show_Message ("Reading %s -- Record", old_acc_file.File_Type ());
	Set_Progress (1000);	

	while (old_acc_file.Read ()) {
		Show_Progress ();

		type = End_Code (old_acc_file.To_Type ());
		if (type != PARKING_ID) continue;

		type = End_Code (old_acc_file.From_Type ());
		if (type != LOCATION_ID) continue;

		location = old_acc_file.From_ID ();

		map_ptr = (Map_Data *) activity_map.Get (&location);
		if (map_ptr == NULL || map_ptr->new_id == 0) {
			Warning ("Activity Location %d was Not Mapped to a New Location", location);
			continue;
		}
		park_rec.old_id = old_acc_file.To_ID ();

		if (map_ptr->old_id == map_ptr->new_id) {
			park_rec.new_id = park_rec.old_id;
		} else {
			park_rec.new_id = 0;

			for (acc_ptr = access_data.First (); acc_ptr; acc_ptr = access_data.Next ()) {
				if (acc_ptr->From_Type () == LOCATION_ID && 
					acc_ptr->From_ID () == map_ptr->new_id &&
					acc_ptr->To_Type () == PARKING_ID) {

					park_rec.new_id = acc_ptr->To_ID ();
					break;
				}
			}
			if (park_rec.new_id == 0) {
				Warning ("A Parking Lot was Not Connected to New Location %d", map_ptr->new_id);
			}
		}
		if (!parking_map.Add (&park_rec)) {
			Error ("Adding Activity Map Record");
		}
	}
	End_Progress ();
	
	Print (2, "Number of %s Records = %d", old_acc_file.File_Type (), Progress_Count ());

	if (park_map_flag) {
		for (map_ptr = (Map_Data *) parking_map.First_Key (); map_ptr; map_ptr = (Map_Data *) parking_map.Next_Key ()) {
			new_park_file.Put_Field (1, map_ptr->old_id);
			new_park_file.Put_Field (2, map_ptr->new_id);

			if (!new_park_file.Write ()) {
				Error ("Writing %s", new_park_file.File_Type ());
			}
		}
		new_park_file.Close ();
	}

	//---- new location to parking ----

	if (zone_flag) {
		for (acc_ptr = access_data.First (); acc_ptr; acc_ptr = access_data.Next ()) {
			if (acc_ptr->From_Type () == LOCATION_ID && 
				acc_ptr->To_Type () == PARKING_ID) {
				location = acc_ptr->From_ID ();

				if (loc_park.Get_Index (&location) == 0) {
					park_rec.old_id = location;
					park_rec.new_id = acc_ptr->To_ID ();

					if (!loc_park.Add (&park_rec)) {
						Error ("Adding Parking Location");
					}
				}
			}
		}
	}
}
