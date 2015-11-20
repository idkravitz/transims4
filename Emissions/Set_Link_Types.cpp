//*********************************************************
//	Set_Link_Types.cpp - identify link location types
//*********************************************************

#include "Emissions.hpp"

#include "In_Polygon.hpp"

//---------------------------------------------------------
//	Set_Link_Types
//---------------------------------------------------------

void Emissions::Set_Link_Types (void)
{
	int node, type, region;
	int xa, ya, xb, yb, x, y, zone;
	bool keep;

	Link_Data *link_ptr;
	Node_Data *node_ptr;
	Zone_Data *zone_ptr;

	Show_Message ("Setting Link Location Codes -- Record");
	Set_Progress (1000);

	xa = ya = xb = yb = 0;
	keep = true;

	//---- process each link ----

	for (link_ptr = link_data.First (); link_ptr; link_ptr = link_data.Next ()) {
		Show_Progress ();

		link_ptr->Use (0);
		link_ptr->Aoffset (1);
		link_ptr->Boffset (0);

		if (link_eq_flag) {
			keep = false;
			if (link_ptr->AB_Dir () > 0) {
				if (link_list.Get_Index (link_ptr->AB_Dir ()) != 0) keep = true;
			}
			if (link_ptr->BA_Dir () > 0) {
				if (link_list.Get_Index (link_ptr->BA_Dir ()) != 0) keep = true;
			}
			if (!keep) continue;
		}
		
		type = 1;
		region = 0;
		keep = true;

		//---- get the coordinates ----

		if (zone_flag || location_flag || subarea_flag || select_flag) {
			node = link_ptr->Anode ();

			node_ptr = node_data.Get (node);

			xa = node_ptr->X ();
			ya = node_ptr->Y ();

			node = link_ptr->Bnode ();

			node_ptr = node_data.Get (node);

			xb = node_ptr->X ();
			yb = node_ptr->Y ();

			x = (xa + xb) / 2;
			y = (ya + yb) / 2;

			//---- check the zone range ----

			if (zone_flag || location_flag) {
				zone = Closest_Zone (x, y);

				if (select_flag) {
					if (!select_zone.In_Range (zone)) {
						type = region = 0;
						keep = false;
					}
				}
			}

			//---- check the subarea polygon ----

			if (type == 1 && subarea_flag) {
				if (!In_Polygon (UnRound (x), UnRound (y), &select_subarea.points)) {
					type = region = 0;
					keep = false;
				}
			}

			//---- set the area type ----

			if (type == 1 && (zone_flag || location_flag)) {
				if (at_equiv_flag) {
					type = zone_equiv.Zone_Group (zone);
				} else if (zone_flag) {
					zone_ptr = zone_data.Get (zone);
					if (zone_ptr != NULL) {
						type = zone_ptr->Area_Type ();
					} else {
						type = 0;
					}
				}
				if (type == 0) type = 1;

				if (reg_equiv_flag) {
					region = region_equiv.Zone_Group (zone);
				} else {
					region = 0;
				}
			}

			if (keep) {
				link_ptr->Use (1);
				link_ptr->Aoffset (type);
				link_ptr->Boffset (region);
			}
		}
	}
	End_Progress ();
}
