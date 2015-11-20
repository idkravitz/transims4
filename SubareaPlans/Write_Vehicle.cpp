//*********************************************************
//	Write_Vehicle.cpp - Write the Subarea Vehicle File
//*********************************************************

#include "SubareaPlans.hpp"

#include "Vehicle_File.hpp"

//---------------------------------------------------------
//	Write_Vehicle
//---------------------------------------------------------

void SubareaPlans::Write_Vehicle (void)
{
	Vehicle_Data *veh_ptr;
	Vehicle_File *new_vehicle;

	new_vehicle = (Vehicle_File *) Demand_Db_Base (NEW_VEHICLE);

	Show_Message (2, "Writing %s -- Record", new_vehicle->File_Type ());
	Set_Progress (50000);

	max_vehicle = 0;

	for (veh_ptr = vehicle_data.First_Key (); veh_ptr; veh_ptr = vehicle_data.Next_Key ()) {
		Show_Progress ();

		if (veh_ptr->Location () > 0) {
			new_vehicle->Vehicle (veh_ptr->Vehicle ());
			new_vehicle->Household (veh_ptr->Household ());
			new_vehicle->Location (veh_ptr->Location ());
			new_vehicle->Type (veh_ptr->Type ());
			new_vehicle->Sub_Type (veh_ptr->Sub_Type ());

			if (!new_vehicle->Write ()) {
				Error ("Writing Subarea Vehicle File");
			}
			max_vehicle++;
		}
	}
	End_Progress ();
	new_vehicle->Close ();

	Print (2, "Number of Subarea Vehicles = %d", max_vehicle);
}
