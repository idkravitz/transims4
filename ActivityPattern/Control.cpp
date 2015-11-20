//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "ActivityPattern.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void ActivityPattern::Program_Control (void)
{
	char *str_ptr, *format_ptr;

	//---- open the household list file ----
	
	str_ptr = Get_Control_String (HOUSEHOLD_LIST);

	if (str_ptr != NULL) {
		hhlist_file.File_Type ("Household List");
		Print (1);

		hhlist_file.Open (Project_Filename (str_ptr, Extension ()));
		hhlist_flag = true;
	}

	//---- open network and demand files ----

	Demand_Service::Program_Control ();

	//---- get the time of day format ----
	
	str_ptr = Get_Control_String (TIME_OF_DAY_FORMAT);

	if (str_ptr == NULL) {
		str_ptr = "HOURS";
	}
	if (!activity_time.Format (str_ptr)) {
		Error ("Time of Day Format %s was Unrecognized", str_ptr);
	}
    Print (2, "Time of Day Format = %s", str_ptr);

	//---- open the household type script ----

	str_ptr = Get_Control_String (HOUSEHOLD_TYPE_SCRIPT);

	if (str_ptr == NULL) {
		if (Demand_Db_Base (HOUSEHOLD)->Field_Number ("TYPE") == 0) goto control_error;
	} else {
		Print (1);
		hhold_type_file.File_Type ("Household Type Script");

		hhold_type_file.Open (Project_Filename (str_ptr));
		hhold_type_flag = true;
	}

	//---- open the survey household file ----

	str_ptr = Get_Control_String (SURVEY_HOUSEHOLD_FILE);

	if (str_ptr == NULL) goto control_error;

	Print (1);
	survey_hh_file.File_Type ("Survey Household File");

	format_ptr = Get_Control_String (SURVEY_HOUSEHOLD_FORMAT);

	if (format_ptr != NULL) {
		survey_hh_file.Dbase_Format (format_ptr);
	}
	survey_hh_file.Open (Project_Filename (str_ptr));

	//---- open the survey household weights ----

	str_ptr = Get_Control_String (SURVEY_HOUSEHOLD_WEIGHTS);

	if (str_ptr != NULL) {
		weight_file.File_Type ("Survey Household Weights");

		weight_file.Open (Project_Filename (str_ptr));
		weight_flag = true;
	}

	//---- open the survey type script ----

	str_ptr = Get_Control_String (SURVEY_TYPE_SCRIPT);

	if (str_ptr == NULL) {
		if (survey_hh_file.Field_Number ("TYPE") == 0) goto control_error;
	} else {
		survey_type_file.File_Type ("Survey Type Script");

		survey_type_file.Open (Project_Filename (str_ptr));
		survey_type_flag = true;
	}

	//---- open the survey population file ----

	str_ptr = Get_Control_String (SURVEY_POPULATION_FILE);

	if (str_ptr == NULL) goto control_error;

	Print (1);
	survey_pop_file.File_Type ("Survey Population File");

	format_ptr = Get_Control_String (SURVEY_POPULATION_FORMAT);

	if (format_ptr != NULL) {
		survey_pop_file.Dbase_Format (format_ptr);
	}
	survey_pop_file.Open (Project_Filename (str_ptr));

	//---- open the survey activity file ----

	str_ptr = Get_Control_String (SURVEY_ACTIVITY_FILE);

	if (str_ptr == NULL) goto control_error;

	Print (1);
	survey_act_file.File_Type ("Survey Activity File");

	format_ptr = Get_Control_String (SURVEY_ACTIVITY_FORMAT);

	if (format_ptr != NULL) {
		survey_act_file.Dbase_Format (format_ptr);
	}
	survey_act_file.Open (Project_Filename (str_ptr));

	//---- open the activity pattern file ----
	
	str_ptr = Get_Control_String (ACTIVITY_PATTERN_FILE);

	if (str_ptr != NULL) {
		Print (1);
		pattern_file.File_Type ("Activity Pattern File");

		format_ptr = Get_Control_String (ACTIVITY_PATTERN_FORMAT);

		if (format_ptr != NULL) {
			pattern_file.Dbase_Format (format_ptr);
		}
		if (!pattern_file.Open (Project_Filename (str_ptr, Extension ()))) {
			Error ("Opening %s", pattern_file.File_Type ());
		}
		update_flag = true;
	}

	//---- open the problem file ----
	
	str_ptr = Get_Control_String (NEW_PROBLEM_FILE);

	if (str_ptr != NULL) {
		Print (1);

		format_ptr = Get_Control_String (NEW_PROBLEM_FORMAT);

		if (format_ptr != NULL) {
			problem_file.Dbase_Format (format_ptr);
		}
		problem_file.ActGen_Flag ();
		problem_file.Create (Project_Filename (str_ptr, Extension ()));
		problem_flag = true;
	}

	//---- open the household match file ----

	str_ptr = Get_Control_String (NEW_HOUSEHOLD_MATCH_FILE);

	if (str_ptr != NULL) {
		match_flag = true;
		match_file.File_Type ("New Household Match File");

		if (!match_file.Create (Project_Filename (str_ptr, Extension ()))) {
			Error ("Opening %s", match_file.File_Type ());
		}
		if (!match_file.Write ("Type\tHousehold\tPerson\tAge\tGroup\tWorker\tGender\tSurveyHH\tPerson\tAge\tGroup\tWorker\tGender")) {
			Error ("Writing %s Header", match_file.File_Type ());
		}
	}

	//---- open the household person count file ----

	str_ptr = Get_Control_String (NEW_HOUSEHOLD_PERSON_COUNT);

	if (str_ptr != NULL) {
		count_flag = true;
		count_file.File_Type ("New Household Person Count");

		if (!count_file.Create (Project_Filename (str_ptr, Extension ()))) {
			Error ("Opening %s", count_file.File_Type ());
		}
		if (!count_file.Write ("Household\tPerson\tCount")) {
			Error ("Writing %s Header", count_file.File_Type ());
		}
	}

	//---- open the new activity pattern file ----
	
	str_ptr = Get_Control_String (NEW_ACTIVITY_PATTERN_FILE);

	if (str_ptr == NULL) goto control_error;

	Print (1);
	new_pattern_file.File_Type ("New Activity Pattern File");

	format_ptr = Get_Control_String (NEW_ACTIVITY_PATTERN_FORMAT);

	if (format_ptr != NULL) {
		new_pattern_file.Dbase_Format (format_ptr);
	}
	if (!new_pattern_file.Create (Project_Filename (str_ptr, Extension ()))) {
		Error ("Creating %s", new_pattern_file.File_Type ());
	}

	//---- copy the replicate activity file ----

	if (update_flag) {
		if (!hhlist_flag) {
			Error ("A Household List file is required for Activity Pattern Updates");
		}
		new_pattern_file.Clear_Fields ();
		new_pattern_file.Replicate_Fields (&pattern_file);

		new_pattern_file.Write_Header ();
	}

	//---- time schedule variance ----

	str_ptr = Get_Control_String (TIME_SCHEDULE_VARIANCE);

	if (str_ptr != NULL) {
		max_variance = atoi (str_ptr);

		if (max_variance < 0 || max_variance > 60) {
			Error ("Time Schedule Variance %d is Out of Range (0..60)", max_variance);
		}
		Print (2, "Time Schedule Variance = %d minutes", max_variance);
		max_variance *= 60;
	}

	//---- random number seed ----

	str_ptr = Get_Control_String (RANDOM_NUMBER_SEED);

	if (str_ptr != NULL) {
		random.Seed (atoi (str_ptr));
	}
	Print (2, "Random Number Seed = %d", random.Seed ());

	if (time_variance > 0) {
		random_time.Seed (random.Seed () + 1);
	}
	return;

control_error:
	Error ("Missing Control Key = %s", Current_Key ());
} 
