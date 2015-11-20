//*********************************************************
//	Zone_Location.cpp - Reallocate a Zone Location
//*********************************************************

#include "Relocate.hpp"

//---------------------------------------------------------
//	Zone_Location
//---------------------------------------------------------

int Relocate::Zone_Location (int hhold, int person, int trip, int location, int vehicle, bool org_flag)
{
	int zone, new_location;
	double prob, cum;

	Map_Data *map_ptr, *set_ptr, set_rec;
	Zone_Weight *wt_ptr, wt_rec;

	new_location = location;

	set_ptr = (Map_Data *) old_new_map.Get (&location);

	if (set_ptr != NULL) {
		new_location = set_ptr->new_id;
	} else {
		zone = loc_zone [activity_map.Get_Index (&location)];

		if (zone_list.Get_Index (zone)) {
			prob = random.Probability ();

			cum = 0;
			wt_rec.zone = zone;
			wt_rec.new_id = 0;

			for (wt_ptr = (Zone_Weight *) zone_weight.Get_GE (&wt_rec); wt_ptr; wt_ptr = (Zone_Weight *) zone_weight.Next_Key ()) {
				cum += wt_ptr->weight;

				if (prob < cum) break;
			}
			new_location = wt_ptr->new_id;

			set_rec.old_id = location;
			set_rec.new_id = new_location;
			old_new_map.Add (&set_rec);
		}
	}
	if (location != new_location) {
		if (change_flag) {
			fprintf (change_file.File (), "%d\t%d\t%d\t%s\t%d\t%d\n", hhold, person, trip, ((org_flag) ? "ORG" : "DES"), location, new_location);
		}
		if (org_flag && vehicle > 0) {
			map_ptr = (Map_Data *) loc_park.Get (&new_location);

			if (map_ptr != NULL) {
				if (veh_map.Get_Index (&vehicle) == 0) {
					set_rec.old_id = vehicle;
					set_rec.new_id = map_ptr->new_id;
					if (!veh_map.Add (&set_rec)) {
						Error ("Adding Vehicle Record");
					}
					if (change_flag) {
						fprintf (change_file.File (), "%d\t%d\t%d\t%s\t%d\t%d\n", hhold, person, trip, "VEH", vehicle, map_ptr->new_id);
					}
				}
			}
		}
	}
	return (new_location);
}
