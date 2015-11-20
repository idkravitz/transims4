//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "Reschedule.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void Reschedule::Program_Control (void)
{
	int i, num_periods, size;
	double dvalue;
	char *str_ptr, *format_ptr, buffer [FIELD_BUFFER];
	
	int period_code [] = { RUN_START, RUN_MID, RUN_END, RUN_START, 
			RUN_MID, RUN_END };

	char *period_method [] = { "RUN_START", "RUN_MIDPOINT", "RUN_END", "SCHEDULE_START", 
			"SCHEDULE_MIDPOINT", "SCHEDULE_END", NULL };

	Demand_Service::Program_Control ();

	header_flag = Network_File_Flag (ROUTE_HEADER);

	//---- check for turning movement delays ----

	delay_flag = Demand_File_Flag (LINK_DELAY);

	if (delay_flag) {
		turn_delay = (Demand_Db_Base (LINK_DELAY)->Dbase_Format () != VERSION3);
	}
	if (!turn_delay) {
		Network_File_False (LANE_CONNECTIVITY);
	} else if (!Network_File_Flag (LANE_CONNECTIVITY)) {
		Warning ("Lane Connectivity is needed to include Turn Delays");
		turn_delay = false;
		Show_Message (1);
	}
	rider_flag = Demand_File_Flag (RIDERSHIP);
	veh_type_flag = Demand_File_Flag (VEHICLE_TYPE);

	Print (1);

	//---- get the select transit modes ----
	
	str_ptr = Get_Control_String (SELECT_TRANSIT_MODES);

	if (str_ptr != NULL) {
		Print (1, "Select Transit Modes = %s", str_ptr);

		size = REGIONRAIL + 1;

		memset (select_modes, '\0', size * sizeof (bool));

		while (str_ptr != NULL) {
			str_ptr = Get_Token (str_ptr, buffer);

			if (buffer [0] == '\0') break;

			select_modes [Transit_Code (buffer)] = true;
		}
	} else {
		for (i=1; i <= REGIONRAIL; i++) {
			select_modes [i] = true;
		}
	}
	
	//---- get the select transit routes ----

	str_ptr = Get_Control_String (SELECT_TRANSIT_ROUTES);

	if (str_ptr != NULL) {
		Print (1, "Select Transit Routes = %s", str_ptr);

		if (!select_routes.Add_Ranges (str_ptr)) {
			File_Error ("Transit Route Range", str_ptr);
		}
		route_flag = true;
	}

	//---- get the time of day format ----
	
	format_ptr = Get_Control_String (TIME_OF_DAY_FORMAT);

	if (format_ptr == NULL) {
		format_ptr = "24_HOUR_CLOCK";
	}
	if (!select_periods.Format (format_ptr) || !time_periods.Format (format_ptr)) {
		Error ("Time of Day Format %s was Unrecognized", format_ptr);
	}

	//---- get the select time periods ----

	str_ptr = Get_Control_String (SELECT_TIME_PERIODS);

	if (str_ptr != NULL) {
		Print (1, "Select Time Periods = %s", str_ptr);

		if (!select_periods.Add_Ranges (str_ptr)) {
			Error ("Select Time Periods");
		}
		period_flag = true;

		//---- get the time period method ----
		
		str_ptr = Get_Control_String (TIME_PERIOD_METHOD);

		if (str_ptr != NULL) {
			Print (1, "Time Period Method = %s", str_ptr);
			method = 0;

			for (i=0; period_method [i] != NULL; i++) {
				if (str_cmp (str_ptr, period_method [i]) == 0) {
					method = period_code [i];
					break;
				}
			}
			if (method == 0) {
				Error ("Time Period Method %s was Unrecognized", str_ptr);
			}
		}
	}

	Print (2, "Time of Day Format = %s", format_ptr);

	//---- get the time periods ----

	str_ptr = Get_Control_String (TRANSIT_TIME_PERIODS);

	if (str_ptr != NULL) {
		Print (1, "Transit Time Period Breaks = %s", str_ptr);
	} else {
		str_ptr = "None";
	}
	if (!time_periods.Add_Breaks (str_ptr)) {
		Error ("Adding Transit Time Period Breaks");
	}

	//---- initialize the time period factors ----

	num_periods = time_periods.Num_Ranges ();

	time_factor = new double [num_periods + 1];
	min_dwell = new int [num_periods + 1];
	max_dwell = new int [num_periods + 1];

	if (time_factor == NULL || min_dwell == NULL || max_dwell == NULL) {
		Error ("Insufficient Memory for %d Time Factors", num_periods);
	}
	for (i=0; i <= num_periods; i++) {
		time_factor [i] = 1.0;
		min_dwell [i] = Round (3.0);
		max_dwell [i] = Round (120.0);
	}

	//---- update the travel time factors ----
	
	str_ptr = Get_Control_String (TRANSIT_TRAVEL_TIME_FACTOR);

	if (str_ptr != NULL) {
		Print (1, "Transit Travel Time Factors = %s", str_ptr);
		i = 1;

		while (str_ptr != NULL) {
			str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));
			if (buffer [0] == '\0') break;

			if (i > num_periods) {
				Warning ("Too Many Travel Time Factors");
				break;
			}
			dvalue = atof (buffer);

			if (dvalue < 0.5 || dvalue > 3.0) {
				Error ("Travel Time Factor %.2lf is Out of Range (0.5-3.0)", dvalue);
			}
			time_factor [i++] = dvalue;
		}
	}

	//---- minimum dwell time ----

	str_ptr = Get_Control_String (MINIMUM_DWELL_TIME);

	if (str_ptr != NULL) {
		if (!rider_flag || (rider_flag && !veh_type_flag)) {
			Print (1, "Minimum Dwell Time = %s seconds", str_ptr);
			i = 1;

			while (str_ptr != NULL) {
				str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));
				if (buffer [0] == '\0') break;

				if (i > num_periods) {
					Warning ("Too Many Minimum Dwell Times");
					break;
				}
				dvalue = atof (buffer);

				if (dvalue < 0.0 || dvalue > 300.0) {
					Error ("Minimum Dwell Time %.1lf is Out of Range (0..300)", dvalue);
				}
				min_dwell [i++] = Round (dvalue);
			}
		} else {
			Warning ("Minimum Dwell Time Key will Not be Used");
		}
	}

	//---- maximum dwell time ----

	str_ptr = Get_Control_String (MAXIMUM_DWELL_TIME);

	if (str_ptr != NULL) {
		if (rider_flag && delay_flag && !veh_type_flag) {
			Print (1, "Maximum Dwell Time = %s seconds", str_ptr);
			i = 1;

			while (str_ptr != NULL) {
				str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));
				if (buffer [0] == '\0') break;

				if (i > num_periods) {
					Warning ("Too Many Maximum Dwell Times");
					break;
				}
				dvalue = atof (buffer);

				if (dvalue < 0 || dvalue > 3600.0) {
					Error ("Maximum Dwell Time %.1lf is Out of Range (0..3600)", dvalue);
				}
				max_dwell [i++] = Round (dvalue);
			}
		} else {
			Warning ("Maximum Dwell Time Key will Not be Used");
		}
	}

	//---- check the range ----

	if (rider_flag && delay_flag && !veh_type_flag) {
		for (i=0; i <= num_periods; i++) {
			if (min_dwell [i] > max_dwell [i]) {
				Error ("Minimum %.1lf and Maximum %.1lf Dwell Times are Incompatible",
					UnRound (min_dwell [i]), UnRound (max_dwell [i]));
			}
		}
	}

	//---- open the distribution file ----

	str_ptr = Get_Control_String (NEW_DISTRIBUTION_FILE);

	if (str_ptr != NULL) {
		time_diff.Open_Distribution (str_ptr);
		diff_flag = true;
	} else {
		if (Report_Flag (TOTAL_CHANGE) || Report_Flag (PERIOD_CHANGE) ||
			Report_Flag (PERIOD_SUM)) diff_flag = true;
	}
	if (diff_flag) {
		if (!time_diff.Time_Periods (&time_periods)) {
			Error ("Insufficient Memory for Time Period Data");
		}
	}

	//---- random schedule shift ----

	str_ptr = Get_Control_String (RANDOM_SCHEDULE_SHIFT);

	if (str_ptr != NULL) {
		shift_flag = true;

		max_shift = atoi (str_ptr);

		if (max_shift < 1 || max_shift > 3600) {
			Error ("Random Schedule Shift %d is Out of Range (1..3600)", max_shift);
		}
		Print (2, "Maximum Random Schedule Shift = %d seconds", max_shift);
	}

	//---- run start time file ----

	str_ptr = Get_Control_String (RUN_START_TIME_FILE);

	if (str_ptr != NULL) {
		if (shift_flag) {
			Error ("Random Shifts and Run Start Times are Incompatible");
		}
		run_flag = true;
		run_file.File_Type ("Run Start Time File");

		if (!run_file.Open (Project_Filename (str_ptr))) {
			Error ("Opening %s", run_file.File_Type ());
		}
	}


	if (shift_flag || service_flag) {

		//---- random number seed ----

		str_ptr = Get_Control_String (RANDOM_NUMBER_SEED);

		if (str_ptr != NULL) {
			random.Seed (atoi (str_ptr));
		}
		Print (2, "Random Number Seed = %d", random.Seed ());
	}

	//---- segment schedule factor ----

	str_ptr = Get_Control_String (SEGMENT_SCHEDULE_FACTOR);

	if (str_ptr != NULL) {
		if (shift_flag || run_flag || service_flag) {
			Error ("Segment Factoring is Incompatible with Other Options");
		}
		link_factor = atof (str_ptr);

		if (link_factor < 0.3 || link_factor > 3.0 || link_factor == 1.0) {
			Error ("Segment Schedule Factor %.2lf is Out of Range (0.3..3.0)", link_factor);
		}
		Print (2, "Segment Schedule Factor = %.3lf", link_factor);

		//---- open the link equivalence ----

		str_ptr = Get_Control_String (LINK_EQUIVALENCE_FILE);

		if (str_ptr == NULL) {
			Error ("A Link Equivalance File is needed for Segment Schedule Factoring");
		}
		Print (1);
		link_equiv.Open (Project_Filename (str_ptr));
		equiv_flag = true;
	}

	//---- show the reports ----

	List_Reports ();

	//---- read the equiv file ----

	if (equiv_flag) {
		link_equiv.Read (Report_Flag (LINK_EQUIV));
	}
} 
