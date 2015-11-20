//*********************************************************
//	Read_Route.cpp - Read Router Header and Node Files
//*********************************************************

#include "TransitNet.hpp"

//---------------------------------------------------------
//	Read_Route
//---------------------------------------------------------

void TransitNet::Read_Route (void)
{
	int i, node, node0, route, veh_type, mode, begin, end, num_in, tot_time, spd_time;
	int run_time, cum_time, offset, headway, speed, dwell;
	double ttime, factor;
	bool first, stop_flag;
	Use_Type use;

	AB_Data *ab_ptr;
	Dir_Path_Data *dir_ptr, *last_dir;
	Route_Node rnode_rec, *rnode_ptr;
	Driver_Data *driver_ptr;
	Path_Node *path_ptr;
	Route_Data *route_ptr;

	//---- read the route file ----

	Show_Message ("Reading Route Header and Nodes Files -- Record");
	Set_Progress (100);
	num_in = 0;

	for (route_ptr = route_data.First (); route_ptr; route_ptr = route_data.Next ()) {
		Show_Progress ();

		route_node.Reset ();

		first = true;
		begin = end = 0;

		route = route_ptr->Route ();
		mode = route_ptr->Mode ();

		use = (mode <= EXPRESS_BUS) ? BUS : RAIL;

		run_time = Round (route_ptr->Min_Time ());

		for (i=1; i <= num_periods; i++) {
			headway = route_ptr->Headway (i);
			if (headway <= 0) continue;

			offset = route_ptr->Offset (i);

			if (offset < 0) {
				offset = (int) (headway * random.Probability () + 0.5);
				route_ptr->Offset (i, offset);
			}
		}

		//---- read the node records ----

		rnode_rec.Dwell (0);
		rnode_rec.TTime (0);
		node0 = 0;

		for (i=1; i <= route_ptr->Nodes (); i++) {

			node = route_ptr->Node (i);

			if (node < 0) {
				stop_flag = false;
				node = -node;
			} else {
				stop_flag = true;
			}

			if (dwell_flag) {
				dwell = route_ptr->Dwell (i);
			} else {
				if (stop_flag) {
					dwell = min_dwell;
				} else {
					dwell = 0;
				}
			}
			ttime = route_ptr->Time (i);
			speed = route_ptr->Speed (i);

			//---- check the node file ----

			if (node_data.Get_Index (node) > 0) {
				if (first) {
					rnode_rec.Node (node);
					rnode_rec.Dir (0);

					rnode_rec.Add_Dwell (Round (dwell));
					rnode_rec.Add_TTime (Round (ttime));

					if (!route_node.Add (&rnode_rec)) {
						Error ("Insufficient Memory for Route Nodes");
					}
				} else {

					//---- find the link(s) between route nodes ----

					ab_ptr = ab_key.Get (node0, node);

					if (ab_ptr == NULL) {

						//---- attempt to build a minimum distance path ----

						Node_Path (node0, node, use);

						if (path_node.Num_Records () == 0) {

							//---- try to build an auto path ----

							Node_Path (node0, node, CAR);

							if (path_node.Num_Records () != 0) {
								Warning ("Path from %d and %d on Route %d includes Use Restrinctions", node0, node, route);
							}
						}

						if (path_node.Num_Records () == 0) {
							Warning ("No Path between Nodes %d and %d on Route %d", node0, node, route);
							break;
						} else {

							//---- calculate the network travel time for the path ----

							tot_time = spd_time = 0;

							for (path_ptr = path_node.First (); path_ptr; path_ptr = path_node.Next ()) {
								dir_ptr = dir_data [path_ptr->Dir ()];

								tot_time += dir_ptr->Time0 ();

								if (speed_flag && speed > 0) {
									spd_time += Round ((double) dir_ptr->Length () / speed);
								}
							}

							//---- check the transit travel time ----

							if (ttime == 0.0) {
								if (spd_time > 0) {
									ttime = UnRound (spd_time) + dwell;
									tot_time = spd_time;
								} else {
									ttime = UnRound (tot_time) + dwell;
								}
							} else if (ttime < UnRound (tot_time)) {
								Warning ("Route %d Node %d to %d travel time %.1lf < %.1lf", route, 
									node0, node, ttime, UnRound (tot_time));
							}

							//---- distribute the transit travel time to each link on the path ----

							if (tot_time < 1) tot_time = 1;
							factor = ttime / UnRound (tot_time);

							for (path_ptr = path_node.Last (); path_ptr; path_ptr = path_node.Previous ()) {
								rnode_rec.Node (path_ptr->Node ());
								rnode_rec.Dir (path_ptr->Dir ());

								dir_ptr = dir_data [path_ptr->Dir ()];

								ttime = dir_ptr->Time0 () * factor;

								rnode_rec.TTime ((int) (ttime + 0.5));
								rnode_rec.Dwell (Round (dwell));

								if (!route_node.Add (&rnode_rec)) {
									Error ("Insufficient Memory for Route Nodes");
								}
							}
						}

					} else {

						//---- calculate and check the transit travel time ----

						rnode_rec.Node (node);
						rnode_rec.Dir (ab_ptr->dir);

						dir_ptr = dir_data [ab_ptr->dir];

						if (!Use_Permission (dir_ptr->Use (), use)) {
							Warning ("Link %d does Not Permit %s Service", 
								dir_ptr->Link (), Transit_Code ((Transit_Type) mode));
						}
						if (ttime == 0.0) {
							if (speed_flag && speed > 0) {
								ttime = (double) dir_ptr->Length () / speed + dwell;
							} else {
								ttime = UnRound (dir_ptr->Time0 ()) + dwell;
							}
						} else if (ttime < UnRound (dir_ptr->Time0 ())) {
							Warning ("Route %d Link %d travel time %.1lf < %.1lf", route, 
								dir_ptr->Link (), ttime, UnRound (dir_ptr->Time0 ()));
						}
						rnode_rec.Add_Dwell (Round (dwell));
						rnode_rec.Add_TTime (Round (ttime));

						if (!route_node.Add (&rnode_rec)) {
							Error ("Insufficient Memory for Route Nodes");
						}
					}
				}
				rnode_rec.Dwell (0);
				rnode_rec.TTime (0);
				first = false;
				end = 0;
				node0 = node;

			} else {

				//---- remove route nodes that were collapsed from the network ----

				if (first) {
					begin++;
				} else {
					rnode_rec.Add_Dwell (Round (dwell));
					rnode_rec.Add_TTime (Round (ttime));
				}
				end++;
			}
		}

		if (route_node.Num_Records () <= 1) {
			Warning ("Route %d has Missing Node Records", route);
			continue;
		}
		if (begin) {
			Print (1, "%d Node%sat the Beginning of Route %d did Not Exist", 
					begin, ((begin > 1) ? "s " : " "), route);
			end_warnings++;
		}
		if (end) {
			Print (1, "%d Node%sat the End of Route %d did Not Exist", 
					end, ((end > 1) ? "s " : " "), route);
			end_warnings++;
		}

		//---- add route to the edit list ----

		if (!edit_list.Add (route)) {
			Error ("Adding Route to the Edit List");
		}

		//---- check for an existing route ----

		driver_ptr = driver_data.Get (route);

		if (driver_ptr == NULL) {
			driver_ptr = driver_data.New_Record (true);
			driver_ptr->Route (route);
			if (!driver_data.Add ()) {
				Error ("Adding Transit Driver Data");
			}
			driver_ptr = driver_data.Record ();
		}

		//---- create the driver data ---

		driver_ptr->Links (route_node.Num_Records () - 1);

		switch (mode) {
			case LOCAL_BUS:
				veh_type = 4;
				break;
			case EXPRESS_BUS:
				veh_type = 5;
				break;
			case TROLLEY:
				veh_type = 6;
				break;
			case STREETCAR:
				veh_type = 7;
				break;
			case LRT:
				veh_type = 8;
				break;
			case RAPIDRAIL:
				veh_type = 9;
				break;
			case REGIONRAIL:
				veh_type = 10;
				break;
		}
		driver_ptr->Type (veh_type);
		driver_ptr->Sub_Type (0);

		//---- process each route node ----

		node0 = 0;
		last_dir = NULL;
		cum_time = 0;

		for (i=0, rnode_ptr = route_node.First (); rnode_ptr; rnode_ptr = route_node.Next (), i++) {

			node = rnode_ptr->Node ();

			if (node0 == 0) {
				node0 = node;
				continue;
			}
			ab_ptr = ab_key.Get (node0, node);

			if (ab_ptr == NULL) {
				Error ("No Link between Nodes %d and %d on Route %d", node0, node, route);
			}
			node0 = node;
			rnode_ptr->Dir (ab_ptr->dir);

			dir_ptr = dir_data [ab_ptr->dir];

			//---- check the travel time ----

			if (rnode_ptr->TTime () == 0) {
				rnode_ptr->TTime (dir_ptr->Time0 () + rnode_ptr->Dwell ());
			}
			cum_time += rnode_ptr->TTime ();

			//---- save the driver link ----

			driver_ptr->Link_Dir (i, dir_ptr->Link_Dir ());

			//---- check the connection ----

			if (last_dir != NULL && connect_flag) {
				if (connect_data.Get (last_dir->Link_Dir (), dir_ptr->Link_Dir ()) == NULL) {
					Warning ("No Connection from Link %d to %d on Route %d", last_dir->Link (), dir_ptr->Link (), route);
					connect_warnings++;
				}
			}
			last_dir = dir_ptr;
		}

		//---- adjust the link travel times to match the run time ----

		if (run_time > 0 && cum_time > 0 && run_time != cum_time) {
			ttime = (double) run_time / cum_time;

			cum_time = 0;

			for (rnode_ptr = route_node.First (); rnode_ptr; rnode_ptr = route_node.Next ()) {
				cum_time += run_time = (int) (rnode_ptr->TTime () * ttime + 0.5);
				rnode_ptr->TTime (run_time);
			}
		}
		route_ptr->Min_Time (Resolve (cum_time));

		//---- save the transit route -----

		Save_Route (route_ptr);
		num_in++;
	}
	End_Progress ();

	Print (1, "Number of Transit Routes = %d", num_in);
}
