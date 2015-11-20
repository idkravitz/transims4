//*********************************************************
//	Set_Zone.cpp - assign a zone to each node
//*********************************************************

#include "DynusTPlan.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Set_Zone
//---------------------------------------------------------

void DynusTPlan::Set_Zone (void)
{
	int *node_zone_ptr, zone, nzone;
	double dx, dy, diff, min_diff;

	Node_Data *node_ptr;
	Zone_Data *zone_ptr;

	if (!node_zone.Num_Records (node_data.Num_Records ())) {
		Error ("Allocating Node Zone Data");
	}

	//---- initialize the zone map ----

	nzone = 0;

	for (zone_ptr = zone_data.First (); zone_ptr; zone_ptr = zone_data.Next ()) {
		zone_ptr->Area_Type (0);
	}

	//---- write the node records ----

	for (node_ptr = node_data.First (); node_ptr; node_ptr = node_data.Next ()) {
		zone = 0;
		min_diff = -1.0;

		for (zone_ptr = zone_data.First (); zone_ptr; zone_ptr = zone_data.Next ()) {
			dx = zone_ptr->X () - node_ptr->X ();
			dy = zone_ptr->Y () - node_ptr->Y ();

			diff = dx * dx + dy * dy;
			if (diff < min_diff || min_diff < 0) {
				min_diff = diff;
				zone = zone_data.Record_Index ();
			}
		}
		if (zone > 0) {
			zone_ptr = zone_data [zone];

			if (zone_ptr->Area_Type () == 0) {
				nzone++;
				zone_ptr->Area_Type (nzone);
				zone = nzone;
			} else {
				zone = zone_ptr->Area_Type ();
			}
			node_zone_ptr = node_zone.Record (node_data.Record_Index ());
			*node_zone_ptr = zone;
		}
	}
}
