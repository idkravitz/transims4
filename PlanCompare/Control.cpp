//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "PlanCompare.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void PlanCompare::Program_Control (void)
{
	int i, dump, num, lvalue;
	char *str_ptr;
	double dvalue, dvalue2;
	
	char *period_control [] = {"MID-TRIP", "ORIGIN", "DESTINATION", NULL};

	//---- open the input plans ----

	bool type_flag = true;

	if (Get_Control_String (NODE_LIST_PATHS) != NULL) {
		type_flag = Get_Control_Flag (NODE_LIST_PATHS);
	}
	str_ptr = Get_Control_String (INPUT_PLAN_FILE);

	if (str_ptr == NULL) goto control_error;

	input_plans.File_Type ("Input Plan File");	

	str_ptr = Project_Filename (str_ptr, Extension ());

	Print_Filename (2, input_plans.File_Type (), str_ptr);

	input_plans.Filename (str_ptr);

	str_ptr = Get_Control_String (INPUT_PLAN_FORMAT);

	if (str_ptr != NULL) {
		input_plans.File_Format (str_ptr);
		Print (1, "%s Format = %s", input_plans.File_Type (), str_ptr);
	}
	input_plans.Node_Based_Flag (type_flag);

	if (!input_plans.Open (0)) {
		File_Error ("Opening Input Plan File", input_plans.Filename ());
	}
	type_flag = input_plans.Node_Based_Flag ();

	//---- open the input trip file ----

	str_ptr = Get_Control_String (INPUT_TRIP_FILE);

	if (str_ptr != NULL) {
		input_trips.File_Type ("Input Trip File");

		str_ptr = Project_Filename (str_ptr);

		if (!input_trips.Open (str_ptr)) {
			File_Error ("Opening Input Trip File", input_trips.Filename ());
		}
	}

	//---- open the compare plans ----

	str_ptr = Get_Control_String (COMPARE_PLAN_FILE);

	if (str_ptr == NULL) goto control_error;

	compare_plans.File_Type ("Compare Plan File");	

	str_ptr = Project_Filename (str_ptr, Extension ());

	Print_Filename (2, compare_plans.File_Type (), str_ptr);

	compare_plans.Filename (str_ptr);

	str_ptr = Get_Control_String (COMPARE_PLAN_FORMAT);

	if (str_ptr != NULL) {
		compare_plans.File_Format (str_ptr);
		Print (1, "%s Format = %s", compare_plans.File_Type (), str_ptr);
	}
	compare_plans.Node_Based_Flag (type_flag);

	if (!compare_plans.Open (0)) {
		File_Error ("Opening Compare Plan File", compare_plans.Filename ());
	}
	if (compare_plans.Node_Based_Flag () != type_flag) {
		Error ("Incompatible Plan Formats");
	}

	//---- open the compare location file ----

	str_ptr = Get_Control_String (COMPARE_LOCATION_FILE);

	if (str_ptr != NULL) {
		compare_loc_file.File_Type ("Compare Location File");

		str_ptr = Project_Filename (str_ptr);

		if (!compare_loc_file.Open (str_ptr)) {
			File_Error ("Opening Compare Location File", compare_loc_file.Filename ());
		}
		compare_loc_flag = true;
	}

	//---- open the compare trip file ----

	str_ptr = Get_Control_String (COMPARE_TRIP_FILE);

	if (str_ptr != NULL) {
		compare_trips.File_Type ("Compare Trip File");

		str_ptr = Project_Filename (str_ptr);

		if (!compare_trips.Open (str_ptr)) {
			File_Error ("Opening Compare Trip File", compare_trips.Filename ());
		}
	}

	//---- open the output plans ----

	str_ptr = Get_Control_String (OUTPUT_PLAN_FILE);

	output_flag = (str_ptr != NULL);

	if (output_flag) {
		output_plans.File_Type ("Output Plan File");	

		str_ptr = Project_Filename (str_ptr, Extension ());

		Print_Filename (2, output_plans.File_Type (), str_ptr);

		output_plans.Filename (str_ptr);
		output_plans.File_Access (CREATE);

		str_ptr = Get_Control_String (OUTPUT_PLAN_FORMAT);

		if (str_ptr != NULL) {
			output_plans.File_Format (str_ptr);
			Print (1, "%s Format = %s", output_plans.File_Type (), str_ptr);
		}
		output_plans.Node_Based_Flag (type_flag);
	}

	//---- open network and demand files ----

	Demand_Service::Program_Control ();

	//---- household list file ----

	str_ptr = Get_Control_String (HOUSEHOLD_LIST);

	if (str_ptr != NULL) {
		hhlist_file.File_Type ("Household List File");

		str_ptr = Project_Filename (str_ptr, Extension ());

		Print_Filename (2, hhlist_file.File_Type (), str_ptr);

		hhlist_file.Filename (str_ptr);
		hhlist_file.Extend (input_plans.Extend ());

		if (!hhlist_file.Open (0)) {
			File_Error ("Opening Household List File", hhlist_file.Filename ());
		}
		hhlist_flag = true;
	}

	//---- get the select travelers ----
	
	str_ptr = Get_Control_String (SELECT_TRAVELERS);

	if (str_ptr != NULL) {
		Print (2, "Select Traveler IDs = %s", str_ptr);

		if (!travelers.Add_Ranges (str_ptr)) {
			File_Error ("Traveler ID Range", str_ptr);
		}
		traveler_flag = (travelers.Num_Ranges () > 0);
	}

	//---- get the select trip modes ----

	for (i=1; i < MAX_MODE; i++) {
		trip_mode [i] = true;
	}
	if (input_trips.Is_Open () && compare_trips.Is_Open ()) {
		str_ptr = Get_Control_String (SELECT_TRIP_MODES);

		if (str_ptr != NULL) {
			Print (2, "Select Trips for Modes = %s", str_ptr);

			memset (trip_mode, '\0', MAX_MODE * sizeof (bool));
			num = 0;
			Data_Range range;
			Range_Data *ptr;

			range.Add_Ranges (str_ptr);

			for (ptr = range.First (); ptr; ptr = range.Next ()) {
				for (i=ptr->Low (); i <= ptr->High (); i++) {
					if (i < 1 || i >= MAX_MODE) {
						Error ("Selected Mode %d is Out of Range (1..%d)", i, MAX_MODE-1);
					}
					trip_mode [i] = true;
					num++;
				}
			}
			if (num == 0) {
				Error ("No Modes were Selected for Processing");
			}
			drive_access_flag = (trip_mode [5] || trip_mode [6] || trip_mode [13] || trip_mode [14]);
			if (drive_access_flag) mode_flag = true;
		}
	}

	//---- ignore location differences ----

	str_ptr = Get_Control_String (IGNORE_LOCATION_DIFFERENCES);

	if (str_ptr != NULL) {
		Print (2, "Ignore Location Differences = %s", str_ptr);

		ignore_flag = Get_Control_Flag (IGNORE_LOCATION_DIFFERENCES);
	}

	//---- index by compare location ----

	if (ignore_flag) {
		str_ptr = Get_Control_String (INDEX_BY_COMPARE_LOCATION);

		if (str_ptr != NULL) {
			Print (2, "Index by Compare Location = %s", str_ptr);

			loc_flag = Get_Control_Flag (INDEX_BY_COMPARE_LOCATION);
		}
	}

	//---- only compare drive legs ----

	str_ptr = Get_Control_String (ONLY_COMPARE_DRIVE_LEGS);

	if (str_ptr != NULL) {
		Print (2, "Only Compare Drive Legs = %s", str_ptr);

		drive_flag = Get_Control_Flag (ONLY_COMPARE_DRIVE_LEGS);
	}

	//---- compare generalized costs ----

	str_ptr = Get_Control_String (COMPARE_GENERALIZED_COSTS);

	if (str_ptr != NULL) {
		Print (2, "Compare Generalized Costs = %s", str_ptr);

		cost_flag = Get_Control_Flag (COMPARE_GENERALIZED_COSTS);
	}

	//---- get the percent time difference ----

	str_ptr = Get_Control_String (PERCENT_TIME_DIFFERENCE);

	if (str_ptr != NULL) {
		Get_Double (str_ptr, &percent_diff);

		if (percent_diff < 0.0 || percent_diff > 100.0) {
			Error ("Percent Time Difference %.2lf is Out of Range (0.0-100.0)", percent_diff);
		}
		if (percent_diff == 0.0) {
			Print (2, "Time Differences are Not Considered");
		}
	}
	time_diff_flag = (percent_diff != 0.0);

	if (time_diff_flag) {
		Print (2, "Percent Time Difference = %.1lf%%", percent_diff);

		percent_diff /= 100.0;

		//---- get the minimum time difference ----

		str_ptr = Get_Control_String (MINIMUM_TIME_DIFFERENCE);

		if (str_ptr != NULL) {
			Get_Double (str_ptr, &dvalue);

			if (dvalue < 0.0 || dvalue > 60.0) {
				Error ("Minimum Time Difference %.2lf is Out of Range (0-60)", dvalue);
			}
		} else {
			dvalue = min_time;
		}
		Print (1, "Minimum Time Difference = %.2lf minutes", dvalue);

		//---- get the maximum time difference ----

		str_ptr = Get_Control_String (MAXIMUM_TIME_DIFFERENCE);

		if (str_ptr != NULL) {
			Get_Double (str_ptr, &dvalue2);

			if (dvalue2 < dvalue || dvalue2 > 1440) {
				Error ("Maximum Time Difference %.2lf is Out of Range (%.2lf-1440)", dvalue2, dvalue);
			}
		} else {
			dvalue2 = max_time;
		}
		Print (1, "Maximum Time Difference = %.2lf minutes", dvalue2);

		min_time = (int) (dvalue * 60.0 + 0.5);
		max_time = (int) (dvalue2 * 60.0 + 0.5);
	}

	//---- get the percent path difference ----

	str_ptr = Get_Control_String (PERCENT_PATH_DIFFERENCE);

	if (str_ptr != NULL) {
		Get_Double (str_ptr, &path_diff);

		if (path_diff < 0.0 || path_diff > 100.0) {
			Error ("Percent Path Difference %.2lf is Out of Range (0.0-100.0)", path_diff);
		}
		if (path_diff == 0.0) {
			Print (2, "Path Differences are Not Considered");
		}
	}
	path_diff_flag = (path_diff != 0.0);

	if (path_diff_flag) {
		Print (2, "Percent Path Difference = %.1lf%%", path_diff);

		path_diff /= 100.0;

		//---- get the minimum path difference ----

		str_ptr = Get_Control_String (MINIMUM_PATH_DIFFERENCE);

		if (str_ptr != NULL) {
			Get_Integer (str_ptr, &min_path);

			if (min_path < 0 || min_path > 100) {
				Error ("Minimum Path Difference %d is Out of Range (0-100)", min_path);
			}
		}
		Print (1, "Minimum Path Difference = %d legs", min_path);

		//---- get the maximum path difference ----

		str_ptr = Get_Control_String (MAXIMUM_PATH_DIFFERENCE);

		if (str_ptr != NULL) {
			Get_Integer (str_ptr, &max_path);

			if (max_path < min_path || max_path > 100000) {
				Error ("Maximum Path Difference %d is Out of Range (%d-100000)", max_path, min_path);
			}
		}
		Print (1, "Maximum Path Difference = %d legs", max_path);
	}

	if (!time_diff_flag && !path_diff_flag) {
		Error ("Time Difference and/or Path Difference Criteria are Required");
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

		if (max_percent < 0.1 || max_percent > 100.0) {
			Error ("Maximum Percent Selected %.1lf is Out of Range (0.1-100.0)", max_percent);
		}
	}
	Print (1, "Maximum Percent Selected = %.1lf%%", max_percent);

	max_percent /= 100.0;

	//---- select by percent difference ----

	str_ptr = Get_Control_String (SELECT_BY_PERCENT_DIFFERENCE);

	if (str_ptr != NULL) {
		diff_flag = Get_Control_Flag (SELECT_BY_PERCENT_DIFFERENCE);

		Print (1, "Select by Percent Difference = %s", str_ptr);
	}
	if (!diff_flag) {
		str_ptr = Get_Control_String (SELECT_BY_RELATIVE_GAP);

		if (str_ptr != NULL) {
			gap_flag = Get_Control_Flag (SELECT_BY_RELATIVE_GAP);

			Print (1, "Select by Relative Gap = %s", str_ptr);

			if (gap_flag && !time_diff_flag) {
				Error ("Time Differences Must be Specified for Relative Gap Selection");
			}
		}
	}

	//---- get the select time periods ----
	
	str_ptr = Get_Control_String (SELECT_TIME_PERIODS);

	if (str_ptr != NULL) {
		Print (1, "Select Time Periods = %s", str_ptr);

		select_time.Format ("24_HOUR_CLOCK");

		if (!select_time.Add_Ranges (str_ptr)) {
			File_Error ("Select Time Period Range", str_ptr);
		}
	}

	if (!diff_flag) {

		//---- random number seed ----

		str_ptr = Get_Control_String (RANDOM_NUMBER_SEED);

		if (str_ptr != NULL) {
			random.Seed (atoi (str_ptr));
		}
		if (str_ptr != NULL || percent != 1.0 || max_percent != 1.0) {
			Print (1, "Random Number Seed = %d", random.Seed ());
		}
	}

	//---- get the time of day format ----
	
	str_ptr = Get_Control_String (TIME_OF_DAY_FORMAT);

	if (str_ptr == NULL) {
		str_ptr = "24_HOUR_CLOCK";
	}
	if (!time_periods.Format (str_ptr)) {
		Error ("Time of Day Format %s was Unrecognized", str_ptr);
	}
	Print (2, "Time of Day Format = %s", str_ptr);

	//---- get summary increment ----
	
	str_ptr = Get_Control_String (SUMMARY_TIME_INCREMENT);

	if (str_ptr != NULL) {
		increment = atol (str_ptr);

		if (increment < 0 || increment > 240) {
			Error ("Summary Time Increment %d is Out of Range (0-240)", increment);
		}
		increment *= 60;
	}
	if (increment != 0) {
		time_periods.Period_Flag (true);
	}
	time_periods.Increment (increment);

	//---- get the summary time periods ----
	
	str_ptr = Get_Control_String (SUMMARY_TIME_PERIODS);

	if (str_ptr == NULL) {
		str_ptr = (char *) "All";
	}
	Print (2, "Summary Time Periods = %s", str_ptr);

	if (!time_periods.Add_Ranges (str_ptr)) {
		File_Error ("Time Period Range", str_ptr);
	}
	num_inc = time_periods.Num_Ranges ();

	if (increment > 0) {
		Print (1, "Summary Time Increment = %d minutes", (increment / 60));
	} else {
		Print (1, "One Summary Increment per Time Period will be Output");
	}

	//---- get the period control point ----

	str_ptr = Get_Control_String (PERIOD_CONTROL_POINT);

	if (str_ptr != NULL) {
		for (i=0; period_control [i] != NULL; i++) {
			if (str_cmp (str_ptr, period_control [i]) == 0) break;
		}
		if (period_control [i] == NULL) {
			Error ("Period Control Point %s was Unrecognized", str_ptr);
		}
		period_point = i;
	} else {
		period_point = 0;
	}
	Print (1, "Period Control Point = %s", period_control [period_point]);

	//---- get the highest match difference ----

	str_ptr = Get_Control_String (HIGHEST_MATCH_DIFFERENCE);

	if (str_ptr != NULL) {
		Get_Integer (str_ptr, &lvalue);

		if (lvalue < 0) {
			Error ("Highest Match Difference %d is Out of Range (>=0)", lvalue);
		}
		Print (1, "Highest Match Difference = %d", lvalue);
		if (lvalue > 0) high_diff = lvalue;
	}

	//---- get the lowest match difference ----

	str_ptr = Get_Control_String (LOWEST_MATCH_DIFFERENCE);

	if (str_ptr != NULL) {
		Get_Integer (str_ptr, &lvalue);

		if (lvalue > 0) {
			Error ("Lowest Match Difference %d is Out of Range (<=0)", lvalue);
		}
		Print (1, "Lowest Match Difference = %d", lvalue);
		if (lvalue < 0) low_diff = lvalue;
	}

	//---- open the distribution file ----

	str_ptr = Get_Control_String (NEW_DISTRIBUTION_FILE);

	if (str_ptr != NULL) {
		time_diff.Open_Distribution (str_ptr);
		distrib_flag = time_flag = true;
	}

	//---- open the cost distribution file ----

	str_ptr = Get_Control_String (NEW_COST_DISTRIBUTION_FILE);

	if (str_ptr != NULL) {
		cost_diff.Open_Distribution (str_ptr);
		cost_distrib_flag = cost_data_flag = true;
	}

	//---- open the mode distribution file ----

	str_ptr = Get_Control_String (NEW_MODE_DISTRIBUTION_FILE);

	if (str_ptr != NULL) {
		mode_diff.Open_Mode_Distribution (str_ptr);
		mode_diff.Generalized_Cost (cost_flag);
		mode_flag = mode_file_flag = true;
	}

	//---- open the mode distribution data file ----

	str_ptr = Get_Control_String (NEW_MODE_DISTRIBUTION_DATA_FILE);

	if (str_ptr != NULL) {
		mode_data.Open_Mode_Distribution (str_ptr, 5, false);
		mode_data.Generalized_Cost (cost_flag);
		mode_flag = mode_data_flag = true;
	}

	//---- open the difference file ----

	str_ptr = Get_Control_String (NEW_DIFFERENCE_FILE);

	if (str_ptr != NULL) {
		diff_file_flag = true;

		diff_file.File_Type ("New Difference File");
		diff_file.File_Access (Db_Code::CREATE);

		Print (1);
		if (!diff_file.Open (Project_Filename (str_ptr))) {
			File_Error ("Creating New Difference File", diff_file.Filename ());
		}

		//---- write the file header ----

		if (cost_flag) {
			fprintf (diff_file.File (), "HOUSEHOLD\tPERSON\tTRIP\tINPUT_COST\tCOMPARE_COST\tDIFFERENCE\n");
		} else {
			fprintf (diff_file.File (), "HOUSEHOLD\tPERSON\tTRIP\tINPUT_TIME\tCOMPARE_TIME\tDIFFERENCE\n");
		}
	}

	//---- open the start difference file ----

	str_ptr = Get_Control_String (NEW_START_DIFFERENCE_FILE);

	if (str_ptr != NULL) {
		start_flag = true;

		start_file.File_Type ("New Start Difference File");
		start_file.File_Access (Db_Code::CREATE);

		Print (1);
		if (!start_file.Open (Project_Filename (str_ptr))) {
			File_Error ("Creating New Start Difference File", start_file.Filename ());
		}

		//---- write the file header ----

		if (cost_flag) {
			fprintf (start_file.File (), "START_ID\tTYPE\tCOUNT\tINPUT_COST\tCOMPARE_COST\tDIFFERENCE\n");
		} else {
			fprintf (start_file.File (), "START_ID\tTYPE\tCOUNT\tINPUT_TIME\tCOMPARE_TIME\tDIFFERENCE\n");
		}
	}

	//---- open the end difference file ----

	str_ptr = Get_Control_String (NEW_END_DIFFERENCE_FILE);

	if (str_ptr != NULL) {
		end_flag = true;

		end_file.File_Type ("New End Difference File");
		end_file.File_Access (Db_Code::CREATE);

		Print (1);
		if (!end_file.Open (Project_Filename (str_ptr))) {
			File_Error ("Creating New End Difference File", end_file.Filename ());
		}

		//---- write the file header ----

		if (cost_flag) {
			fprintf (end_file.File (), "END_ID\tTYPE\tCOUNT\tINPUT_COST\tCOMPARE_COST\tDIFFERENCE\n");
		} else {
			fprintf (end_file.File (), "END_ID\tTYPE\tCOUNT\tINPUT_TIME\tCOMPARE_TIME\tDIFFERENCE\n");
		}
	}

	//---- open the trip time file ----

	str_ptr = Get_Control_String (NEW_TRIP_TIME_FILE);

	if (str_ptr != NULL) {
		trip_flag = true;

		time_file.File_Type ("New Trip Time File");
		time_file.File_Access (Db_Code::CREATE);

		Print (1);
		if (!time_file.Open (Project_Filename (str_ptr))) {
			File_Error ("Creating New Trip Time File", time_file.Filename ());
		}

		//---- write the file header ----

		fprintf (time_file.File (), "PERIOD\tINPUT_START\tCOMPARE_START\tSTART_DIFF\tINPUT_END\tCOMPARE_END\tEND_DIFF\n");

		memset (trip_time, '\0', sizeof (trip_time));
	}

	//---- open the traveler match file ----

	str_ptr = Get_Control_String (NEW_TRAVELER_MATCH_FILE);

	if (str_ptr != NULL) {
		match_out_flag = true;

		match_file.File_Type ("New Traveler Match File");
		match_file.File_Access (Db_Code::CREATE);

		Print (1);
		if (!match_file.Open (Project_Filename (str_ptr))) {
			File_Error ("Creating New Traveler Match File", match_file.Filename ());
		}

		//---- write the file header ----

		fprintf (match_file.File (), "TRAVELER\tTRIP\tSTART_TIME1\tSTART_ID1\tSTART_TYPE1\tEND_TIME1\tEND_ID1\tEND_TYPE1\tTTIME1\tGCOST1");
		fprintf (match_file.File (), "\tSTART_TIME2\tSTART_ID2\tSTART_TYPE2\tEND_TIME2\tEND_ID2\tEND_TYPE2\tTTIME2\tGCOST2");
		fprintf (match_file.File (), "\tTIME_DIFF\tGCOST_DIFF");

	}

	//---- open the incomplete trip file ----

	str_ptr = Get_Control_String (NEW_INCOMPLETE_TRIP_FILE);

	if (str_ptr != NULL) {
		incomplete_flag = true;

		incomplete_file.File_Type ("New Incomplete Trip File");
		incomplete_file.File_Access (Db_Code::CREATE);

		Print (1);
		if (!incomplete_file.Open (Project_Filename (str_ptr))) {
			File_Error ("Creating New Incomplete Trip File", match_file.Filename ());
		}

		//---- write the file header ----

		fprintf (incomplete_file.File (), "TRAVELER\tTRIP\tMODE\tTRIP_TIME\tTYPE\tTTIME1\tGCOST1\tTTIME2\tGCOST2\tTIME_DIFF\tGCOST_DIFF");
	}

	//---- open the period difference file ----

	str_ptr = Get_Control_String (NEW_PERIOD_DIFFERENCE_FILE);

	if (str_ptr != NULL) {
		period_diff_flag = true;

		period_file.File_Type ("New Period Difference File");
		period_file.File_Access (Db_Code::CREATE);

		Print (1);
		if (!period_file.Open (Project_Filename (str_ptr))) {
			File_Error ("Creating New Period Difference File", period_file.Filename ());
		}

		//---- write the file header ----

		if (cost_flag) {
			fprintf (period_file.File (), "PERIOD\tCOUNT\tINPUT_COST\tCOMPARE_COST\tDIFFERENCE\tTRIP_GAP\n");
		} else {
			fprintf (period_file.File (), "PERIOD\tCOUNT\tINPUT_TIME\tCOMPARE_TIME\tDIFFERENCE\tTRIP_GAP\n");
		}
	}
	if (period_diff_flag || Report_Flag (TRIP_GAP)) {
		period_diff.Num_Records (num_inc);
		gap_data_flag = true;
	}

	//---- read report types ----

	compare_flag = path_flag = false;
	dump_path = dump_time = dump_cost = MAX_INTEGER;

	for (i=First_Report (); i != 0; i=Next_Report ()) {
		compare_flag = true;

		if (i == PATH_CHANGE || i == DUMP_PATH_CHANGES) {
			path_flag = true;
			if (i == DUMP_PATH_CHANGES) {
				dump = (int) (Report_Data () + 0.5);
				if (dump < dump_path) {
					dump_path = dump;
				}
			}
		} else if (i == DUMP_TIME_CHANGES) {
			dump = (int) (Report_Data () + 0.5);
			if (dump < dump_time) {
				dump_time = dump;
			}
		} else if (i == COST_CHANGE || i == DUMP_COST_CHANGES) {
			cost_data_flag = true;
			if (i == DUMP_COST_CHANGES) {
				dump = (int) (Report_Data () + 0.5);
				if (dump < dump_cost) {
					dump_cost = dump;
				}
			}
		}
	}

	//---- report space ----

	memset (time_changes, '\0', sizeof (time_changes));
	memset (path_changes, '\0', sizeof (path_changes));
	memset (cost_changes, '\0', sizeof (cost_changes));

	increment = MIDNIGHT / TOTAL_TIME_PERIOD;

	if (Report_Flag (TTIME_DISTRIB) || Report_Flag (TOTAL_DISTRIB) ||
		Report_Flag (COST_DISTRIB) || Report_Flag (TCOST_DISTRIB)) time_flag = true;

	period_flag = Report_Flag (PERIOD_SUM) || Report_Flag (COST_SUM);

	//---- allocate space for change data ----

	if (!time_diff.Time_Periods (&time_periods) || !cost_diff.Time_Periods (&time_periods)) {
		Error ("Insufficient Memory for Time Period Data");
	}
	return;

	//---- error message ----

control_error:
	Error ("Missing Control Key = %s", Current_Key ());
} 
