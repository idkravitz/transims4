//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "ProblemSelect.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void ProblemSelect::Program_Control (void)
{
	int type;
	char *str_ptr, *format_ptr, buffer [FIELD_BUFFER];

	//---- open network files ----

	Demand_Service::Program_Control ();

	//---- open the plan file ----

	str_ptr = Get_Control_String (PROBLEM_FILE);

	if (str_ptr == NULL) goto control_error;

	format_ptr = Get_Control_String (PROBLEM_FORMAT);

	if (format_ptr != NULL) {
		problem_file.File_Format (format_ptr);
	}
	if (Partition ()) {
		problem_file.Filename (Project_Filename (str_ptr), Partition_Number ());
	} else {
		problem_file.Filename (Project_Filename (str_ptr));
	}
	Print_Filename (2, problem_file.File_Type (), problem_file.Groupname ());

	if (!problem_file.Open (0)) {
		File_Error (problem_file.File_Type (), problem_file.Filename ());
	}

	//---- get the household list ----

	str_ptr = Get_Control_String (HOUSEHOLD_LIST);

	if (str_ptr != NULL) {
		hh_flag = true;
		str_ptr = Project_Filename (str_ptr, Extension ());
		hhold_file.File_Type ("Household List");

		Print_Filename (2, hhold_file.File_Type (), str_ptr);

		hhold_file.Filename (str_ptr);

		if (!hhold_file.Open (0)) {
			File_Error ("Opening Household List", hhold_file.Filename ());
		}
	}

	//---- get the household list ----

	str_ptr = Get_Control_String (NEW_HOUSEHOLD_LIST);

	if (str_ptr == NULL) goto control_error;

	str_ptr = Project_Filename (str_ptr, Extension ());
	new_hhold_file.File_Type ("New Household List");
	new_hhold_file.File_Access (Db_Code::CREATE);

	Print_Filename (2, new_hhold_file.File_Type (), str_ptr);

	new_hhold_file.Filename (str_ptr);

	if (!new_hhold_file.Open (0)) {
		File_Error ("Opening New Household List", new_hhold_file.Filename ());
	}

	//---- get the select links ----
	
	str_ptr = Get_Control_String (SELECT_LINKS);

	if (str_ptr != NULL) {
		Print (2, "Select Links = %s", str_ptr);

		if (!links.Add_Ranges (str_ptr)) {
			File_Error ("Link Range", str_ptr);
		}
		link_flag = true;
		if (!Network_File_Flag (LINK) || !Network_File_Flag (NODE)) {
			Error ("Link and Node files are required for Link-Based Processing");
		}
	}

	//---- get the select time periods ----
	
	str_ptr = Get_Control_String (SELECT_TIME_PERIODS);

	if (str_ptr != NULL) {
		Print (2, "Select Time Periods = %s", str_ptr);

		//---- get the time of day format ----
		
		format_ptr = Get_Control_String (TIME_OF_DAY_FORMAT);

		if (format_ptr != NULL) {
			if (!times.Format (str_ptr)) {
				Error ("Time of Day Format %s was Unrecognized", format_ptr);
			}
			Print (1, "Time of Day Format = %s", format_ptr);
		}
		if (!times.Add_Ranges (str_ptr)) {
			File_Error ("Time Period Range", str_ptr);
		}
		time_flag = true;
	}

	//---- get the select problem types ----
	
	str_ptr = Get_Control_String (SELECT_PROBLEM_TYPES);

	if (str_ptr != NULL) {
		Print (2, "Select Problem Types = %s", str_ptr);

		while (str_ptr != NULL) {
			str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));

			if (buffer [0] != '\0') {
				type = Problem_Code (buffer);
				str_fmt (buffer, sizeof (buffer), "%d", type);

				if (!types.Add_Ranges (buffer)) {
					File_Error ("Problem Types", str_ptr);
				}
				type_flag = true;
			}
		}
	}
		
	//---- get the select subarea polygon ----

	str_ptr = Get_Control_String (SELECT_SUBAREA_POLYGON);

	if (str_ptr != NULL) {
		Print (1);
		select_subarea.File_Type ("Select Subarea Polygon");

		select_subarea.Open (Project_Filename (str_ptr));
		
		if (!select_subarea.Read_Record ()) {
			Error ("Reading %s", select_subarea.File_Type ());
		}
		subarea_flag = true;

		if (problem_file.Link_Field () > 0) {
			if (!Network_File_Flag (NODE) || !Network_File_Flag (LINK)) {
				Error ("Node and Link files are Required for Select Subarea Polygon");
			}
		}
		if (!Network_File_Flag (ACTIVITY_LOCATION)) {
			Error ("An Activity Location file is Required for Select Subarea Polygon");
		}
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

	//---- get the max percent selected ----

	str_ptr = Get_Control_String (MAXIMUM_PERCENT_SELECTED);

	if (str_ptr != NULL) {
		Get_Double (str_ptr, &max_percent);

		if (max_percent < 1.0 || max_percent > 100.0) {
			Error ("Maximum Percent Selected %.1lf is Out of Range (1.0-100.0)", max_percent);
		}
	}
	Print (1, "Maximum Percent Selected = %.1lf%%", max_percent);

	max_percent /= 100.0;

	//---- random number seed ----

	str_ptr = Get_Control_String (RANDOM_NUMBER_SEED);

	if (str_ptr != NULL) {
		random.Seed (atoi (str_ptr));
	}
	if (str_ptr != NULL || percent != 1.0 || max_percent != 1.0) {
		Print (1, "Random Number Seed = %d", random.Seed ());
	}
	return;

	//---- error message ----

control_error:
	Error ("Missing Control Key = %s", Current_Key ());
} 
