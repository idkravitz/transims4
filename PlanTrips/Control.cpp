//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "PlanTrips.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void PlanTrips::Program_Control (void)
{
	char *str_ptr;
	bool type_flag = true;

	//---- open network files ----

	Demand_Service::Program_Control ();

	trip_flag = Demand_File_Flag (TRIP);
	act_flag = Demand_File_Flag (ACTIVITY);

	new_trip_flag = Demand_File_Flag (NEW_TRIP);
	new_act_flag = Demand_File_Flag (NEW_ACTIVITY);

	if (new_trip_flag) {
		trip_file = (Trip_File *) Demand_Db_File (NEW_TRIP);
	}
	if (new_act_flag) {
		act_file = (Activity_File *) Demand_Db_File (NEW_ACTIVITY);
	}

	//---- open the plan file ----

	if (Get_Control_String (NODE_LIST_PATHS) != NULL) {
		type_flag = Get_Control_Flag (NODE_LIST_PATHS);
	}
	str_ptr = Get_Control_String (PLAN_FILE);

	if (str_ptr == NULL) goto control_error;

	str_ptr = Project_Filename (str_ptr, Extension ());

	Print_Filename (2, plan_file.File_Type (), str_ptr);

	plan_file.Filename (str_ptr);

	str_ptr = Get_Control_String (PLAN_FORMAT);

	if (str_ptr != NULL) {
		plan_file.File_Format (str_ptr);
		Print (1, "%s Format = %s", plan_file.File_Type (), str_ptr);
	}
	plan_file.Node_Based_Flag (type_flag);
	plan_file.Plan_Memory (ALLOCATE_MEMORY);

	if (!plan_file.Open (0)) {
		File_Error ("Opening Plan File", plan_file.Filename ());
	}
	type_flag = plan_file.Node_Based_Flag ();

	//---- get the household list ----

	str_ptr = Get_Control_String (HOUSEHOLD_LIST);

	if (str_ptr != NULL) {
		str_ptr = Project_Filename (str_ptr, Extension ());

		hhlist_file.File_Type ("Household List");
		hhlist_file.Filename (str_ptr);

		if (plan_file.Extend () > 0 && hhlist_file.Extend () == 0) {
			hhlist_file.Extend (plan_file.Extend ());
		}
		Print_Filename (2, hhlist_file.File_Type (), str_ptr);

		if (!hhlist_file.Open (0)) {
			File_Error ("Opening Household List", hhlist_file.Filename ());
		}
		hhlist_flag = true;
	}

	//---- open the new plan file ----

	str_ptr = Get_Control_String (NEW_PLAN_FILE);

	if (str_ptr != NULL) {
		plan_flag = true;
		str_ptr = Project_Filename (str_ptr, Extension ());

		new_plan_file.File_Type ("New Plan File");
		Print_Filename (2, new_plan_file.File_Type (), str_ptr);

		new_plan_file.Filename (str_ptr);
		new_plan_file.File_Access (CREATE);
		new_plan_file.Extend (plan_file.Extend ());

		str_ptr = Get_Control_String (NEW_PLAN_FORMAT);

		if (str_ptr != NULL) {
			new_plan_file.File_Format (str_ptr);
			Print (1, "%s Format = %s", new_plan_file.File_Type (), str_ptr);
		}
		new_plan_file.Node_Based_Flag (type_flag);

		if (!new_plan_file.Open (0)) {
			File_Error ("Creating Plan File", new_plan_file.Filename ());
		}
	}

	//---- get the new household list ----

	str_ptr = Get_Control_String (NEW_HOUSEHOLD_LIST);

	if (str_ptr != NULL) {
		str_ptr = Project_Filename (str_ptr, Extension ());

		hhold_file.File_Type ("New Household List");
		hhold_file.File_Access (CREATE);
		hhold_file.Filename (str_ptr);

		if (plan_file.Extend () > 0 && hhold_file.Extend () == 0) {
			hhold_file.Extend (plan_file.Extend ());
		}
		Print_Filename (2, hhold_file.File_Type (), str_ptr);

		if (!hhold_file.Open (0)) {
			File_Error ("Opening New Household List", hhold_file.Filename ());
		}
		hhold_flag = true;
	}
	if (!new_trip_flag && !new_act_flag && !plan_flag && !hhold_flag) {
		Print (1);
		Warning ("No Output Files will be Generated");
		Show_Message (1);
	}

	//---- get the time of day format ----
	
	str_ptr = Get_Control_String (TIME_OF_DAY_FORMAT);

	if (str_ptr == NULL) {
		str_ptr = "SECONDS";
	}
	if (!trip_time.Format (str_ptr)) {
		Error ("Time of Day Format %s was Unrecognized", str_ptr);
	}
    Print (2, "Time of Day Format = %s", str_ptr);

	//---- create independent trips ----
	
	str_ptr = Get_Control_String (CREATE_INDEPENDENT_TRIPS);

	if (str_ptr != NULL) {
		Print (2, "Create Independent Trips = %s", str_ptr);

		create_flag = Get_Control_Flag (CREATE_INDEPENDENT_TRIPS);

		if (create_flag) {
			if (act_flag || new_act_flag) {
				Error ("Input or Output Activity Files are incompatible with Independent Trips");
			}
			if (trip_flag) {
				Error ("An Input Trip File is incompatible with Independent Trips");
			}
			if (!new_trip_flag) {
				Error ("A New Trip File is required for Independent Trips");
			}
			if (!Demand_File_Flag (VEHICLE) || !Demand_File_Flag (NEW_VEHICLE)) {
				Error ("Input and Output Vehicle Files are required for Independent Trips");
			} else {
				veh_file = (Vehicle_File *) Demand_Db_File (NEW_VEHICLE);
			}
			if (!plan_flag) {
				Error ("A New Plan File is required for Independent Trips");
			}
		}
	}
	return;

	//---- error message ----

control_error:
	Error ("Missing Control Key = %s", Current_Key ());
} 
