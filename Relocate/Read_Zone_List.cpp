//*********************************************************
//	Read_Zone_List.cpp - Read the Zone List
//*********************************************************

#include "Relocate.hpp"

//---------------------------------------------------------
//	Read_Zone_List
//---------------------------------------------------------

void Relocate::Read_Zone_List (void)
{
	int zone;
	double total;
	char *str_ptr;

	Zone_Weight wt_rec, *wt_ptr;
	Location_Data *loc_ptr;

	//---- process the zone list ----

	Show_Message ("Reading %s -- Record", zone_file.File_Type ());
	Set_Progress (1000);

	while (zone_file.Read ()) {
		Show_Progress ();

		str_ptr = zone_file.Record ();

		Get_Integer (str_ptr, &zone);

		if (zone <= 0) continue;

		if (!zone_list.Add (zone)) {
			Error ("Adding %d to Zone List", zone);
		}
	}
	End_Progress ();

	Print (2, "Number of Zone List Records Read = %d", Progress_Count ());

	if (Progress_Count () != zone_list.Num_Records ()) {
		Print (1, "Number of Zone List Records Kept = %d", zone_list.Num_Records ());
	}
	zone_file.Close ();

	//---- calculate the zone probabilities ----

	for (loc_ptr = location_data.First (); loc_ptr; loc_ptr = location_data.Next ()) {
		wt_rec.zone = loc_ptr->Zone ();
		wt_rec.new_id = loc_ptr->Location ();
		wt_rec.weight = 1.0;

		if (!zone_weight.Add (&wt_rec)) {
			Error ("Adding Location Weight");
		}
	}

	//---- count the number of locations in the zone ----

	zone = 0;
	total = 0;

	for (wt_ptr = (Zone_Weight *) zone_weight.First_Key (); wt_ptr; wt_ptr = (Zone_Weight *) zone_weight.Next_Key ()) {
		if (wt_ptr->zone == zone) {
			total += wt_ptr->weight;
		} else {
			if (zone != 0 && total > 0) {
				wt_rec.zone = zone;
				wt_rec.new_id = 0;

				for (wt_ptr = (Zone_Weight *) zone_weight.Get_GE (&wt_rec); wt_ptr; wt_ptr = (Zone_Weight *) zone_weight.Next_Key ()) {
					if (wt_ptr->zone != zone) break;
					wt_ptr->weight /= total;
				}
			}
			zone = wt_ptr->zone;
			total = wt_ptr->weight;
		}
	}
	if (zone != 0 && total > 0) {
		wt_rec.zone = zone;
		wt_rec.new_id = 0;

		for (wt_ptr = (Zone_Weight *) zone_weight.Get_GE (&wt_rec); wt_ptr; wt_ptr = (Zone_Weight *) zone_weight.Next_Key ()) {
			if (wt_ptr->zone != zone) break;
			wt_ptr->weight /= total;
		}
	}
}
