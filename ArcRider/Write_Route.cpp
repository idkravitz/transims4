//*********************************************************
//	Write_Route - draw the transit line demand
//*********************************************************

#include "ArcRider.hpp"

//---------------------------------------------------------
//	Write_Route
//---------------------------------------------------------

void ArcRider::Write_Route (void)
{
	int i, j, num_links, num, link, link_dir, dir, index1, index2;
	int load, riders, max_load, on, off, board, alight, stop, run, runs, schedule;
	int route_field, name_field, mode_field, stops_field, runs_field, riders_field;
	int load_field, board_field, alight_field;
	double offset, length, link_len, side;
	bool offset_flag, route_flag, tod_flag, link_flag, select;

	Link_Data *link_ptr;
	Rider_Data *rider_ptr;
	Driver_Data *driver_ptr;
	Data_Range *range;

	tod_flag = (times.Num_Ranges () > 0);	
	route_flag = (transit_routes.Num_Ranges () > 0);
	link_flag = (links.Num_Records () > 0);
	offset_flag = (route_offset != 0.0);
	side = 0.0;

	route_field = arcview_line.Required_Field ("ROUTE");
	name_field = arcview_line.Optional_Field ("NAME");
	mode_field = arcview_line.Optional_Field ("MODE");
	stops_field = arcview_line.Optional_Field ("STOPS");
	runs_field = arcview_line.Optional_Field ("RUNS");
	riders_field = arcview_line.Optional_Field ("RIDERS");
	load_field = arcview_line.Optional_Field ("MAX_LOAD");
	board_field = arcview_line.Optional_Field ("MAX_BOARD");
	alight_field = arcview_line.Optional_Field ("MAX_ALIGHT");

	Show_Message ("Writing %s -- Record", arcview_line.File_Type ());
	Set_Progress (10000);

	//---- get the route data ----

	for (rider_ptr = rider_data.First_Key (); rider_ptr; rider_ptr = rider_data.Next_Key ()) {
		Show_Progress ();

		if (mode_flag && !travel_modes [rider_ptr->Mode ()]) continue;
		if (route_flag && !transit_routes.In_Range (rider_ptr->Route ())) continue;

		//---- check the link criteria ----

		driver_ptr = driver_data [rider_ptr->Driver ()];

		num_links = driver_ptr->Links ();

		if (link_flag) {

			//---- check the link selection criteria ----

			select = false;

			for (range = (Data_Range *) links.First (); range && !select; range = (Data_Range *) links.Next ()) {

				num = range->Num_Ranges ();
				index1 = index2 = 0;

				for (i=1; i <= num_links; i++) {
					link = driver_ptr->Link (i);

					index2 = range->In_Index (link);
					if (!index2) continue;

					if (index2 != index1 + 1) {
						index2 = 0;
						break;
					}
					if (index2 == num) {
						select = true;
						break;
					}
					index1 = index2;
				}
			}
			if (!select) continue;
		}

		//---- save the route ridership data ----

		riders = board = alight = max_load = runs = 0;

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

			load = 0;

			for (stop=1; stop <= rider_ptr->Stops (); stop++) {
				on = rider_ptr->Board (run, stop);
				if (on > board) board = on;

				off = rider_ptr->Alight (run, stop);
				if (off > alight) alight = off;

				load += on - off;
				if (load > max_load) max_load = load;

				riders += on;
			}
		}
		if (riders == 0) continue;

		//---- save the data to the file ----

		arcview_line.Put_Field (route_field, rider_ptr->Route ());
		arcview_line.Put_Field (name_field, (char *) rider_ptr->Name ());
		arcview_line.Put_Field (mode_field, Transit_Code ((Transit_Type) rider_ptr->Mode ()));
		arcview_line.Put_Field (stops_field, rider_ptr->Stops ());
		arcview_line.Put_Field (runs_field, runs);
		arcview_line.Put_Field (riders_field, riders);
		arcview_line.Put_Field (load_field, max_load);
		arcview_line.Put_Field (board_field, board);
		arcview_line.Put_Field (alight_field, alight);
				
		arcview_line.points.Reset ();

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
				if (!arcview_line.points.Add (points [j])) goto point_error;
			}
		}

		if (!arcview_line.Write_Record ()) {
			Error ("Writing %s", arcview_line.File_Type ());
		}
		num_line++;
	}
	End_Progress ();

	arcview_line.Close ();
	return;

point_error:
	Error ("Insufficient Memory for %d Points", arcview_line.points.Num_Points ());

}
