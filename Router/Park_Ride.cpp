//*********************************************************
//	Park_Ride.cpp - build a park-&-ride plan
//*********************************************************

#include "Router.hpp"

#include <math.h>

//---------------------------------------------------------
//	Park_Ride_Plan
//---------------------------------------------------------

int Router::Park_Ride_Plan (Vehicle_Data *veh_ptr)
{
	int i, stat, acc, rec, imped, cost;
	unsigned cum_a, cum_b;
	int tod_a, tod_b, cost_a, cost_b, time_a, time_b;

	Trip_End_Data org, *des_ptr;
	Location_Data *loc_ptr;
	Parking_Data *park_ptr;
	List_Data *acc_ptr;
	Access_Data *access_ptr;
	Path_Data *path_ptr, path_root;

	//---- walk to vehicle (parking) ----

	stat = Walk_to_Vehicle (veh_ptr);
	if (stat) return (stat);

	//---- identify the best park-&-ride lots ----

	if (!Park_Ride_Lots ()) return (Set_Problem (PARK_PROBLEM));

	//---- build the path between the origin and destination parking lots ----

	Drive_Path (CAR, false);

	//---- initialize the path data ----

	for (i=0; i <= max_paths; i++) {
		loc_path [i].Zero_Fill ();
	}

	//---- add activity locations attached to parking lots to processing list ----

	first_ptr = &path_root;

	first_ptr->Next (0);
	first_ptr->Type (0);
	first_ptr->Xfer (0);
	first_ptr->Mode (NO_TRANSIT);

	last_ptr = first_ptr;

	org = *(org_array.First ());

	park_array.Reset ();

	for (des_ptr = des_array.First (); des_ptr != NULL; des_ptr = des_array.Next ()) {
		if (des_ptr->Cum () == MAX_INTEGER) continue;

		park_array.Add (des_ptr);

		cum_a = des_ptr->Cum ();
		tod_a = des_ptr->TOD ();
		cost_a = des_ptr->Cost ();
		time_a = des_ptr->Time ();

		//---- add the parking cost and penalty ----

		if (park_imp_flag || (park_hours_flag && park_hours > 0)) {
			park_ptr = parking_data [des_ptr->Rec ()];

			if (park_hours_flag && park_hours > 0) {
				cost = Resolve (park_ptr->Hourly () * park_hours);
				if (cost > park_ptr->Daily ()) {
					cost = park_ptr->Daily ();
				}
				cost_a += cost;
				cum_a += (cost * value_cost + value_round) >> value_roll;
			}
			if (park_imp_flag) {
				cum_a += park_ptr->Penalty ();
			}
		}

		//---- add the egress links to the processing queue ----

		for (acc = parking_egress [des_ptr->Rec ()]->From_List (); acc; acc = acc_ptr->From_List ()) {
			access_ptr = access_data [acc];
			acc_ptr = access_list [acc];

			loc_ptr = location_data [access_ptr->To_ID ()];

			path_ptr = loc_path [0] [access_ptr->To_ID ()];

			//---- check the time schedule ----

			tod_b = tod_a + access_ptr->Time ();

			if (tod_b > max_tod) {
				time_flag = true;
				continue;
			}

			//---- check walk distance ----

			time_b = time_a + access_ptr->Time ();

			if (walk_flag && time_b >= max_walk) {
				length_flag = true;
				continue;
			}
			cost_b = cost_a + access_ptr->Cost ();

			//---- check the cumulative impedance ----

			imped = (access_ptr->Time () * value_walk + access_ptr->Cost () * value_cost + value_round) >> value_roll;

			cum_b = cum_a + imped;

			if (cum_b >= path_ptr->Cum () && path_ptr->Cum () > 0) continue;

			if (path_ptr->Next () == 0 && last_ptr != path_ptr) {
				last_ptr->Next (access_ptr->To_ID ());
				last_ptr->Type (LOCATION_ID);
				last_ptr->Xfer (0);
				last_ptr->Mode (NO_TRANSIT);
				last_ptr = path_ptr;
			}
			path_ptr->Cum (cum_b);
			path_ptr->TOD (tod_b);
			path_ptr->Cost (cost_b);
			path_ptr->Time (time_b);
			path_ptr->Prev (-(park_array.Record_Index ()));
			path_ptr->Layer (PARKING_ID);
			path_ptr->Path (0);
		}
	}

	//---- set drive path error ----

	if (first_ptr->Next () == 0) {
		return (Set_Drive_Error ());
	}

	//---- initialize the transit path data ----

	for (i=0; i <= max_paths; i++) {
		node_path [i].Zero_Fill ();
		if (i < max_paths) {
			board_path [i].Zero_Fill ();
			alight_path [i].Zero_Fill ();
		}
	}

	//---- build the path ----

	stat = Build_Transit_Legs ();

	if (stat) return (stat);

	//---- find the best park-&-ride lot ----

	Set_Leg_Trip_Ends ();

	//---- save the drive plan ----

	des_ptr = org_array.Last ();

	if (des_ptr == NULL || des_ptr->Layer () != PARKING_ID) {
		return (Set_Transit_Error ());
	}

	rec = -des_ptr->Prev ();

	veh_ptr->Location (park_array [rec]->Rec ());

	stat = Save_Plan (&org, park_array [rec], Plan_File::AUTO_MODE, veh_ptr->Vehicle ());

	if (stat) return (stat);

	//---- save the walk to the activity location ----

	stat = Save_Plan (park_array [rec], des_ptr);

	if (stat) return (stat);

	//---- save the plan record ----

	return (Save_Transit_Legs ());
}

