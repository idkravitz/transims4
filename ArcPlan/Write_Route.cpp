//*********************************************************
//	Write_Route - draw the transit route between stops
//*********************************************************

#include "ArcPlan.hpp"

//---------------------------------------------------------
//	Write_Route
//---------------------------------------------------------

void ArcPlan::Write_Route (Offset_Data *org_ptr, Offset_Data *des_ptr)
{
	int i, j, route, num_legs, stop, link_dir, dir;
	int start_link_dir, end_link_dir;
	double offset, length, link_len, side;
	bool offset_flag, board, alight;

	Link_Data *link_ptr;
	Line_Data *line_ptr;
	Driver_Data *driver_ptr;
	Offset_Data *stop_ptr;

	offset_flag = (route_offset != 0.0);
	side = 0.0;

	//---- get the route data ----

	route = plan_file.Route ();

	line_ptr = line_data.Get (route);

	if (line_ptr == NULL) {
		Warning ("Transit Route %d was Not Found", route);
		return;
	}
	board = alight = false;

	//---- create the route shape ----

	if (driver_flag) {

		//---- get the start and end link offsets ----

		start_link_dir = org_ptr->Link_Dir ();
		end_link_dir = des_ptr->Link_Dir ();
	
		//---- get the route links ----

		driver_ptr = driver_data [line_ptr->Driver ()];

		num_legs = driver_ptr->Links ();

		//---- create transit legs ----

		for (i=1; i <= num_legs; i++) {

			dir = driver_ptr->Dir (i);
			link_dir = driver_ptr->Link_Dir (i);

			link_ptr = link_data.Get (driver_ptr->Link (i));

			link_len = UnRound (link_ptr->Length ());

			length = link_len - 2 * route_offset;

			if (dir == 0) {
				offset = route_offset;
			} else {
				offset = link_len - route_offset;
			}

			if (!board) {
				if (start_link_dir != link_dir) continue;

				offset = org_ptr->Offset ();

				if (dir == 0) {
					length = link_len - offset - route_offset;
				} else {
					length = offset - route_offset;
				}
			}

			if (end_link_dir == link_dir) {
				alight = true;

				if (dir == 0) {
					length = des_ptr->Offset () - offset;
				} else {
					length = offset - des_ptr->Offset ();
				}
			}

			if (offset_flag) {
				if (link_ptr->AB_Dir () > 0 && link_ptr->BA_Dir () > 0) {
					side = route_offset;
				} else {
					side = 0.0;
				}
			}

			Link_Shape (link_ptr, dir, &points, offset, length, side);

			//---- draw the link segment ----

			if (side == 0.0 && !board) {
				j = 2;
			} else {
				j = 1;
			}
			for (; j <= points.Num_Points (); j++) {
				if (!arcview_plan.points.Add (points [j])) goto point_error;
			}
			board = true;
			if (alight == true) break;
		}

	} else {

		for (i=1; i <= line_ptr->Stops (); i++) {
			stop = line_ptr->Stop (i);

			if (!board) {
				if (org_ptr->ID () != stop) continue;
				board = true;
				stop_ptr = org_ptr;
			} else {
				stop_ptr = stop_offset.Get (stop);
				if (stop_ptr == NULL) goto stop_error;
			}
			link_ptr = link_data.Get (stop_ptr->Link ());

			offset = stop_ptr->Offset ();

			if (offset_flag) {
				if (link_ptr->AB_Dir () > 0 && link_ptr->BA_Dir () > 0) {
					side = route_offset;
				} else {
					side = 0.0;
				}
			}
			Link_Shape (link_ptr, stop_ptr->Dir (), &points, offset, 0.0, side);

			if (!arcview_plan.points.Add (points [1])) goto point_error;

			if (des_ptr->ID () == stop) {
				alight = true;
				break;
			}
		}
	}
	if (board == false || alight == false) {
		Warning ("Stops %d to %d were Not Found on Route %d", org_ptr->ID (), des_ptr->ID (), route);
	}
	return;

point_error:
	Error ("Insufficient Memory for %d Points", arcview_plan.points.Num_Points ());

stop_error:
	Warning ("Route %d Stop %d was Not Found in the Stop File", line_ptr->Route (), stop);

}
