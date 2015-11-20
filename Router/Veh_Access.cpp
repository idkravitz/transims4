//*********************************************************
//	Veh_Access.cpp - Vehicle Parking <-> Activity Location
//*********************************************************

#include "Router.hpp"

#include "string.h"

//---------------------------------------------------------
//	Walk_to_Vehicle
//---------------------------------------------------------

int Router::Walk_to_Vehicle (Vehicle_Data *veh)
{
	int acc, best, imped;
	Trip_End_Data *des_ptr, lot;
	Parking_Data *park_ptr;
	List_Data *acc_ptr;
	Access_Data *access_ptr = 0;

	//---- identify the parking lot ----

	park_ptr = parking_data [veh->Location ()];

	if (park_ptr == NULL) {
		return (Set_Problem (ACCESS_PROBLEM));
	}
	lot.ID (park_ptr->ID ());
	lot.Link (park_ptr->Link ());
	lot.Offset (park_ptr->Offset ());
	lot.Type (PARKING_ID);

	//---- find the access link ----

	for (acc = parking_access [trip_org.Rec ()]->From_List (); acc; acc = acc_ptr->From_List ()) {
		acc_ptr = access_list [acc];
		access_ptr = access_data [acc];

		if (access_ptr->To_ID () == veh->Location ()) break;
	}

	if (acc > 0 || limit_access) {
		if (acc == 0) {
			return (Set_Problem (ACCESS_PROBLEM));
		}

		//---- save the access information ----

		imped = (access_ptr->Time () * value_walk + access_ptr->Cost () * value_cost + value_round) >> value_roll;

		lot.Prev (-acc);
		lot.Cum (trip_org.Cum () + imped);
		lot.TOD (trip_org.TOD () + access_ptr->Time ());
		lot.Time (trip_org.Time () + access_ptr->Time ());
		lot.Cost (trip_org.Cost () + access_ptr->Cost ());

	} else {

		Location_Data *loc_ptr;

		//---- set the origin location ----

		org_array.Reset ();

		org_array.Add (&trip_org);

		//---- find locations attached to the parking lot ----

		des_array.Reset ();
		park_array.Reset ();

		for (acc = parking_egress [veh->Location ()]->To_List (); acc; acc = acc_ptr->To_List ()) {
			access_ptr = access_data [acc];
			acc_ptr = access_list [acc];

			loc_ptr = location_data [access_ptr->From_ID ()];

			imped = (access_ptr->Time () * value_walk + access_ptr->Cost () * value_cost + value_round) >> value_roll;

			lot.Cum (imped);
			lot.Cost (access_ptr->Cost ());
			lot.TOD (access_ptr->Time ());
			lot.Time (access_ptr->Time ());
			lot.Prev (0);

			lot.Rec (access_ptr->From_ID ());
			lot.ID (loc_ptr->ID ());
			lot.Link (loc_ptr->Link ());
			lot.Offset (loc_ptr->Offset ());
			lot.Type (LOCATION_ID);

			park_array.Add (&lot);

			lot.Cum (MAX_INTEGER);

			des_array.Add (&lot);
		}
		if (des_array.Num_Records () == 0) {
			return (Set_Problem (ACCESS_PROBLEM));
		}

		//---- build a walk path to the parking lot ----

		Node_Path (WALK, false);

		best = Best_Combination ();

		if (best == 0) {
			if (length_flag) {
				return (Set_Problem (WALK_PROBLEM));
			} else if (time_flag) {
				return (Set_Problem (TIME_PROBLEM));
			}
			return (Set_Problem (PATH_PROBLEM));
		}

		//---- save the plan record ----

		int stat = Save_Plan (&trip_org, des_array [best]);

		if (stat) return (stat);

		//---- reset the access information ----

		trip_org = *(des_array [best]);
		des_ptr = park_array [best];	

		lot.Prev (0);
		lot.Cum (trip_org.Cum () + des_ptr->Cum ());
		lot.TOD (trip_org.TOD () + des_ptr->TOD ());
		lot.Time (trip_org.Time () + des_ptr->Time ());
		lot.Cost (trip_org.Cost () + des_ptr->Cost ());
	}
	org_array.Reset ();

	org_array.Add (&lot);

	return (Save_Plan (&trip_org, &lot));
}

