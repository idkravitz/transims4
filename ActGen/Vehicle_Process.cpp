//*********************************************************
//	Vehicle_Process.cpp - Read the Vehicle File
//*********************************************************

#include "ActGen.hpp"

#include "Vehicle_File.hpp"

//---------------------------------------------------------
//	Vehicle_Processing
//---------------------------------------------------------

bool ActGen::Vehicle_Processing (Db_File *fh)
{
	Vehicle_Data *vehicle_ptr;
	HHold_Data *household_ptr;
	Vehicle_File *file;

	file = (Vehicle_File *) fh;

	if (First_Record ()) {
		int max_rec;

		if (hhlist_flag) {
			max_rec = nhh_list * 10;
		} else {
			max_rec = file->Estimate_Records ();
		}
		if (!vehicle_data.Max_Records (max_rec)) {
			Error ("Insufficient Memory for Vehicle Data");
		}
	}
	if (hhlist_flag) {
		if (hhold_list.Get_Index (file->Household ()) == 0) return (false);
	}
	if (Demand_Service::Vehicle_Processing (fh)) {
		vehicle_ptr = vehicle_data.New_Record ();

		household_ptr = household_data.Get (vehicle_ptr->Household ());
		if (household_ptr == NULL) return (false);

		vehicle_ptr->Household (household_ptr->Vehicle_List ());
		household_ptr->Vehicle_List (vehicle_data.Num_Records () + 1);
		return (true);
	}
	return (false);
}
