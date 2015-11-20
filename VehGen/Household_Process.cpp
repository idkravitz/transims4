//*********************************************************
//	Household_Process.cpp - Read the Household File
//*********************************************************

#include "VehGen.hpp"

#include "Household_File.hpp"
#include "Vehicle_File.hpp"

//---------------------------------------------------------
//	Household_Processing
//---------------------------------------------------------

bool VehGen::Household_Processing (Db_File *fh)
{
	int i, num_veh;

	Household_File *household_file = (Household_File *) fh;

	Vehicle_File *vehicle_file = (Vehicle_File *) Demand_Db_Base (NEW_VEHICLE);

	if (vehicle_flag) {
		num_veh = household_file->Vehicles ();
	} else {
		num_veh = 1;
	}
	if (num_veh > 0) {
		int household = household_file->Household ();
		int location = household_file->Location ();

		Parking_Lot *parking_ptr;

		parking_ptr = (Parking_Lot *) parking_lot.Get (&location);

		if (parking_ptr == NULL) {
			location = -1;
			num_problem++;
		} else {
			location = parking_ptr->parking;
		}
		vehicle_file->Household (household);
		vehicle_file->Location (location);
		vehicle_file->Type (1);
		vehicle_file->Sub_Type (0);

		for (i=0; i < num_veh; i++) {
			vehicle_file->Vehicle (++num_vehicle);

			if (!vehicle_file->Write ()) {
				Error ("Writing Vehicle File");
			}
		}
	}
	num_hhold++;

	//---- don't save the record ----

	return (false);
}