//---------------------------------------------------------
//	Park_Ride_Lots 
//---------------------------------------------------------

int Router::Park_Ride_Lots (void)
{
	int org_x, org_y, des_x, des_y;
	int dist, dist1, dist2, max_dist, ratio;
	double dx, dy;

	Trip_End_Data lot;
	Location_Data *loc_ptr;
	Parking_Data *park_ptr;
	Park_Ride_Data *lot_ptr;

	des_array.Reset ();

	if (park_ride.Num_Records () == 0) return (0);

	loc_ptr = location_data [trip_org.Rec ()];

	org_x = loc_ptr->X ();
	org_y = loc_ptr->Y ();

	loc_ptr = location_data [trip_des.Rec ()];

	des_x = loc_ptr->X ();
	des_y = loc_ptr->Y ();

	dx = org_x - des_x;
	dy = org_y - des_y;

	max_dist = (int) (2.0 * sqrt (dx * dx + dy * dy) + 0.5);

	//---- process each park-&-ride lot ----

	for (lot_ptr = park_ride.First (); lot_ptr != NULL; lot_ptr = park_ride.Next ()) {

		//---- calculate the distance ----

		dx = org_x - lot_ptr->X ();
		dy = org_y - lot_ptr->Y ();

		dist1 = (int) (sqrt (dx * dx + dy * dy) + 0.5);

		dx = des_x - lot_ptr->X ();
		dy = des_y - lot_ptr->Y ();

		dist2 = (int) (sqrt (dx * dx + dy * dy) + 0.5);

		dist = dist1 + dist2;

		if (dist == 0 || dist > max_dist) continue;

		ratio = 100 * dist1 / dist;

		if (ratio > max_parkride) continue;

		//---- add the new record ----

		memset (&lot, '\0', sizeof (Trip_End_Data));

		park_ptr = parking_data [lot_ptr->Parking ()];

		lot.Rec (lot_ptr->Parking ());
		lot.Cum (MAX_INTEGER);
		lot.ID (park_ptr->ID ());
		lot.Link (park_ptr->Link ());
		lot.Offset (park_ptr->Offset ());
		lot.Type (PARKING_ID);

		des_array.Add (&lot);
	}
	return (des_array.Num_Records ());
}

//---------------------------------------------------------
//	Park_Ride_Return
//---------------------------------------------------------

int Router::Park_Ride_Return (Vehicle_Data *veh_ptr)
{
	int stat, acc;

	Trip_End_Data des;

	Location_Data *loc_ptr;
	Access_Data *access_ptr;

	des = trip_des;

	//---- transit to the vehicle ----

	if (veh_ptr->Location () == 0) return (Set_Problem (ACCESS_PROBLEM));

	acc = parking_egress [veh_ptr->Location ()]->To_List ();
	
	if (acc == 0) return (Set_Problem (ACCESS_PROBLEM));

	access_ptr = access_data [acc];

	loc_ptr = location_data [access_ptr->From_ID ()];

	trip_des.Cum (MAX_INTEGER);
	trip_des.Rec (access_ptr->From_ID ());
	trip_des.ID (loc_ptr->ID ());
	trip_des.Link (loc_ptr->Link ());
	trip_des.Offset (loc_ptr->Offset ());
	trip_des.Type (LOCATION_ID);

	stat = Transit_Plan ();

	if (stat) return (stat);

	//---- build the drive path ----

	trip_org = trip_des;
	trip_des = des;
	
	return (Drive_Plan ((Use_Type) veh_ptr->Type (), veh_ptr));
}
