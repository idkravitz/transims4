//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "Router.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void Router::Program_Control (void)
{
	int i, lvalue;
	double dvalue, value_res;
	char *str_ptr, *format_ptr, *value_text, buffer [FIELD_BUFFER];

	//---- select modes ----
	
	str_ptr = Get_Control_String (ROUTE_SELECTED_MODES);

	if (str_ptr != NULL) {
		memset (select_mode, '\0', sizeof (select_mode));
		int num = 0;
		Data_Range range;
		Range_Data *ptr;

		range.Add_Ranges (str_ptr);

		for (ptr = range.First (); ptr; ptr = range.Next ()) {
			for (i=ptr->Low (); i <= ptr->High (); i++) {
				if (i < 1 || i >= MAX_MODE) {
					Error ("Selected Mode %d is Out of Range (1..%d)", i, MAX_MODE-1);
				}
				select_mode [i] = true;
				num++;
			}
		}
		if (num == 0) {
			Error ("No Modes were Selected for Processing");
		}
	} else {
		for (int i=0; i < MAX_MODE; i++) {
			select_mode [i] = true;
		}
	}

	//---- new mode ----
	
	str_ptr = Get_Control_String (ROUTE_WITH_SPECIFIED_MODE);

	if (str_ptr != NULL) {
		mode_flag = true;

		str_ptr = Get_Integer (str_ptr, &new_mode);

		if (new_mode < 1 || new_mode >= MAX_MODE) {
			Error ("Specified Mode %d is Out of Range (1..%d)", new_mode, MAX_MODE);
		}
	} else {
		mode_flag = false;
	}

	//---- select routing flags ----

	trip_flag = (Get_Control_String ("TRIP_FILE") != NULL);
	activity_flag = (Get_Control_String ("ACTIVITY_FILE") != NULL);
	select_org = (Get_Control_String (ROUTE_FROM_SPECIFIED_LOCATIONS) != NULL);
	select_des = (Get_Control_String (ROUTE_TO_SPECIFIED_LOCATIONS) != NULL);
	select_od = (select_org || select_des);
	select_time = (Get_Control_String (ROUTE_AT_SPECIFIED_TIMES) != NULL);
	ignore_veh_flag = Get_Control_Flag (IGNORE_VEHICLE_ID);
	skim_flag = Get_Control_Flag (UPDATE_PLAN_RECORDS);

	if (!trip_flag && !activity_flag) {
		if (select_od) {
			Demand_Option_False (VEHICLE);
			ignore_veh_flag = true;
		} else if (!skim_flag) {
			Error ("A Trip or Activity File is Required for Routing");
		}
	}
	if (skim_flag) {
		AB_Key_Flag  (true);
	}

	//---- check for drive modes ----

	drive_net = (mode_flag && (new_mode == DRIVE_ALONE || new_mode == PNR_OUT || 
		new_mode == PNR_IN || new_mode == CARPOOL2 || new_mode == CARPOOL3 || new_mode== CARPOOL4 ||
		new_mode == KNR_OUT || new_mode == KNR_IN)) || 
		(!mode_flag && (select_mode [DRIVE_ALONE] || select_mode [PNR_OUT] || 
		select_mode [PNR_IN] || select_mode [CARPOOL2] || select_mode [CARPOOL3] || select_mode [CARPOOL4] ||
		select_mode [KNR_OUT] || select_mode [KNR_IN]));

	//---- set flags ----

	if (!drive_net) {
		Demand_File_False (LINK_DELAY);
		Demand_Option_False (VEHICLE);
	} else if (ignore_veh_flag) {
		Demand_Option_False (VEHICLE);
	} else {
		Demand_File_True (VEHICLE);
	}

	//---- open network files ----

	Demand_Service::Program_Control ();

	veh_type_flag = Demand_File_Flag (VEHICLE_TYPE);

	//---- set network flags ----

	transit_net = Network_File_Flag (TRANSIT_STOP) && 
					Network_File_Flag (TRANSIT_ROUTE) && 
					Network_File_Flag (TRANSIT_SCHEDULE);

	if (!transit_net && mode_flag && ((new_mode >= TRANSIT && new_mode <= PNR_IN) || 
		new_mode == KNR_OUT || new_mode == KNR_IN)) {
		Error ("Transit Routing Requires a Transit Network");
	}
	if (transit_net && (mode_flag && !((new_mode >= TRANSIT && new_mode <= PNR_IN) || 
		new_mode == KNR_OUT || new_mode == KNR_IN)) ||
		(!mode_flag && !(select_mode [TRANSIT] || select_mode [RAIL_TRANSIT] ||
		select_mode [PNR_OUT] || select_mode [PNR_IN] || 
		select_mode [KNR_OUT] || select_mode [KNR_IN]))) {
		transit_net = false;
	}
	if (!transit_net) {
		Network_File_False (TRANSIT_STOP);
		Network_File_False (TRANSIT_FARE);
		Network_File_False (TRANSIT_ROUTE);
		Network_File_False (TRANSIT_SCHEDULE);
	}

	if (drive_net) {
		if (Get_Control_String (LIMIT_PARKING_ACCESS) != NULL) {
			limit_access = Get_Control_Flag (LIMIT_PARKING_ACCESS);
		}
		toll_flag = Network_File_Flag (TOLL);
		delay_flag = Demand_File_Flag (LINK_DELAY);
	}
	walk_net = !limit_access || transit_net || (mode_flag && new_mode == ALL_WALK) ||
		(!mode_flag && select_mode [ALL_WALK]);

	bike_net = (mode_flag && new_mode == BICYCLE) || (!mode_flag && select_mode [BICYCLE]);

	loc_net = walk_net || bike_net;

	if (transit_net && (select_mode [PNR_OUT] || (mode_flag && new_mode == PNR_OUT))) {
		park_ride_flag = true;
	}
	if (transit_net && (select_mode [KNR_OUT] || (mode_flag && new_mode == KNR_OUT))) {
		kiss_ride_flag = true;
	}

	//---- open the household list file ----
	
	str_ptr = Get_Control_String (HOUSEHOLD_LIST);

	if (str_ptr != NULL) {
		hhlist_file.File_Type ("Household List");
		Print (1);

		hhlist_file.Open (Project_Filename (str_ptr, Extension ()));
		hhlist_flag = true;
	}

	//---- open the household record file ----
	
	str_ptr = Get_Control_String (HOUSEHOLD_RECORD_FILE);

	if (str_ptr != NULL) {
		Print (1);

		hhrec_file.Open (Project_Filename (str_ptr, Extension ()));
		hhrec_flag = true;

		if (hhrec_flag && hhlist_flag) {
			Error ("A Household List and Household Record File are Not Permitted");
		}
	}

	//---- check for a household file ----

	if (Demand_File_Flag (HOUSEHOLD) && !activity_flag && !trip_flag) {
		hhold_flag = true;

		//---- open the household type script ----

		str_ptr = Get_Control_String (HOUSEHOLD_TYPE_SCRIPT);

		if (str_ptr == NULL) goto control_error;

		Print (1);
		hhold_type_file.File_Type ("Household Type Script");

		hhold_type_file.Open (Project_Filename (str_ptr));
	}

	//---- get the file points ----

	if (activity_flag) {
		activity_file = (Activity_File *) Demand_Db_Base (ACTIVITY);
		Demand_File_False (ACTIVITY);		//---- custom read ----
	}
	if (trip_flag) {
		trip_file = (Trip_File *) Demand_Db_Base (TRIP);
		Demand_File_False (TRIP);			//---- custom read ----
	}

	//---- open the parking penalty file ----
	
	str_ptr = Get_Control_String (PARKING_PENALTY_FILE);

	if (str_ptr != NULL) {
		park_imp_file.File_Type ("Parking Penalty File");
		Print (1);

		park_imp_file.Open (Project_Filename (str_ptr));
		park_imp_flag = true;

		park_imp_file.Required_Field ("PARKING", "LOT", "ID", "NUMBER", "LOT_ID");
		park_imp_file.Required_Field ("PENALTY", "IMP", "IMPEDANCE", "IMPED", "PEN");
	}

	//---- open the transit penalty file ----
	
	str_ptr = Get_Control_String (TRANSIT_PENALTY_FILE);

	if (str_ptr != NULL) {
		stop_imp_file.File_Type ("Transit Penalty File");
		Print (1);

		stop_imp_file.Open (Project_Filename (str_ptr));
		stop_imp_flag = true;

		stop_imp_file.Required_Field ("STOP", "STOP_ID", "ID");
		stop_imp_file.Required_Field ("PENALTY", "IMP", "IMPEDANCE", "IMPED", "PEN");
	}

	//---- get the time of day format ----
	
	str_ptr = Get_Control_String (TIME_OF_DAY_FORMAT);

	if (str_ptr == NULL) {
		str_ptr = "24_HOUR_CLOCK";
	}
	if (!trip_time.Format (str_ptr)) {
		Error ("Time of Day Format %s was Unrecognized", str_ptr);
	}
	Print (2, "Time of Day Format = %s", str_ptr);

	//---- open the old plan file ----
		
	if (Get_Control_String (NODE_LIST_PATHS) != NULL) {
		node_flag = Get_Control_Flag (NODE_LIST_PATHS);
	}

	if (hhlist_flag || hhrec_flag || skim_flag) {
		str_ptr = Get_Control_String (PLAN_FILE);

		if (str_ptr != NULL) {
			Print (1);
			format_ptr = Get_Control_String (PLAN_FORMAT);

			if (format_ptr != NULL) {
				old_plan_file.File_Format (format_ptr);
			}
			old_plan_file.Node_Based_Flag (node_flag);

			old_plan_file.Open (Project_Filename (str_ptr, Extension ()));
			old_plan_flag = true;

			if (old_plan_file.Record_Format () == BINARY) {
				Print (1, "%s Format = BINARY", old_plan_file.File_Type ());
			}
			node_flag = old_plan_file.Node_Based_Flag ();
		} else if (skim_flag) {
			Error ("An Input Plan File is Required for Plan Updates");
		}
	}

	//---- open the plan file ----

	str_ptr = Get_Control_String (NEW_PLAN_FILE);

	if (str_ptr != NULL) {
		Print (1);
		format_ptr = Get_Control_String (NEW_PLAN_FORMAT);

		if (format_ptr != NULL) {
			plan_file.File_Format (format_ptr);
		}
		plan_file.Node_Based_Flag (node_flag);

		plan_file.Create (Project_Filename (str_ptr, Extension ()));
		save_plans = true;

		if (plan_file.Record_Format () == BINARY) {
			Print (1, "%s Format = BINARY", plan_file.File_Type ());
		}

		//---- node list paths ----	

		if (node_flag) {
			Print (1, "Plan File will contain Node List Paths");
		} else {
			Print (1, "Plan File will contain Link List Paths");
		}
	} else if (old_plan_flag) {
		Error ("A New Plan File is Required to Update an Existing Plan File");
	}

	//---- select modes ----
	
	str_ptr = Get_Control_String (ROUTE_SELECTED_MODES);

	if (str_ptr != NULL) {
		Print (2, "Routes will be constructed for Selected Modes = %s", str_ptr);
	}

	//---- select purposes ----
	
	str_ptr = Get_Control_String (ROUTE_SELECTED_PURPOSES);

	if (str_ptr != NULL) {
		Print (1, "Route Selected Trip Purposes = %s", str_ptr);

		if (str_cmp (str_ptr, "ALL") != 0) {
			purpose_flag = true;
			if (!purpose_range.Add_Ranges (str_ptr)) {
				Error ("Adding Purpose Ranges");
			}
		}
	}

	//---- new mode ----

	if (mode_flag) {
		Print (2, "Route with Specified Mode = %d", new_mode);
	} else if (select_od || select_time) {
		Print (1);
	}

	//---- route from specified locations -----

	if (select_org) {
		str_ptr = Get_Control_String (ROUTE_FROM_SPECIFIED_LOCATIONS);

		Print (1, "Route from Specified Locations = %s", str_ptr);

		if (str_cmp (str_ptr, "ALL") == 0) {
			select_org = false;
		} else if (!org_range.Add_Ranges (str_ptr)) {
			Error ("Adding Origin Ranges");
		}
	}

	//---- route to specified locations ----

	if (select_des) {
		str_ptr = Get_Control_String (ROUTE_TO_SPECIFIED_LOCATIONS);

		Print (1, "Route to Specified Locations = %s", str_ptr);

		if (str_cmp (str_ptr, "ALL") == 0) {
			select_des = false;
		} else if (!des_range.Add_Ranges (str_ptr)) {
			Error ("Adding Destination Ranges");
		}
	}

	//---- route at specified times -----

	if (select_time) {

		//---- get time increment ----

		lvalue = 0;

		if (!trip_flag && !activity_flag) {
			str_ptr = Get_Control_String (ROUTE_BY_TIME_INCREMENT);

			if (str_ptr != NULL) {
				lvalue = atol (str_ptr);

				if (lvalue < 0 || lvalue > 240) {
					Error ("Time Increment %d is Out of Range (0-240)", lvalue);
				}
				lvalue *= 60;
			}
			if (lvalue != 0) {
				time_range.Period_Flag (true);
			}
			time_range.Increment (lvalue);
		}

		//---- get the time periods ----
	
		str_ptr = Get_Control_String (ROUTE_AT_SPECIFIED_TIMES);

		Print (1, "Route at Specified Times = %s", str_ptr);

		if (str_cmp (str_ptr, "ALL") == 0 && (trip_flag || activity_flag)) {
			select_time = false;
		}
		if (select_time) {
			if (!time_range.Add_Ranges (str_ptr)) {
				Error ("Adding Time Ranges");
			}
			if (lvalue > 0) {
				Print (1, "Route by Time Increment = %d minutes", (lvalue / 60));
			}
		}
	}

	//---- update plan records ----

	str_ptr = Get_Control_String (UPDATE_PLAN_RECORDS);

	if (str_ptr != NULL) {
		Print (1, "Update Plan Records = %s", str_ptr);

		if (skim_flag) {
			str_ptr = Get_Control_String (PRINT_UPDATE_WARNINGS);

			if (str_ptr != NULL) {
				Print (1, "Print Update Warnings = %s", str_ptr);

				warn_flag = Get_Control_Flag (PRINT_UPDATE_WARNINGS);
			}
		}
	}

	//---- ignore vehicle id ----

	str_ptr = Get_Control_String (IGNORE_VEHICLE_ID);

	if (str_ptr != NULL) {
		Print (1, "Ignore Vehicle ID = %s", str_ptr);
	}
	Print (1);

	//---- limit parking access ----

	if (drive_net) {
		if (limit_access) {
			Print (1, "Access to Parking Lots will be Limited to Process Links");
		} else {
			Print (1, "Access to Parking Lots can include Walk Trips");
		}
	}
	
	//---- write walk path ----

	if (walk_net) {
		walk_detail = Get_Control_Flag (WALK_PATH_DETAILS);

		if (walk_detail) {
			Print (1, "Walk Path Details will be written to the Plan File");
		} else {
			Print (1, "Walk Path Details will Not be written to the Plan File");
		}
	}

	//---- adjust activity schedule ----
	
	adjust_flag = Get_Control_Flag (ADJUST_ACTIVITY_SCHEDULE);

	if (adjust_flag) {
		Print (1, "Activity Schedules can be Adjusted as Needed");
	} else {
		Print (1, "Activity Schedules should not be Adjusted");
	}

	//---- ignore activity durations ----
	
	duration_flag = Get_Control_Flag (IGNORE_ACTIVITY_DURATIONS);

	if (duration_flag) {
		Print (1, "Activity Durations will be Ignored");
	} else {
		Print (1, "Activity Durations will be Maintained");
	}

	//---- ignore time constraints ----
	
	ignore_time = Get_Control_Flag (IGNORE_TIME_CONSTRAINTS);

	if (ignore_time) {
		Print (1, "Activity Schedule Constraints will be Ignored");
	} else {
		Print (1, "Activity Schedule Constraints will be Considered");

		str_ptr = Get_Control_String (END_TIME_CONSTRAINT);

		if (str_ptr != NULL) {
			Get_Integer (str_ptr, &end_time);

			if (end_time < 0 || end_time > 240) {
				Error ("End Time Constraint %d is Out of Range (0..240 minutes)", end_time);
			}
		}
		Print (1, "End Time Constraint = %d minutes", end_time);

		end_time = Round (end_time * 60.0);
	}

	//---- ignore routing errors ----
	
	ignore_errors = Get_Control_Flag (IGNORE_ROUTING_PROBLEMS);

	if (ignore_errors) {
		Print (1, "Routing Errors will be Replaced by Magic Moves");
	} else {
		Print (1, "Routing Errors will Terminate Plan Construction");
	}

	//---- interpolate link delays -----

	interpolate_flag = Get_Control_Flag (INTERPOLATE_LINK_DELAYS);

	if (interpolate_flag) {
		Print (1, "Link Delays will be Interpolated using Two Time Periods");
	}

	//---- random impedance ----
	
	str_ptr = Get_Control_String (PERCENT_RANDOM_IMPEDANCE);

	if (str_ptr != NULL) {
		random_imped = atoi (str_ptr);

		if (random_imped < 0 || random_imped > 100) {
			Error ("Percent Random Impedance %d is Out of Range (0..100)", random_imped);
		}
	}
	if (random_imped == 0) {
		random_flag = false;

		Print (2, "Random Impedance Adjustments are Disabled");

	} else {
		random_flag = true;

		Print (2, "Percent Random Impedance = %d", random_imped);

		//---- random number seed ----

		str_ptr = Get_Control_String (RANDOM_NUMBER_SEED);

		if (str_ptr != NULL) {
			random.Seed (atoi (str_ptr));
		}
		Print (1, "Random Number Seed = %d", random.Seed ());
	}

	//---- walk speed ----
	
	str_ptr = Get_Control_String (WALK_SPEED);

	if (str_ptr != NULL) {
		walk_speed = atof (str_ptr);

		if (walk_speed < 0.5 || walk_speed > 10.0) {
			Error ("Walk Speed %.1lf is Out of Range (0.5..10)", walk_speed);
		}
	}
	Print (2, "Walk Speed = %.2lf meters per second", walk_speed);

	//---- bicycle speed ----
	
	if (bike_net) {
		str_ptr = Get_Control_String (BICYCLE_SPEED);

		if (str_ptr != NULL) {
			bike_speed = atof (str_ptr);

			if (bike_speed < 1.0 || bike_speed > 20.0) {
				Error ("Bicycle Speed %.1lf is Out of Range (1..20)", bike_speed);
			}
		}
		Print (1, "Bicycle Speed = %.2lf meters per second", bike_speed);
	}

	//---- impedance value processing ----

	value_res = (1 << value_roll);
	value_text = (char *) ((hhold_flag) ? "s " : " ");

	//---- walk time value ----
	
	str_ptr = Get_Control_String (WALK_TIME_VALUE);

	Print (2, "Walk Time Value%s=", value_text);

	do {
		if (str_ptr != NULL) {
			str_ptr = Get_Double (str_ptr, &dvalue);
		} else {
			dvalue = value_walk;
		}
		if (dvalue < 0.0 || dvalue > 1000.0) {
			Error ("Walk Time Value %.1lf is Out of Range (0..1000)", dvalue);
		}
		Print (0, " %.2lf ", dvalue);

		value_walk = (int) (dvalue * value_res + 0.5);

		if (!hhold_flag) break;

		if (!walk_value.Add (value_walk)) goto mem_error;
	} while (str_ptr != NULL);

	Print (0, "impedance units per second");

	//---- bicycle time value ----

	if (bike_net) {
		str_ptr = Get_Control_String (BICYCLE_TIME_VALUE);

		Print (1, "Bicycle Time Value%s=", value_text);

		do {
			if (str_ptr != NULL) {
				str_ptr = Get_Double (str_ptr, &dvalue);
			} else {
				dvalue = value_bike;
			}
			if (dvalue < 0.0 || dvalue > 1000.0) {
				Error ("Bicycle Time Value %.1lf is Out of Range (0..1000)", dvalue);
			}
			Print (0, " %.2lf ", dvalue);

			value_bike = (int) (dvalue * value_res + 0.5);

			if (!hhold_flag) break;

			if (!bike_value.Add (value_bike)) goto mem_error;
		} while (str_ptr != NULL);

		Print (0, "impedance units per second");
	}

	if (transit_net) {

		//---- first wait value ----
		
		str_ptr = Get_Control_String (FIRST_WAIT_VALUE);

		Print (1, "First Wait Time Value%s=", value_text);

		do {
			if (str_ptr != NULL) {
				str_ptr = Get_Double (str_ptr, &dvalue);
			} else {
				dvalue = value_wait;
			}
			if (dvalue < 0.0 || dvalue > 1000.0) {
				Error ("First Wait Time Value %.1lf is Out of Range (0..1000)", dvalue);
			}
			Print (0, " %.2lf ", dvalue);

			value_wait = (int) (dvalue * value_res + 0.5);

			if (!hhold_flag) break;

			if (!wait_value.Add (value_wait)) goto mem_error;
		} while (str_ptr != NULL);

		Print (0, "impedance units per second");

		//---- transfer wait value ----
		
		str_ptr = Get_Control_String (TRANSFER_WAIT_VALUE);

		Print (1, "Transfer Wait Time Value%s=", value_text);
		
		do {
			if (str_ptr != NULL) {
				str_ptr = Get_Double (str_ptr, &dvalue);
			} else {
				dvalue = value_xfer;
			}
			if (dvalue < 0.0 || dvalue > 1000.0) {
				Error ("Transfer Wait Value %.1lf is Out of Range (0..1000)", dvalue);
			}
			Print (0, " %.2lf ", dvalue);

			value_xfer = (int) (dvalue * value_res + 0.5);

			if (!hhold_flag) break;

			if (!xfer_value.Add (value_xfer)) goto mem_error;
		} while (str_ptr != NULL);

		Print (0, "impedance units per second");
	}

	//---- vehicle time value ----
	
	str_ptr = Get_Control_String (VEHICLE_TIME_VALUE);

	Print (1, "Vehicle Time Value%s=", value_text);

	do {
		if (str_ptr != NULL) {
			str_ptr = Get_Double (str_ptr, &dvalue);
		} else {
			dvalue = value_time;
		}
		if (dvalue < 0.0 || dvalue > 1000.0) {
			Error ("Vehicle Time Value %.1lf is Out of Range (0..1000)", dvalue);
		}
		Print (0, " %.2lf ", dvalue);

		value_time = (int) (dvalue * value_res + 0.5);

		if (!hhold_flag) break;

		if (!time_value.Add (value_time)) goto mem_error;
	} while (str_ptr != NULL);

	Print (0, "impedance units per second");

	//---- distance value ----
	
	str_ptr = Get_Control_String (DISTANCE_VALUE);

	Print (1, "Distance Value%s=", value_text);

	do {
		if (str_ptr != NULL) {
			str_ptr = Get_Double (str_ptr, &dvalue);
		} else {
			dvalue = value_distance;
		}
		if (dvalue < 0.0 || dvalue > 1000.0) {
			Error ("Distance Value %.1lf is Out of Range (0..1000)", dvalue);
		}
		Print (0, " %.2lf ", dvalue);

		value_distance = (int) (dvalue * value_res + 0.5);

		if (!hhold_flag) break;

		if (!distance_value.Add (value_distance)) goto mem_error;
	} while (str_ptr != NULL);

	Print (0, "impedance units per meter");

	//---- cost value ----
	
	str_ptr = Get_Control_String (COST_VALUE);

	Print (1, "Cost Value%s=", value_text);

	do {
		if (str_ptr != NULL) {
			str_ptr = Get_Double (str_ptr, &dvalue);
		} else {
			dvalue = value_cost;
		}
		if (dvalue < 0.0 || dvalue > 1000.0) {
			Error ("Cost Value %.1lf is Out of Range (0..1000)", dvalue);
		}
		Print (0, " %.2lf ", dvalue);

		value_cost = Round (dvalue * value_res);

		if (!hhold_flag) break;

		if (!cost_value.Add (value_cost)) goto mem_error;
	} while (str_ptr != NULL);

	Print (0, "impedance units per cent");

	//---- check the impedance values ----

	for (i=1; i <= time_value.Num_Records (); i++) {
		if (walk_value.Best (i) == 0 && wait_value.Best (i) == 0 && time_value.Best (i) == 0 &&
			distance_value.Best (i) == 0 && cost_value.Best (i) == 0) {

			Error ("At least One Impedance Value must be Non-Zero for Household Type %d", i);
		}
	}
	
	//---- left turn penalty ----
	
	str_ptr = Get_Control_String (LEFT_TURN_PENALTY);

	if (str_ptr != NULL) {

		Print (2, "Left Turn Penalt%s=", ((hhold_flag) ? "ies " : "y "));

		do {
			if (str_ptr != NULL) {
				str_ptr = Get_Double (str_ptr, &dvalue);
			} else {
				dvalue = left_imped;
			}
			if (dvalue < 0.0 || dvalue > 10000.0) {
				Error ("Left Turn Penalty %.1lf is Out of Range (0..10000)", dvalue);
			}
			Print (0, " %.2lf ", dvalue);

			left_imped = Round (dvalue);

			if (!hhold_flag) break;

			if (!imped_left.Add (left_imped)) goto mem_error;
		} while (str_ptr != NULL);

		Print (0, "impedance units per left turn");
	}
		
	//---- right turn penalty ----
	
	str_ptr = Get_Control_String (RIGHT_TURN_PENALTY);

	if (str_ptr != NULL) {

		Print (1, "Right Turn Penalt%s=", ((hhold_flag) ? "ies " : "y "));

		do {
			if (str_ptr != NULL) {
				str_ptr = Get_Double (str_ptr, &dvalue);
			} else {
				dvalue = right_imped;
			}
			if (dvalue < 0.0 || dvalue > 10000.0) {
				Error ("Right Turn Penalty %.1lf is Out of Range (0..10000)", dvalue);
			}
			Print (0, " %.2lf ", dvalue);

			right_imped = Round (dvalue);

			if (!hhold_flag) break;

			if (!imped_right.Add (right_imped)) goto mem_error;
		} while (str_ptr != NULL);

		Print (0, "impedance units per right turn");
	}

	//---- u-turn penalty ----
	
	str_ptr = Get_Control_String (U_TURN_PENALTY);

	if (str_ptr != NULL) {

		Print (1, "U-Turn Penalt%s=", ((hhold_flag) ? "ies " : "y "));

		do {
			if (str_ptr != NULL) {
				str_ptr = Get_Double (str_ptr, &dvalue);
			} else {
				dvalue = uturn_imped;
			}
			if (dvalue < 0.0 || dvalue > 10000.0) {
				Error ("U-Turn Penalty %.1lf is Out of Range (0..10000)", dvalue);
			}
			Print (0, " %.2lf ", dvalue);

			uturn_imped = Round (dvalue);

			if (!hhold_flag) break;

			if (!imped_uturn.Add (uturn_imped)) goto mem_error;
		} while (str_ptr != NULL);

		Print (0, "impedance units per U-turn");
	}

	if (transit_net) {
	
		//---- transfer_penalty ----
		
		str_ptr = Get_Control_String (TRANSFER_PENALTY);

		Print (2, "Transfer Penalt%s=", ((hhold_flag) ? "ies " : "y "));

		do {
			if (str_ptr != NULL) {
				str_ptr = Get_Double (str_ptr, &dvalue);
			} else {
				dvalue = xfer_imped;
			}
			if (dvalue < 0.0 || dvalue > 100000.0) {
				Error ("Transfer Penalty %.1lf is Out of Range (0..100000)", dvalue);
			}
			Print (0, " %.2lf ", dvalue);

			xfer_imped = Round (dvalue);

			if (!hhold_flag) break;

			if (!imped_xfer.Add (xfer_imped)) goto mem_error;
		} while (str_ptr != NULL);

		Print (0, "impedance units per transfer");
	
		//---- stop waiting penalty ----
		
		str_ptr = Get_Control_String (STOP_WAITING_PENALTY);

		Print (1, "Stop Waiting Penalt%s=", ((hhold_flag) ? "ies " : "y "));

		do {
			if (str_ptr != NULL) {
				str_ptr = Get_Double (str_ptr, &dvalue);
			} else {
				dvalue = stop_imped;
			}
			if (dvalue < 0.0 || dvalue > 100000.0) {
				Error ("Stop Waiting Penalty %.1lf is Out of Range (0..100000)", dvalue);
			}
			Print (0, " %.2lf ", dvalue);

			stop_imped = Round (dvalue);

			if (!hhold_flag) break;

			if (!imped_stop.Add (stop_imped)) goto mem_error;
		} while (str_ptr != NULL);

		Print (0, "impedance units per boarding");
		
		//---- station waiting penalty ----
		
		str_ptr = Get_Control_String (STATION_WAITING_PENALTY);

		Print (1, "Station Waiting Penalt%s=", ((hhold_flag) ? "ies " : "y "));

		do {
			if (str_ptr != NULL) {
				str_ptr = Get_Double (str_ptr, &dvalue);
			} else {
				dvalue = station_imped;
			}
			if (dvalue < 0.0 || dvalue > 100000.0) {
				Error ("Station Waiting Penalty %.1lf is Out of Range (0..100000)", dvalue);
			}
			Print (0, " %.2lf ", dvalue);

			station_imped = Round (dvalue);

			if (!hhold_flag) break;

			if (!imped_station.Add (station_imped)) goto mem_error;
		} while (str_ptr != NULL);

		Print (0, "impedance units per boarding");

		//---- bus bias factor ----
		
		str_ptr = Get_Control_String (BUS_BIAS_FACTOR);

		Print (1, "Bus Bias Factor%s=", value_text);

		do {
			if (str_ptr != NULL) {
				str_ptr = Get_Token (str_ptr, buffer);

				if (buffer [0] == 'N' || buffer [0] == 'n') {
					lvalue = 100;
				} else {
					lvalue = (int) (atof (buffer) * 100.0 + 0.5);
				}
			} else {
				lvalue = bus_bias;
			}
			if (lvalue < 100 || lvalue > 300) {
				Error ("Bus Bias Factor %.1lf is Out of Range (1.0..3.0)", (double) lvalue / 100.0);
			}
			if (lvalue == 100) {
				Print (0, " N/A ");
			} else {
				Print (0, " %.2lf ", (double) lvalue / 100.0);
			}
			bus_bias = lvalue;

			if (!hhold_flag) break;

			if (!bias_bus.Add (bus_bias)) goto mem_error;
		} while (str_ptr != NULL);

		Print (0, "impedance factor");

		//---- bus bias constant ----
		
		str_ptr = Get_Control_String (BUS_BIAS_CONSTANT);

		Print (1, "Bus Bias Constant%s=", value_text);

		do {
			if (str_ptr != NULL) {
				str_ptr = Get_Token (str_ptr, buffer);

				if (buffer [0] == 'N' || buffer [0] == 'n') {
					dvalue = 0.0;
				} else {
					dvalue = atof (buffer);
				}
			} else {
				dvalue = bus_constant;
			}
			if (dvalue < 0.0) {
				Error ("Bus Bias Constant %.1lf is Out of Range (> 0)", dvalue);
			}
			if (dvalue == 0.0) {
				Print (0, " N/A ");
			} else {
				Print (0, " %.2lf ", dvalue);
			}
			bus_constant = Round (dvalue * 100);

			if (!hhold_flag) break;

			if (!add_bus.Add (bus_constant)) goto mem_error;
		} while (str_ptr != NULL);

		Print (0, "impedance units");

		//---- rail bias factor ----
		
		str_ptr = Get_Control_String (RAIL_BIAS_FACTOR);

		Print (1, "Rail Bias Factor%s=", value_text);

		do {
			if (str_ptr != NULL) {
				str_ptr = Get_Token (str_ptr, buffer);

				if (buffer [0] == 'N' || buffer [0] == 'n') {
					lvalue = 100;
				} else {
					lvalue = (int) (atof (buffer) * 100.0 + 0.5);
				}
			} else {
				lvalue = rail_bias;
			}
			if (lvalue < 10 || lvalue > 100) {
				Error ("Rail Bias Factor %.1lf is Out of Range (0.1..1.0)", (double) lvalue / 100.0);
			}
			if (lvalue == 100) {
				Print (0, " N/A ");
			} else {
				Print (0, " %.2lf ", (double) lvalue / 100.0);
			}
			rail_bias = lvalue;

			if (!hhold_flag) break;

			if (!bias_rail.Add (rail_bias)) goto mem_error;
		} while (str_ptr != NULL);

		Print (0, "impedance factor");

		//---- rail bias constant ----
		
		str_ptr = Get_Control_String (RAIL_BIAS_CONSTANT);

		Print (1, "Rail Bias Constant%s=", value_text);

		do {
			if (str_ptr != NULL) {
				str_ptr = Get_Token (str_ptr, buffer);

				if (buffer [0] == 'N' || buffer [0] == 'n') {
					dvalue = 0.0;
				} else {
					dvalue = atof (buffer);
				}
			} else {
				dvalue = rail_constant;
			}
			if (dvalue > 0.0) {
				Error ("Rail Bias Constant %.1lf is Out of Range (< 0)", dvalue);
			}
			if (dvalue == 0.0) {
				Print (0, " N/A ");
			} else {
				Print (0, " %.2lf ", dvalue);
			}
			rail_constant = -(Round (-dvalue * 100));

			if (!hhold_flag) break;

			if (!add_rail.Add (rail_constant)) goto mem_error;
		} while (str_ptr != NULL);

		Print (0, "impedance units");
	}

	//---- max walk distance ----
	
	str_ptr = Get_Control_String (MAX_WALK_DISTANCE);

	Print (2, "Maximum Walk Distance%s=", value_text);

	do {
		if (str_ptr != NULL) {
			str_ptr = Get_Token (str_ptr, buffer);

			if (buffer [0] == 'N' || buffer [0] == 'n') {
				lvalue = 0;
			} else {
				lvalue = atoi (buffer);
			}
		} else {
			lvalue = max_walk;
		}
		if (lvalue != 0 && (lvalue < 100 || lvalue > 10000)) {
			Error ("Maximum Walk Distance %d is Out of Range (100..10000)", lvalue);
		}
		if (lvalue == 0) {
			Print (0, " N/A ");
		} else {
			Print (0, " %d ", lvalue);
		}
		max_walk = Round (lvalue / walk_speed);

		if (!hhold_flag) break;

		if (!walk_max.Add (max_walk)) goto mem_error;
	} while (str_ptr != NULL);

	Print (0, "meters");

	//---- max bike distance ----
	
	if (bike_net) {
		str_ptr = Get_Control_String (MAX_BICYCLE_DISTANCE);

		Print (1, "Maximum Bicycle Distance%s=", value_text);

		do {
			if (str_ptr != NULL) {
				str_ptr = Get_Token (str_ptr, buffer);

				if (buffer [0] == 'N' || buffer [0] == 'n') {
					lvalue = 0;
				} else {
					lvalue = atoi (buffer);
				}
			} else {
				lvalue = max_bike;
			}
			if (lvalue != 0 && (lvalue < 1000 || lvalue > 20000)) {
				Error ("Maximum Bicycle Distance %d is Out of Range (1000..20000)", lvalue);
			}
			if (lvalue == 0) {
				Print (0, " N/A ");
			} else {
				Print (0, " %d ", lvalue);
			}
			max_bike = Round (lvalue / bike_speed);

			if (!hhold_flag) break;

			if (!bike_max.Add (max_bike)) goto mem_error;
		} while (str_ptr != NULL);

		Print (0, "meters");
	}

	if (transit_net) {

		//---- max wait time ----

		str_ptr = Get_Control_String (MAX_WAIT_TIME);

		Print (1, "Maximum Waiting Time%s=", value_text);

		do {
			if (str_ptr != NULL) {
				str_ptr = Get_Token (str_ptr, buffer);

				if (buffer [0] == 'N' || buffer [0] == 'n') {
					lvalue = 0;
				} else {
					lvalue = atoi (buffer);
				}
			} else {
				lvalue = max_wait;
			}
			if (lvalue != 0 && (lvalue < 5 || lvalue > 200)) {
				Error ("Maximum Waiting Time %d is Out of Range (5..200)", lvalue);
			}
			if (lvalue == 0) {
				Print (0, " N/A ");
			} else {
				Print (0, " %d ", lvalue);
			}
			max_wait = Round (lvalue * 60);

			if (!hhold_flag) break;

			if (!wait_max.Add (max_wait)) goto mem_error;
		} while (str_ptr != NULL);

		Print (0, "minutes");

		//---- min wait time ----

		str_ptr = Get_Control_String (MIN_WAIT_TIME);

		Print (1, "Minimum Waiting Time%s=", value_text);

		do {
			if (str_ptr != NULL) {
				str_ptr = Get_Token (str_ptr, buffer);

				if (buffer [0] == 'N' || buffer [0] == 'n') {
					lvalue = 0;
				} else {
					lvalue = atoi (buffer);
				}
			} else {
				lvalue = min_wait;
			}
			if (lvalue < 0 || lvalue > 3600) {
				Error ("Minimum Waiting Time %d is Out of Range (0..3600)", lvalue);
			}
			if (lvalue == 0) {
				Print (0, " N/A ");
			} else {
				Print (0, " %d ", lvalue);
			}
			min_wait = Round (lvalue);

			if (!hhold_flag) break;

			if (!wait_min.Add (min_wait)) goto mem_error;
		} while (str_ptr != NULL);

		Print (0, "seconds");

		//---- max transfers ----

		str_ptr = Get_Control_String (MAX_NUMBER_OF_TRANSFERS);

		Print (1, "Maximum Number of Transfers =");

		do {
			if (str_ptr != NULL) {
				str_ptr = Get_Token (str_ptr, buffer);

				if (buffer [0] == 'N' || buffer [0] == 'n') {
					lvalue = 100;
				} else {
					lvalue = atoi (buffer);
				}
			} else {
				lvalue = max_transfers;
			}
			if (lvalue != 100 && (lvalue < 0 || lvalue > 10)) {
				Error ("Maximum Number of Transfers %d is Out of Range (0..10)", lvalue);
			}
			if (lvalue == 100) {
				Print (0, " N/A ");
			} else {
				Print (0, " %d ", lvalue);
			}
			max_transfers = lvalue;

			if (!hhold_flag) break;

			if (!xfer_max.Add (max_transfers)) goto mem_error;
		} while (str_ptr != NULL);

		//---- max paths ----
		
		str_ptr = Get_Control_String (MAX_NUMBER_OF_PATHS);

		Print (1, "Maximum Number of Paths =");

		lvalue = max_paths;
		max_paths = 0;
		i = 1;

		do {
			if (str_ptr != NULL) {
				str_ptr = Get_Token (str_ptr, buffer);

				if (buffer [0] == 'N' || buffer [0] == 'n') {
					lvalue = 0;
				} else {
					lvalue = atoi (buffer);
				}
			}
			if (lvalue != 0 && (lvalue < 1 || lvalue > MAX_PATHS)) {
				Error ("Maximum Number of Paths %d is Out of Range (1..%d)", lvalue, MAX_PATHS);
			}
			if (lvalue == 0) {
				Print (0, " N/A ");
			} else {
				Print (0, " %d ", lvalue);
			}
			if (!hhold_flag) {
				if (lvalue == 0 || lvalue > max_transfers + 1) {
					max_paths = max_transfers + 1;
				} else {
					max_paths = lvalue;
				}
				break;
			}
			if (lvalue == 0 || lvalue > xfer_max.Best (i) + 1) {
				lvalue = xfer_max.Best (i) + 1;
			}
			i++;
			if (lvalue > max_paths) max_paths = lvalue;

			if (!path_max.Add (lvalue)) goto mem_error;
		} while (str_ptr != NULL);

		//---- max park ride percentage ----

		if (park_ride_flag) {
			str_ptr = Get_Control_String (MAX_PARK_RIDE_PERCENTAGE);

			Print (1, "Maximum Park-&-Ride Percentage%s=", value_text);

			do {
				if (str_ptr != NULL) {
					str_ptr = Get_Token (str_ptr, buffer);

					if (buffer [0] == 'N' || buffer [0] == 'n') {
						lvalue = 0;
					} else {
						lvalue = atoi (buffer);
					}
				} else {
					lvalue = max_parkride;
				}
				if (lvalue != 0 && (lvalue < 1 || lvalue > 100)) {
					Error ("Maximum Park-&-Ride Percentage %d is Out of Range (1..100)", lvalue);
				}
				if (lvalue == 0) {
					Print (0, " N/A ");
				} else {
					Print (0, " %d ", lvalue);
				}
				max_parkride = (lvalue != 0) ? lvalue : 100;

				if (!hhold_flag) break;

				if (!parkride_max.Add (max_parkride)) goto mem_error;
			} while (str_ptr != NULL);
		}

		//---- max kiss ride percentage ----

		if (kiss_ride_flag) {
			str_ptr = Get_Control_String (MAX_KISS_RIDE_PERCENTAGE);

			Print (1, "Maximum Kiss-&-Ride Percentage%s=", value_text);

			do {
				if (str_ptr != NULL) {
					str_ptr = Get_Token (str_ptr, buffer);

					if (buffer [0] == 'N' || buffer [0] == 'n') {
						lvalue = 0;
					} else {
						lvalue = atoi (buffer);
					}
				} else {
					lvalue = max_kissride;
				}
				if (lvalue != 0 && (lvalue < 1 || lvalue > 100)) {
					Error ("Maximum Kiss-&-Ride Percentage %d is Out of Range (1..100)", lvalue);
				}
				if (lvalue == 0) {
					Print (0, " N/A ");
				} else {
					Print (0, " %d ", lvalue);
				}
				max_kissride = (lvalue != 0) ? lvalue : 100;

				if (!hhold_flag) break;

				if (!kissride_max.Add (max_kissride)) goto mem_error;
			} while (str_ptr != NULL);

			//---- kiss ride time factor ----

			str_ptr = Get_Control_String (KISS_RIDE_TIME_FACTOR);

			Print (1, "Kiss-&-Ride Time Factor%s=", value_text);

			do {
				if (str_ptr != NULL) {
					str_ptr = Get_Token (str_ptr, buffer);

					if (buffer [0] == 'N' || buffer [0] == 'n') {
						dvalue = 0;
					} else {
						dvalue = atof (buffer);
					}
				} else {
					dvalue = (double) kissride_factor / 100.0;
				}
				if (dvalue < 1.0 || dvalue > 4.0) {
					Error ("Kiss-&-Ride Time Factor %.2lf is Out of Range (1.0..4.0)", dvalue);
				}
				Print (0, " %.2lf ", dvalue);

				kissride_factor = (int) (dvalue * 100.0 + 0.5);

				if (!hhold_flag) break;

				if (!factor_kissride.Add (kissride_factor)) goto mem_error;
			} while (str_ptr != NULL);

			//---- kiss ride stop types ----

			str_ptr = Get_Control_String (KISS_RIDE_STOP_TYPES);

			if (str_ptr != NULL) {
				memset (kissride_type, '\0', sizeof (kissride_type));
				Print (1, "Kiss-&-Ride Stop Types = ");

				while (str_ptr != NULL) {
					str_ptr = Get_Token (str_ptr, buffer);

					if (buffer [0] == '\0') break;

					i = Stop_Code (buffer);

					if (i < 0 || i > EXTLOAD) {
						Error ("Kiss-&-Ride Stop Type %s is Out of Range", buffer);
					}
					kissride_type [i] = true;
					Print (0, "%s  ", buffer);
				}
			} else {
				Print (1, "Kiss-&-Ride is Permitted at All Stop Types");
				for (i=0; i <= EXTLOAD; i++) {
					kissride_type [i] = true;
				}
			}

			//---- max kiss ride dropoff walk ----

			str_ptr = Get_Control_String (MAX_KISS_RIDE_DROPOFF_WALK);

			if (str_ptr != NULL) {
				kissride_walk = atoi (str_ptr);

				if (kissride_walk < 10 || kissride_walk > 500) {
					Error ("Maximum Kiss-&-Ride Dropoff Walk Distance %d is Out of Range (10..500)", kissride_walk);
				}
			}
			Print (1, "Maximum Kiss-&-Ride Dropoff Walk Distance = %d meters", kissride_walk);
		}

		//---- max legs per path ----

		str_ptr = Get_Control_String (MAX_LEGS_PER_PATH);

		if (str_ptr != NULL) {
			leg_check = atoi (str_ptr);

			if (leg_check < 10 || leg_check > 10000) {
				Error ("Maximum Legs Per Path %d is Out of Range (10..10000)", leg_check);
			}
			Print (1, "Maximum Legs Per Path = %d", leg_check);
		}

		//---- wait leg flag ----

		wait_leg_flag = Get_Control_Flag (ADD_WAIT_TO_TRANSIT_LEG);

		if (wait_leg_flag) {
			Print (1, "Waiting Time will be Added to the Transit Leg");
		} else {
			Print (1, "Waiting Time will be Added to the Access Leg");
		}

		//---- fare class distribution ----

		str_ptr = Get_Control_String (FARE_CLASS_DISTRIBUTION);
			
		if (str_ptr != NULL) {
			Print (1, "Transit Fare Class Distribution = ");
			memset (fare_class, '\0', sizeof (fare_class));
			fare_class_flag = true;
			i = 0;

			while (str_ptr != NULL && i <= SPECIAL) {
				str_ptr = Get_Token (str_ptr, buffer);

				if (buffer [0] == '\0') break;

				dvalue = atof (buffer);
	
				if (dvalue < 0.0 || dvalue > 100.0) {
					Error ("Fare Class Distribution %.2lf is Out of Range (0.0..100.0)", dvalue);
				}
				fare_class [i++] = dvalue;
			}
			dvalue = 0;
			for (--i; i >= 0; i--) {
				dvalue += fare_class [i];
			}
			if (dvalue == 0.0) {
				fare_class [0] = 1.0;
			} else {
				dvalue = 1.0 / dvalue;

				for (i=0; i <= SPECIAL; i++) {
					fare_class [i] *= dvalue;
				}
			}

			//---- convert to a cumulative distribution ----

			dvalue = 0.0;

			for (i=0; i <= SPECIAL; i++) {
				Print (0, "%.1lf ", fare_class [i] * 100.0);
				dvalue += fare_class [i];
				fare_class [i] = dvalue;
			}
			fare_class [SPECIAL] = 1.0;
		}
	}

	if (drive_net) {

		//---- parking hours by purpose ----

		str_ptr = Get_Control_String (PARKING_HOURS_BY_PURPOSE);

		if (str_ptr != NULL) {
			park_hours_flag = true;
			Print (1, "Parking Hours by Trip Purpose = ");

			do {
				if (str_ptr != NULL) {
					str_ptr = Get_Token (str_ptr, buffer);

					if (buffer [0] == 'N' || buffer [0] == 'n') {
						dvalue = 0;
					} else {
						dvalue = atof (buffer);
					}
				} else {
					dvalue = UnRound (park_hours);
				}
				if (dvalue < 0.0 || dvalue > 24.0) {
					Error ("Parking Hours %.2lf is Out of Range (0.0..24.0)", dvalue);
				}
				Print (0, "%.2lf ", dvalue);

				park_hours = Round (dvalue);

				if (!hours_park.Add (park_hours)) goto mem_error;
			} while (str_ptr != NULL);

			Print (0, "hours");
		}

		//---- local facility type ----

		str_ptr = Get_Control_String (LOCAL_FACILITY_TYPE);

		if (str_ptr != NULL) {
			local_type = Facility_Code (str_ptr);

			if (local_type == EXTERNAL) {
				local_flag = false;
			} else if (local_type < MAJOR || local_type > LOCAL) {
				Error ("Local Facility Type %s is Out of Range (MAJOR..LOCAL)", str_ptr);
			} else {
				local_flag = true;
			}
		} else {
			local_flag = false;
		}

		if (local_flag) {
			Break_Check (3);

			Print (2, "Local Facility Type = %s", str_ptr);

			//---- local access distance ----
			
			str_ptr = Get_Control_String (LOCAL_ACCESS_DISTANCE);

			if (str_ptr != NULL) {
				Get_Integer (str_ptr, &local_distance);

				if (local_distance < 100 || local_distance > 7500) {
					Error ("Local Access Distance %d is Out of Range (100..7500)", local_distance);
				}
			}
			Print (1, "Local Access Distance = %d meters", local_distance);

			local_distance = Round (local_distance);
		}

		//---- max circuity ratio ----

		str_ptr = Get_Control_String (MAX_CIRCUITY_RATIO);

		if (str_ptr != NULL) {
			Get_Double (str_ptr, &dvalue);

			if (dvalue == 0.0) {
				Print (2, "Path Circuity Tests are Disabled");
			} else {
				if (dvalue < 1.0 || dvalue > 10.0) {
					Error ("Maximum Circuity Ratio %.0lf is Out of Range (1..10)", dvalue);
				}
				max_ratio = (int) (dvalue * 100.0 + 0.5);
				distance_flag = true;
			}
		}
		if (distance_flag) {
			Break_Check (4);

			Print (2, "Maximum Circuity Ratio = %.2lf", max_ratio / 100.0);
			max_ratio -= 100;

			//---- min circuity distance ----
			
			str_ptr = Get_Control_String (MIN_CIRCUITY_DISTANCE);

			if (str_ptr != NULL) {
				Get_Integer (str_ptr, &min_distance);

				if (min_distance < 0 || min_distance > 10000) {
					Error ("Minimum Circuity Distance %d is Out of Range (0..10000)", min_distance);
				}
			}
			Print (1, "Minimum Circuity Distance = %d meters", min_distance);

			//---- max circuity distance ----
			
			str_ptr = Get_Control_String (MAX_CIRCUITY_DISTANCE);

			if (str_ptr != NULL) {
				Get_Integer (str_ptr, &max_distance);

				if (max_distance < min_distance || max_distance > 100000) {
					Error ("Maximum Circuity Distance %d is Out of Range (%d..100000)", max_distance, min_distance);
				}
			}
			Print (1, "Maximum Circuity Distance = %d meters", max_distance);
			min_distance = Round (min_distance);
			max_distance = Round (max_distance);
		}

		//---- link delay updates and volume factor ----

		str_ptr = Get_Control_String (LINK_DELAY_UPDATE_RATE);

		if (str_ptr != NULL) {
			Get_Integer (str_ptr, &update_rate);

			if (update_rate < 0 || update_rate > 5000) {
				Error ("Link Delay Update Rate %d is Out of Range (0..5000)", update_rate);
			}
			if (update_rate > 0) update_flag = true;
		}
		if (!update_flag) {
			Print (2, "Link Delay Updates are Disabled");
		} else {
			Print (2, "Link Delay Update Rate = %d trips", update_rate);
			Print (1);

			//---- equation parameters ----

			equation.Num_Equations (EXTERNAL);

			for (i=1; i <= EXTERNAL; i++) {
				str_ptr = Get_Control_String (EQUATION_PARAMETERS_x, i);

				equation.Add_Equation (i, str_ptr);
			}

			//---- link delay volume factor ----

			str_ptr = Get_Control_String (LINK_DELAY_VOL_FACTOR);

			if (str_ptr != NULL) {
				Get_Integer (str_ptr, &vol_factor);

				if (vol_factor < 1) {
					Error ("Link Delay Volume Factor %d is Out of Range (1..inf)", update_rate);
				}
				Print (2, "Link Delay Volume Factor = %d", vol_factor);
				Print (1);
			}
		}
		tod_flag = delay_flag || update_flag;

		if (tod_flag) {
			bool flag = false;

			//---- check for link delay time range controls ----

			str_ptr = Get_Control_String (LINK_DELAY_START_TIME);

			if (str_ptr != NULL) {
				lvalue = trip_time.Step (str_ptr);
				if (lvalue < 0 || lvalue > MIDNIGHT * 5) {
					Error ("Link Delay Start Time %s is Out of Range", str_ptr);
				}
				ttime_data.Min_Size (lvalue);
				connect_time.Min_Size (lvalue);

				Print (2, "Link Delay Start Time = %s", trip_time.Format_Step (lvalue));
				flag = true;
			}
			str_ptr = Get_Control_String (LINK_DELAY_END_TIME);

			if (str_ptr != NULL) {
				lvalue = trip_time.Step (str_ptr);
				if (lvalue <= ttime_data.Min_Size () || lvalue > MIDNIGHT * 5) {
					Error ("Link Delay End Time %s is Out of Range", str_ptr);
				}
				ttime_data.Max_Size (lvalue);
				connect_time.Max_Size (lvalue);

				if (!flag) Print (1);
				Print (1, "Link Delay End Time = %s", trip_time.Format_Step (lvalue));
				flag = true;
			}
			if (flag && delay_flag) {
				int size = ttime_data.Period_Size ();

				if (size > 0) {
					ttime_data.Period_Size (size, ttime_data.Min_Size (), ttime_data.Max_Size ());
					connect_time.Period_Size (size, connect_time.Min_Size (), connect_time.Max_Size ());
				}
			}
		}
	}

	//---- open the problem file ----
	
	str_ptr = Get_Control_String (NEW_PROBLEM_FILE);

	if (str_ptr != NULL) {
		Print (1);
		format_ptr = Get_Control_String (NEW_PROBLEM_FORMAT);

		if (format_ptr != NULL) {
			problem_file.Dbase_Format (format_ptr);
		}
		problem_file.Router_Flag ();
		problem_file.Create (Project_Filename (str_ptr, Extension ()));
		problem_flag = true;
	}

	//---- max routing problems ----
	
	str_ptr = Get_Control_String (MAX_ROUTING_PROBLEMS);

	if (str_ptr != NULL) {
		Get_Integer (str_ptr, &lvalue);

		if (lvalue < 0) {
			Error ("Maximum Routing Problems %d is Out of Range", lvalue);
		}
		Max_Problems (lvalue);
	}
	if (Max_Problems () > 0) {
		Print (1, "Maximum Number of Routing Problems = %d", Max_Problems ());
	}

	//---- arcview problem dump ----

	str_ptr = Get_Control_String (ARCVIEW_PROBLEM_DUMP);

	if (str_ptr != NULL) {
		if (!problem_flag) {
			Error ("A Problem File is required for Arcview Problem Dumps");
		}
		dump_flag = true;

		Print (1);
		dump_file.File_Type ("ArcView Problem Dump");
		dump_file.File_Access (CREATE);
		dump_file.Shape_Type (VECTOR);

		dump_file.Replicate_Fields (&problem_file);

		if (!dump_file.Open (Project_Filename (str_ptr))) {
			File_Error ("Opening ArcView Problem Dump", dump_file.Shape_Filename ());
		}
		dump_file.Write_Header ();

		//---- problem dump type ----

		str_ptr = Get_Control_String (PROBLEM_DUMP_TYPE);

		if (str_ptr != NULL) {
			dump_type = atoi (str_ptr);

			Print (1, "Problem Dump Type = %d", dump_type);
		}

		//---- percent problems dumped ----

		str_ptr = Get_Control_String (PERCENT_PROBLEMS_DUMPED);

		if (str_ptr != NULL) {
			percent_dump = Round (atof (str_ptr));

			if (percent_dump < 1 || percent_dump > Round (100)) {
				Error ("Percent Problems Dumped %.1lf is Out of Range (0.1..100)", UnRound (percent_dump));
			}
			Print (1, "Percent Problems Dumped = %.1lf percent", UnRound (percent_dump));

			sel_dump_flag = (percent_dump != Round (100));
		}
	}

	//---- path attribute file ----
	
	str_ptr = Get_Control_String (NEW_PATH_ATTRIBUTE_FILE);

	if (str_ptr != NULL) {
		attribute_file.File_Access (CREATE);
		attribute_file.File_Type ("Path Attribute File");
		Print (1);

		attribute_file.Open (Project_Filename (str_ptr));
		attribute_flag = true;

		fprintf (attribute_file.File (), "TRAVELER\tTRIP\tIMPEDANCE\tWALK\tFIRST_WAIT\tXFER_WAIT\tNUM_XFER\tPENALTY\tTRANSIT\tDRIVE\tCOST\n");
	}

	//---- set default parameters ----

	if (!hhold_flag) {
		walk_flag = (max_walk != 0);
		bike_flag = (max_bike != 0);
		wait_flag = (max_wait != 0);
		rail_bias_flag = (rail_bias != 100 || rail_constant != 0);
		bus_bias_flag = (bus_bias != 100 || bus_constant != 0);
		turn_flag = (left_imped > 0 || right_imped > 0 || uturn_imped > 0);
	}
	return;

control_error:
	Error ("Missing Control Key = %s", Current_Key ());

mem_error:
	Error ("Insufficient Memory for Value Data");
}
