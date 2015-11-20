//*********************************************************
//	Transit_Plan.cpp - Build a Transit Plan
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Transit_Plan
//---------------------------------------------------------

int Router::Transit_Plan (void)
{
	Path_Data *path_ptr, path_root;

	if (trip_org.ID () == trip_des.ID ()) {
		trip_des.TOD (trip_org.TOD ());
		return (0);
	}
	if (!transit_net) return (Set_Problem (MODE_PROBLEM));

	//---- initialize the path data ----

	for (int i=0; i <= max_paths; i++) {
		node_path [i].Zero_Fill ();
		loc_path [i].Zero_Fill ();
		if (i < max_paths) {
			board_path [i].Zero_Fill ();
			alight_path [i].Zero_Fill ();
		}
	}

	//---- add the origin to the activity list ----

	first_ptr = &path_root;

	first_ptr->Next (trip_org.Rec ());
	first_ptr->Type (LOCATION_ID);
	first_ptr->Xfer (0);
	first_ptr->Mode (NO_TRANSIT);

	path_ptr = loc_path [0] [trip_org.Rec ()];

	path_ptr->Cum (trip_org.Cum ());
	path_ptr->TOD (trip_org.TOD ());
	path_ptr->Cost (trip_org.Cost ());
	path_ptr->Time (trip_org.Time ());
	path_ptr->Prev (0);

	last_ptr = path_ptr;

	//---- build the path ----

	int stat = Build_Transit_Legs ();

	if (stat) return (stat);

	//---- identify the origin and destination of each leg ----

	stat = Set_Leg_Trip_Ends ();

	if (stat) return (stat);

	return (Save_Transit_Legs ());
}

//---------------------------------------------------------
//	Build_Transit_Legs
//---------------------------------------------------------

int Router::Build_Transit_Legs (void)
{
	int next, type, xfer, mode;

	for (;;) {
		next = first_ptr->Next ();
		if (next <= 0) break;

		first_ptr->Next (-1);

		type = first_ptr->Type ();
		xfer = first_ptr->Xfer ();
		mode = first_ptr->Mode ();

		switch (type) {
			case NODE_ID:
				Walk_Access (next, xfer, mode);
				break;
			case LOCATION_ID:
				Location_Access (next, xfer, mode);
				break;
			case STOP_ID: 
				Transit_Path (next, xfer, mode);
				break;
			default:
				break;
		}
	}

	//---- check the destination ----

	if (trip_des.Cum () == MAX_INTEGER) {
		return (Set_Transit_Error ());
	}
	return (0);
}

//---------------------------------------------------------
//	Set_Transit_Error
//---------------------------------------------------------

int Router::Set_Transit_Error (void)
{
	if (length_flag) {
		return (Set_Problem (WALK_PROBLEM));
	} else if (wait_time_flag) {
		return (Set_Problem (WAIT_PROBLEM));
	} else if (time_flag) {
		return (Set_Problem (TIME_PROBLEM));
	} else if (use_flag) {
		return (Set_Problem (LINK_PROBLEM));
	}
	return (Set_Problem (PATH_PROBLEM));
}

//---------------------------------------------------------
//	Set_Leg_Trip_Ends
//---------------------------------------------------------

int Router::Set_Leg_Trip_Ends (void)
{
	int rec, path, layer, count;

	Location_Data *loc_ptr;
	Path_Data *path_ptr, path_root;
	Trip_End_Data trip_end;

	//---- identify the origin and destination of each leg ----

	org_array.Reset ();
	des_array.Reset ();

	if (trip_des.Prev () <= 0) return (Set_Problem (PATH_PROBLEM));

	path_ptr = &path_root;

	path_ptr->Prev (trip_des.Prev ());
	path_ptr->Layer (trip_des.Layer ());
	path_ptr->Path (trip_des.Path ());

	des_array.Add (&trip_des);
	count = 0;

	for (;;) { 
		rec = path_ptr->Prev ();

		if (rec <= 0) break;

		//---- check for infinite loops ---

		if (count++ == leg_check) return (Set_Problem (PATH_PROBLEM));

		//---- process the next leg ----

        layer = path_ptr->Layer ();
		path = path_ptr->Path ();

		if (layer == NODE_ID) {
			path_ptr = node_path [path] [rec];
			continue;
		} else if (layer == STOP_ID) {
			path_ptr = alight_path [path] [rec];

			rec = path_ptr->Prev () / nroute;

			path_ptr = board_path [path] [rec];

			rec = path_ptr->Prev ();
			path = path_ptr->Path ();
		}
		path_ptr = loc_path [path] [rec];
		loc_ptr = location_data [rec];

		trip_end.Rec (rec);
		trip_end.ID (loc_ptr->ID ());
		trip_end.Type (LOCATION_ID);
		trip_end.Link (loc_ptr->Link ());
		trip_end.Offset (loc_ptr->Offset ());
		trip_end.Cum (path_ptr->Cum ());
		trip_end.TOD (path_ptr->TOD ());
		trip_end.Cost (path_ptr->Cost ());
		trip_end.Prev (path_ptr->Prev ());
		trip_end.Layer (path_ptr->Layer ());
		trip_end.Path (path_ptr->Path ());

		org_array.Add (&trip_end);

		if (path_ptr->Prev () <= 0) break;

		des_array.Add (&trip_end);
	}
	return (0);
}

