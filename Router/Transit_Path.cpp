//*********************************************************
//	Transit_Path.cpp - Build a Transit Path
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Transit_Path
//---------------------------------------------------------

void Router::Transit_Path (int board, int xfer, int mode)
{
	int acc, run, wait, path, path1, route, index;
	unsigned cum_a, cum_b, cum_s, cum;
	int tod_a, tod_b, tod_s, cost_a, cost_b, cost_s, time_a, time_b, time_s;
	int imp, ttim, tim, stop, alight, variance, value_waiting, wait_imp, penalty;

	Path_Data *path_ptr;
	Path_Array *alight_array, *loc_array;
	Route_Stop_Data *rstop_ptr;
	Fare_Data current_rec, fare_rec, *fare_ptr = 0;
	Line_Data *line_ptr;
	List_Data *acc_ptr;
	Access_Data *access_ptr;

	if (board == 0) return;

	path = (xfer < max_paths) ? xfer : max_paths - 1;
	path1 = (++xfer < max_paths) ? xfer : max_paths;

	first_ptr = path_ptr = board_path [path] [board];

	memset (&current_rec, '\0', sizeof (current_rec));
	memset (&fare_rec, '\0', sizeof (fare_rec));

	alight_array = alight_path + path;
	loc_array = loc_path + path1;

	if (path_ptr->Cum () > trip_des.Cum ()) return;
	
	value_waiting = (xfer > 1) ? value_xfer : value_wait;

	//---- check the cumulative impedance ----

	cum_s = path_ptr->Cum ();

	if (cum_s >= trip_des.Cum ()) return;

	//---- check the activity schedule ----

	tod_s = path_ptr->TOD ();

	if (tod_s >= max_tod) return;

	//---- check walk distance ----

	time_a = time_b = time_s = path_ptr->Time ();

	if (walk_flag && time_s >= max_walk) return;

	cost_b = cost_s = path_ptr->Cost ();

	//---- initialize the fare data ----

	if (fare_flag) {
		fare_rec.from_mode = (char) mode;
		fare_rec.type = (char) traveler_class;
	}

	//---- board a transit route ----

	for (index = stop_list [board]; index; index = rstop_ptr->List ()) {
		rstop_ptr = route_stop [index];

		route = rstop_ptr->Route ();
		stop = rstop_ptr->Stop ();

		line_ptr = line_data [route];
		if (line_ptr == NULL) {
			Error ("route=%d was Not Found in the Line File");
		}

		tod_b = penalty = 0;
		wait = -1;

		for (run=1; run <= line_ptr->Runs (); run++) {
			tod_b = line_ptr->Schedule (run, stop);
			penalty = line_ptr->Penalty (run, stop);

			wait = tod_b - tod_s;
			if (wait >= min_wait) break;
		}

		//---- check the waiting time ----

		if (wait < min_wait) continue;

		if (wait_flag && wait > max_wait) {
			wait_time_flag = true;
			continue;
		}
		//pen_a = pen_b + ((wait * value_waiting + penalty + value_round) >> value_roll);

		if (fare_flag) {
			fare_rec.from_zone = (short) line_ptr->Zone (stop);
			fare_rec.to_mode = (char) line_ptr->Mode ();
			fare_rec.period = (char) fare_data.Period (Resolve (tod_b));
		}
		if (tod_s >= max_tod) {
			time_flag = true;
			continue;
		}
		cum_b = cum_s + ((wait * value_waiting + penalty + value_round) >> value_roll);

		if (cum_b >= trip_des.Cum ()) continue;

		//---- process each alighting options ----

		for (++stop; stop <= line_ptr->Stops (); stop++) {

			//---- get the arrival time ----

			tod_a = line_ptr->Schedule (run, stop);

			//---- check the activity schedule ----

			if (tod_a >= max_tod) {
				time_flag = true;
				break;
			}

			//---- calculate the impedance ----

			ttim = tod_a - tod_b;

			imp = (ttim * value_time + value_round) >> value_roll;

			//---- get the fare ----

			cost_a = cost_b;

			if (fare_flag) {
				fare_rec.to_zone = (short) line_ptr->Zone (stop);

				if (memcmp (&current_rec, &fare_rec, sizeof (fare_rec)) != 0) {
					memcpy (&current_rec, &fare_rec, sizeof (fare_rec));
					fare_ptr = fare_data.Get (&fare_rec);
				}

				if (fare_ptr != NULL) {
					//cost_a = fare_ptr->Fare ();
					cost_a = fare_ptr->fare;

					imp += ((cost_a * value_cost + value_round) >> value_roll);
					cost_a += cost_b;
				}
			}

			//---- apply bias ----

			if (line_ptr->Mode () > EXPRESS_BUS) {
				if (rail_bias_flag) {
					imp = (imp * rail_bias + rail_constant + 50) / 100;
					if (imp < 1) imp = 1;
				}
			} else if (bus_bias_flag) {
				imp = (imp * bus_bias + bus_constant + 50) / 100;
				if (imp < 1) imp = 1;
			}
			
			//---- randomize impedance ----

			wait_imp = value_waiting;

			if (random_flag) {
				variance = (imp * random_imped + 50) / 100;
				if (variance > 0) {
					cum_a = (int) (imp + variance * (random.Probability () - 0.5) + 0.5);
					wait_imp = (wait_imp * cum_a + (imp >> 1)) / imp;
					imp  = cum_a;
				}
			}

			//---- check the cumulative impedance at the alighting stop ----

			if (imp < 1) imp = 1;

			cum_a = cum_b + imp;

			if (cum_a >= trip_des.Cum ()) continue;

			alight = line_ptr->Stop (stop);

			path_ptr = (*alight_array) [alight];

			if (cum_a >= path_ptr->Cum () && path_ptr->Cum () > 0) continue;

			//---- set the path to the alighting stop ----

			path_ptr->Cum (cum_a);
			path_ptr->TOD (tod_a);
			path_ptr->Cost (cost_a);
			path_ptr->Time (time_a);
			path_ptr->Prev (board * nroute + rstop_ptr->Route ());
			path_ptr->Next (wait * 10000 + wait_imp);

			//---- move to an activity location ----

			for (acc = transit_egress [alight]->From_List (); acc; acc = acc_ptr->From_List ()) {
				access_ptr = access_data [acc];
				acc_ptr = access_list [acc];

				//---- check walk distance ----

				if (walk_flag) {
					tim = time_a + access_ptr->Time ();

					if (tim > max_walk) {
						length_flag = true;
						continue;
					}
				}

				//---- check the travel time ----

				tim = tod_a + access_ptr->Time ();

				if (tim > max_tod) {
					time_flag = true;
					continue;
				}

				imp = (access_ptr->Time () * value_walk + access_ptr->Cost () * value_cost + value_round) >> value_roll;
				if (imp < 1) imp = 1;

				cum = cum_a + imp;

				if (cum >= trip_des.Cum ()) continue;

				path_ptr = (*loc_array) [access_ptr->To_ID ()];

				if (cum >= path_ptr->Cum () && path_ptr->Cum () > 0) continue;

				//---- check for the destination ----

				if (access_ptr->To_ID () == trip_des.Rec ()) {
					trip_des.Cum (cum);
					trip_des.TOD (tim);
					trip_des.Cost (cost_a + access_ptr->Cost ());
					trip_des.Time (time_a + access_ptr->Time ());
					trip_des.Prev (alight);
					trip_des.Layer (STOP_ID);
					trip_des.Path (path);
					continue;
				}

				//---- add the stop to the list ----

				if (path_ptr->Next () == -1) {
					path_ptr->Next (first_ptr->Next ());
					path_ptr->Type (first_ptr->Type ());
					path_ptr->Xfer (first_ptr->Xfer ());
					path_ptr->Mode (first_ptr->Mode ());
					first_ptr->Next (access_ptr->To_ID ());
					first_ptr->Type (LOCATION_ID);
					first_ptr->Xfer (xfer);
					first_ptr->Mode (line_ptr->Mode ());
				} else if (path_ptr->Next () == 0 && last_ptr != path_ptr) {
					last_ptr->Next (access_ptr->To_ID ());
					last_ptr->Type (LOCATION_ID);
					last_ptr->Xfer (xfer);
					last_ptr->Mode (line_ptr->Mode ());
					last_ptr = path_ptr;
				}
				path_ptr->Cum (cum);
				path_ptr->TOD (tim);
				path_ptr->Cost (cost_a + access_ptr->Cost ());
				path_ptr->Time (time_a + access_ptr->Time ());
				path_ptr->Prev (alight);
				path_ptr->Layer (STOP_ID);
				path_ptr->Path (path);
			}
		}
	}
}
