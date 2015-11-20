//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "DynusTPlan.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void DynusTPlan::Program_Control (void)
{
	int increment;
	char *str_ptr, buffer [STRING_BUFFER];

	//---- open network files ----

	Demand_Service::Program_Control ();

	if (Demand_File_Flag (VEHICLE) && !Demand_File_Flag (VEHICLE_TYPE)) {
		Error ("A Vehicle Type File is needed to Convert Vehicle IDs to Type Codes");
	}
	if (!Demand_File_Flag (VEHICLE) && Demand_File_Flag (VEHICLE_TYPE)) {
		Error ("A Vehicle File is needed to Convert Vehicle IDs to Type Codes");
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
	plan_file.Plan_Sort (TIME_SORT);

	str_ptr = Get_Control_String (PLAN_FORMAT);

	if (str_ptr != NULL) {
		plan_file.File_Format (str_ptr);
		Print (1, "%s Format = %s", plan_file.File_Type (), str_ptr);
	}
	plan_file.Node_Based_Flag (type_flag);

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

	//---- get output time increment ----
	
	str_ptr = Get_Control_String (OUTPUT_TIME_INCREMENT);

	if (str_ptr != NULL) {
		increment = atol (str_ptr);

		if (increment < 0 || increment > 240) {
			Error ("Output Time Increment %d is Out of Range (0-240)", increment);
		}
		increment *= 60;
	}
	if (!increment == 0) {
		time_range.Period_Flag (true);
	}
	time_range.Increment (increment);

	//---- get the select time period ----
	
	str_ptr = Get_Control_String (SELECT_TIME_PERIOD);

	time_range.Format (Time_Step::CLOCK24);

	if (str_ptr != NULL) {
		Print (2, "Select Time Period = %s", str_ptr);

		if (!time_range.Add_Ranges (str_ptr)) {
			File_Error ("Time Period Range", str_ptr);
		}
	} else {
		time_range.Add_Ranges ("0:00..24:00");
	}
	if (increment > 0) {
		Print (1, "Output Time Increment = %d minutes", (increment / 60));
	}
	start_time = time_range.First ()->Low ();

	//---- get the selection percentage ----

	str_ptr = Get_Control_String (SELECTION_PERCENTAGE);

	if (str_ptr != NULL) {
		Get_Double (str_ptr, &percent);

		if (percent < 0.1 || percent > 100.0) {
			Error ("Selection Percentage %.2lf is Out of Range (0.1-100.0)", percent);
		}
		select_flag = (percent != 100.0);
	}
	Print (2, "Selection Percentage = %.1lf%%", percent);

	percent /= 100.0;

	//---- random number seed ----

	str_ptr = Get_Control_String (RANDOM_NUMBER_SEED);

	if (str_ptr != NULL) {
		random.Seed (atoi (str_ptr));
	}
	if (str_ptr != NULL || percent != 1.0) {
		Print (1, "Random Number Seed = %d", random.Seed ());
	}

	//---- get the DynusT directory  ----
	
	str_ptr = Get_Control_String (DYNUST_PLAN_DIRECTORY);

	if (str_ptr == NULL) goto control_error;
	str_cpy (plan_dir, sizeof (plan_dir), Project_Filename (str_ptr));

	Print (2, "DynusT Plan Directory = %s", plan_dir);

	//---- vehicle.dat ----

	veh_file.File_Type ("DynusT Vehicle File");

	str_fmt (buffer, sizeof (buffer), "%s/vehicle.dat", plan_dir);

	veh_file.Create (buffer);

	//---- path.dat ----

	path_file.File_Type ("DynusT Path File");

	str_fmt (buffer, sizeof (buffer), "%s/path.dat", plan_dir);

	path_file.Create (buffer);

	//---- DynusT user class ----

	str_ptr = Get_Control_String (DYNUST_USER_CLASS);

	if (str_ptr != NULL) {
		user_class = atoi (str_ptr);

		Print (2, "DynusT User Class = %d", user_class);

		if (user_class < 1 || user_class > 5) {
			Error ("DynusT User Class %d is Out of Range (1..5)", user_class);
		}
	}
	return;

	//---- error message ----

control_error:
	Error ("Missing Control Key = %s", Current_Key ());
} 
