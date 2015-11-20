//*********************************************************
//	Write_Sum - draw the transit line groups
//*********************************************************

#include "ArcRider.hpp"

//---------------------------------------------------------
//	Write_Sum
//---------------------------------------------------------

void ArcRider::Write_Sum (void)
{
	int i, j, num_links, num, link_dir, dir, route, routes, tot_on;
	int load, riders, max_load, on, off, board, alight, stop, stops, run, runs, schedule;
	int group_field, name_field, routes_field, stops_field, runs_field, riders_field;
	int load_field, board_field, alight_field;
	double offset, length, link_len, side;
	bool offset_flag, tod_flag;

	Link_Data *link_ptr;
	Rider_Data *rider_ptr;
	Driver_Data *driver_ptr;
	Integer_List *group;

	tod_flag = (times.Num_Ranges () > 0);	
	offset_flag = (route_offset != 0.0);
	side = 0.0;

	group_field = arcview_sum.Required_Field ("GROUP");
	name_field = arcview_sum.Optional_Field ("NAME");
	routes_field = arcview_sum.Optional_Field ("ROUTES");
	stops_field = arcview_sum.Optional_Field ("STOPS");
	runs_field = arcview_sum.Optional_Field ("RUNS");
	riders_field = arcview_sum.Optional_Field ("RIDERS");
	load_field = arcview_sum.Optional_Field ("MAX_LOAD");
	board_field = arcview_sum.Optional_Field ("MAX_BOARD");
	alight_field = arcview_sum.Optional_Field ("MAX_ALIGHT");

	Show_Message ("Writing %s -- Record", arcview_sum.File_Type ());
	Set_Progress (10);

	//---- process each line group ----

	for (num=1; num <= line_equiv.Num_Groups (); num++) {

		group = line_equiv.Group_List (num);
		if (group == NULL) continue;

		Show_Progress ();

		arcview_sum.Put_Field (group_field, num);
		arcview_sum.Put_Field (name_field, line_equiv.Group_Label (num));
		arcview_sum.points.Reset ();
		arcview_sum.parts.Reset ();

		riders = board = alight = max_load = runs = stops = routes = 0;

		//---- sum the lines in the group ----

		for (route = group->First (); route != 0; route = group->Next ()) {

			rider_ptr = rider_data.Get (route);
			if (rider_ptr == NULL) continue;

			//---- check the link criteria ----

			driver_ptr = driver_data [rider_ptr->Driver ()];

			num_links = driver_ptr->Links ();
			stops += rider_ptr->Stops ();

			//---- save the route ridership data ----

			for (run=1; run <= rider_ptr->Runs (); run++) {

				//---- check the time of day criteria ----

				if (tod_flag) {
					schedule = rider_ptr->Schedule (run, 1);
					schedule += rider_ptr->Schedule (run, rider_ptr->Stops ());
					schedule = Resolve ((schedule + 1) / 2);

					if (!times.In_Range (schedule)) continue;
				}
				runs++;

				//---- process the stop data ----

				load = tot_on = 0;

				for (stop=1; stop <= rider_ptr->Stops (); stop++) {
					on = rider_ptr->Board (run, stop);
					if (on > board) board = on;

					off = rider_ptr->Alight (run, stop);
					if (off > alight) alight = off;

					load += on - off;
					if (load > max_load) max_load = load;

					tot_on += on;
					riders += on;
				}
			}
			if (tot_on == 0) continue;

			routes++;

			if (!arcview_sum.parts.Add (arcview_sum.points.Num_Points ())) goto point_error;

			//---- create transit path ----

			for (i=1; i <= num_links; i++) {
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
				if (offset_flag) {
					if (link_ptr->AB_Dir () > 0 && link_ptr->BA_Dir () > 0) {
						side = route_offset;
					} else {
						side = 0.0;
					}
				}
				Link_Shape (link_ptr, dir, &points, offset, length, side);

				//---- add the link to the route ----

				if (side == 0.0 && i > 1) {
					j = 2;
				} else {
					j = 1;
				}
				for (; j <= points.Num_Points (); j++) {
					if (!arcview_sum.points.Add (points [j])) goto point_error;
				}
			}
		}

		//---- save the data to the file ----

		if (routes > 0) {
			arcview_sum.Put_Field (routes_field, routes);
			arcview_sum.Put_Field (stops_field, stops);
			arcview_sum.Put_Field (runs_field, runs);
			arcview_sum.Put_Field (riders_field, riders);
			arcview_sum.Put_Field (load_field, max_load);
			arcview_sum.Put_Field (board_field, board);
			arcview_sum.Put_Field (alight_field, alight);

			if (!arcview_sum.Write_Record ()) {
				Error ("Writing %s", arcview_sum.File_Type ());
			}
			num_sum++;
		}
	}
	End_Progress ();

	arcview_sum.Close ();
	return;

point_error:
	Error ("Insufficient Memory for %d Points", arcview_sum.points.Num_Points ());

}
