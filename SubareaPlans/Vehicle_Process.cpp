//*********************************************************
//	Vehicle_Process.cpp - Read the Vehicle File
//*********************************************************

#include "SubareaPlans.hpp"

#include "Vehicle_File.hpp"

//---------------------------------------------------------
//	Vehicle_Processing
//---------------------------------------------------------

bool SubareaPlans::Vehicle_Processing (Db_File *fh)
{
	if (First_Record ()) {

		//---- reserve memory ----

		Vehicle_File *file = (Vehicle_File *) fh;

		int nveh = file->Estimate_Records ();
		
		if (nveh / 10 < 25000) {
			nveh += nveh / 10;
		} else {
			nveh += 25000;
		}
		if (!vehicle_data.Max_Records (nveh)) {
			Error ("Insufficient Memory for %d Vehicle Records", nveh);
		}
		Compress_Vehicles (false);
	}

	if (Demand_Service::Vehicle_Processing (fh)) {

		Vehicle_Data *vehicle_ptr = vehicle_data.New_Record ();

		if (vehicle_ptr->Vehicle () > max_vehicle) {
			max_vehicle = vehicle_ptr->Vehicle ();
		}
		vehicle_ptr->Location (0);

		return (true);
	}
	return (false);
}
