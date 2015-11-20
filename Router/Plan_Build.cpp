//*********************************************************
//	Plan_Build - Build a Path and Plan records
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Plan_Build
//---------------------------------------------------------

void Router::Plan_Build (Trip_Data *trip_ptr, int type, int duration)
{
	int i, start, arrive, mode, veh_id, stat, time;
	double prob;
	bool skip = false;

	Location_Data *loc_ptr;
	Vehicle_Data *veh_ptr = 0;
	XYZ_Point point;
	start = mode = 0;

	//---- check the household list ----

	if (hhold_flag) {
		value_walk = walk_value.Best (type);
		value_bike = bike_value.Best (type);
		value_time = time_value.Best (type);
		value_wait = wait_value.Best (type);
		value_xfer = xfer_value.Best (type);
		value_distance = distance_value.Best (type);
		value_cost = cost_value.Best (type);
		left_imped = imped_left.Best (type);
		right_imped = imped_right.Best (type);
		uturn_imped = imped_uturn.Best (type);
		xfer_imped = imped_xfer.Best (type);
		stop_imped = imped_stop.Best (type);
		station_imped = imped_station.Best (type);
		bus_bias = bias_bus.Best (type);
		bus_constant = add_bus.Best (type);
		rail_bias = bias_rail.Best (type);
		rail_constant = add_rail.Best (type);
		max_walk = walk_max.Best (type);
		max_bike = bike_max.Best (type);
		max_wait = wait_max.Best (type);
		min_wait = wait_min.Best (type);
		max_transfers = xfer_max.Best (type);
		max_paths = path_max.Best (type);
		max_parkride = parkride_max.Best (type);
		max_kissride = kissride_max.Best (type);
		kissride_factor = factor_kissride.Best (type);

		walk_flag = (max_walk != 0);
		bike_flag = (max_bike != 0);
		wait_flag = (max_wait != 0);
		rail_bias_flag = (rail_bias != 100 || rail_constant != 0);
		bus_bias_flag = (bus_bias != 100 || bus_constant != 0);
		turn_flag = (left_imped > 0 || right_imped > 0 || uturn_imped > 0);
	}
	if (park_hours_flag) {
		if (duration > 0) {
			park_hours = Round (duration / 3600.0);
		} else {
			park_hours = hours_park [trip_ptr->Purpose ()];
		}
	} else {
		park_hours = Round (duration / 3600.0);
	}
	if (fare_class_flag) {
		prob = random.Probability ();

		for (i=0; i <= SPECIAL; i++) {
			if (prob <= fare_class [i]) {
				traveler_class = i;
				break;
			}
		}
	}

	//---- copy old plans ----

	if (old_plan_flag) {
		while (old_plan_file.Traveler () < trip_ptr->Traveler () ||
			(old_plan_file.Traveler () == trip_ptr->Traveler () && 
			old_plan_file.Trip () < trip_ptr->Trip ())) {

			if (old_plan_file.Traveler () > 0) {
				if (!skim_flag || (skim_flag && Update_Plan ())) {
					if (!plan_file.Write (old_plan_file.Plan ())) {
						Error ("Writing Plan File");
					}
				}
			}
			if (!old_plan_file.Read ()) {
				old_plan_flag = false;
				old_plan_file.Close ();
				break;
			}
		}
		if (old_plan_flag) {
			while (old_plan_file.Traveler () == trip_ptr->Traveler () &&
				old_plan_file.Trip () == trip_ptr->Trip ()) {

				if (!old_plan_file.Read ()) {
					old_plan_flag = false;
					old_plan_file.Close ();
					break;
				}
			}
		}
	}
	
	//---- initialize the plan ----
	
	plan_file.Traveler (trip_ptr->Traveler ());
	plan_file.Trip (trip_ptr->Trip ());
	plan_file.Leg (0);

	nprocess++;
	tot_trips++;

	memset (&trip_org, '\0', sizeof (trip_org));
	memset (&trip_des, '\0', sizeof (trip_des));

	if (attribute_flag) {
		Write_Attribute ();

		attribute_data.traveler = trip_ptr->Traveler ();
		attribute_data.trip = trip_ptr->Trip ();
	}

	//---- set the origin ----

	loc_ptr = location_data.Get (trip_ptr->Origin ());

	if (loc_ptr == NULL) {
		stat = Set_Problem (LOCATION_PROBLEM);
		goto path_problem;
	}
	trip_org.Rec (location_data.Record_Index ());
	trip_org.ID (trip_ptr->Origin ());
	trip_org.Link (loc_ptr->Link ());
	trip_org.Offset (loc_ptr->Offset ());
	trip_org.Type (LOCATION_ID);

	start = Round (trip_ptr->Start_Time ());

	trip_org.TOD (start);
	trip_org.Cum (1);

	//---- set the destination ----

	loc_ptr = location_data.Get (trip_ptr->Destination ());

	if (loc_ptr == NULL) {
		stat = Set_Problem (LOCATION_PROBLEM);
		goto path_problem;
	}
	trip_des.Rec (location_data.Record_Index ());
	trip_des.ID (trip_ptr->Destination ());
	trip_des.Link (loc_ptr->Link ());
	trip_des.Offset (loc_ptr->Offset ());
	trip_des.Type (LOCATION_ID);
	trip_des.Cum (MAX_INTEGER);

	arrive = Round (trip_ptr->End_Time ());

	if (ignore_time) {
		max_tod = MAX_INTEGER;
	} else {
//			max_tod = (arrive + max_end_time [purpose]) << RESOLUTION;
		max_tod = arrive + end_time;
	}

	//---- check the path mode ----

	stat = 0;
	mode = trip_ptr->Mode ();

	if (mode_flag) {
		mode = new_mode;
	}

	//---- get the vehicle record ----

	if (mode == DRIVE_ALONE || mode == PNR_OUT || mode == PNR_IN || 
		mode == CARPOOL2 || mode == CARPOOL3 || mode == CARPOOL4 ||
		mode == KNR_OUT || mode == KNR_IN) {

		if (dump_flag) {
			link_path.Zero_Fill ();
		}
		veh_id = trip_ptr->Vehicle ();

		if (ignore_veh_flag) {
			veh_ptr = &default_vehicle;
			if (veh_id <= 0) veh_id = 1;
		} else {
			if (veh_id <= 0 || (veh_ptr = vehicle_data.Get (veh_id)) == NULL) {
				stat = Set_Problem (VEHICLE_PROBLEM);
				goto path_problem;
			}
		}

		//---- reposition the vehicle ----

		if (veh_flag || ignore_veh_flag) {
			List_Data *list_ptr = parking_access [trip_org.Rec ()];
			
			if (list_ptr != NULL) {
				Access_Data *access_ptr = access_data [list_ptr->From_List ()];

				if (access_ptr != NULL) {
					veh_ptr->Location (access_ptr->To_ID ());
				}
			}
		} else if (reset_veh_flag) {
			veh_ptr->Location (vehicle_home [vehicle_data.Record_Index ()]);
		}
	} else {
		veh_id = 0;
	}

	//---- build the path ----

	time_flag = dist_flag = length_flag = zero_flag = wait_time_flag = use_flag = reset_veh_flag = false;

	switch (mode) {
		case ALL_WALK:		//---- walk ----
			stat = Node_Plan (WALK);
			break;
		case DRIVE_ALONE:		//---- drive ----
			stat = Drive_Plan ((Use_Type) veh_ptr->Type (), veh_ptr);
			break;
		case TRANSIT:		//---- walk to transit ----
			//bias_flag = false;
			stat = Transit_Plan ();
			break;
		case RAIL_TRANSIT:		//---- walk to rail ----
			stat = Transit_Plan ();
			break;
		case PNR_OUT:		//---- drive to transit ----
			stat = Park_Ride_Plan (veh_ptr);
			break;
		case PNR_IN:		//---- transit to drive ----
			stat = Park_Ride_Return (veh_ptr);
			break;
		case BICYCLE:		//---- bike ----
			stat = Node_Plan (BIKE);
			break;
		case MAGIC_MOVE:		//---- magic move ----
			stat = Magic_Move (arrive, 2);
			break;
		case SCHOOL_BUS:		//---- school bus ----
			stat = Magic_Move (arrive, 1);
			break;
		case CARPOOL2:			//---- carpool 2+ ----
			stat = Drive_Plan (HOV2, veh_ptr);
			break;
		case CARPOOL3:			//---- carpool 3+ ----
			stat = Drive_Plan (HOV3, veh_ptr);
			break;
		case CARPOOL4:			//---- carpool 4+ ----
			stat = Drive_Plan (HOV4, veh_ptr);
			break;
		case KNR_OUT:			//---- ride to transit ----
			stat = Kiss_Ride_Plan (veh_ptr);
			break;
		case KNR_IN:			//---- transit to ride ----
			stat = Kiss_Ride_Return (veh_ptr);
			break;
		default:
			stat = Set_Problem (MODE_PROBLEM);
			break;
	}
	if (!stat && !walk_detail && walk_active) {
		stat = Save_Plan (&walk_org, &trip_des, Plan_File::WALK_MODE);
	}
	if (stat) {
		if (!ignore_errors) goto path_problem;
		if (veh_id > 0) {
			veh_ptr->Location (0);
			List_Data *park_ptr = parking_access [trip_des.Rec ()];
			if (park_ptr != NULL) {
				Access_Data *acc_ptr = access_data [park_ptr->To_List ()];
				if (acc_ptr != NULL) {
					veh_ptr->Location (acc_ptr->From_ID ());
				}
			}
			//veh_ptr->Location (access_data [parking_access [trip_des.Rec ()]->To_List ()]->From_ID ());
		}
		if (Magic_Move (start, 2)) {
			skip = true;
			goto path_problem;
		}
		nmagic++;
	} else if (attribute_flag) {
		Write_Attribute ();
	}

path_problem:
	if (stat) {
		if (!ignore_errors) {
			skip = true;
		}
		if (problem_flag) {
			problem_file.Time (trip_time.Format_Step (trip_ptr->Start_Time ()));
			problem_file.Link (0);
			problem_file.Household (trip_ptr->Household ());
			problem_file.Person (trip_ptr->Person ());
			problem_file.Trip (trip_ptr->Trip ());
			problem_file.Mode (trip_ptr->Mode ());
			problem_file.Problem (stat);
			problem_file.Start (trip_time.Format_Step (trip_ptr->Start_Time ()));
			problem_file.Origin (trip_ptr->Origin ());
			problem_file.Arrive (trip_time.Format_Step (trip_ptr->End_Time ()));
			problem_file.Destination (trip_ptr->Destination ());

			problem_file.Write ();

			//---- arcview problem dump ----

			if (!dump_flag || (dump_type != 0 && dump_type != stat)) return;
			if (sel_dump_flag) {
				if (percent_dump > Round (random.Probability () * 100.0)) return;
			}
			dump_file.Copy_Fields (&problem_file);

			dump_file.parts.Reset ();
			dump_file.points.Reset ();
			dump_file.parts.Add (0);
			if (!dump_file.points.Max_Points (2)) goto dump_error;

			loc_ptr = location_data.Get (trip_ptr->Origin ());

			point.x = UnRound (loc_ptr->X ());
			point.y = UnRound (loc_ptr->Y ());

			if (!dump_file.points.Add (&point)) goto dump_error;

			loc_ptr = location_data.Get (trip_ptr->Destination ());

			point.x = UnRound (loc_ptr->X ());
			point.y = UnRound (loc_ptr->Y ());

			if (!dump_file.points.Add (&point)) goto dump_error;

			if (!dump_file.Write_Record ()) goto dump_error;

			if (mode == DRIVE_ALONE) {
				int anode, bnode;
				Link_Data *link_ptr;
				Dir_Data *dir_ptr;
				Node_Data *node_ptr;
				Path_Data *path_ptr;

				for (path_ptr = link_path.First (); path_ptr; path_ptr = link_path.Next ()) {
					if (path_ptr->Cum () > 0) {
						dir_ptr = dir_data [link_path.Record_Index ()];
						link_ptr = link_data [dir_ptr->Link ()];

						time = path_ptr->TOD () - start;
						problem_file.Arrive (trip_time.Format_Duration (Resolve (time)));

						if (dir_ptr->Dir () == 1) {
							anode = link_ptr->Bnode ();
							bnode = link_ptr->Anode ();
							problem_file.Destination (-link_ptr->Link ());
						} else {
							anode = link_ptr->Anode ();
							bnode = link_ptr->Bnode ();
							problem_file.Destination (link_ptr->Link ());
						}
						dump_file.Copy_Fields (&problem_file);
						dump_file.points.Reset ();

						node_ptr = node_data [anode];

						point.x = UnRound (node_ptr->X ());
						point.y = UnRound (node_ptr->Y ());

						if (!dump_file.points.Add (&point)) goto dump_error;

						node_ptr = node_data [bnode];

						point.x = UnRound (node_ptr->X ());
						point.y = UnRound (node_ptr->Y ());

						if (!dump_file.points.Add (&point)) goto dump_error;

						if (!dump_file.Write_Record ()) goto dump_error;
					}
				}
			}
		}
	}
	return;

dump_error:
	Error ("Writing Arcview Dump File");
}
