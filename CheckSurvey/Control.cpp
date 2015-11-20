//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "CheckSurvey.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void CheckSurvey::Program_Control (void)
{
	int i, num;
	double factor;
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

	//---- open the new survey activity file ----
	
	str_ptr = Get_Control_String (NEW_SURVEY_ACTIVITY_FILE);

	if (str_ptr != NULL) {
		Print (1);
		new_act_file.File_Type ("New Survey Activity File");

		format_ptr = Get_Control_String (NEW_SURVEY_ACTIVITY_FORMAT);

		if (format_ptr != NULL) {
			new_act_file.Dbase_Format (format_ptr);
		}
		new_act_file.Create (Project_Filename (str_ptr));
		new_act_flag = true;

		new_act_file.Clear_Fields ();
		new_act_file.Replicate_Fields (&survey_act_file);

		if (survey_act_file.Field_Number ("NOTES") == 0) {
			if (Notes_Name_Flag ()) {
				new_act_file.Add_Field ("NOTES", Db_Code::STRING, 40);
			}
		}
		new_act_file.Write_Header ();

		new_act_file.Notes_Field (new_act_file.Field_Number ("NOTES"));
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

	//---- get the purpose anchor order ----

	str_ptr = Get_Control_String (PURPOSE_ANCHOR_ORDER);

	if (str_ptr != NULL) {
		Print (2, "Purpose Anchor Order = %s", str_ptr);

		while (str_ptr != NULL) {
			str_ptr = Get_Integer (str_ptr, &num);

			if (num > 0) {
				anchor_list.Add (num);
			}
		}
	} else {
		anchor_list.Add (1);
	}

	//---- get the average travel speed ----

	str_ptr = Get_Control_String (AVERAGE_TRAVEL_SPEED);

	if (str_ptr != NULL) {
		Print (2, "Average Travel Speed = %s ... meters/second", str_ptr);

		for (i=1; i < MAX_MODE; i++) {
			if (str_ptr != NULL) {
				str_ptr = Get_Double (str_ptr, &factor);

				if (Round (factor) <= 0) {
					Error ("Averge Speed %.1lf is Out of Range", factor);
				}
			}
			avg_speed [i] = Round (factor);
		}
	}

	//---- get the additional travel time ----

	str_ptr = Get_Control_String (ADDITIONAL_TRAVEL_TIME);

	if (str_ptr != NULL) {
		Print (1, "Additional Travel Time = %s ... seconds", str_ptr);

		for (i=1; i < MAX_MODE; i++) {
			if (str_ptr != NULL) {
				str_ptr = Get_Integer (str_ptr, &num);
			}
			add_time [i] = num;
		}
	}

	//---- get the maximum walk time ----

	str_ptr = Get_Control_String (MAXIMUM_WALK_TIME);

	if (str_ptr != NULL) {
		Get_Integer (str_ptr, &max_walk);

		if (max_walk < 300) {
			Error ("Maximum Walk Time %d is Out of Range (300+ seconds)", max_walk);
		}
	}
	Print (2, "Maximum Walk Time = %d seconds (%d minutes)", max_walk, (max_walk + 30) / 60);	

	//---- get the maximum trip time ----

	str_ptr = Get_Control_String (MAXIMUM_TRIP_TIME);

	if (str_ptr != NULL) {
		Get_Integer (str_ptr, &max_trip);

		if (max_trip < 1800) {
			Error ("Maximum Trip Time %d is Out of Range (1800+ seconds)", max_trip);
		}
	}
	Print (1, "Maximum Trip Time = %d seconds (%d minutes)", max_trip, (max_trip + 30) / 60);	

	//---- random number seed ----

	str_ptr = Get_Control_String (RANDOM_NUMBER_SEED);

	if (str_ptr != NULL) {
		random.Seed (atoi (str_ptr));
	}
	Print (2, "Random Number Seed = %d", random.Seed ());

	length_flag = Report_Flag (TRIP_LENGTH);
	tour_flag = Report_Flag (TOUR_LENGTH);
	trip_purpose_flag = Report_Flag (TRIP_PURPOSE);
	tour_purpose_flag = Report_Flag (TOUR_PURPOSE);
	mode_flag = Report_Flag (MODE_LENGTH);
	mode_purpose_flag = Report_Flag (MODE_PURPOSE);

	return;

control_error:
	Error ("Missing Control Key = %s", Current_Key ());
} 
