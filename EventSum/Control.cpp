//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "EventSum.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void EventSum::Program_Control (void)
{
	int i;
	char *str_ptr, *format_ptr;
	double dvalue, dvalue2;

	Range_Data *range_ptr;

	//---- open network and demand files ----

	Demand_Service::Program_Control ();

	//---- override default trip and activity processing ----

	trip_file_flag = Demand_File_Flag (TRIP);
	if (trip_file_flag) Demand_File_False (TRIP);

	activity_flag = Demand_File_Flag (ACTIVITY);
	if (activity_flag) Demand_File_False (ACTIVITY);

	if ((trip_flag || activity_flag) && !Network_File_Flag (ACTIVITY_LOCATION)) {
		Error ("Activity Location File is needed for Trip or Activity File Processing");
	}

	//---- open the event file ----

	str_ptr = Get_Control_String (EVENT_FILE);

	if (str_ptr == NULL) goto control_error;

	Print (1);
	format_ptr = Get_Control_String (EVENT_FORMAT);

	if (format_ptr != NULL) {
		event_file.Dbase_Format (format_ptr);
	}
	if (!event_file.Open (Project_Filename (str_ptr))) {
		File_Error ("Opening Event File", event_file.Filename ());
	}

	//---- household list file ----

	str_ptr = Get_Control_String (HOUSEHOLD_LIST);

	if (str_ptr != NULL) {
		hhlist_file.File_Type ("Household List File");

		str_ptr = Project_Filename (str_ptr, Extension ());

		Print_Filename (2, hhlist_file.File_Type (), str_ptr);

		hhlist_file.Filename (str_ptr);

		if (!hhlist_file.Open (0)) {
			File_Error ("Opening Household List File", hhlist_file.Filename ());
		}
		hhlist_flag = true;
	}

	//---- get the compare travel time file ----

	str_ptr = Get_Control_String (TRAVEL_TIME_FILE);

	if (str_ptr != NULL) {
		Print (1);

		format_ptr = Get_Control_String (TRAVEL_TIME_FORMAT);

		if (format_ptr != NULL) {
			time_in_file.Dbase_Format (format_ptr);
		}
		time_in_file.Open (Project_Filename (str_ptr));
		time_in_flag = true;
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

	//---- get the select household range ----

	str_ptr = Get_Control_String (SELECT_HOUSEHOLD_RANGE);

	if (str_ptr != NULL) {
		Print (2, "Select Household Range = %s", str_ptr);

		hhold_range.Add_Ranges (str_ptr);

		range_flag = (hhold_range.Num_Ranges () > 0);
	}

	//---- get the select trip modes ----

	str_ptr = Get_Control_String (SELECT_TRIP_MODES);

	if (str_ptr != NULL) {

		Print (2, "Select Trip Modes = %s", str_ptr);

		memset (trip_modes, '\0', MAX_MODE * sizeof (bool));
		int num = 0;
		Data_Range range;
		Range_Data *ptr;

		range.Add_Ranges (str_ptr);

		for (ptr = range.First (); ptr; ptr = range.Next ()) {
			for (i=ptr->Low (); i <= ptr->High (); i++) {
				if (i < 1 || i >= MAX_MODE) {
					Error ("Selected Mode %d is Out of Range (1..%d)", i, MAX_MODE-1);
				}
				trip_modes [i] = true;
				num++;
			}
		}
		if (num == 0) {
			Error ("No Modes were Selected for Processing");
		}
	} else {
		for (i=0; i < MAX_MODE; i++) {
			trip_modes [i] = true;
		}
	}

	//---- get the select plan modes ----

	str_ptr = Get_Control_String (SELECT_PLAN_MODES);

	if (str_ptr != NULL) {

		Print (2, "Select Plan Modes = %s", str_ptr);

		memset (plan_modes, '\0', (DRIVER_MODE + 1) * sizeof (bool));
		int num = 0;
		Data_Range range;
		Range_Data *ptr;

		range.Add_Ranges (str_ptr);

		for (ptr = range.First (); ptr; ptr = range.Next ()) {
			for (i=ptr->Low (); i <= ptr->High (); i++) {
				if (i < 0 || i > DRIVER_MODE) {
					Error ("Selected Mode %d is Out of Range (0..%d)", i, DRIVER_MODE);
				}
				plan_modes [i] = true;
				num++;
			}
		}
		if (num == 0) {
			Error ("No Modes were Selected for Processing");
		}
	} else {
		for (i=0; i <= DRIVER_MODE; i++) {
			plan_modes [i] = true;
		}
	}

	//---- combine event legs ----

	str_ptr = Get_Control_String (COMBINE_EVENT_LEGS);

	if (str_ptr != NULL) {
		Print (2, "Combine Event Legs = %s", str_ptr);

		combine_flag = Get_Control_Flag (COMBINE_EVENT_LEGS);
	}

	//---- new household list file ----

	str_ptr = Get_Control_String (NEW_HOUSEHOLD_LIST);

	if (str_ptr != NULL) {
		if (!hhlist_flag) {
			Error ("A New Household List requires an Input Household List");
		}
		new_hhlist_file.File_Type ("New Household List File");
		new_hhlist_file.File_Access (CREATE);

		str_ptr = Project_Filename (str_ptr, Extension ());

		Print_Filename (2, new_hhlist_file.File_Type (), str_ptr);

		new_hhlist_file.Filename (str_ptr);
		if (new_hhlist_file.Extend () == 0 && hhlist_file.Extend () != 0) {
			new_hhlist_file.Extend (hhlist_file.Extend ());
		}
		if (!new_hhlist_file.Open (0)) {
			File_Error ("Creating Household List File", new_hhlist_file.Filename ());
		}
		new_hhold_flag = true;
	}

	//---- get the travel time file ----

	str_ptr = Get_Control_String (NEW_TRAVEL_TIME_FILE);

	if (str_ptr != NULL) {
		Print (1);

		format_ptr = Get_Control_String (NEW_TRAVEL_TIME_FORMAT);

		if (format_ptr != NULL) {
			travel_time_file.Dbase_Format (format_ptr);
		}
		travel_time_file.Create (Project_Filename (str_ptr));
		time_file_flag = true;
	}

	//---- get the time summary file ----

	str_ptr = Get_Control_String (NEW_TIME_SUMMARY_FILE);

	if (str_ptr != NULL) {
		Print (1);

		format_ptr = Get_Control_String (NEW_TIME_SUMMARY_FORMAT);

		if (format_ptr != NULL) {
			time_sum_file.Dbase_Format (format_ptr);
		}
		time_sum_file.Create (Project_Filename (str_ptr));
		time_sum_flag = true;

		//---- get the minimum schedule variance ----

		str_ptr = Get_Control_String (MINIMUM_SCHEDULE_VARIANCE);

		if (str_ptr != NULL) {
			Get_Double (str_ptr, &dvalue);

			if (dvalue < 0.0 || dvalue > 100.0) {
				Error ("Minimum Schedule Variance %.2lf is Out of Range (0.0-100.0 minutes)", dvalue);
			}
			Print (1, "Minimum Schedule Variance = %.2lf minutes", dvalue);
			min_variance = (int) (dvalue * 60.0 + 0.5);
		}
	}

	//---- get the time distribution file ----

	str_ptr = Get_Control_String (NEW_DISTRIBUTION_FILE);

	if (str_ptr != NULL) {
		time_diff.Open_Distribution (str_ptr);
		distrib_flag = time_flag = true;
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

		fprintf (diff_file.File (), "HOUSEHOLD\tPERSON\tTRIP\tINPUT_TIME\tCOMPARE_TIME\tDIFFERENCE\n");
	}

	//---- create output event file ----

	str_ptr = Get_Control_String (NEW_LINK_EVENT_FILE);

	if (str_ptr != NULL) {
		Print (1);
		link_file.File_Type ("New Link Event File");

		format_ptr = Get_Control_String (NEW_LINK_EVENT_FORMAT);

		if (format_ptr != NULL) {
			link_file.Dbase_Format (format_ptr);
		}		
		link_file.Create (Project_Filename (str_ptr));

		for (range_ptr = time_periods.First (); range_ptr != NULL; range_ptr = time_periods.Next ()) {
			link_file.Add (range_ptr);
		}
		link_file.Create_Fields ();
		link_file.Write_Header ();
		link_flag = true;
	}

	//---- open the link equivalence ----

	if (Report_Flag (GROUP_EVENT)) {
		str_ptr = Get_Control_String (LINK_EQUIVALENCE_FILE);

		if (str_ptr == NULL) {
			Error ("A Link Equivalence File is Required for Group Event Reports");
		}
		Print (1);
		link_equiv.Open (Project_Filename (str_ptr));
		group_flag = true;
	}

	//---- get the percent time difference ----

	str_ptr = Get_Control_String (PERCENT_TIME_DIFFERENCE);

	if (str_ptr != NULL) {
        Get_Double (str_ptr, &percent_diff);

		if (percent_diff != 0.0) {
			select_flag = true;

			if (percent_diff < 0.1 || percent_diff > 100.0) {
				Error ("Percent Time Difference %.2lf is Out of Range (0.1-100.0)", percent_diff);
			}
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

			//---- get the selection percentage ----

			str_ptr = Get_Control_String (SELECTION_PERCENTAGE);

			if (str_ptr != NULL) {
				Get_Double (str_ptr, &percent);

				if (percent < 0.1 || percent > 100.0) {
					Error ("Selection Percentage %.2lf is Out of Range (0.1-100.0)", percent);
				}
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
		}
	}

	//---- open the plan file ----

	str_ptr = Get_Control_String (PLAN_FILE);

	if (str_ptr != NULL) {
		update_flag = true;

		str_ptr = Project_Filename (str_ptr, Extension ());

		Print_Filename (2, plan_file.File_Type (), str_ptr);

		plan_file.Filename (str_ptr);

		format_ptr = Get_Control_String (PLAN_FORMAT);

		if (format_ptr != NULL) {
			plan_file.File_Format (format_ptr);
			Print (1, "%s Format = %s", plan_file.File_Type (), format_ptr);
		}
		if (!plan_file.Open (0)) {
			File_Error ("Opening Plan File", plan_file.Filename ());
		}

		//---- open the new plan file ----

		str_ptr = Get_Control_String (NEW_PLAN_FILE);

		if (str_ptr == NULL) {
			Error ("A New Plan File is required for Plan Time Updates");
		}
		str_ptr = Project_Filename (str_ptr, Extension ());

		new_plan_file.File_Type ("New Plan File");
		Print_Filename (1, new_plan_file.File_Type (), str_ptr);

		new_plan_file.Filename (str_ptr);
		new_plan_file.File_Access (Db_Code::CREATE);
		new_plan_file.Node_Based_Flag (plan_file.Node_Based_Flag ());
		new_plan_file.Extend (plan_file.Extend ());

		format_ptr = Get_Control_String (NEW_PLAN_FORMAT);

		if (format_ptr != NULL) {
			new_plan_file.File_Format (format_ptr);
			Print (1, "%s Format = %s", new_plan_file.File_Type (), format_ptr);
		} else {
			new_plan_file.File_Format (plan_file.File_Format ());
		}
	}

	//---- read report types ----

	List_Reports ();

	//---- check the network flags ----

	if (link_flag || group_flag) {
		if (!Network_File_Flag (LINK)) {
			Error ("A Link File is Required for Link-Based Output");
		}
	} else {
		Network_File_False (LINK);
	}

	if (Report_Flag (TTIME_DISTRIB) || Report_Flag (TOTAL_DISTRIB)) time_flag = true;
	period_flag = Report_Flag (PERIOD_SUM);

	//---- allocate space for change data ----

	if (time_flag || period_flag) {
		if (!time_diff.Time_Periods (&time_periods)) {
			Error ("Insufficient Memory for Time Period Data");
		}
	}

	//---- process support data ----

	if (group_flag) {
		link_equiv.Read (Report_Flag (LINK_EQUIV));
	}

	//---- set the compare flag ----

	compare_flag = (time_in_flag || trip_file_flag || activity_flag);
	return;

	//---- error message ----

control_error:
	Error ("Missing Control Key = %s", Current_Key ());
} 
