//*********************************************************
//	Write_Route - write a transit route file
//*********************************************************

#include "ArcNet.hpp"

//---------------------------------------------------------
//	Write_Route
//---------------------------------------------------------

void ArcNet::Write_Route (void)
{
	int i, j, k, num_legs, stop, link_dir, dir, runs, time, overlap, t1, t2;
	int route_field, stops_field, mode_field, name_field, notes_field;
	double offset, length, stop_off, link_len, side, side_offset;
	bool offset_flag;

	Db_Base *file;

	char *stop_error = "Route %d Stop %d was Not Found in the Stop File";
	char *path_error = "Route %d Stop %d was Not Found on Driver Path";

	file = Network_Db_Base (TRANSIT_ROUTE);

	route_field = file->Required_Field ("ROUTE", "LINE", "ROUTE_ID", "LINE_ID");
	stops_field = file->Required_Field ("STOPS", "NSTOPS", "NUM_STOPS");
	mode_field = file->Required_Field ("MODE", "TYPE");
	name_field = file->Optional_Field ("NAME", "ROUTE_NAME", "RTNAME", "DESCRIPTION");
	notes_field = file->Optional_Field ("NOTES");

	Link_Data *link_ptr;
	Line_Data *line_ptr;
	Offset_Data *stop_ptr;
	Driver_Data *driver_ptr;
	XYZ_Point point;
	
	Show_Message ("Writing Transit Route -- Record");
	Set_Progress (1000);

	offset_flag = (route_offset != 0.0);
	side = 0.0;

	//---- set the overlap count ----

	if (!overlap_flag) {
		for (link_ptr = link_data.First (); link_ptr; link_ptr = link_data.Next ()) {
			link_ptr->Aoffset (0);
			link_ptr->Boffset (0);
		}
	}

	//---- process each transit route ----

	for (line_ptr = line_data.First (); line_ptr != NULL; line_ptr = line_data.Next ()) {
		Show_Progress ();

		arcview_route.Put_Field (route_field, line_ptr->Route ());
		arcview_route.Put_Field (mode_field, Transit_Code ((Transit_Type) line_ptr->Mode ()));
		arcview_route.Put_Field (stops_field, line_ptr->Stops ());
		arcview_route.Put_Field (name_field, (char *) line_ptr->Name ());
		arcview_route.Put_Field (notes_field, (char *) line_ptr->Notes ());

		arcview_route.Put_Field ("NUM_RUNS", line_ptr->Runs ());

		//---- save the number of runs in each period ----

		if (schedule_flag) {
			if (select_time) {
				for (i=1; i <= line_ptr->Runs (); i++) {
					t1 = Resolve (line_ptr->Schedule (i, 1));
					t2 = Resolve (line_ptr->Schedule (i, line_ptr->Stops ()));

					if (time_range.Span_Range (t1, t2)) break;
				}
				if (i > line_ptr->Runs ()) continue;
			}
			for (i=j=1; i <= time_period.Num_Ranges (); i++) {
				runs = 0;

				for (; j <= line_ptr->Runs (); j++) {
					time = Resolve (line_ptr->Schedule (j, 1));

					if (time_period.In_Index (time) != i) break;
					runs++;
				}
				arcview_route.Put_Field (run_field + i, runs);
			}
		}

		//---- create the route shape ----

		arcview_route.points.Reset ();

		if (driver_flag) {
			driver_ptr = driver_data [line_ptr->Driver ()];

			arcview_route.Put_Field ("NUM_LINKS", driver_ptr->Links ());
			arcview_route.Put_Field ("VEH_TYPE", driver_ptr->Type ());
			arcview_route.Put_Field ("SUB_TYPE", driver_ptr->Sub_Type ());

			num_legs = driver_ptr->Links ();

			//---- find the end index ----

			stop = line_ptr->Stop (line_ptr->Stops ());

			stop_ptr = stop_offset.Get (stop);
			if (stop_ptr == NULL) {
				Check_Message (stop_error, line_ptr->Route (), stop);
				continue;
			}
			for (i=num_legs; i >= 1; i--) {
				if (stop_ptr->Link_Dir () == driver_ptr->Link_Dir (i)) break;
			}
			if (i < 1) {
				Check_Message (path_error, line_ptr->Route (), stop_ptr->ID ());
				continue;
			}
			num_legs = i;

			//---- find the start index ----

			stop = line_ptr->Stop (1);

			stop_ptr = stop_offset.Get (stop);
			if (stop_ptr == NULL) {
				Check_Message (stop_error, line_ptr->Route (), stop);
				continue;
			}
			for (i=1; i <= num_legs; i++) {
				if (stop_ptr->Link_Dir () == driver_ptr->Link_Dir (i)) break;
			}
			if (i > num_legs) {
				Check_Message (path_error, line_ptr->Route (), stop_ptr->ID ());
				continue;
			}

			//---- create transit legs ----

			for (j=1; i <= num_legs; i++) {
				dir = driver_ptr->Dir (i);
				link_dir = driver_ptr->Link_Dir (i);

				link_ptr = link_data.Get (driver_ptr->Link (i));

				link_len = UnRound (link_ptr->Length ());
				side_offset = route_offset;

				if (offset_flag) {
					if (!overlap_flag) {
						if (dir == 0) {
							overlap = link_ptr->Aoffset () + 1;
							link_ptr->Aoffset (overlap);
						} else {
							overlap = link_ptr->Boffset () + 1;
							link_ptr->Boffset (overlap);
						}
						side = side_offset *= overlap;
					} else if (link_ptr->AB_Dir () > 0 && link_ptr->BA_Dir () > 0) {
						side = side_offset;
					} else {
						side = 0.0;
					}
				}
				if (side_offset > link_len / 3.0) {
					side_offset = link_len / 3.0;
				}
				if (dir == 0) {
					offset = side_offset;
				} else {
					offset = link_len - side_offset;
				}

				//---- check for stops ----

				while (stop_ptr->Link_Dir () == link_dir) {

					stop_off = stop_ptr->Offset ();

					if (dir == 0) {
						length = stop_off - offset;
					} else {
						length = offset - stop_off;
					}

					Link_Shape (link_ptr, dir, &points, offset, length, side);

					offset = stop_off;

					if (j != 1) {

						//---- draw the link to the stop ----

						if (side == 0.0) {
							k = 2;
						} else {
							k = 1;
						}
						for (; k <= points.Num_Points (); k++) {
							if (!arcview_route.points.Add (points [k])) goto mem_error;
						}
					}
					point.x = stop_ptr->X ();
					point.y = stop_ptr->Y ();
					point.z = stop_ptr->Z ();

					if (!arcview_route.points.Add (&point)) goto mem_error;

					//---- find the next stop -----

					if (++j > line_ptr->Stops ()) break;

					if (!arcview_route.points.Add (points.Last ())) goto mem_error;

					stop = line_ptr->Stop (j);

					stop_ptr = stop_offset.Get (stop);
					if (stop_ptr == NULL) {
						Check_Message (stop_error, line_ptr->Route (), stop);
						continue;
					}
				}

				//---- complete the link ----

				if (i < num_legs) {
					if (dir == 0) {
						length = link_len - offset - side_offset;
					} else {
						length = offset - side_offset;
					}

					Link_Shape (link_ptr, dir, &points, offset, length, side);

					if (offset_flag) {
						k = 1;
					} else {
						k = 2;
					}
					for (; k <= points.Num_Points (); k++) {
						if (!arcview_route.points.Add (points [k])) goto mem_error;
					}
				}
			}

		} else {

			for (i=1; i <= line_ptr->Stops (); i++) {
				stop = line_ptr->Stop (i);

				stop_ptr = stop_offset.Get (stop);

				if (stop_ptr == NULL) {
					Check_Message (stop_error, line_ptr->Route (), stop);
					continue;
				}
				point.x = stop_ptr->X ();
				point.y = stop_ptr->Y ();
				point.z = stop_ptr->Z ();

				if (!arcview_route.points.Add (&point)) goto mem_error;
			}
		}

		if (!arcview_route.Write_Record ()) {
			Error ("Writing ArcView Transit Driver File");
		}
	}
	End_Progress ();

	arcview_route.Close ();
	return;

mem_error:
	Error ("Insufficient Memory for Transit Route Shape Points");
}
