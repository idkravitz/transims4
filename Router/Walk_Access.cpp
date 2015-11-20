//*********************************************************
//	Walk_Access.cpp - build away from a Walk Node
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Walk_Access
//---------------------------------------------------------

void Router::Walk_Access (int anode, int xfer, int mode)
{
	int bnode, lnk, offset, offset_factor, path;
	unsigned cum_a, cum_b;
	int tod_a, tod_b, cost_a, cost_b, time_a, time_b;
	int imp, tim, imped, time, variance, loc, acc;
	bool ab_flag;

	Link_Data *link_ptr;
	List_Data *list_ptr;
	Path_Data *path_ptr;
	Path_Array *node_array, *loc_array;
	Location_Data *loc_ptr;

	if (anode == 0) return;

	path = (xfer < max_paths) ? xfer : max_paths;

	node_array = node_path + path;
	loc_array = loc_path + path;

	first_ptr = path_ptr = (*node_array) [anode];

	//---- check the cumulative impedance ----

	cum_a = path_ptr->Cum ();

	if (cum_a >= trip_des.Cum ()) return;

	//---- check the activity schedule ----

	tod_a = path_ptr->TOD ();

	if (tod_a >= max_tod) {
		time_flag = true;
		return;
	}

	//---- check walk distance ----

	time_a = path_ptr->Time ();

	if (walk_flag && time_a >= max_walk) {
		length_flag = true;
		return;
	}

	cost_b = cost_a = path_ptr->Cost ();

	//---- process each link leaving the A node ----

	for (lnk = walk_list [anode]; lnk; lnk = list_ptr->Next (ab_flag)) {

		link_ptr = link_data [lnk];

		ab_flag = (link_ptr->Anode () == anode);

		bnode = (ab_flag) ? link_ptr->Bnode () : link_ptr->Anode ();

		path_ptr = (*node_array) [bnode];

		//---- calculate the impedance ----

		list_ptr = walk_link [lnk];

		time = (int) (link_ptr->Length () / walk_speed + 0.5);

		imped = (time * value_walk + value_round) >> value_roll;
		if (imped < 1) imped = 1;

		if (random_flag) {
			variance = (imped * random_imped + 50) / 100;
			if (variance > 0) {
				imped = (int) (imped + variance * (random.Probability () - 0.5) + 0.5);
			}
		}

		//---- check activity locations on the link ----

		for (loc = link_list [lnk]; loc; loc = loc_list [loc]) {
			loc_ptr = location_data [loc];
			acc = transit_access [loc]->From_List ();

			//---- check for the destination or transit access ----

			if (loc_ptr->ID () != trip_des.ID () && !acc) continue;

			//---- get distance ----

			offset = (ab_flag) ? loc_ptr->Offset () : link_ptr->Length () - loc_ptr->Offset ();
		
			offset_factor = ((offset << offset_roll) + (link_ptr->Length () >> 1)) / link_ptr->Length ();

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

			//---- check the cumulative impedance ----

			imp = (imped * offset_factor + offset_round) >> offset_roll;
			if (imp < 1) imp = 1;

			cum_b = cum_a + imp;

			if (cum_b >= trip_des.Cum ()) continue;

			//---- check for the destination ----

			if (loc_ptr->ID () == trip_des.ID ()) {
				trip_des.Cum (cum_b);
				trip_des.TOD (tod_b);
				trip_des.Cost (cost_b);
				trip_des.Time (time_b);
				trip_des.Prev (anode);
				trip_des.Layer (NODE_ID);
				trip_des.Path (path);
				continue;
			}

			//---- check the location ----

			path_ptr = (*loc_array) [loc];

			if (cum_b >= path_ptr->Cum () && path_ptr->Cum () > 0) continue;

			//---- add the location to the processing list ---

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
			path_ptr->Prev (anode);
			path_ptr->Layer (NODE_ID);
			path_ptr->Path (path);
		}

		//---- check the time ----

		tod_b = tod_a + time;

		if (tod_b >= max_tod) {
			time_flag = true;
			continue;
		}

		//---- check walk distance ----

		time_b = time_a + time;

		if (walk_flag && time_b >= max_walk) {
			length_flag = true;
			continue;
		}

		//---- check the cumulative impedance at B ----

		if (imped < 1) imped = 1;

		cum_b = cum_a + imped;

		if (cum_b >= trip_des.Cum ()) continue;
		
		path_ptr = (*node_array) [bnode];

		if (cum_b >= path_ptr->Cum () && path_ptr->Cum () > 0) continue;

		//---- add the bnode to the processing list ----

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
		path_ptr->Prev (anode);
		path_ptr->Layer (NODE_ID);
		path_ptr->Path (path);
	}
}
