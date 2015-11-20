//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "VISSIMPlan.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void VISSIMPlan::Program_Control (void)
{
	int increment;
	char *str_ptr;

	//---- open network files ----

	Demand_Service::Program_Control ();

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
		time_range.Add_Ranges ("0:00..23:59:59");
	}
	if (increment > 0) {
		Print (1, "Output Time Increment = %d minutes", (increment / 60));
	}

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

	//---- open the vissim xml file  ----
	
	str_ptr = Get_Control_String (NEW_VISSIM_XML_FILE);

	if (str_ptr != NULL) {
		xml_file.File_Type ("New VISSIM XML File");
		Print (1);

		xml_file.Create (Project_Filename (str_ptr));
	}
	return;

	//---- error message ----

control_error:
	Error ("Missing Control Key = %s", Current_Key ());
} 
