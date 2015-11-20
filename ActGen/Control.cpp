//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "ActGen.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void ActGen::Program_Control (void)
{
	int i, j, num_group;
	double factor;
	char *str_ptr, buffer [FIELD_BUFFER], *format_ptr;
	
	Purpose_Group *group_ptr;
	Skim_Group *skim_ptr;

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

	zone_flag = Network_File_Flag (ZONE);
	update_flag = Demand_File_Flag (ACTIVITY);

	//---- copy the replicate activity file ----

	if (update_flag) {
		if (!hhlist_flag) {
			Error ("A Household List file is required for Activity Updates");
		}
		Activity_File *file, *new_file;

		file = (Activity_File *) Demand_Db_Base (ACTIVITY);
		new_file = (Activity_File *) Demand_Db_Base (NEW_ACTIVITY);

		new_file->Clear_Fields ();
		new_file->Replicate_Fields (file);

		new_file->Write_Header ();
	}

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

	//---- open the problem file ----
	
	str_ptr = Get_Control_String (NEW_PROBLEM_FILE);

	if (str_ptr != NULL) {
		Print (1);

		format_ptr = Get_Control_String (NEW_PROBLEM_FORMAT);

		if (format_ptr != NULL) {
			problem_file.Dbase_Format (format_ptr);
		}
		problem_file.ActGen_Flag ();
		if (!problem_file.Create (Project_Filename (str_ptr, Extension ()))) {
			Error ("Creating %s", problem_file.File_Type ());
		}
		problem_flag = true;
	}

	//---- open the household match file ----

	str_ptr = Get_Control_String (NEW_HOUSEHOLD_MATCH_FILE);

	if (str_ptr != NULL) {
		match_flag = true;
		match_file.File_Type ("New Household Match File");

		if (!match_file.Create (Project_Filename (str_ptr, Extension ()))) {
			Error ("Creating %s", match_file.File_Type ());
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

	if (str_ptr != NULL) {
		Print (1);
		new_pattern_file.File_Type ("New Activity Pattern File");

		format_ptr = Get_Control_String (NEW_ACTIVITY_PATTERN_FORMAT);

		if (format_ptr != NULL) {
			new_pattern_file.Dbase_Format (format_ptr);
		}
		if (!new_pattern_file.Create (Project_Filename (str_ptr, Extension ()))) {
			Error ("Creating %s", new_pattern_file.File_Type ());
		}
		new_pattern_flag = true;
	}

	//---- open the trip time file ----

	str_ptr = Get_Control_String (NEW_TRIP_TIME_FILE);

	if (str_ptr != NULL) {
		if (trip_time_length.Open_Trip_Length_File (str_ptr, "New Trip Time File")) {
			time_length_flag = true;
			trip_time_length.Increment (60);	//---- minutes ----
		}
	}

	//---- open the trip distance file ----

	str_ptr = Get_Control_String (NEW_TRIP_DISTANCE_FILE);

	if (str_ptr != NULL) {
		if (trip_distance_length.Open_Trip_Length_File (str_ptr, "New Trip Distance File")) {
			distance_length_flag = true;
			trip_distance_length.Increment (500);	//---- 500 meters ----
		}
	}

	//---- open the balancing factor file ----

	str_ptr = Get_Control_String (BALANCING_FACTOR_FILE);

	if (str_ptr != NULL) {
		balance_file.File_Type ("Balancing Factor File");
		Print (1);

		format_ptr = Get_Control_String (BALANCING_FACTOR_FORMAT);

		if (format_ptr != NULL) {
			balance_file.Dbase_Format (format_ptr);
		}
		balance_file.Open (Project_Filename (str_ptr));
		balance_flag = true;

		if (balance_file.Field_Number ("ZONE") == 0) {
			Error ("Balancing Factor File ZONE field Not Found");
		}
	}

	//---- get the distance calculation ----

	str_ptr = Get_Control_String (DISTANCE_CALCULATION);

	if (str_ptr != NULL) {
		distance_method = Distance_Code (str_ptr);
	}
	Print (2, "Distance Calculation = %s", Distance_Code (distance_method));

	//---- get the maximum walk distance ----

	str_ptr = Get_Control_String (MAXIMUM_WALK_DISTANCE);

	if (str_ptr != NULL) {
		Get_Integer (str_ptr, &max_walk);

		if (max_walk < 100 || max_walk > 10000) {
			Error ("Maximum Walk Distance %d is Out of Range (100..10000)", max_walk);
		}
		Print (1, "Maximum Walk Distance = %d meters", max_walk);
	}

	//---- get the average travel speed ----

	str_ptr = Get_Control_String (AVERAGE_TRAVEL_SPEED);

	if (str_ptr != NULL) {
		Print (1, "Average Travel Speed = %s ... meters/second", str_ptr);
		factor = 1.0;

		for (j=1; j < MAX_MODE; j++) {
			if (str_ptr != NULL) {
				str_ptr = Get_Double (str_ptr, &factor);

				if (Round (factor) <= 0) {
					Error ("Averge Speed %.1lf is Out of Range", factor);
				}
			}
			avg_speed [j] = Round (factor);
		}
	}

	//---- get the additional travel time ----

	str_ptr = Get_Control_String (ADDITIONAL_TRAVEL_TIME);

	if (str_ptr != NULL) {
		Print (1, "Additional Travel Time = %s ... seconds", str_ptr);
		i = 0;

		for (j=1; j < MAX_MODE; j++) {
			if (str_ptr != NULL) {
				str_ptr = Get_Integer (str_ptr, &i);
			}
			add_time [j] = i;
		}
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

	//---- get the number of groups ----

	num_group = Highest_Control_Group (ACTIVITY_PURPOSE_RANGE_x, 0);

	if (num_group == 0) {
		Error ("No Activity Purpose Groups were Found");
	}

	//---- allocate purpose space ----

	if (!purpose_group.Max_Records (num_group)) {
		Error ("Insufficient Memory for %d Activity Purpose Groups", num_group);
	}

	//---- process each file ----

	for (i=1; i <= num_group; i++) {
		
		str_ptr = Get_Control_String (ACTIVITY_PURPOSE_RANGE_x, i);

		if (str_ptr == NULL) continue;

		if (!Break_Check (7)) {
			Print (1);
		}
		group_ptr = purpose_group.New_Record (true);

		group_ptr->Group (i);

		Print (1, "Activity Purpose Range #%d = %s", i, str_ptr);

		if (!group_ptr->Purpose_Range (str_ptr)) {
			Error ("Activity Purpose Range %s is Illogical", str_ptr);
		}

		//---- get the anchor flag ----

		group_ptr->Anchor (Get_Control_Flag (ACTIVITY_ANCHOR_FLAG_x, i));

		Print (1, "Activity Anchor Flag #%d = %s", i, (group_ptr->Anchor () ? "True" : "False"));

		//---- get the schedule constraint ----

		str_ptr = Get_Control_String (SCHEDULE_CONSTRAINT_x, i);

		if (str_ptr != NULL) {
			group_ptr->Constraint (Constraint_Code (str_ptr));
		} else {
			group_ptr->Constraint (NO_CONSTRAINT);
		}
		Print (1, "Schedule Constraint #%d = %s", i, Constraint_Code (group_ptr->Constraint ()));

		//---- get the zone based method flag ----

		str_ptr = Get_Control_String (ZONE_BASED_METHOD_x, i);

		if (str_ptr != NULL) {
			group_ptr->Method (Get_Control_Flag (ZONE_BASED_METHOD_x, i));
		} else {
			group_ptr->Method (true);
		}
		Print (1, "Zone-Based Method #%d = %s", i, (group_ptr->Method () ? "True" : "False"));

		if (group_ptr->Method () && !zone_flag) {
			Error ("A Zone file is required for the Zone-Based Method");
		}

		//---- get the location choice script -----

		str_ptr = Get_Control_String (LOCATION_CHOICE_SCRIPT_x, i);

		if (str_ptr != NULL) {
			Db_File *file = new Db_File ();

			str_fmt (buffer, sizeof (buffer), "Location Choice Script #%d", i);

			file->File_Type (buffer);
			file->Open (Project_Filename (str_ptr));

			group_ptr->Script_File (file);
			choice_flag = skim_flag = true;
		}

		//---- get the zone weight field ----

		str_ptr = Get_Control_String (ZONE_WEIGHT_FIELD_x, i);

		if (str_ptr != NULL) {
			if (!group_ptr->Method ()) {
				Warning ("Zone Weight Field is only used for the Zone-Based Method");
			} else {
				Db_Base *file = Network_Db_Base (ZONE);

				group_ptr->Zone_Field (file->Field_Number (str_ptr));

				if (group_ptr->Zone_Field () == 0) {
					Error ("Zone Weight Field %s was Not Found", str_ptr);
				}
				Print (1, "Zone Weight Field Name = %s, Number = %d", str_ptr, group_ptr->Zone_Field ());

				//---- get the zone weight factor ----

				str_ptr = Get_Control_String (ZONE_WEIGHT_FACTOR_x, i);

				if (str_ptr != NULL) {
					factor = atof (str_ptr);

					group_ptr->Zone_Factor (factor);
				} else {
					group_ptr->Zone_Factor (1.0);
				}
				Print (1, "Zone Weight Factor = %g", group_ptr->Zone_Factor ());

				str_ptr = Get_Control_String (BALANCING_FACTOR_FIELD_x, i);

				if (str_ptr != NULL) {
					if (!balance_flag) {
						Error ("A Balancing Factor File is Required for Field Processing");
					}
					group_ptr->Balance_Field (balance_file.Field_Number (str_ptr));

					if (group_ptr->Balance_Field () == 0) {
						Error ("Balancing Factor Field %s was Not Found", str_ptr);
					}
					Print (1, "Balancing Factor Field Name = %s, Number = %d", str_ptr, group_ptr->Balance_Field ());
				} else {
					group_ptr->Balance_Field (0);
				}
			}
		} else {
			group_ptr->Zone_Field (0);
			
			if (!group_ptr->Model () && group_ptr->Method ()) {
				Error ("Zone Weight Field is required for the Default Zone-Based Model");
			}
		}

		//---- get the location weight field ----

		str_ptr = Get_Control_String (LOCATION_WEIGHT_FIELD_x, i);

		if (str_ptr != NULL) {
			Db_Base *file = Network_Db_Base (ACTIVITY_LOCATION);

			group_ptr->Location_Field (file->Field_Number (str_ptr));

			if (group_ptr->Location_Field () == 0) {
				Error ("Location Weight Field %s was Not Found", str_ptr);
			}
			Print (1, "Location Weight Field Name = %s, Number = %d", str_ptr, group_ptr->Location_Field ());
		} else {
			group_ptr->Location_Field (0);

			if (!group_ptr->Model () && !group_ptr->Method ()) {
				Error ("Location Weight Field is required for the Default Location-Based Model");
			} else {
				Print (1, "All Locations have Equal Weight (1.0)");
			}
		}

		//---- get the location weighting factor ----

		if (!group_ptr->Method ()) {
			str_ptr = Get_Control_String (LOCATION_WEIGHT_FACTOR_x, i);

			if (str_ptr != NULL) {
				factor = atof (str_ptr);

				group_ptr->Location_Factor (factor);
			} else {
				group_ptr->Location_Factor (1.0);
			}
			Print (1, "Location Weight Factor = %g", group_ptr->Location_Factor ());
		}

		//---- default model keys ----

		if (!group_ptr->Model ()) {

			//---- get the skim time field ----

			str_ptr = Get_Control_String (SKIM_TIME_FIELD_x, i);

			if (str_ptr != NULL) {
				Print (1, "Skim Time Field = %s", str_ptr);

				str_cpy (buffer, sizeof (buffer), str_ptr);

				str_ptr = strchr (str_ptr, '.');

				if (str_ptr == NULL) goto field_error;
				*str_ptr++ = '\0';

				if (str_cmp (buffer, "Skim", 4)) goto field_error;

				group_ptr->Time_Skim (atoi (buffer+4));

				if (group_ptr->Time_Skim () == 0) goto field_error;

				group_ptr->Skim_Field (str_ptr);
				skim_flag = true;
			} else {
				Print (1, "The Location Choice Model is Distance-Based");
				group_ptr->Time_Skim (0);
				group_ptr->Time_Field (0);
			}

			//---- mode factors ----

			if (group_ptr->Time_Skim () == 0) {

				//---- get the mode distance factors ----

				str_ptr = Get_Control_String (MODE_DISTANCE_FACTORS_x, i);

				if (str_ptr != NULL) {
					Print (1, "Mode Distance Factors = %s ... (*meters)", str_ptr);
					factor = 1.0;

					for (j=1; j < MAX_MODE; j++) {
						if (str_ptr != NULL) {
							str_ptr = Get_Double (str_ptr, &factor);

							if (factor > 0.0) {
								Error ("Mode Distance Factor %g is Not Negative", factor);
							}
						}
						group_ptr->Distance_Factor (j, factor);
					}
				} else {
					for (j=1; j < MAX_MODE; j++) {
						group_ptr->Distance_Factor (j, -0.001);
					}
					Print (1, "Mode Distance Factors = %g", group_ptr->Distance_Factor (1));
				}

			} else {

				//---- get the mode time factors ----

				str_ptr = Get_Control_String (MODE_TIME_FACTORS_x, i);

				if (str_ptr != NULL) {
					Print (1, "Mode Time Factors = %s ... (*skim)", str_ptr);
					factor = 1.0;

					for (j=1; j < MAX_MODE; j++) {
						if (str_ptr != NULL) {
							str_ptr = Get_Double (str_ptr, &factor);

							if (factor > 0.0) {
								Error ("Mode Time Factor %g is Not Negative", factor);
							}
						}
						group_ptr->Time_Factor (j, factor);
					}
				} else {
					for (j=1; j < MAX_MODE; j++) {
						group_ptr->Time_Factor (j, -0.001);
					}
					Print (1, "Mode Time Factors = %g", group_ptr->Time_Factor (1));
				}
			}
		}

		//---- add the group record to the list ----

		purpose_group.Add ();
	}

	//---- get the number of skims ----

	num_group = Highest_Control_Group (ZONE_SKIM_FILE_x, 0);

	if (skim_flag && num_group != 0) {

		//---- process each file ----

		for (i=1; i <= num_group; i++) {
			
			str_ptr = Get_Control_String (ZONE_SKIM_FILE_x, i);

			if (str_ptr == NULL) continue;

			if (!Break_Check (4)) {
				Print (1);
			}
			skim_ptr = new Skim_Group ();

			skim_ptr->Group (i);

			format_ptr = Get_Control_String (ZONE_SKIM_FORMAT_x, i);

			if (format_ptr != NULL) {
				skim_ptr->Skim_Format (format_ptr);
			}
			skim_ptr->Skim_Filename (Project_Filename (str_ptr));

			//---- time period equiv ----

			str_ptr = Get_Control_String (TIME_PERIOD_EQUIVALENCE_x, i);

			if (str_ptr != NULL) {
				skim_ptr->Time_Period_File (Project_Filename (str_ptr));
			}

			//---- skim memory factor ----

			str_ptr = Get_Control_String (SKIM_MEMORY_FACTOR_x, i);

			if (str_ptr != NULL) {
				factor = atof (str_ptr);

				if (factor < 0.05 || factor > 1.0) {
					Error ("Skim Memory Factor %.2lf is Out of Range (0.05..1.0)", factor);
				}
				skim_ptr->Memory_Factor (factor);
				Print (1, "Skim Memory Factor #%d = %.2lf", i, factor);
			}

			//---- save the skim group ----

			if (!skim_group.Add (skim_ptr)) {
				Error ("Adding Zone Skim Group");
			}
		}
	}

	//---- check for travel time fields ----

	if (Highest_Control_Group (TRAVEL_TIME_FIELD_MODE_x, 0) > 0) {
		Print (1);
	}

	for (i=1; i < MAX_MODE; i++) {
		str_ptr = Get_Control_String (TRAVEL_TIME_FIELD_MODE_x, i);

		if (str_ptr != NULL) {
			Print (1, "Travel Time Field Mode %d = %s", i, str_ptr);

			str_cpy (buffer, sizeof (buffer), str_ptr);

			str_ptr = strchr (str_ptr, '.');

			if (str_ptr == NULL) goto field_error;
			*str_ptr++ = '\0';

			if (str_cmp (buffer, "Skim", 4)) goto field_error;

			num_group = atoi (buffer+4);

			skim_ptr = skim_group [num_group];

			if (skim_ptr == NULL) {
				Error ("Skim %d was Not Found for Travel Time Field Mode %d", num_group, i);
			}
			Db_Base *file = skim_ptr->Skim_A ();

			j = file->Field_Number (str_ptr);

			if (j == 0) {
				Error ("Skim%d Field %s was Not Found", num_group, str_ptr);
			}
			time_field [i] = (num_group << 16) + j;
		}
	}

	//---- set the activity location database fields ----

	location_db.Replicate_Fields (Network_Db_Base (ACTIVITY_LOCATION), false);
	location_db.File_ID ("Location");

	household_db.Replicate_Fields (Demand_Db_Base (HOUSEHOLD), false);
	household_db.File_ID ("Household");

	if (zone_flag) {
		zone_db.Replicate_Fields (Network_Db_Base (ZONE), false);
		zone_db.File_ID ("Zone");
	}
	if (skim_flag) {
		for (group_ptr = purpose_group.First (); group_ptr; group_ptr = purpose_group.Next ()) {
			i = group_ptr->Time_Skim ();

			if (i > 0) {
				skim_ptr = skim_group [i];

				if (skim_ptr == NULL) {
					Error ("Skim %d was Not Found for Purpose Group %d", i, group_ptr->Group ());
				}
				Db_Base *file = skim_ptr->Skim_A ();

				group_ptr->Time_Field (file->Field_Number (group_ptr->Skim_Field ()));

				if (group_ptr->Time_Field () == 0) {
					Error ("Skim%d Time Field %s was Not Found for Group %d", i, group_ptr->Skim_Field (), group_ptr->Group ());
				}
			}
		}
	}
	length_flag = Report_Flag (TRIP_LENGTH);
	tour_flag = Report_Flag (TOUR_LENGTH);
	trip_purpose_flag = Report_Flag (TRIP_PURPOSE);
	tour_purpose_flag = Report_Flag (TOUR_PURPOSE);
	mode_flag = Report_Flag (MODE_LENGTH);
	mode_purpose_flag = Report_Flag (MODE_PURPOSE);
	act_problem_flag = Report_Flag (ACT_PROBLEM);

	//---- setup the dump list ----

	dump_list_flag = Report_Flag (LOCATION_DETAIL);

	if (dump_list_flag) {
		for (i=First_Report (); i != 0; i=Next_Report ()) {
			if (i == LOCATION_DETAIL) {
				j = (int) Report_Data ();

				if (j == 0) {
					dump_flag = true;
					dump_list_flag = false;
					break;
				}
				if (!dump_list.Add (j)) {
					Error ("Adding Household %d to the Dump List", j);
				}
			}
		}
	}
	return;

control_error:
	Error ("Missing Control Key = %s", Current_Key ());

field_error:
	Error ("Skim File Number is Missing from %s", buffer);
} 
