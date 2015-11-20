//*********************************************************
//	Drive_Vines.cpp - Build Specified Paths
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Drive_Vines
//---------------------------------------------------------

void Router::Drive_Vines (void)
{
	int hhold, mode, origin, destination, org_rec, des_rec, nloc, imped, start;
	int org_acc, org_park, acc, park, cost, passengers;
	unsigned cum;

	Use_Type use;

	Location_Data *org_ptr, *des_ptr;
	Vehicle_Data *veh_ptr;
	Range_Data *range_ptr;
	Trip_Data trip_rec;
	Access_Data *access_ptr;
	Parking_Data *park_ptr;
	Trip_End_Data *des;

	//---- initialize the data ----

	Show_Message ("Building Specified Vines -- Record");
	Set_Progress (1000);

	hhold = 0;
	veh_ptr = &default_vehicle;

	if (mode_flag) {
		mode = new_mode;
	} else {
		mode = DRIVE_ALONE;
	}
	if (mode == DRIVE_ALONE) {
		use = CAR;
		passengers = 0;
	} else if (mode == CARPOOL2) {
		use = HOV2;
		passengers = 1; 
	} else if (mode == CARPOOL3) {
		use = HOV3;
		passengers = 2;
	} else {
		use = HOV4;
		passengers = 3;
	}
	nloc = location_data.Num_Records ();

	walk_flag = (max_walk != 0);
	turn_flag = (left_imped > 0 || right_imped > 0 || uturn_imped > 0);

	if (park_hours_flag) {
		park_hours = hours_park [1];
	} else {
		park_hours = 0;
	}

	//---- loop through each time increment ----

	for (range_ptr = time_range.First (); range_ptr; range_ptr = time_range.Next ()) {

		start = Round (range_ptr->Low ());

		//---- loop through each origin location ----

		for (org_rec=1; org_rec <= nloc; org_rec++) {
			org_ptr = location_data [org_rec];
			origin = org_ptr->Location ();

			if (select_org && !org_range.In_Range (origin)) continue;
	
			//---- find a parking lot ----

			org_acc = parking_access [org_rec]->From_List ();
			if (org_acc == 0) continue;

			access_ptr = access_data [org_acc];

			org_park = access_ptr->To_ID ();
			imped = (access_ptr->Time () * value_walk + access_ptr->Cost () * value_cost + value_round) >> value_roll;

			park_ptr = parking_data [org_park];

			memset (&walk_org, '\0', sizeof (walk_org));

			walk_org.Rec (org_rec);
			walk_org.ID (origin);
			walk_org.Link (org_ptr->Link ());
			walk_org.Offset (org_ptr->Offset ());
			walk_org.Type (LOCATION_ID);
			walk_org.TOD (start);
			walk_org.Cum (1);

			memset (&trip_org, '\0', sizeof (trip_org));

			trip_org.Rec (org_park);
			trip_org.ID (park_ptr->ID ());
			trip_org.Link (park_ptr->Link ());
			trip_org.Offset (park_ptr->Offset ());
			trip_org.Type (PARKING_ID);
			trip_org.Prev (-1);
			trip_org.Cum (imped);
			trip_org.TOD (start + access_ptr->Time ());
			trip_org.Time (access_ptr->Time ());
			trip_org.Cost (access_ptr->Cost ());

			//---- loop through each destination location ----

			des_array.Reset ();
			org_array.Reset ();

			org_array.Add (&trip_org);

			for (des_rec=1; des_rec <= nloc; des_rec++) {
				if (org_rec == des_rec) continue;

				des_ptr = location_data [des_rec];
				destination = des_ptr->Location ();

				if (select_des && !des_range.In_Range (destination)) continue;

				//---- find a parking lot ----

				acc = parking_access [des_rec]->To_List ();
				if (acc == 0) continue;

				access_ptr = access_data [acc];

				park = access_ptr->From_ID ();
				park_ptr = parking_data [park];

				trip_des.Rec (park);
				trip_des.ID (park_ptr->ID ());
				trip_des.Link (park_ptr->Link ());
				trip_des.Offset (park_ptr->Offset ());
				trip_des.Type (PARKING_ID);
				trip_des.Cum (MAX_INTEGER);

				max_tod = MAX_INTEGER;

				des_array.Add (&trip_des);
			}
			if (des_array.Num_Records () == 0) continue;

			//--- build the tree ----

			Drive_Path (use, false);

			//---- save each destination ----

			for (des = des_array.First (); des != NULL; des = des_array.Next ()) {
				if (des->Cum () == MAX_INTEGER) continue;

				acc = parking_egress [des->Rec ()]->From_List ();
				if (acc == 0) continue;

				access_ptr = access_data [acc];

				Show_Progress ();
				nprocess++;
				hhold++;

				plan_file.Household (hhold);
				plan_file.Trip (1);
				plan_file.Leg (0);
	
				Save_Plan (&walk_org, &trip_org);

				Save_Plan (&trip_org, des, Plan_File::AUTO_MODE, 1, passengers);

				memcpy (&trip_des, des, sizeof (trip_des));

				//---- add the parking cost and penalty ----

				cost = trip_des.Cost () + access_ptr->Cost ();

				imped = (access_ptr->Time () * value_walk + access_ptr->Cost () * value_cost + value_round) >> value_roll;
				cum = trip_des.Cum () + imped;

				if (park_imp_flag || (park_hours_flag && park_hours > 0)) {
					park_ptr = parking_data [des->Rec ()];

					if (park_hours_flag && park_hours > 0) {
						park = Resolve (park_ptr->Hourly () * park_hours);
						if (park > park_ptr->Daily ()) {
							park = park_ptr->Daily ();
						}
						cost += park;
						cum += (park * value_cost + value_round) >> value_roll;
					}
					if (park_imp_flag) {
						cum += park_ptr->Penalty ();
					}
				}

				//---- set the destination location ----

				destination = access_ptr->To_ID ();
				des_ptr = location_data [destination];

				trip_des.Rec (destination);
				trip_des.ID (des_ptr->Location ());
				trip_des.Type (LOCATION_ID);
				trip_des.Link (des_ptr->Link ());
				trip_des.Offset (des_ptr->Offset ());
				trip_des.Prev (-1);
				trip_des.Cum (cum);
				trip_des.TOD (trip_des.TOD () + access_ptr->Time ());
				trip_des.Time (trip_des.Time () + access_ptr->Time ());
				trip_des.Cost (cost);

				Save_Plan (des, &trip_des);
			}
		}
	}
	End_Progress ();
}
