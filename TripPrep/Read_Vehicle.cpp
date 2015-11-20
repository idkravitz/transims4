//*********************************************************
//	Read_Vehicle.cpp - Read the Vehicle File
//*********************************************************

#include "TripPrep.hpp"

//---------------------------------------------------------
//	Read_Vehicle
//---------------------------------------------------------

void TripPrep::Read_Vehicle (void)
{
	int num_out, next_in, next_merge;
	bool read_in, read_merge, save_in, save_merge;

	Vehicle_File *file, *new_file;
	
	file = (Vehicle_File *) Demand_Db_Base (VEHICLE);
	new_file = (Vehicle_File *) Demand_Db_Base (NEW_VEHICLE);

	//---- process each vehicle ----

	if (merge_veh_flag) {
		Show_Message ("Merging %s -- Record", file->File_Type ());
		if (merge_veh_file.Read ()) {
			next_merge = merge_veh_file.Vehicle ();
		} else {
			next_merge = MAX_INTEGER;
		}
	} else {
		Show_Message ("Reading %s -- Record", file->File_Type ());
		next_merge = MAX_INTEGER;
	}
	Set_Progress (10000);

	num_out = 0;
	read_merge = read_in = save_in = save_merge = false;

	if (file->Read ()) {
		next_in = file->Vehicle ();
	} else {
		next_in = MAX_INTEGER;
	}

	//---- process each record ----

	while (next_in != MAX_INTEGER || next_merge != MAX_INTEGER) {
		Show_Progress ();

		//---- set the processing flags ----

		if (next_merge < next_in) {
			read_merge = true;
			if (trip_flag || activity_flag) {
				save_merge = (vehicle_list.Get_Index (next_merge) != 0);
			} else {
				save_merge = true;
			}
			save_in = read_in = false;
		} else {
			if (next_merge == next_in) {
				if (trip_flag || activity_flag) {
					save_in = (vehicle_list.Get_Index (next_in) != 0);
				} else {
					save_in = true;
				}
				if (!save_in) {
					if (trip_flag || activity_flag) {
						save_merge = (vehicle_list.Get_Index (next_merge) != 0);
					} else {
						save_merge = true;
					}
				} else {
					save_merge = false;
				}
				read_merge = read_in = true;
			} else {
				read_in = save_in = true;
				if (trip_flag || activity_flag) {
					save_in = (vehicle_list.Get_Index (next_in) != 0);
				}
				read_merge = save_merge = false;
			}
		}
		
		//---- write the input vehicle to the output file ----

		if (save_in) {
			new_file->Copy_Fields (file);

			if (move_org_flag) {
				int veh, *ptr;
				veh = file->Vehicle ();
				ptr = (int *) move_veh_map.Get (&veh);
				if (ptr != NULL) {
					new_file->Location (ptr [1]);
				}
			}
			if (!new_file->Write ()) {
				Error ("Writing %s", new_file->File_Type ());
			}
			num_out++;
		}
		
		//---- write the merge vehicle to the output file ----

		if (save_merge) {		
			new_file->Copy_Fields (&merge_veh_file);

			if (!new_file->Write ()) {
				Error ("Writing %s", new_file->File_Type ());
			}
			num_out++;
		}

		//---- get the next merge vehicle ----

		if (read_merge) {
			if (merge_veh_file.Read ()) {
				next_merge = merge_veh_file.Vehicle ();
			} else {
				next_merge = MAX_INTEGER;
			}
		}
		
		//---- get the next input vehicle ----

		if (read_in) {
			if (file->Read ()) {
				next_in = file->Vehicle ();
			} else {
				next_in = MAX_INTEGER;
			}
		}
	}
	End_Progress ();

	Print (2, "Number of Vehicle Records Read = %d", Progress_Count ());
	Print (1, "Number of Vehicle Records Written = %d", num_out);

	file->Close ();
	new_file->Close ();
}
