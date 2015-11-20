//*********************************************************
//	Node_Path.cpp - Build a Node-based Path
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Node_Path
//---------------------------------------------------------

int Router::Node_Path (Use_Type type, bool best_flag)
{
	int anode, bnode, lnk, offset, offset_factor;
	unsigned cum_a, cum_b, max_cum, hi_cum;
	int tod_a, tod_b, cost_a, cost_b, time_a, time_b;
	int imp, imped, tim, time, variance, best_des;
	bool ab_flag, des_flag, flag;

	int  max_time, value;
	double speed;
	Integer_List *alist;
	Trip_End_Data *org_ptr, *des_ptr;
	Link_Data *link_ptr;
	List_Data *list_ptr;
	List_Array *list_data;
	Path_Data *path_ptr, path_root;
	
	//---- initialize the path building ----

	if (type == WALK) {
		flag = walk_flag;
		max_time = max_walk;
		alist = &walk_list;
		list_data = &walk_link;
		value = value_walk;
		speed = walk_speed;
	} else {
		flag = bike_flag;
		max_time = max_bike;
		alist = &bike_list;
		list_data = &bike_link;
		value = value_bike;
		speed = bike_speed;
	}

	(*node_path).Zero_Fill ();

	first_ptr = last_ptr = &path_root;

	first_ptr->Next (0);

	best_des = 0;
	max_cum = MAX_INTEGER;

	//---- calculate the access link impedance ----

	for (org_ptr = org_array.First (); org_ptr != NULL; org_ptr = org_array.Next ()) {

		//---- get the link data ----

		link_ptr = link_data [org_ptr->Link ()];

		if (!Use_Permission (link_ptr->Use (), type)) {
			use_flag = true;
			continue;
		}

		time = (int) (link_ptr->Length () / speed + 0.5);

		imped = (time * value + value_round) >> value_roll;
		if (imped < 1) imped = 1;

		if (random_flag) {
			variance = (imped * random_imped + 50) / 100;
			if (variance > 0) {
				imped = (int) (imped + variance * (random.Probability () - 0.5) + 0.5);
			}
		}

		//---- initialize the anode values ----

		cum_a = org_ptr->Cum ();
		tod_a = org_ptr->TOD ();
		cost_b = cost_a = org_ptr->Cost ();
		time_a = org_ptr->Time ();

		//---- check for org and des on the same link ----

		des_flag = false;

		for (des_ptr = des_array.First (); des_ptr != NULL; des_ptr = des_array.Next ()) {
			if (org_ptr->Link () != des_ptr->Link ()) continue;

			//---- get distance ----

			offset = abs (des_ptr->Offset () - org_ptr->Offset ());

			offset_factor = ((offset << offset_roll) + (link_ptr->Length () >> 1)) / link_ptr->Length ();

			//---- calculate time ----

			tim = (time * offset_factor + offset_round) >> offset_roll;

			//---- check the time schedule ----

			tod_b = tod_a + tim;

			if (tod_b > max_tod) {
				time_flag = true;
				continue;
			}

			//---- check max distance ----

			time_b = time_a + tim;

			if (flag && time_b >= max_time) {
				length_flag = true;
				continue;
			}

			//---- check the cumulative impedance ----

			imp = (imped * offset_factor + offset_round) >> offset_roll;
			if (imp < 1) imp = 1;

			cum_b = cum_a + imp;

			if (cum_b >= max_cum || (cum_b >= des_ptr->Cum () && des_ptr->Cum ())) continue;

			des_ptr->Cum (cum_b);
			des_ptr->TOD (tod_b);
			des_ptr->Cost (cost_b);
			des_ptr->Time (time_b);
			des_ptr->Prev (-(org_array.Record_Index ()));

			des_flag = true;

			if (best_flag) {
				max_cum = cum_b;
				best_des = des_array.Record_Index ();
			}
		}

		//---- update the max cum ----

		if (des_flag && !best_flag) {
			hi_cum = 0;

			for (des_ptr = des_array.First (); des_ptr != NULL; des_ptr = des_array.Next ()) {
				if (des_ptr->Cum () > hi_cum) hi_cum = des_ptr->Cum ();
			}
			if (hi_cum > 0 && hi_cum < max_cum) max_cum = hi_cum;
		}

		//---- process each direction ----

		for (lnk=0; lnk < 2; lnk++) {

			//---- get distance ----

			if (lnk == 0) {
				offset = org_ptr->Offset ();
				bnode = link_ptr->Anode ();
			} else {
				offset = link_ptr->Length () - org_ptr->Offset ();
				bnode = link_ptr->Bnode ();
			}
			offset_factor = ((offset << offset_roll) + (link_ptr->Length () >> 1)) / link_ptr->Length ();

			//---- calculate time ----

			tim = (time * offset_factor + offset_round) >> offset_roll;

			//---- check the time schedule ----

			tod_b = tod_a + tim;

			if (tod_b > max_tod) {
				time_flag = true;
				continue;
			}

			//---- check max distance ----

			time_b = time_a + tim;

			if (flag && time_b >= max_time) {
				length_flag = true;
				continue;
			}

			//---- check the cumulative impedance ----

			imp = (imped * offset_factor + offset_round) >> offset_roll;
			if (imp < 1) imp = 1;

			cum_b = cum_a + imp;

			path_ptr = (*node_path) [bnode];

			if (cum_b >= max_cum || (cum_b >= path_ptr->Cum () && path_ptr->Cum ())) continue;

			if (path_ptr->Next () == 0 && last_ptr != path_ptr) {
				last_ptr->Next (bnode);
				last_ptr = path_ptr;
			}			
			path_ptr->Cum (cum_b);
			path_ptr->TOD (tod_b);
			path_ptr->Cost (cost_b);
			path_ptr->Time (time_b);
			path_ptr->Prev (-(org_array.Record_Index ()));
		}
	}

	//---- build the path ----

	for (;;) {
		anode = first_ptr->Next ();
		if (anode <= 0) break;

		first_ptr->Next (-1);

		first_ptr = (*node_path) [anode];

		//---- check the cumulative impedance ----

		cum_a = first_ptr->Cum ();

		if (cum_a >= max_cum) continue;

		//---- check the activity schedule ----

		tod_a = first_ptr->TOD ();

		if (tod_a >= max_tod) {
			time_flag = true;
			continue;
		}

		//---- check max distance ----

		time_a = first_ptr->Time ();

		if (flag && time_a >= max_time) {
			length_flag = true;
			continue;
		}

		cost_b = cost_a = first_ptr->Cost ();

		//---- process each link leaving the A node ----

		for (lnk = (*alist) [anode]; lnk; lnk = list_ptr->Next (ab_flag)) {

			link_ptr = link_data [lnk];

			ab_flag = (link_ptr->Anode () == anode);

			bnode = (ab_flag) ? link_ptr->Bnode () : link_ptr->Anode ();

			path_ptr = (*node_path) [bnode];

			//---- calculate the impedance ----

			list_ptr = (*list_data) [lnk];

			time = (int) (link_ptr->Length () / speed + 0.5);

			imped = (time * value + value_round) >> value_roll;
			if (imped < 1) imped = 1;

			if (random_flag) {
				variance = (imped * random_imped + 50) / 100;
				if (variance > 0) {
					imped = (int) (imped + variance * (random.Probability () - 0.5) + 0.5);
				}
			}

			//---- check for the trip end ----

			des_flag = false;

			for (des_ptr = des_array.First (); des_ptr != NULL; des_ptr = des_array.Next ()) {
				if (lnk != des_ptr->Link ()) continue;

				offset = (ab_flag) ? des_ptr->Offset () : link_ptr->Length () - des_ptr->Offset ();
				
				offset_factor = ((offset << offset_roll) + (link_ptr->Length () >> 1)) / link_ptr->Length ();

				//---- calculate the time ----

				tim = (time * offset_factor + offset_round) >> offset_roll;

				//---- check the time schedule ----

				tod_b = tod_a + tim;

				if (tod_b > max_tod) {
					time_flag = true;
					continue;
				}

				//---- check max distance ----

				time_b = time_a + tim;

				if (flag && time_b >= max_time) {
					length_flag = true;
					continue;
				}
				
				//---- check the cumulative impedance ----

				imp = (imped * offset_factor + offset_round) >> offset_roll;
				if (imp < 1) imp = 1;

				cum_b = cum_a + imp;

				if (cum_b >= max_cum || (cum_b >= des_ptr->Cum () && des_ptr->Cum ())) continue;

				des_ptr->Cum (cum_b);
				des_ptr->TOD (tod_b);
				des_ptr->Cost (cost_b);
				des_ptr->Time (time_b);
				des_ptr->Prev (anode);

				des_flag = true;

				if (best_flag) {
					max_cum = cum_b;
					best_des = des_array.Record_Index ();
				}
			}

			//---- update the max cum ----

			if (des_flag && !best_flag) {
				hi_cum = 0;

				for (des_ptr = des_array.First (); des_ptr != NULL; des_ptr = des_array.Next ()) {
					if (des_ptr->Cum () > hi_cum) hi_cum = des_ptr->Cum ();
				}
				if (hi_cum > 0 && hi_cum < max_cum) max_cum = hi_cum;
			}

			//---- check the time ----

			tod_b = tod_a + time;

			if (tod_b >= max_tod) {
				time_flag = true;
				continue;
			}

			//---- check max distance ----

			time_b = time_a + time;

			if (flag && time_b >= max_time) {
				length_flag = true;
				continue;
			}

			//---- check the cumulative impedance at B ----

			cum_b = cum_a + imped;

			if (cum_b >= max_cum || (cum_b >= path_ptr->Cum () && path_ptr->Cum ())) continue;

			if (path_ptr->Next () == -1) {
				path_ptr->Next (first_ptr->Next ());
				first_ptr->Next (bnode);
			} else if (path_ptr->Next () == 0 && last_ptr != path_ptr) {
				last_ptr->Next (bnode);
				last_ptr = path_ptr;
			}
			path_ptr->Cum (cum_b);
			path_ptr->TOD (tod_b);
			path_ptr->Cost (cost_b);
			path_ptr->Time (time_b);
			path_ptr->Prev (anode);
		}
	}
	return (best_des);
}
