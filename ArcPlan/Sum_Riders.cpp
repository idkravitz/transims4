//*********************************************************
//	Sum_Riders - sum the transit route between stops
//*********************************************************

#include "ArcPlan.hpp"

//---------------------------------------------------------
//	Sum_Riders
//---------------------------------------------------------

void ArcPlan::Sum_Riders (Offset_Data *org_ptr, Offset_Data *des_ptr)
{
	int i, route, num_legs, link_dir, dir, link;
	int start_link_dir, end_link_dir;
	bool board, alight;

	Link_Data *link_ptr;
	Line_Data *line_ptr;
	Driver_Data *driver_ptr;

	//---- get the route data ----

	route = plan_file.Route ();

	line_ptr = line_data.Get (route);

	if (line_ptr == NULL) {
		Warning ("Transit Route %d was Not Found", route);
		return;
	}
	driver_ptr = driver_data [line_ptr->Driver ()];

	num_legs = driver_ptr->Links ();

	//---- get the start and end link offsets ----

	start_link_dir = org_ptr->Link_Dir ();
	end_link_dir = des_ptr->Link_Dir ();

	board = alight = false;
	dir = 0;

	//---- increment the transit legs ----

	for (i=1; i <= num_legs; i++) {

		//---- check for board and alight link ----

		link_dir = driver_ptr->Link_Dir (i);

		if (!board && start_link_dir != link_dir) continue;
		if (end_link_dir == link_dir) alight = true;

		link = driver_ptr->Link (i);

		link_ptr = link_data.Get (link);

		if (link_ptr == NULL) {
			Warning ("Transit Route %d Link %d was Not Found", route, link);
			return;
		}
		dir = driver_ptr->Dir (i);

		if (dir == 0) {
			dir = link_ptr->AB_Dir ();
		} else {
			dir = link_ptr->BA_Dir ();
		}
		if (dir == 0) {
			Warning ("Route %d Link %d %s was Not Found", route, link, 
				((driver_ptr->Dir (i) == 0) ? "A->B" : "B->A"));
			return;
		}

		//---- sum the rider ----

		if (board) {
			load_data [dir]++;
		}
		board = true;
		if (alight == true) break;
	}
	if (board == false || alight == false) {
		Warning ("Stops %d to %d were Not Found on Route %d", org_ptr->ID (), des_ptr->ID (), route);
	}
}