//---------------------------------------------------------
//	Save_Transit_Legs
//---------------------------------------------------------

int Router::Save_Transit_Legs (void)
{
	int board, alight, route_id, layer, path, stat, rec, tim, wait, wait_imp, value_waiting, penalty;

	Line_Data *line_ptr;
	Stop_Data *stop_ptr;
	Path_Data *path_ptr, *alight_ptr;
	Trip_End_Data *org_ptr, *des_ptr, board_end, alight_end;

	//---- save each leg ----

	for (org_ptr = org_array.Last (), des_ptr = des_array.Last (); 
		des_ptr != NULL && org_ptr != NULL; 
		des_ptr = des_array.Previous (), org_ptr = org_array.Previous ()) {

		rec = des_ptr->Prev ();
		layer = des_ptr->Layer ();
		path = des_ptr->Path ();

		if (layer == STOP_ID) {

			//---- process a transit leg ----

			alight_ptr = alight_path [path] [rec];

			alight = stop_data [rec]->ID ();

			line_ptr = line_data [alight_ptr->Prev () % nroute];

			route_id = line_ptr->Route ();
			board = alight_ptr->Prev () / nroute;
			wait = alight_ptr->Next () / 10000;
			value_waiting = alight_ptr->Next () % 10000;

			path_ptr = board_path [path] [board];

			stop_ptr = stop_data [board];
			board = stop_ptr->ID ();

			//----- retrieve the wait time ----

			tim = alight_ptr->TOD () - path_ptr->TOD ();

			if (wait >= tim) {
				ntransit++;
				wait = tim / 2;
			}
			wait_imp = ((wait * value_waiting + value_round) >> value_roll);

			if (!wait_leg_flag) {
				path_ptr->Cum (path_ptr->Cum () + wait_imp);
				path_ptr->TOD (path_ptr->TOD () + wait);
			}
			if (attribute_flag && (stop_imped > 0 || station_imped > 0)) {
				if (stop_ptr->Type () == STOP) {
					penalty = stop_imped;
				} else {
					penalty = station_imped;
				}
			} else {
				penalty = 0;
			}

			//---- save the boarding leg ----

			board_end.ID (board);
			board_end.Type (STOP_ID);
			board_end.Cum (path_ptr->Cum ());
			board_end.TOD (path_ptr->TOD ());
			board_end.Cost (path_ptr->Cost ());
			board_end.Prev (0);

			if (!walk_detail && walk_active) {
				stat = Save_Plan (&walk_org, &board_end);
			} else {
				stat = Save_Plan (org_ptr, &board_end);
			}
			if (stat) {
				ntransit++;
				return (stat);
			}

			//---- save the in-vehicle leg ----

			alight_end.ID (alight);
			alight_end.Type (STOP_ID);
			alight_end.Cum (alight_ptr->Cum ());
			alight_end.TOD (alight_ptr->TOD ());
			alight_end.Cost (alight_ptr->Cost ());
			alight_end.Prev (0);

			stat = Save_Plan (&board_end, &alight_end, Plan_File::TRANSIT_MODE, route_id, 0, wait, penalty);
			if (stat) return (stat);

			//---- save the alighting leg ----

			if (walk_detail) {
				stat = Save_Plan (&alight_end, des_ptr);

				if (stat) {
					ntransit++;
					return (stat);
				}
			} else {
				walk_active = true;
				walk_org = alight_end;
			}
			continue;
		}

		//---- save a walk leg ----

		if (walk_detail) {
			stat = Save_Plan (org_ptr, des_ptr);
			if (stat) return (stat);
		} else if (!walk_active) {
			walk_active = true;
			walk_org = *org_ptr;
		}
	}
	return (0);
}
