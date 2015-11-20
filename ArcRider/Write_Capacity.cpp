//*********************************************************
//	Write_Capacity - draw the transit run capacity file
//*********************************************************

#include "ArcRider.hpp"

#define MAX_SEGMENT		20

//---------------------------------------------------------
//	Write_Capacity
//---------------------------------------------------------

void ArcRider::Write_Capacity (void)
{
	int i, j, k, num_links, num, link, link_dir, dir, index1, index2;
	int seg, segment, stop1 [MAX_SEGMENT], stop2 [MAX_SEGMENT];
	int ride, on, off, stop, stops, run, runs, time, period, last_per;
	int per_runs, cap_runs, max_run, run_max, run_tot, cap_tot, max_load;
	int mode_field,	route_field, time_field, period_field, avg_runs_field, avg_load_field, avg_fac_field;
	int cap_runs_field, cap_load_field,	cap_fac_field, max_run_field, max_load_field, max_fac_field;
	double offset, length, link_len, side, avg_fac, cap_fac, max_fac, stop_off, side_offset;
	bool offset_flag, route_flag, tod_flag, link_flag, select;

	Link_Data *link_ptr;
	Rider_Data *rider_ptr;
	Driver_Data *driver_ptr;
	Data_Range *range;
	Veh_Type_Data *veh_type_ptr;
	Offset_Data *stop_ptr;

	tod_flag = (times.Num_Ranges () > 0);	
	route_flag = (transit_routes.Num_Ranges () > 0);
	link_flag = (links.Num_Records () > 0);
	offset_flag = (route_offset != 0.0);
	side = 0.0;

	mode_field = arcview_cap.Field_Number ("MODE");		
	route_field = arcview_cap.Field_Number ("ROUTE");
	time_field = arcview_cap.Field_Number ("TIME");
	period_field = arcview_cap.Field_Number ("PERIOD");
	avg_runs_field = arcview_cap.Field_Number ("AVG_RUNS");
	avg_load_field = arcview_cap.Field_Number ("AVG_LOAD");
	avg_fac_field = arcview_cap.Field_Number ("AVG_FAC");
	cap_runs_field = arcview_cap.Field_Number ("CAP_RUNS");
	cap_load_field = arcview_cap.Field_Number ("CAP_LOAD");
	cap_fac_field = arcview_cap.Field_Number ("CAP_FAC");
	max_run_field = arcview_cap.Field_Number ("MAX_RUN");
	max_load_field = arcview_cap.Field_Number ("MAX_LOAD");
	max_fac_field = arcview_cap.Field_Number ("MAX_FAC");

	Show_Message ("Writing %s -- Record", arcview_cap.File_Type ());
	Set_Progress (100);

	//---- get the route data ----

	for (rider_ptr = rider_data.First_Key (); rider_ptr; rider_ptr = rider_data.Next_Key ()) {
		Show_Progress ();

		if (mode_flag && !travel_modes [rider_ptr->Mode ()]) continue;
		if (route_flag && !transit_routes.In_Range (rider_ptr->Route ())) continue;

		//---- check the link criteria ----

		driver_ptr = driver_data [rider_ptr->Driver ()];
		
		veh_type_ptr = veh_type_data.Get (driver_ptr->Type (), driver_ptr->Sub_Type ());
		if (veh_type_ptr == NULL) continue;

		if (veh_type_ptr->Capacity () <= 0) continue;

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
		stops = rider_ptr->Stops ();
		runs = rider_ptr->Runs ();
		last_per = cap_runs = 0;

		//---- process each run ----

		for (run=1; ; run++) {
			if (run <= runs) {
				time = (rider_ptr->Time (run, 1) + rider_ptr->Time (run, stops)) / 2;

				period = times.In_Index (Resolve (time));
			} else {
				period = 0;
			}
			if (period != last_per) {
				if (last_per > 0 && cap_runs > 0) {
					if (per_runs > 0) {
						avg_fac = (double) run_tot / per_runs;
						run_tot = (int) (avg_fac + 0.5);
						avg_fac /= veh_type_ptr->Capacity ();
					} else {
						run_tot = 0;
						avg_fac = 0;
					}
					if (cap_runs > 0) {
						cap_fac = (double) cap_tot / cap_runs;
						cap_tot = (int) (cap_fac + 0.5);
						cap_fac /= veh_type_ptr->Capacity ();
					} else {
						cap_tot = 0;
						cap_fac = 0;
					}
					if (max_run > 0) {
						max_fac = (double) max_load / veh_type_ptr->Capacity ();
					} else {
						max_fac = 0.0;
					}
					arcview_cap.Put_Field (mode_field, Transit_Code ((Transit_Type) rider_ptr->Mode ()));
					arcview_cap.Put_Field (route_field, rider_ptr->Route ());
					arcview_cap.Put_Field (time_field, (char *) times.Range_Format (last_per));
					arcview_cap.Put_Field (period_field, last_per);
					arcview_cap.Put_Field (avg_runs_field, per_runs);
					arcview_cap.Put_Field (avg_load_field, run_tot);
					arcview_cap.Put_Field (avg_fac_field, avg_fac);
					arcview_cap.Put_Field (cap_runs_field, cap_runs);
					arcview_cap.Put_Field (cap_load_field, cap_tot);
					arcview_cap.Put_Field (cap_fac_field, cap_fac);
					arcview_cap.Put_Field (max_run_field, max_run);
					arcview_cap.Put_Field (max_load_field, max_load);
					arcview_cap.Put_Field (max_fac_field, max_fac);

					arcview_cap.parts.Reset ();
					arcview_cap.points.Reset ();

					//---- draw each segment ----

					for (seg = 0; seg <= segment; seg++) {
						arcview_cap.parts.Add (arcview_cap.points.Num_Points ());

						//---- find the end index ----

						stop = rider_ptr->Stop (stop2 [seg]);

						stop_ptr = stop_offset.Get (stop);

						for (i=num_links; i >= 1; i--) {
							if (stop_ptr->Link_Dir () == driver_ptr->Link_Dir (i)) break;
						}
						index2 = i;

						//---- find the start index ----

						stop = rider_ptr->Stop (stop1 [seg]);

						stop_ptr = stop_offset.Get (stop);

						for (i=1; i <= index2; i++) {
							if (stop_ptr->Link_Dir () == driver_ptr->Link_Dir (i)) break;
						}
						index1 = i;

						//---- create transit legs ----

						for (j=stop1 [seg]; i <= index2; i++) {
							dir = driver_ptr->Dir (i);
							link_dir = driver_ptr->Link_Dir (i);

							link_ptr = link_data.Get (driver_ptr->Link (i));

							link_len = UnRound (link_ptr->Length ());

							if (offset_flag) {
								if (link_ptr->AB_Dir () > 0 && link_ptr->BA_Dir () > 0) {
									side = route_offset;
								} else {
									side = 0.0;
								}
							}
							side_offset = route_offset;

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

								if (j != stop1 [seg]) {

									//---- draw the link to the stop ----

									if (side == 0.0) {
										k = 2;
									} else {
										k = 1;
									}
									for (; k <= points.Num_Points (); k++) {
										if (!arcview_cap.points.Add (points [k])) goto point_error;
									}
								}

								//---- find the next stop -----

								if (++j > stop2 [seg]) break;

								stop = rider_ptr->Stop (j);

								stop_ptr = stop_offset.Get (stop);
							}

							//---- complete the link ----

							if (i < index2) {
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

								if (side == 0.0 && i > index1) {
									j = 2;
								} else {
									j = 1;
								}
								for (; j <= points.Num_Points (); j++) {
									if (!arcview_cap.points.Add (points [j])) goto point_error;
								}
							}
						}
					}
					if (!arcview_cap.Write_Record ()) {
						Error ("Writing %s", arcview_cap.File_Type ());
					}
					num_cap++;
				}
				last_per = period;
				per_runs = cap_runs = max_run = run_tot = cap_tot = max_load = segment = 0;
				memset (stop1, '\0', sizeof (stop1));
				memset (stop2, '\0', sizeof (stop2));
			}
			if (run > runs) break;
			if (last_per == 0) continue;

			//---- sum the line ridership ----

			run_max = ride = 0;

			for (stop=1; stop <= stops; stop++) {
				on = rider_ptr->Board (run, stop);
				off = rider_ptr->Alight (run, stop);
				ride += on - off;
				if (ride > run_max) run_max = ride;

				if (ride > veh_type_ptr->Capacity ()) {
					if (stop1 [segment] == 0) {
						stop1 [segment] = stop;
						stop2 [segment] = stop + 1;
					} else if (stop2 [segment] == stop || segment == MAX_SEGMENT - 1) {
						stop2 [segment] = stop + 1;
					} else {
						segment++;
						stop1 [segment] = stop;
						stop2 [segment] = stop + 1;
					}
				}
			}
			per_runs++;
			run_tot += run_max;
			if (run_max > veh_type_ptr->Capacity ()) {
				if (run_max > max_load) {
					max_load = run_max;
					max_run = run;
				}
				cap_runs++;
				cap_tot += run_max;
			}
		}


	}
	End_Progress ();

	arcview_cap.Close ();
	return;

point_error:
	Error ("Insufficient Memory for %d Points", arcview_cap.points.Num_Points ());

}
