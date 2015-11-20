//*********************************************************
//	Read_Vehicle.cpp - Read the Vehicle File
//*********************************************************

#include "TripSum.hpp"

#include "Vehicle_File.hpp"

//---------------------------------------------------------
//	Read_Vehicle
//---------------------------------------------------------

void TripSum::Read_Vehicle (void)
{
	int id, num_out;

	Vehicle_File *file, *new_file;
	
	file = (Vehicle_File *) Demand_Db_Base (VEHICLE);
	new_file = (Vehicle_File *) Demand_Db_Base (NEW_VEHICLE);

	//---- process each vehicle ----

	Show_Message ("Reading %s -- Record", file->File_Type ());
	Set_Progress (10000);

	num_out = 0;

	while (file->Read ()) {
		Show_Progress ();

		id = file->Vehicle ();
		if (id == 0) continue;

		if (vehicle_list.Get_Index (id) == 0) continue;

		new_file->Copy_Fields (file);

		if (!new_file->Write ()) {
			Error ("Writing %s", new_file->File_Type ());
		}
		num_out++;
	}
	End_Progress ();

	Print (2, "Number of Vehicle Records Read = %d", Progress_Count ());
	Print (1, "Number of Vehicle Records Written = %d", num_out);

	file->Close ();
	new_file->Close ();
}
