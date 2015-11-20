//*********************************************************
//	Set_Location.cpp - identify link-offset location codes
//*********************************************************

#include "Emissions.hpp"

#include "In_Polygon.hpp"

//---------------------------------------------------------
//	Set_Location
//---------------------------------------------------------

void Emissions::Set_Location (void)
{
	int i, j, nbox, half_box, node, dir, len, type, region;
	int xa, ya, xb, yb, x1, y1, x2, y2, x, y, zone;
	bool keep;

	Link_Data *link_ptr;
	Node_Data *node_ptr;
	Zone_Data *zone_ptr;
	Speed_Bin_Data *box_ptr, *bin_ptr;

	Show_Message ("Locating Link Offsets -- Record");
	Set_Progress (1000);

	half_box = box_length / 2;
	xa = ya = xb = yb = 0;
	keep = true;

	//---- process each link ----

	for (link_ptr = link_data.First (); link_ptr; link_ptr = link_data.Next ()) {
		Show_Progress ();

		//---- set the box index ----

		if (box_length > 0) {
			nbox = (link_ptr->Length () + (3 * box_length / 4)) / box_length;
		} else {
			nbox = 1;
		}

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
		}

		//---- process each direction ----

		for (i=0; i < 2; i++) {
			if (i == 0) {
				dir = link_ptr->AB_Dir ();
				x1 = xa;
				y1 = ya;
				x2 = xb;
				y2 = yb;
			} else {
				dir = link_ptr->BA_Dir ();
				x1 = xb;
				y1 = yb;
				x2 = xa;
				y2 = ya;
			}
			if (dir == 0) continue;
			if (link_eq_flag && link_list.Get_Index (dir) == 0) continue;

			x2 = x2 - x1;
			y2 = y2 - y1;

			//---- process each box ----

			for (j=1; j <= nbox; j++) {
				box_ptr = box_location.New_Record (true);
				if (box_ptr == NULL) goto mem_error;

				box_ptr->link_dir = dir;

				if (j == nbox) {
					box_ptr->offset = link_ptr->Length ();
				} else {
					box_ptr->offset = j * box_length;
				}
				type = 1;
				region = 0;
				keep = true;

				//---- calculate the box location ----

				if (zone_flag || location_flag || subarea_flag || select_flag) {
					len = box_ptr->offset - half_box;
					if (len < 0) {
						len = 0;
					} else if (len > link_ptr->Length ()) {
						len = link_ptr->Length ();
					}
					x = x1 + x2 * len / link_ptr->Length ();
					y = y1 + y2 * len / link_ptr->Length ();

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
				}

				//---- box location ----

				box_ptr->bin [0] = (region << 8) + type;

				if (!box_location.Add ()) goto mem_error;

				//---- add the speed bin data ----

				if (link_flag && keep) {
					bin_ptr = speed_bin_data.New_Record (true);

					bin_ptr->link_dir = box_ptr->link_dir;
					bin_ptr->offset = box_ptr->offset;

					if (!speed_bin_data.Add ()) goto mem_error;
				}
			}
		}
	}
	End_Progress ();
	return;

mem_error:
	Error ("Insufficient Space for Speed Box Data");
}
