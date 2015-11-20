//*********************************************************
//	Copy_Vehicles.cpp - copy plans to the new plan file
//*********************************************************

#include "AdjustPlans.hpp"

//---------------------------------------------------------
//	Copy_Vehicles
//---------------------------------------------------------

void AdjustPlans::Copy_Vehicles (void)
{
	int i, vehicle, household, num_veh;

	Vehicle_Data *vehicle_ptr;
	Vehicle_File *vehicle_file, *new_veh_file;

	vehicle_file = (Vehicle_File *) Demand_Db_File (VEHICLE);
	new_veh_file = (Vehicle_File *) Demand_Db_File (NEW_VEHICLE);

	//---- set the filename and extension ----

	Show_Message (2, "Copying %s -- Vehicle", vehicle_file->File_Type ());
	Set_Progress (10000);

	num_veh = 0;

	//---- read each vehicle record ----

	while (vehicle_file->Read ()) {
		Show_Progress ();

		vehicle = vehicle_file->Vehicle ();

		vehicle_ptr = vehicle_data.Get (vehicle);

		if (vehicle_ptr != NULL && vehicle_ptr->Passengers () < 1) continue;

		new_veh_file->Copy_Fields (vehicle_file);

		new_veh_file->Household (vehicle_file->Household ());
		new_veh_file->Vehicle (vehicle_file->Vehicle ());
		new_veh_file->Location (vehicle_file->Location ());
		new_veh_file->Type (vehicle_file->Type ());
		new_veh_file->Sub_Type (vehicle_file->Sub_Type ());

		if (!new_veh_file->Write ()) goto write_error;
		num_veh++;

		if (vehicle_ptr == NULL || vehicle_ptr->Passengers () == 1) continue;

		//---- replicated the vehicle ----

		vehicle = vehicle_ptr->Location ();
		household = vehicle_ptr->Household ();

		for (i=1; i < vehicle_ptr->Passengers (); i++) {
			vehicle++;
			household++;

			new_veh_file->Household (household);
			new_veh_file->Vehicle (vehicle);

			if (!new_veh_file->Write ()) goto write_error;
			num_veh++;
		}
	}
	End_Progress ();

	vehicle_file->Close ();
	new_veh_file->Close ();

	Break_Check (4);
	Print (1);
	Write (1, "Number of Vehicles Read = %d", Progress_Count ());
	Write (1, "Number of Vehicles Written = %d", num_veh);
	return;

write_error:
	Error ("Writing New Vehicle File");
}