//---------------------------------------------------------
//	Destination_Parking
//---------------------------------------------------------

bool Router::Destination_Parking (void)
{
	int acc, imped, cost;
	Trip_End_Data lot;
	Parking_Data *park_ptr;
	Access_Data *access_ptr;
	List_Data *acc_ptr;

	park_array.Reset ();
	des_array.Reset ();

	//---- find parking lots attached to the destination ----

	for (acc = parking_access [trip_des.Rec ()]->To_List (); acc; acc = acc_ptr->To_List ()) {
		access_ptr = access_data [acc];
		acc_ptr = access_list [acc];

		park_ptr = parking_data [access_ptr->From_ID ()];

		//---- calculate the parking cost ----

		if (park_hours_flag && park_hours > 0) {
			cost = Resolve (park_ptr->Hourly () * park_hours);
			if (cost > park_ptr->Daily ()) {
				cost = park_ptr->Daily ();
			}
		} else {
			cost = 0;
		}
		cost += access_ptr->Cost ();

		//---- add the parking impedance penalty ----

		if (park_imp_flag) {
			imped = park_ptr->Penalty ();
		} else {
			imped = 0;
		}
		imped += (access_ptr->Time () * value_walk + cost * value_cost + value_round) >> value_roll;

		lot.Cum (imped);
		lot.Cost (cost);
		lot.TOD (access_ptr->Time ());
		lot.Time (access_ptr->Time ());
		lot.Prev (0);

		lot.Rec (access_ptr->From_ID ());
		lot.ID (park_ptr->ID ());
		lot.Link (park_ptr->Link ());
		lot.Offset (park_ptr->Offset ());
		lot.Type (PARKING_ID);

		park_array.Add (&lot);

		lot.Cum (MAX_INTEGER);

		des_array.Add (&lot);
	}

	//---- identify remote parking options ----

	if (!limit_access) {
	}
	return (park_array.Num_Records () > 0);
}

//---------------------------------------------------------
//	Parking_to_Activity
//---------------------------------------------------------

int Router::Parking_to_Activity (Trip_End_Data *org, Trip_End_Data *lot, Trip_End_Data *des)
{
	//---- limit access path ----

	if (lot->Prev () <= 0) {
		des->Cum (org->Cum () + lot->Cum ());
		des->TOD (org->TOD () + lot->TOD ());
		des->Cost (org->Cost () + lot->Cost ());
		des->Time (org->Time () + lot->Time ());
		des->Prev (0);

		if (des->TOD () > max_tod) {
			return (Set_Problem (TIME_PROBLEM));
		}
		if (walk_flag && des->Time () >= max_walk) {
			return (Set_Problem (WALK_PROBLEM));
		}
	} else {

		//---- walk path ----
	}
	return (Save_Plan (org, des));
}

//---------------------------------------------------------
//	Best_Combination
//---------------------------------------------------------

int Router::Best_Combination (void)
{
	int best, best_cum, cum;
	Trip_End_Data *des_ptr, *lot_ptr;

	//---- identify the best combination ----

	best = 0;
	best_cum = MAX_INTEGER;

	for (des_ptr = des_array.First (); des_ptr != NULL; des_ptr = des_array.Next ()) {
		if (des_ptr->Cum () == MAX_INTEGER) continue;

		lot_ptr = park_array [des_array.Record_Index ()];

		cum = des_ptr->Cum () + lot_ptr->Cum ();

		if (cum >= best_cum) continue;

		if ((des_ptr->TOD () + lot_ptr->TOD ()) > max_tod) {
			time_flag = true;
			continue;
		}
		if (walk_flag && (des_ptr->Time () + lot_ptr->Time ()) >= max_walk) {
			length_flag = true;
			continue;
		}
		best = des_array.Record_Index ();
		best_cum = cum;
	}
	return (best);
}
