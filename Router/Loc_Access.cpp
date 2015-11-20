//*********************************************************
//	Loc_Access.cpp - build away from an Activity Location
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Location_Access
//---------------------------------------------------------

void Router::Location_Access (int org, int xfer, int mode)
{
	int bnode, lnk, offset, offset_factor, path, path1;
	unsigned cum_a, cum_b;
	int tod_a, tod_b, cost_a, cost_b, time_a, time_b;
	int imp, imped, tim, time, variance, loc, acc;

	Link_Data *link_ptr;
	Path_Data *path_ptr;
	Path_Array *board_array, *loc_array, *node_array;
	Location_Data *org_ptr, *loc_ptr;
	Access_Data *access_ptr;
	List_Data *acc_ptr;
	Stop_Data *stop_ptr;

	if (org == 0) return;

	path = (xfer < max_paths) ? xfer : max_paths;
	path1 = (path < max_paths) ? path : path - 1;

	loc_array = loc_path + path;	
	board_array = board_path + path1;
	node_array = node_path + path;

	first_ptr = path_ptr = (*loc_array) [org];

	org_ptr = location_data [org];

	if (org_ptr->ID () == trip_des.ID () || path_ptr->Cum () > trip_des.Cum ()) return;

	//---- initialize the anode values ---

	cum_a = path_ptr->Cum ();
	tod_a = path_ptr->TOD ();
	cost_a = path_ptr->Cost ();
	time_a = path_ptr->Time ();

	link_ptr = link_data [org_ptr->Link ()];

	//---- add boarding options to the list ----

	if (xfer <= max_transfers) {

		for (acc = transit_access [org]->From_List (); acc; acc = acc_ptr->From_List ()) {
			acc_ptr = access_list [acc];
			access_ptr = access_data [acc];

			//---- check walk distance ----

			time_b = time_a + access_ptr->Time ();

			if (walk_flag && time_b > max_walk) {
				length_flag = true;
				continue;
			}

			//---- check the travel time ----

			tod_b = tod_a + access_ptr->Time ();

			if (tod_b > max_tod) {
				time_flag = true;
				continue;
			}

			//---- check the cumulative impedance ---

			imped = (access_ptr->Time () * value_walk + access_ptr->Cost () * value_cost + value_round) >> value_roll;

			if (xfer > 0) {
				imped += xfer_imped;
			}
			if (stop_imped > 0 || station_imped > 0) {
				stop_ptr = stop_data [access_ptr->To_ID ()];

				if (stop_ptr != NULL) {
					if (stop_ptr->Type () == STOP) {
						imped += stop_imped;
					} else {
						imped += station_imped;
					}
				}
			}
			if (imped < 1) imped = 1;

			cum_b = cum_a + imped;

			if (cum_b >= trip_des.Cum ()) continue;

			path_ptr = (*board_array) [access_ptr->To_ID ()];

			if (cum_b >= path_ptr->Cum () && path_ptr->Cum () > 0) continue;

			//---- add the stop to the list ----

			if (path_ptr->Next () == -1) {
				path_ptr->Next (first_ptr->Next ());
				path_ptr->Type (first_ptr->Type ());
				path_ptr->Xfer (first_ptr->Xfer ());
				path_ptr->Mode (first_ptr->Mode ());
				first_ptr->Next (access_ptr->To_ID ());
				first_ptr->Type (STOP_ID);		//**** BOARD_LAYER;
				first_ptr->Xfer (xfer);
				first_ptr->Mode (mode);
			} else if (path_ptr->Next () == 0 && last_ptr != path_ptr) {
				last_ptr->Next (access_ptr->To_ID ());
				last_ptr->Type (STOP_ID);		//---- board ----
				last_ptr->Xfer (xfer);
				last_ptr->Mode (mode);
				last_ptr = path_ptr;
			}
			path_ptr->Cum (cum_b);
			path_ptr->TOD (tod_b);
			path_ptr->Cost (cost_a + access_ptr->Cost ());
			path_ptr->Time (time_b);
			path_ptr->Prev (org);
			path_ptr->Layer (LOCATION_ID);
			path_ptr->Path (path);
		}
	}

	//---- check for walk use restrictions ----
		
	if (!Use_Permission (link_ptr->Use (), WALK)) {	
		use_flag = true;
		return;
	}

	//---- calculate the random impedance for the link ----

	time = (int) (link_ptr->Length () / walk_speed + 0.5);

	imped = (time * value_walk + value_round) >> value_roll;
	if (imped < 1) imped = 1;

	if (random_flag) {
		variance = (imped * random_imped + 50) / 100;
		if (variance > 0) {
			imped = (int) (imped + variance * (random.Probability () - 0.5) + 0.5);
		}
	}

	//---- process activity locations on the current link ----

	for (loc = link_list [org_ptr->Link ()]; loc; loc = loc_list [loc]) {
		loc_ptr = location_data [loc];
		acc = transit_access [loc]->From_List ();

		//---- check for the destination or transit access ----

		if (loc == org || (loc_ptr->ID () != trip_des.ID () && !acc)) continue;

		//---- get distance and add round-off protection ----

		offset = abs (loc_ptr->Offset () - org_ptr->Offset ()) + 1;
		
		offset_factor = ((offset << offset_roll) + (link_ptr->Length () >> 1)) / link_ptr->Length ();
		if (offset_factor < 1) offset_factor = 1;

		//---- calculate time ----

		tim = (time * offset_factor + offset_round) >> offset_roll;

		//---- check the time schedule ----

		tod_b = tod_a + tim;

		if (tod_b > max_tod) {
			time_flag = true;
			continue;
		}

		//---- check walk distance ----

		time_b = time_a + tim;

		if (walk_flag && time_b >= max_walk) {
			length_flag = true;
			continue;
		}

		cost_b = cost_a;

		//---- check the cumulative impedance ----

		imp = (imped * offset_factor + offset_round) >> offset_roll;
		if (imp < 1) imp = 1;

		cum_b = cum_a + imp;

		if (cum_b >= trip_des.Cum ()) continue;

		path_ptr = (*loc_array) [loc];

		if (cum_b >= path_ptr->Cum () && path_ptr->Cum () > 0) continue;

		//---- check for the destination ----

		if (loc_ptr->ID () == trip_des.ID ()) {
			trip_des.Cum (cum_b);
			trip_des.TOD (tod_b);
			trip_des.Cost (cost_b);
			trip_des.Time (time_b);
			trip_des.Prev (org);
			trip_des.Layer (LOCATION_ID);
			trip_des.Path (path);
			continue;
		}

		//---- set the path to a location with transit access ---

		if (path_ptr->Next () == -1) {
			path_ptr->Next (first_ptr->Next ());
			path_ptr->Type (first_ptr->Type ());
			path_ptr->Xfer (first_ptr->Xfer ());
			path_ptr->Mode (first_ptr->Mode ());
			first_ptr->Next (loc);
			first_ptr->Type (LOCATION_ID);
			first_ptr->Xfer (xfer);
			first_ptr->Mode (mode);
		} else if (path_ptr->Next () == 0 && last_ptr != path_ptr) {
			last_ptr->Next (loc);
			last_ptr->Type (LOCATION_ID);
			last_ptr->Xfer (xfer);
			last_ptr->Mode (mode);
			last_ptr = path_ptr;
		}
		path_ptr->Cum (cum_b);
		path_ptr->TOD (tod_b);
		path_ptr->Cost (cost_b);
		path_ptr->Time (time_b);
		path_ptr->Prev (org);
		path_ptr->Layer (LOCATION_ID);
		path_ptr->Path (path);
	}

	//---- add the nodes at the end of the link to the path queue ----

	for (lnk=0; lnk < 2; lnk++) {

		//---- get distance ----

		if (lnk == 0) {
			offset = link_ptr->Length () - org_ptr->Offset ();
			bnode = link_ptr->Bnode ();
		} else {
			offset = org_ptr->Offset ();
			bnode = link_ptr->Anode ();
		}
		offset_factor = ((offset << offset_roll) + (link_ptr->Length () >> 1)) / link_ptr->Length ();

		//---- calculate time ----

		tim = (time * offset_factor + offset_round) >> offset_roll;

		//---- check the time schedule ----

		tod_b = tod_a + tim;

		if (tod_b >= max_tod) {
			time_flag = true;
			continue;
		}

		//---- check walk distance ----

		time_b = time_a + tim;

		if (walk_flag && time_b >= max_walk) {
			length_flag = true;
			continue;
		}

		cost_b = cost_a;

		//---- check the cumulative impedance ----

		imp = (imped * offset_factor + offset_round) >> offset_roll;
		if (imp < 1) imp = 1;

		cum_b = cum_a + imp;

		if (cum_b >= trip_des.Cum ()) continue;

		path_ptr = (*node_array) [bnode];

		if (cum_b >= path_ptr->Cum () && path_ptr->Cum () > 0) continue;

		if (path_ptr->Next () == -1) {
			path_ptr->Next (first_ptr->Next ());
			path_ptr->Type (first_ptr->Type ());
			path_ptr->Xfer (first_ptr->Xfer ());
			path_ptr->Mode (first_ptr->Mode ());
			first_ptr->Next (bnode);
			first_ptr->Type (NODE_ID);
			first_ptr->Xfer (xfer);
			first_ptr->Mode (mode);
		} else if (path_ptr->Next () == 0 && last_ptr != path_ptr) {
			last_ptr->Next (bnode);
			last_ptr->Type (NODE_ID);
			last_ptr->Xfer (xfer);
			last_ptr->Mode (mode);
			last_ptr = path_ptr;
		}
		path_ptr->Cum (cum_b);
		path_ptr->TOD (tod_b);
		path_ptr->Cost (cost_b);
		path_ptr->Time (time_b);
		path_ptr->Prev (org);
		path_ptr->Layer (LOCATION_ID);
		path_ptr->Path (path);
	}
}
