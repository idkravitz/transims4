//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "PlanSum.hpp"

#include "Delay_File.hpp"
#include "Utility.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void PlanSum::Program_Control (void)
{
	int  i, num;
	bool link_flag, path_flag, report_flag;
	char *str_ptr, *format_ptr;

	Range_Data *range_ptr;
	
	char *time_control [] = {"ORIGIN", "DESTINATION", "MID-TRIP", NULL};

	num = 0;
	link_flag = path_flag = report_flag = false;

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

	//---- household list file ----

	str_ptr = Get_Control_String (HOUSEHOLD_LIST);

	if (str_ptr != NULL) {
		hhlist_file.File_Type ("Household List File");

		str_ptr = Project_Filename (str_ptr, Extension ());

		Print_Filename (2, hhlist_file.File_Type (), str_ptr);

		hhlist_file.Filename (str_ptr);
		hhlist_file.Extend (plan_file.Extend ());

		if (!hhlist_file.Open (0)) {
			File_Error ("Opening Household List File", hhlist_file.Filename ());
		}
		hhlist_flag = true;
	}

	//---- household person count file ----

	str_ptr = Get_Control_String (HOUSEHOLD_PERSON_COUNT);

	if (str_ptr != NULL) {
		count_file.File_Type ("Household Person Count");

		str_ptr = Project_Filename (str_ptr, Extension ());

		Print_Filename (2, count_file.File_Type (), str_ptr);

		count_file.Filename (str_ptr);
		count_file.Extend (plan_file.Extend ());

		if (!count_file.Open (0)) {
			File_Error ("Opening Household Person Count", count_file.Filename ());
		}
		count_flag = true;
	}

	//---- get summary increment ----
	
	str_ptr = Get_Control_String (SUMMARY_TIME_INCREMENT);

	if (str_ptr != NULL) {
		increment = atol (str_ptr);

		if (increment < 0 || increment > 240) {
			Error ("Summary Time Increment %d is Out of Range (0-240)", increment);
		}
		increment *= 60;
	}
	if (increment == 0) {
		Time_Increment (MIDNIGHT);
	} else {
		time_periods.Period_Flag (true);
		Time_Increment (increment);
	}
	time_periods.Increment (increment);
	time_periods.Format ("24_HOUR_CLOCK");

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
	if (num_inc < 1) num_inc = 1;

	ttime_data.Periods (num_inc);

	cap_factor = (double) time_periods.Max_Count () / (3600.0 * num_inc);

	if (increment > 0) {
		Print (1, "Summary Time Increment = %d minutes", (increment / 60));
	} else {
		Print (1, "One Summary Increment per Time Period will be Output");
	}

	//---- open network and demand files ----

	Demand_Service::Program_Control ();

	//---- link delay support ----

	delay_in = Demand_File_Flag (LINK_DELAY);
	delay_out = Demand_File_Flag (NEW_LINK_DELAY);

	if (delay_in || delay_out) {
		Delay_File *delay_file;

		if (delay_in) {
			delay_file = (Delay_File *) Demand_Db_Base (LINK_DELAY);
			turn_delay = (delay_file->Dbase_Format () != VERSION3);
		}
		if (delay_out) {
			delay_file = (Delay_File *) Demand_Db_Base (NEW_LINK_DELAY);
			turn_delay = (turn_delay || delay_file->Dbase_Format () != VERSION3);
		}
		if (!turn_delay) {
			Network_File_False (LANE_CONNECTIVITY);
		} else if (!Network_File_Flag (LANE_CONNECTIVITY)) {
			Warning ("Lane Connectivity is needed to Summarize Turning Movements");
			Show_Message (1);
			turn_delay = false;
		}
		link_flag = true;

		//---- equation parameters ----

		if (delay_out) {
			if (delay_in) {
				str_ptr = Get_Control_String (KEEP_INPUT_TRAVEL_TIMES);

				if (str_ptr != NULL) {
					Print (1, "Keep Input Travel Times = %s", str_ptr);

					vc_flag = !Get_Control_Flag (KEEP_INPUT_TRAVEL_TIMES);
				}
			}
			if (vc_flag) {
				Print (1);
				equation.Num_Equations (EXTERNAL);

				for (i=1; i <= EXTERNAL; i++) {
					str_ptr = Get_Control_String (EQUATION_PARAMETERS_x, i);
					equation.Add_Equation (i, str_ptr);
				}
			}
		}
	}

	//---- get the new plan file ----

	if (delay_in) {
		str_ptr = Get_Control_String (NEW_PLAN_FILE);

		if (str_ptr != NULL) {
			update_flag = true;

			str_ptr = Project_Filename (str_ptr, Extension ());

			new_plan_file.File_Type ("New Plan File");
			Print_Filename (2, new_plan_file.File_Type (), str_ptr);

			new_plan_file.Filename (str_ptr);
			new_plan_file.File_Access (Db_Code::CREATE);
			new_plan_file.File_Format (plan_file.File_Format ());
			new_plan_file.Extend (plan_file.Extend ());
			new_plan_file.Node_Based_Flag (type_flag);
		}
	}

	//---- create output volume file ----

	str_ptr = Get_Control_String (NEW_LINK_VOLUME_FILE);

	if (str_ptr != NULL) {
		Print (1);
		volume_file.File_Type ("Link Volume File");

		format_ptr = Get_Control_String (NEW_LINK_VOLUME_FORMAT);

		if (format_ptr != NULL) {
			volume_file.Dbase_Format (format_ptr);
		}		
		volume_file.Create (Project_Filename (str_ptr));

		for (range_ptr = time_periods.First (); range_ptr != NULL; range_ptr = time_periods.Next ()) {
			volume_file.Add (range_ptr);
		}
		volume_file.Create_Fields ();
		volume_file.Write_Header ();

		volume_flag = link_flag = true;
	}

	//---- open the link equivalence ----

	if (Report_Flag (LINK_GROUP) || Report_Flag (RIDER_GROUP)) {
		str_ptr = Get_Control_String (LINK_EQUIVALENCE_FILE);

		if (str_ptr == NULL) {
			Error ("A Link Equivalence File is Required for Link Group Reports");
		}
		Print (1);
		link_equiv.Open (Project_Filename (str_ptr));
		link_flag = true;
	}

	//---- get the trip time file ----

	str_ptr = Get_Control_String (NEW_TRIP_TIME_FILE);

	if (str_ptr != NULL) {
		time_file.File_Type ("Trip Time File");
		Print (1);

		time_file.Create (Project_Filename (str_ptr));
		time_flag = true;
	}

	//---- get the zone to zone skim file ----

	str_ptr = Get_Control_String (NEW_ZONE_SKIM_FILE);

	if (str_ptr != NULL) {
		Print (1);
		if (!Network_File_Flag (ACTIVITY_LOCATION)) {
			Error ("Zone Skims required an Activity Location File");
		}

		//---- check for zone field name ----

		if (!Network_Db_Base (ACTIVITY_LOCATION)->Required_Field ("ZONE", "TAZ")) {
			Error ("The Zone Field was Not Found on the Activity Location File");
		}

		//---- check the number of increments ----

		if (!skim_data.Set_Key (num_inc)) {
			Error ("Zone Skim Index Limits Exceeded");
		}
	
		//---- get the skim mode selection ----

		str_ptr = Get_Control_String (SKIM_MODE_SELECTION);

		if (str_ptr != NULL) {

			Print (1, "Generate Skims with Selected Modes = %s", str_ptr);

			memset (skim_mode, '\0', MAX_MODE * sizeof (bool));
			num = 0;

			while (str_ptr != NULL) {
				str_ptr = Get_Integer (str_ptr, &i);

				if (i < 1 || i >= MAX_MODE) {
					Error ("Selected Mode %d is Out of Range (1..%d)", i, MAX_MODE-1);
				}
				skim_mode [i] = true;
				num++;
			}
			if (num == 0) {
				Error ("No Modes were Selected for Processing");
			}
		} else {
			for (i=1; i < MAX_MODE; i++) {
				skim_mode [i] = true;
			}
		}
		skim_file.Transit_Flag (skim_mode [TRANSIT] || skim_mode [RAIL_TRANSIT] ||
					skim_mode [PNR_OUT] || skim_mode [PNR_IN]);

		skim_file.Drive_Flag (skim_mode [DRIVE_ALONE] || skim_mode [PNR_OUT] || skim_mode [PNR_IN] ||
				skim_mode [CARPOOL2] || skim_mode [CARPOOL3] || skim_mode [CARPOOL4]);

		skim_file.Other_Flag (skim_mode [BICYCLE] || skim_mode [MAGIC_MOVE] || skim_mode [SCHOOL_BUS]);

		//---- skim total time ----

		str_ptr = Get_Control_String (SKIM_TOTAL_TIME);

		if (str_ptr != NULL) {
			Print (1, "Skim Total Time = %s", str_ptr);

			time_skim_flag = Get_Control_Flag (SKIM_TOTAL_TIME);

			skim_file.Total_Time_Flag (time_skim_flag);
		}

		//---- skim trip length ----

		str_ptr = Get_Control_String (SKIM_TRIP_LENGTH);

		if (str_ptr != NULL) {
			Print (1, "Skim Trip Length = %s", str_ptr);

			length_flag = Get_Control_Flag (SKIM_TRIP_LENGTH);
		}
		skim_file.Length_Flag (length_flag);

		//---- special skim data ----

		if (time_skim_flag) {
			if (length_flag) {
				time_len_skim_data.Set_Key (num_inc);
			} else {
				time_skim_data.Set_Key (num_inc);
			}
		}

		//---- create the file ----

		format_ptr = Get_Control_String (NEW_ZONE_SKIM_FORMAT);

		if (format_ptr != NULL) {
			skim_file.Dbase_Format (format_ptr);
		}
		str_ptr = Get_Control_String (NEW_ZONE_SKIM_FILE);

		skim_file.Create (Project_Filename (str_ptr));
		skim_flag = true;

		//---- get the time control point ----

		str_ptr = Get_Control_String (SKIM_TIME_CONTROL_POINT);

		if (str_ptr != NULL) {
			Print (1, "Skim Time Control Point = %s", str_ptr);

			for (i=0; time_control [i] != NULL; i++) {
				if (str_cmp (str_ptr, time_control [i]) == 0) break;
			}
			if (time_control [i] == NULL) {
				Error ("Time Control Point %s was Unrecognized", str_ptr);
			}
			skim_point = i;
		}

		//---- nearest neighbor factor ----

		str_ptr = Get_Control_String (NEAREST_NEIGHBOR_FACTOR);

		if (str_ptr != NULL) {
			neighbor_factor = atof (str_ptr);
			if (neighbor_factor == 0.0) {
				Print (1, "Nearest Neighbor Calculations are Disabled");
				neighbor_flag = false;
			} else {
				Print (1, "Nearest Neighbor Factor = %s", str_ptr);
				if (neighbor_factor < 0.01 || neighbor_factor > 1.0) {
					Error ("Nearest Neighbor Factor %.2lf is Out of Range (0.0, 0.01..1.0)", neighbor_factor);
				}
				neighbor_flag = true;
			}
		}
	}

	//---- get the trip table file ----

	str_ptr = Get_Control_String (NEW_TRIP_TABLE_FILE);

	if (str_ptr != NULL) {
		Print (1);
		if (!Network_File_Flag (ACTIVITY_LOCATION)) {
			Error ("Trip Tables required an Activity Location File");
		}

		//---- check for zone field name ----

		if (!Network_Db_Base (ACTIVITY_LOCATION)->Required_Field ("ZONE", "TAZ")) {
			Error ("The Zone Field was Not Found on the Activity Location File");
		}

		//---- check the number of increments ----

		if (!trip_table.Set_Key (num_inc)) {
			Error ("Trip Table Index Limits Exceeded");
		}
	
		//---- create the file ----

		format_ptr = Get_Control_String (NEW_TRIP_TABLE_FORMAT);

		if (format_ptr != NULL) {
			trip_file.Dbase_Format (format_ptr);
		}
		trip_file.File_Type ("Trip Table File");
		trip_file.Period_Flag (num_inc > 1);

		trip_file.Create (Project_Filename (str_ptr));
		trip_flag = true;

		//---- get the trip mode selection ----

		str_ptr = Get_Control_String (TRIP_MODE_SELECTION);

		if (str_ptr != NULL) {

			Print (1, "Generate Trips for Selected Modes = %s", str_ptr);

			memset (trip_mode, '\0', MAX_MODE * sizeof (bool));
			num = 0;

			while (str_ptr != NULL) {
				str_ptr = Get_Integer (str_ptr, &i);

				if (i < 1 || i >= MAX_MODE) {
					Error ("Selected Mode %d is Out of Range (1..%d)", i, MAX_MODE-1);
				}
				trip_mode [i] = true;
				num++;
			}
			if (num == 0) {
				Error ("No Modes were Selected for Processing");
			}
		} else {
			for (i=1; i < MAX_MODE; i++) {
				trip_mode [i] = true;
			}
		}
	}

	if (skim_flag || trip_flag) {

		//---- open the zone equivalence ----

		str_ptr = Get_Control_String (ZONE_EQUIVALENCE_FILE);

		if (str_ptr != NULL) {
			Print (1);
			zone_equiv.Open (Project_Filename (str_ptr));
			zone_flag = true;
		}
	}

	//---- get the turn movement file ----

	str_ptr = Get_Control_String (NEW_TURN_MOVEMENT_FILE);

	if (str_ptr != NULL) {
		Print (1);

		//---- create the file ----

		format_ptr = Get_Control_String (NEW_TURN_MOVEMENT_FORMAT);

		if (format_ptr != NULL) {
			turn_file.Dbase_Format (format_ptr);
		}
		turn_file.Create (Project_Filename (str_ptr));
		turn_flag = true;

		//---- get the turn node range ----

		str_ptr = Get_Control_String (TURN_NODE_RANGE);

		if (str_ptr != NULL) {
			Print (1, "Turn Node Range = %s", str_ptr);

			node_range = new Data_Range ();

			if (!node_range->Add_Ranges (str_ptr)) {
				File_Error ("Turn Node Range", str_ptr);
			}
		}
		movement_data = new Movement_Array * [num_inc + 1];

		if (movement_data == NULL) {
			Error ("Creating Turn Movement Data");
		}
		for (i=1; i <= num_inc; i++) {
			movement_data [i] = new Movement_Array ();
		}
	}

	//---- ridership summary ----

	passenger_flag = Report_Flag (SUM_PASSENGERS);
	xfer_flag = Report_Flag (SUM_TRANSFERS);
	xfer_detail = Report_Flag (XFER_DETAILS);

	if (Demand_File_Flag (NEW_RIDERSHIP) || Report_Flag (SUM_RIDERS) ||
		Report_Flag (SUM_STOPS) || xfer_flag || xfer_detail ||
		Report_Flag (STOP_GROUP) || passenger_flag || Report_Flag (RIDER_GROUP)) {

		transit_flag = true;

		if (!Network_File_Flag (TRANSIT_STOP) || !Network_File_Flag (TRANSIT_ROUTE) ||
			!Network_File_Flag (TRANSIT_SCHEDULE)) {

			Error ("Transit Network Files are Required for Ridership Output");
		}
		if (passenger_flag || Report_Flag (RIDER_GROUP) || Demand_File_Flag (NEW_RIDERSHIP)) {
			if (!Network_File_Flag (TRANSIT_DRIVER)) {
				Error ("A Transit Driver File is needed for Passengers Summaries");
			}
			link_flag = true;
		} else if (!update_flag && !length_flag) {
			Network_File_False (TRANSIT_DRIVER);
		}

		//---- open the stop equivalence ----

		if (Report_Flag (STOP_GROUP)) {
			str_ptr = Get_Control_String (STOP_EQUIVALENCE_FILE);

			if (str_ptr == NULL) {
				Error ("A Stop Equivalance File is needed for Stop Group Reports");
			}
			Print (1);
			stop_equiv.Open (Project_Filename (str_ptr));
		}

		if (Demand_File_Flag (NEW_RIDERSHIP)) {
			str_ptr = Get_Control_String (OUTPUT_ALL_TRANSIT_STOPS);

			if (str_ptr != NULL) {
				Print (2, "Output All Transit Stops = %s", str_ptr);

				stops_flag = Get_Control_Flag (OUTPUT_ALL_TRANSIT_STOPS);
			}
		}
	} else if (!update_flag) {
		Network_File_False (TRANSIT_STOP);
		Network_File_False (TRANSIT_ROUTE);
		Network_File_False (TRANSIT_SCHEDULE);
		Network_File_False (TRANSIT_DRIVER);
	}

	//---- read report types ----

	List_Reports ();

	if (Report_Flag (TOP_100) || Report_Flag (VC_RATIO) || Report_Flag (LINK_GROUP)) {
		link_flag = report_flag = true;
	}
	if (link_flag && (!Network_File_Flag (LINK) || !Network_File_Flag (NODE))) {
		Error ("Link and Node Files are Required for Link-Based Output");
	}
	if (Network_File_Flag (LANE_USE) && !Network_File_Flag (LINK)) {
		Error ("A Link File is Required for Lane-Use Processing");
	}
	if (volume_flag || delay_in || delay_out) {
		if (!Network_File_Flag (PARKING) && type_flag) {
			Error ("A Parking File is Required for Volume or Delay Output");
		}
	}

	//---- process support data ----

	if (Report_Flag (LINK_GROUP) || Report_Flag (RIDER_GROUP)) {
		link_equiv.Read (Report_Flag (LINK_EQUIV));
	}
	if (zone_flag) {
		zone_equiv.Read (Report_Flag (ZONE_EQUIV));
	}
	if (Report_Flag (STOP_GROUP)) {
		stop_equiv.Read (Report_Flag (STOP_EQUIV));
	}

	//---- allocate work space ----

	if (time_flag || Report_Flag (TRIP_TIME)) {
		time_flag = true;

		if (!start_time.Num_Records (num_inc)) goto mem_error;
		if (!end_time.Num_Records (num_inc)) goto mem_error;
		if (!mid_time.Num_Records (num_inc)) goto mem_error;
	}

	if (xfer_flag || xfer_detail) {
		num = 1;
		if (xfer_detail) {
			num = time_periods.Num_Ranges () * 2 + 1;
		}
		boardings = new int * [num];
		if (boardings == NULL) goto board_error;

		for (i=0; i < num; i++) {
			boardings [i] = new int [MAX_BOARD * MAX_BOARD];
			if (boardings [i] == NULL) goto board_error;
			memset (boardings [i], '\0', sizeof (int) * MAX_BOARD * MAX_BOARD);
		}
	}

	//---- initialize the trip summary data ----

	travel_flag = Report_Flag (SUM_TRAVEL);

	if (travel_flag && num_inc > 1) {
		Trip_Sum_Data trip_sum_rec;

		for (i=1; i <= num_inc; i++) {
			trip_sum_rec.Group (i);

			if (!trip_sum_data.Add (&trip_sum_rec)) {
				Error ("Adding Travel Summary Records");
			}
		}
	}

	//---- initialize the passenger summary data ----

	if (passenger_flag && num_inc > 1) {
		Trip_Sum_Data pass_sum_rec;

		for (i=1; i <= num_inc; i++) {
			pass_sum_rec.Group (i);

			if (!pass_sum_data.Add (&pass_sum_rec)) {
				Error ("Adding Passenger Summary Records");
			}
		}
	}

	//---- allocate time and volume memory ----

	if (delay_out || volume_flag || report_flag) {
		if (delay_in) {
			ttime_data.Layers (3);
			if (turn_delay) {
				connect_time.Layers (3);
			}
			layer = 3;
		} else {
			ttime_data.Layers (1);
			if (turn_delay) {
				connect_time.Layers (1);
			}
			layer = 1;
		}
	}
	return;

	//---- error message ----

control_error:
	Error ("Missing Control Key = %s", Current_Key ());

mem_error:
	Error ("Insufficient Memory for %d Point Diurnal Distribution", num);

board_error:
	Error ("Insufficient Memory for %d Transfer Summaries");
} 
