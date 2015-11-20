//*********************************************************
//	Save_Route.cpp - save transit route data
//*********************************************************

#include "TransitNet.hpp"

//---------------------------------------------------------
//	Save_Route
//---------------------------------------------------------

void TransitNet::Save_Route (Route_Data *route_ptr)
{
	int i, j, p, leg, nleg, run, stop, first_offset, last_offset, headway, route, mode;
	int change, spacing, nsplit, length, offset, time_inc, offset_time, cum_time;
	int dwell, ttime, next_dwell, next_ttime, prev_dwell, last_stop, time;
	bool bus_flag, left_flag, prev_left, first_flag, mid_flag, last_flag, short_flag, put_flag;
	double time_fac;

	Dir_Path_Data *dir_ptr, *next_dir;
	Route_Node *rnode_ptr;
	Route_Stop rstop_rec, *rstop_ptr;
	Stop_Data *stop_ptr, *last_ptr;
	Line_Data *line_ptr;
	Range_Data *range_ptr, *tod_ptr;

	//---- check for an existing route ----

	route = route_ptr->Route ();
	line_ptr = line_data.Get (route);

	if (line_ptr == NULL) {
		line_ptr = line_data.New_Record (true);
		line_ptr->Route (route);
		if (!line_data.Add ()) {
			Error ("Adding Transit Route Data");
		}
		line_ptr = line_data.Record ();
	}

	//---- assign stops to route links ----

	nleg = route_node.Num_Records ();

	route_stop.Reset ();

	next_dir = NULL;
	put_flag = false;

	mode = route_ptr->Mode ();

	cum_time = next_dwell = next_ttime = dwell = 0;
	bus_flag = (mode == LOCAL_BUS || mode == EXPRESS_BUS);
	prev_left = left_flag = false;

	for (leg=1; leg <= nleg; leg++) {

		if (leg == 1) {
			rnode_ptr = route_node [leg];
			dwell = rnode_ptr->Dwell ();
			continue;
		}
		prev_dwell = dwell;

		if (leg == 2) {
			rnode_ptr = route_node [leg];
			dir_ptr = dir_data [rnode_ptr->Dir ()];

			dwell = rnode_ptr->Dwell ();
			ttime = rnode_ptr->TTime ();
		} else {
			dir_ptr = next_dir;

			dwell = next_dwell;
			ttime = next_ttime;
		}
		left_flag = false;

		if (leg < nleg) {
			rnode_ptr = route_node [leg+1];
			next_dir = dir_data [rnode_ptr->Dir ()];

			next_dwell = rnode_ptr->Dwell ();
			next_ttime = rnode_ptr->TTime ();

			if ((dir_ptr->Thru () + dir_ptr->Right ()) > 1) {
				change = compass.Change (dir_ptr->Out_Bearing (), next_dir->In_Bearing ());
				left_flag = (change <= left_turn);
			}
		} else {
			next_dir = NULL;
			next_dwell = next_ttime = 0;
		}

		//---- set the stop criteria ----

		length = dir_ptr->Length () - dir_ptr->Aoffset () - dir_ptr->Boffset ();
		short_flag = (length < (2 * stop_offset));

		first_flag = mid_flag = last_flag = false;

		if (leg == 2) {
			first_flag = true;
			if (prev_dwell == 0) {		
				Warning ("A Stop was Added at the Beginning of Route %d", route);
			}
		} else if (!short_flag && prev_dwell > 0) {
			if (stop_type == FARSIDE || prev_left || !put_flag) {
				first_flag = true;
			}
		}
		if (bus_flag && !short_flag && prev_dwell > 0 && dwell > 0 && dir_ptr->Type () > 0) {
			mid_flag = true;
		}
		if (leg == nleg) {
			last_flag = true;
			if (dwell == 0) {
				Warning ("A Stop was Added at the End of Route %d", route);
			}
		} else if (!short_flag && dwell > 0) {
			if ((stop_type == NEARSIDE && !left_flag) || (!mid_flag && next_dir != NULL && next_dir->Type () == 0)) {
				last_flag = true;
			}
		}
		prev_left = left_flag;
		put_flag = false;

		if (first_flag || last_flag || mid_flag) {
			first_offset = dir_ptr->Aoffset () + stop_offset;
			last_offset = dir_ptr->Length () - dir_ptr->Boffset () - stop_offset;

			offset_time = (int) ((double) first_offset * ttime / dir_ptr->Length () + 0.5);

			//---- check for special conditions ----

			if (dir_ptr->Type () == 0 || short_flag) {
				Warning ("Route %d must Stop on Link %d", route, dir_ptr->Link ());

				if (length < (4 * stop_offset)) {
					nsplit = 0;
					time_inc = spacing = 0;
				} else {
					nsplit = 1;
					spacing = length - 2 * stop_offset;
					time_inc = (int) ((double) spacing * ttime / dir_ptr->Length () + 0.5);
				}

			} else {

				//---- normal stop spacing ----

				spacing = min_stop_spacing [dir_ptr->Type ()];
				if (spacing == 0) spacing = length;
		
				nsplit = (length - (2 * stop_offset)) / spacing;
				if (nsplit < 1) nsplit = 1;
					
				spacing = (length - 2 * stop_offset) / nsplit;
				time_inc = (int) ((double) spacing * ttime / dir_ptr->Length () + 0.5);
			}
			nsplit++;

			//---- add stops to the route list ----

			stop = dir_ptr->Stop_List ();
			last_stop = 0;
			stop_ptr = NULL;

			for (i=1; i <= nsplit; i++) {
				if (i == 1) {
					ttime -= offset_time;
					cum_time += offset_time;
					offset = first_offset;
					if (!first_flag) continue;
				} else if (i == nsplit) {
					ttime -= time_inc;
					cum_time += time_inc;
					offset = last_offset;
					if (!last_flag) continue;
				} else {
					ttime -= time_inc;
					cum_time += time_inc;
					offset += spacing;
					if (!mid_flag) continue;
				}

				//---- check for an existing stop ----

				for (; stop; stop = stop_ptr->Traveler ()) {
					stop_ptr = stop_data [stop];

					if (stop_ptr->Offset () >= offset - stop_offset) break;

					last_stop = stop;
				}

				if (stop > 0 && stop_ptr->Offset () >= offset - stop_offset && 
					stop_ptr->Offset () <= offset + stop_offset) {

					//---- use existing stop ----

					stop_ptr->Space (stop_ptr->Space () + 1);
					if (bus_flag) {
						stop_ptr->Use (stop_ptr->Use () | bus_code);
					} else {
						stop_ptr->Use (stop_ptr->Use () | rail_code);
					}

					//---- increment the stop number ----

					last_stop = stop;
					stop = stop_ptr->Traveler ();

				} else {

					//---- create a new stop ----

					stop_ptr = stop_data.New_Record (true);
					if (stop_ptr == NULL) goto mem_error;

					stop_ptr->Stop (++max_stop);
					stop_ptr->Link_Dir (dir_ptr->Link_Dir ());
					stop_ptr->Offset (offset);

					stop_ptr->Space (1);
					if (bus_flag) {
						stop_ptr->Use (bus_code);
					} else {
						stop_ptr->Use (rail_code);
					}
					if (!stop_data.Add ()) goto mem_error;

					//---- update the stop list ----

					if (last_stop == 0) {
						last_stop = stop_data.Num_Records ();
						stop_ptr->Traveler (dir_ptr->Stop_List ());
						dir_ptr->Stop_List (last_stop);
					} else {
						last_ptr = stop_data [last_stop];
						last_stop = stop_data.Num_Records ();
						stop_ptr->Traveler (last_ptr->Traveler ());
						last_ptr->Traveler (last_stop);
					}
				}

				//---- save the route stop ----

				rstop_rec.Stop (stop_ptr->Stop ());
				rstop_rec.Time (cum_time);
				rstop_rec.Zone (dir_ptr->Zone ());

				if (!route_stop.Add (&rstop_rec)) goto mem_error;
				put_flag = true;
			}
		}
		cum_time += ttime;
	}

	//---- save the route file ----

	line_ptr->Route (route);
	line_ptr->Stops (route_stop.Num_Records ());
	line_ptr->Mode (mode);
	line_ptr->Name ((char *) route_ptr->Name ());
	line_ptr->Notes ((char *) route_ptr->Notes ());

	ttime = 0;

	for (i=1, rstop_ptr = route_stop.First (); rstop_ptr; rstop_ptr = route_stop.Next (), i++) {
		line_ptr->Stop (i, rstop_ptr->Stop ());

		line_ptr->Zone (i, rstop_ptr->Zone ());
		line_ptr->Time_Flag (i, 0);

		//---- convert to time increments ----

		cum_time = rstop_ptr->Time ();
		rstop_ptr->Time (cum_time - ttime);
		ttime = cum_time;
	}

	//---- count the number of runs ----

	for (i=1, run=0; i <= num_periods; i++) {
		headway = route_ptr->Headway (i);
		if (headway == 0) continue;

		range_ptr = time_period [i];

		ttime = range_ptr->Low () + route_ptr->Offset (i);

		for (; ttime <= range_ptr->High (); ttime += headway) {
			run++;
		}
	}

	line_ptr->Runs (run);

	//---- write the schedule file ----

	for (i=1, run=0; i <= num_periods; i++) {
		headway = route_ptr->Headway (i);
		if (headway == 0) continue;

		range_ptr = time_period [i];

		ttime = range_ptr->Low () + route_ptr->Offset (i);

		time = route_ptr->TTime (i);

		if (time > 0 && route_ptr->Min_Time () > 0) {
			time_fac = (double) time / route_ptr->Min_Time ();
		} else {
			time_fac = 1.0;
		}
		for (; ttime <= range_ptr->High (); ttime += headway) {
			run++;

			time_inc = ttime;
			tod_ptr = range_ptr;
			p = i;

			for (j=1, rstop_ptr = route_stop.First (); rstop_ptr; rstop_ptr = route_stop.Next (), j++) {
				if (time_inc > tod_ptr->High () && p < num_periods) {
					tod_ptr = time_period [++p];
				}
				time = (int) (rstop_ptr->Time () * time_fac + 0.5);

				if (bus_flag) {
					time_inc += (int) (UnRound (time) * time_factor [p-1] + 0.5);
				} else {
					time_inc += Resolve (time);
				}
				line_ptr->Schedule (run, j, Round (time_inc));
			}
		}
	}
	return;

mem_error:
	Error ("Adding Stop Data");
}

