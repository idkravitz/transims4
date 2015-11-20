//*********************************************************
//	Read_Vehicle.cpp - Read the Vehicle File
//*********************************************************

#include "Relocate.hpp"

#include "Vehicle_File.hpp"

//---------------------------------------------------------
//	Read_Vehicle
//---------------------------------------------------------

void Relocate::Read_Vehicle (void)
{
	int id, num_out, num_update;

	Map_Data *map_ptr;
	Vehicle_File *file, *new_file;
	
	file = (Vehicle_File *) Demand_Db_Base (VEHICLE);
	new_file = (Vehicle_File *) Demand_Db_Base (NEW_VEHICLE);

	//---- process each vehicle ----

	Show_Message ("Reading %s -- Record", file->File_Type ());
	Set_Progress (10000);

	num_out = num_update = 0;

	while (file->Read ()) {
		Show_Progress ();

		id = file->Vehicle ();
		if (id == 0) continue;

		new_file->Copy_Fields (file);

		if (zone_flag) {
			map_ptr = (Map_Data *) veh_map.Get (&id);

			if (map_ptr != NULL) {
				new_file->Location (map_ptr->new_id);
				num_update++;
			}
		} else {
			id = file->Location ();

			map_ptr = (Map_Data *) parking_map.Get (&id);

			if (map_ptr == NULL) {
				Warning ("Parking Lot %d was not Mapped to a New Location", id);
			} else if (map_ptr->new_id != id) {
				new_file->Location (map_ptr->new_id);
				num_update++;
			}
		}
		if (!new_file->Write ()) {
			Error ("Writing %s", new_file->File_Type ());
		}
		num_out++;
	}
	End_Progress ();

	Print (2, "Number of Vehicle Records Read = %d", Progress_Count ());
	Print (1, "Number of Vehicle Records Written = %d", num_out);
	Print (1, "Number of Vehicle Locations Updated = %d", num_update);

	file->Close ();
	new_file->Close ();
}
