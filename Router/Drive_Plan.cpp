//*********************************************************
//	Drive_Plan.cpp - Build a Drive Plan
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Drive_Plan
//---------------------------------------------------------

int Router::Drive_Plan (Use_Type type, Vehicle_Data *veh_ptr)
{
	int stat, best, passengers;

	//---- walk to vehicle (parking) ----

	stat = Walk_to_Vehicle (veh_ptr);

	if (stat) {
		return (stat);
	}

	//---- identify the destination parking lots ----

	if (!Destination_Parking ()) {
		return (Set_Problem (ACCESS_PROBLEM));
	}

	//---- build the path between the origin and destination parking lots ----

	Drive_Path (type, false);

	//---- identify the best destination ----

	best = Best_Combination ();

	//---- set the error codes ----

	if (best == 0) {
		return (Set_Drive_Error ());
	}

	//---- save the drive plan ----

	veh_ptr->Location (des_array [best]->Rec ());

	if (type == HOV2) {
		passengers = 1;
	} else if (type == HOV3) {
		passengers = 2;
	} else if (type == HOV4) {
		passengers = 3;
	} else {
		passengers = 0;
	}
	stat = Save_Plan (org_array.First (), des_array [best], Plan_File::AUTO_MODE, veh_ptr->Vehicle (), passengers);

	if (stat) {
		return (stat);
	}

	//---- walk to activity ----

	return (Parking_to_Activity (des_array [best], park_array [best], &trip_des));
}

//---------------------------------------------------------
//	Set_Drive_Error
//---------------------------------------------------------

int Router::Set_Drive_Error (void)
{
	if (zero_flag) {
		return (Set_Problem (NODE_PROBLEM));
	//} else if (use_flag) {
	//	return (Set_Problem (USE_PROBLEM));
	} else if (time_flag) {
		return (Set_Problem (TIME_PROBLEM));
	} else if (dist_flag) {
		return (Set_Problem (DIST_PROBLEM));
	} else if (length_flag) {
		return (Set_Problem (WALK_PROBLEM));
	} else if (use_flag) {
		return (Set_Problem (USE_PROBLEM));
	}
	return (Set_Problem (PATH_PROBLEM));
}
