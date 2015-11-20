//*********************************************************
//	Vehicle_Process.cpp - Read the Vehicle File
//*********************************************************

#include "Router.hpp"

#include "Vehicle_File.hpp"

//---------------------------------------------------------
//	Vehicle_Processing
//---------------------------------------------------------

bool Router::Vehicle_Processing (Db_File *fh)
{
	if (First_Record ()) {
		int nveh;

		//---- reserve memory ----
			
		if (hhlist_flag) {
			nveh = household_type.Num_Records ();
		} else {
			Vehicle_File *file = (Vehicle_File *) fh;

			nveh = file->Estimate_Records ();
		}
		if (!vehicle_data.Max_Records (nveh) || !vehicle_home.Max_Records (nveh)) {
			Error ("Insufficient Memory for %d Vehicle Records", nveh);
		}
	}

	if (Demand_Service::Vehicle_Processing (fh)) {

		Vehicle_Data *vehicle_ptr = vehicle_data.New_Record ();

		if (hhlist_flag && !skim_flag) {
			int hhold = vehicle_ptr->Household ();

			if (household_type.Get (hhold) == NULL) return (false);
		}
		if (veh_type_flag) {
			Veh_Type_Data *veh_type_ptr;

			veh_type_ptr = veh_type_data.Get (vehicle_ptr->Type (), vehicle_ptr->Sub_Type ());
			
			if (veh_type_ptr == NULL) {
				Error ("Vehicle %d Type %d-%d was Not Found in the Vehicle Type File", 
					vehicle_ptr->Vehicle (), vehicle_ptr->Type (), vehicle_ptr->Sub_Type ());
			}
			vehicle_ptr->Type (veh_type_ptr->Use ());
		} else {
			vehicle_ptr->Type (Veh_Type_Map (vehicle_ptr->Type ()));
		}
		if (!vehicle_home.Add (vehicle_ptr->Location ())) {
			Error ("Adding to Vehicle Home Location");
		}
		return (true);
	}
	return (false);
}
