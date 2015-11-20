//*********************************************************
//	Save_Plan.cpp - Save a Plan Leg
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Save_Plan
//---------------------------------------------------------

int Router::Save_Plan (Trip_End_Data *org, Trip_End_Data *des, int mode, int mode_id, int passengers, int wait, int penalty)
{
	int id, tim, imp, cost, ndata, *data, *next, max_size, anode, bnode, lnk;
	bool path_flag, driver_flag, transit_flag, ab_flag, xfer_flag;

	Path_Data *path_ptr, path_root;
	TTime_Data *ttime_ptr;
	Link_Data *link_ptr;

	xfer_flag = driver_flag = path_flag = transit_flag = false;
	bnode = 0;

	if (mode == Plan_File::WALK_MODE) {
		xfer_flag = walk_active;
		walk_active = false;
		driver_flag = false;
		path_flag = walk_detail;
		transit_flag = false;
		bnode = 0;
	} else if (mode == Plan_File::BIKE_MODE) {
		driver_flag = false;
		path_flag = true;
		transit_flag = false;
		bnode = 0;
	} else if (mode == Plan_File::AUTO_MODE) {
		driver_flag = true;
		path_flag = true;
		transit_flag = false;
	} else if (mode == Plan_File::TRANSIT_MODE) {
		driver_flag = false;
		path_flag = false;
		transit_flag = true;
	}

	//---- check the origin and destination ----

	if (des->ID () == org->ID () && des->Type () == org->Type () && !xfer_flag) return (0);

	//---- save the plan record ----

	imp = des->Cum () - org->Cum ();
	tim = des->TOD () - org->TOD ();
	cost = des->Cost () - org->Cost ();

	if (imp < 0 || tim < 0 || cost < 0) {
		if (transit_flag) ntransit++;
		trip_org = *org;
		return (Set_Problem (TIME_PROBLEM));
	}
	plan_file.Leg (plan_file.Leg () + 1);
	plan_file.Time (Resolve (org->TOD ()));
	plan_file.Start_ID (org->ID ());
	plan_file.Start_Type (org->Type ());
	plan_file.End_ID (des->ID ());
	plan_file.End_Type (des->Type ());
	plan_file.Duration (Resolve (tim));
	plan_file.Stop_Time (Resolve (des->TOD ()));
	plan_file.Cost (cost);
	plan_file.GCF (Resolve (imp));
	plan_file.Driver_Flag ((driver_flag) ? 1 : 0);
	plan_file.Mode (mode);

	if (attribute_flag) {
		attribute_data.impedance += imp;

		if (mode == Plan_File::WALK_MODE) {
			attribute_data.walk += tim;
		} else if (mode == Plan_File::AUTO_MODE) {
			attribute_data.drive += tim;
		} else if (mode == Plan_File::TRANSIT_MODE) {
			attribute_data.transit += tim;

			if (attribute_data.num_xfer > 0) {
				attribute_data.xfer_wait += wait;
				attribute_data.penalty += penalty + xfer_imped;
			} else {
				attribute_data.first_wait += wait;
				attribute_data.penalty += penalty;
			}
			attribute_data.num_xfer++;
		}
		attribute_data.cost += cost;
	}

	//---- add nodes to the plan record ----

	if (des->Prev () > 0 && path_flag) {

		data = plan_file.Data (&max_size);
		max_size--;

		next = data + max_size;
		ndata = 0;

		path_ptr = &path_root;
		path_ptr->Prev (des->Prev ());
		path_ptr->Layer (des->Layer ());
		path_ptr->Path (des->Path ());

		for (;;) {
			id = path_ptr->Prev ();
			
			if (id <= 0) break;
			
			if (driver_flag) {
				path_ptr = link_path [id];

				ttime_ptr = ttime_data [id];

				//---- update the volume ----

				if (update_flag) {
					if (path_ptr->Prev () > 0) {
						tim = link_path [path_ptr->Prev ()]->TOD ();
					} else {
						tim = org->TOD ();
					}
					tim = ttime_data.Period (tim);

					if (ttime_ptr->Periods () == 0) {
						if (!ttime_ptr->Periods (ttime_data.Periods ())) {
							Error ("Insufficient Memory for Link Volume Data");
						}
					}
					ttime_ptr->Add_Volume (tim, 1);
				}

				//---- set the external value ----

				id = ttime_ptr->Link ();

				if (node_flag) {
					if (ttime_ptr->Dir () == 0) {	//---- AB -----
						id = link_data [id]->Bnode ();
					} else {			//---- BA ----
						id = link_data [id]->Anode ();
					}
					id = node_data [id]->Node ();
				} else {
					if (ttime_ptr->Dir () == 0) {	//---- AB -----
						id = link_data [id]->ID ();
					} else {			//---- BA ----
						id = -(link_data [id]->ID ());
					}
				}

			} else {
				if (path_ptr->Layer () != NODE_ID) break;

				path_ptr = node_path [path_ptr->Path ()] [id];

				if (node_flag) {
					id = node_data [id]->Node ();
				} else {
					anode = id;
					if (bnode > 0) {
						if (mode == Plan_File::WALK_MODE) {
							for (lnk = walk_list [anode]; lnk; lnk = walk_link [lnk]->Next (ab_flag)) {
								link_ptr = link_data [lnk];
								ab_flag = (link_ptr->Anode () == anode);

								if (bnode == ((ab_flag) ? link_ptr->Bnode () : link_ptr->Anode ())) {
									id = link_ptr->ID ();
									if (!ab_flag) id = -id;
									break;
								}
							}
						} else {
							for (lnk = bike_list [anode]; lnk; lnk = bike_link [lnk]->Next (ab_flag)) {
								link_ptr = link_data [lnk];
								ab_flag = (link_ptr->Anode () == anode);

								if (bnode == ((ab_flag) ? link_ptr->Bnode () : link_ptr->Anode ())) {
									id = link_ptr->ID ();
									if (!ab_flag) id = -id;
									break;
								}
							}
						}
					} else {
						link_ptr = link_data [des->Link ()];
						if (anode == link_ptr->Bnode ()) {
							id = -(link_ptr->ID ());
						} else {
							id = link_ptr->ID ();
						}
					}
					bnode = anode;
				}
			}
			if (++ndata >= max_size) {
				trip_org = *org;
				return (Set_Problem (SIZE_PROBLEM));
			}
			*next-- = id;
		}
		if (driver_flag) {
			if (node_flag) ndata--;
			if (ndata < 1) return (Set_Problem (NODE_PROBLEM));

			ndata += 2;
			*next-- = passengers;		//---- number of passengers ----
			*next-- = mode_id;			//---- vehicle id ----
			ntrips++;
		} else if (!transit_flag && !node_flag && org->Link () > 0) {
			link_ptr = link_data [org->Link ()];
			if (bnode > 0) {
				if (bnode == link_ptr->Anode ()) {
					id = -(link_ptr->ID ());
				} else {
					id = link_ptr->ID ();
				}
			} else {
				if (org->Offset () > des->Offset ()) {
					id = -(link_ptr->ID ());
				} else {
					id = link_ptr->ID ();
				}
			}
			ndata++;
			*next-- = id;
		}
		plan_file.Tokens (ndata);

		if (ndata > 0) {
			memmove (data, ++next, ndata * sizeof (int));
		}
	} else if (driver_flag) {
		return (Set_Problem (NODE_PROBLEM));
	} else if (transit_flag) {
		plan_file.Tokens (1);
		plan_file.Route (mode_id);
	} else {
		plan_file.Tokens (0);
	}

	//---- save the plan ----

	if (save_plans && !plan_file.Write ()) {
		Error ("Writing Plan File");
	}

	//---- update travel times ----

	if (driver_flag && update_flag) {
		if (!(++num_trips % update_rate)) {
			Update_Travel_Times ();
		}
	}
	return (0);
}
