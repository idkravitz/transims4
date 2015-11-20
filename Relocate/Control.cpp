//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "Relocate.hpp"

#include "Vehicle_File.hpp"
#include "Utility.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void Relocate::Program_Control (void)
{
	char *str_ptr;

	//---- open network and demand files ----

	Demand_Service::Program_Control ();

	//---- open the zone relocation file ----

	str_ptr = Get_Control_String (ZONE_RELOCATION_FILE);

	if (str_ptr != NULL) {
		zone_flag = true;

		zone_file.File_Type ("Zone Relocation File");
		Print (1);

		if (!zone_file.Open (Project_Filename (str_ptr))) {
			Error ("Opening %s", zone_file.File_Type ());
		}

		//---- random number seed ----

		str_ptr = Get_Control_String (RANDOM_NUMBER_SEED);

		if (str_ptr != NULL) {
			random.Seed (atoi (str_ptr));
		}
		Print (1, "Random Number Seed = %d", random.Seed ());
	}

	//---- setup trip processing ----

	if (Demand_File_Flag (TRIP)) {
		trip_flag = true;

		if (!Demand_File_Flag (NEW_TRIP)) {
			Error ("A New Trip File is Required");
		}
		trip_file = (Trip_File *) Demand_Db_Base (TRIP);
		new_trip_file = (Trip_File *) Demand_Db_Base (NEW_TRIP);

		new_trip_file->Clear_Fields ();
		new_trip_file->Replicate_Fields (trip_file);

		new_trip_file->Write_Header ();
	}
	Demand_File_False (TRIP);

	//---- setup activity file processing ----

	if (Demand_File_Flag (ACTIVITY)) {
		activity_flag = true;

		if (!Demand_File_Flag (NEW_ACTIVITY)) {
			Error ("A New Activity File is Required");
		}
		activity_file = (Activity_File *) Demand_Db_Base (ACTIVITY);
		new_act_file = (Activity_File *) Demand_Db_Base (NEW_ACTIVITY);

		new_act_file->Clear_Fields ();
		new_act_file->Replicate_Fields (activity_file);

		new_act_file->Write_Header ();
	}
	Demand_File_False (ACTIVITY);

	//---- setup vehicle processing ----

	if (Demand_File_Flag (NEW_VEHICLE)) {
		vehicle_flag = true;

		if (!Demand_File_Flag (VEHICLE)) {
			Error ("Vehicle File is required for Vehicle Processing");
		}
		if (!Network_File_Flag (PROCESS_LINK)) {
			Error ("Processing Link File is required for Vehicle Processing");
		}
		Vehicle_File *file, *new_file;

		file = (Vehicle_File *) Demand_Db_Base (VEHICLE);
		new_file = (Vehicle_File *) Demand_Db_Base (NEW_VEHICLE);

		new_file->Clear_Fields ();
		new_file->Replicate_Fields (file);

		new_file->Write_Header ();
	}
	Demand_File_False (VEHICLE);

	//---- open the plan file ----

	str_ptr = Get_Control_String (PLAN_FILE);

	if (str_ptr != NULL) {
		plan_flag = true;

		if (!Network_File_Flag (PROCESS_LINK)) {
			Error ("Processing Link File is required for Plan Processing");
		}
		str_ptr = Project_Filename (str_ptr, Extension ());

		Print_Filename (2, plan_file.File_Type (), str_ptr);

		plan_file.Filename (str_ptr);

		if (Get_Control_String (NODE_LIST_PATHS) != NULL) {
			type_flag = Get_Control_Flag (NODE_LIST_PATHS);
		}
		plan_file.Node_Based_Flag (type_flag);

		str_ptr = Get_Control_String (PLAN_FORMAT);

		if (str_ptr != NULL) {
			plan_file.File_Format (str_ptr);
			Print (1, "%s Format = %s", plan_file.File_Type (), str_ptr);
		}
		if (!plan_file.Open (0)) {
			File_Error ("Opening Plan File", plan_file.Filename ());
		}
		type_flag = plan_file.Node_Based_Flag ();

		//---- node list paths ----

		if (type_flag) {
			Print (1, "Plan File contains Node List Paths");
		} else {
			Print (1, "Plan File contains Link List Paths");
		}

		//---- open the output file ----

		str_ptr = Get_Control_String (NEW_PLAN_FILE);

		if (str_ptr == NULL) goto control_error;

		new_plan_file.File_Type ("New Plan File");
		new_plan_file.File_Access (CREATE);

		str_ptr = Project_Filename (str_ptr, Extension ());

		Print_Filename (2, new_plan_file.File_Type (), str_ptr);

		new_plan_file.Filename (str_ptr);

		new_plan_file.Node_Based_Flag (type_flag);

		str_ptr = Get_Control_String (PLAN_FORMAT);

		if (str_ptr != NULL) {
			new_plan_file.File_Format (str_ptr);
			Print (1, "%s Format = %s", new_plan_file.File_Type (), str_ptr);
		}
		if (!new_plan_file.Open (0)) {
			File_Error ("Creating Plan File", new_plan_file.Filename ());
		}

		network_flag = (Network_File_Flag (LINK) && Network_File_Flag (NODE));

		if (network_flag) {

			//---- open the old node file ----

			str_ptr = Get_Control_String (OLD_NODE_FILE);

			if (str_ptr == NULL) goto control_error;

			old_node_file.File_Type ("Old Node File");
			Print (1);

			if (!old_node_file.Open (Project_Filename (str_ptr))) {
				Error ("Opening %s", old_node_file.File_Type ());
			}

			//---- open the old link file ----

			str_ptr = Get_Control_String (OLD_LINK_FILE);

			if (str_ptr == NULL) goto control_error;

			old_link_file.File_Type ("Old Link File");

			if (!old_link_file.Open (Project_Filename (str_ptr))) {
				Error ("Opening %s", old_link_file.File_Type ());
			}

			//---- open the parking file ----

			str_ptr = Get_Control_String (OLD_PARKING_FILE);

			if (str_ptr == NULL) goto control_error;

			old_park_file.File_Type ("Old Parking File");

			if (!old_park_file.Open (Project_Filename (str_ptr))) {
				Error ("Opening %s", old_park_file.File_Type ());
			}
		}
	}

	//---- open the old activity location file ----

	str_ptr = Get_Control_String (OLD_ACTIVITY_LOCATIONS);

	if (str_ptr == NULL) goto control_error;

	old_loc_file.File_Type ("Old Activity Locations");
	Print (1);

	if (!old_loc_file.Open (Project_Filename (str_ptr))) {
		Error ("Opening %s", old_loc_file.File_Type ());
	}

	//---- open the old process link file ----

	str_ptr = Get_Control_String (OLD_PROCESS_LINKS);

	if (str_ptr == NULL) {
		if (vehicle_flag || plan_flag) goto control_error;
	} else {
		old_acc_file.File_Type ("Old Process Links");
		Print (1);

		if (!old_acc_file.Open (Project_Filename (str_ptr))) {
			Error ("Opening %s", old_acc_file.File_Type ());
		}
	}

	//---- open the household list file ----
	
	str_ptr = Get_Control_String (NEW_HOUSEHOLD_LIST);

	if (str_ptr != NULL) {
		hhlist_file.File_Type ("New Household List");
		hhlist_file.File_Access (CREATE);
						
		str_ptr = Project_Filename (str_ptr);

		if (Partition ()) {
			hhlist_file.Filename (str_ptr, Partition_Number ());
		} else {
			hhlist_file.Filename (str_ptr);
		}
		Print_Filename (2, hhlist_file.File_Type (), hhlist_file.Groupname ());

		if (!hhlist_file.Open (0)) {
			File_Error (hhlist_file.File_Type (), hhlist_file.Filename ());
		}
		hhlist_flag = true;
	}
	loop_flag = trip_flag && activity_flag && hhlist_flag && !plan_flag;

	//---- open the new location map file ----
	
	str_ptr = Get_Control_String (NEW_LOCATION_MAP_FILE);

	if (str_ptr != NULL) {
		new_loc_file.File_Type ("New Location Map File");
		new_loc_file.File_Access (CREATE);
		Print (1);

		if (!new_loc_file.Open (Project_Filename (str_ptr))) {
			Error ("Creating %s", new_loc_file.File_Type ());
		}
		loc_map_flag = true;

		new_loc_file.Add_Field ("OLD_LOC", INTEGER, 10);
		new_loc_file.Add_Field ("NEW_LOC", INTEGER, 10);

		new_loc_file.Write_Header ();
	}

	//---- open the new parking map file ----
	
	str_ptr = Get_Control_String (NEW_PARKING_MAP_FILE);

	if (str_ptr != NULL) {
		new_park_file.File_Type ("New Parking Map File");
		new_park_file.File_Access (CREATE);
		Print (1);

		if (!new_park_file.Open (Project_Filename (str_ptr))) {
			Error ("Creating %s", new_park_file.File_Type ());
		}
		park_map_flag = true;

		new_park_file.Add_Field ("OLD_PARK", INTEGER, 10);
		new_park_file.Add_Field ("NEW_PARK", INTEGER, 10);

		new_park_file.Write_Header ();
	}

	//---- open the new parking map file ----

	if (zone_flag) {
		str_ptr = Get_Control_String (NEW_CHANGE_FILE);

		if (str_ptr != NULL) {
			change_file.File_Type ("New Change File");
			change_file.File_Access (CREATE);
			Print (1);

			if (!change_file.Open (Project_Filename (str_ptr))) {
				Error ("Creating %s", change_file.File_Type ());
			}
			change_flag = true;

			fprintf (change_file.File (), "HHOLD\tPERSON\tTRIP\tTYPE\tOLD\tNEW\n");
		}
	}
	return;

control_error:
	Error ("Missing Control Key = %s", Current_Key ());
} 
