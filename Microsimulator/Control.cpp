//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "Microsimulator.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void Microsimulator::Program_Control (void)
{
	int i, lvalue;
	double dvalue;
	char *str_ptr, *format_ptr, buffer [FIELD_BUFFER];
	bool plan_flag;

	char *method [] = { "CELL-BASED", "DISTANCE-BASED" };

	//---- open network files ----

	Demand_Service::Program_Control ();

	//---- set the transit flag ----

	transit_flag = Network_File_Flag (TRANSIT_STOP);

	if (transit_flag) {
		if (!Network_File_Flag (TRANSIT_ROUTE) || !Network_File_Flag (TRANSIT_SCHEDULE) ||
			!Network_File_Flag (TRANSIT_DRIVER)) {
			Error ("Transit Simulation requires Route, Schedule, and Driver files");
		}
		if (line_array == NULL) {
			line_array = (Line_Array *) new Rider_Array ();
		}
	} else {
		Network_File_False (TRANSIT_ROUTE);
		Network_File_False (TRANSIT_FARE);
		Network_File_False (TRANSIT_SCHEDULE);
		Network_File_False (TRANSIT_DRIVER);
	}

	//---- check the signal keys ----

	signal_flag = Network_File_Flag (SIGNALIZED_NODE);

	if (signal_flag) {
		if (!Network_File_Flag (TIMING_PLAN) || !Network_File_Flag (PHASING_PLAN)) {
			Error ("Traffic Signals require Timing and Phasing Plans");
		}
	} else {
		Network_File_False (TIMING_PLAN);
		Network_File_False (PHASING_PLAN);
		Network_File_False (DETECTOR);
		Network_File_False (SIGNAL_COORDINATOR);
	}
	boundary_flag = Demand_File_Flag (BOUNDARY_SPEED);

	//---- open the plan file ----
	
	if (Get_Control_String (NODE_LIST_PATHS) != NULL) {
		node_flag = Get_Control_Flag (NODE_LIST_PATHS);
		plan_flag = true;
	} else {
		plan_flag = false;
	}
	str_ptr = Get_Control_String (PLAN_FILE);

	if (str_ptr == NULL) goto control_error;

	str_ptr = Project_Filename (str_ptr, Extension ());

	Print_Filename (2, plan_file.File_Type (), str_ptr);

	plan_file.Filename (str_ptr);

	format_ptr = Get_Control_String (PLAN_FORMAT);

	if (format_ptr != NULL) {
		plan_file.File_Format (format_ptr);
		Print (1, "%s Format = %s", plan_file.File_Type (), format_ptr);
	}
	plan_file.Node_Based_Flag (node_flag);

	if (!plan_file.Open (0)) {
		File_Error ("Opening Plan File", plan_file.Filename ());
	}
	if (plan_flag && node_flag != plan_file.Node_Based_Flag ()) {
		Warning ("NODE_LIST_PATH and Plan File *.def are Inconsistent");
	}
	node_flag = plan_file.Node_Based_Flag ();

	//---- node list paths ----
	
	if (node_flag) {
		Print (1, "Plan Files contain Node List Paths");
		AB_Key_Flag  (true);
	} else {
		Print (1, "Plan Files contain Link List Paths");
	}

	//---- cell size ----

	str_ptr = Get_Control_String (CELL_SIZE);

	if (str_ptr != NULL) {
		Get_Double (str_ptr, &dvalue);

		if (dvalue < 1.0 || dvalue > 25.0) {
			Error ("Cell Size %.2lf is Out of Range (1..25)", dvalue);
		}
		cell_size = Round (dvalue);
	} else {
		dvalue = UnRound (cell_size);
	}
	Print (2, "Cell Size = %.2lf meters", dvalue);

	half_cell = (cell_size + 1) / 2;
	cell_round = 3 * cell_size / 4;

	//---- time step ----

	str_ptr = Get_Control_String (TIME_STEPS_PER_SECOND);

	if (str_ptr != NULL) {
		Get_Integer (str_ptr, &steps_per_second);

		if (steps_per_second < 1 || steps_per_second > 25) {
			Error ("%d Time Steps per Second is Out of Range (1..25)", steps_per_second);
		}
	}
	Print (1, "Time Steps per Second = %d", steps_per_second);

	time_step.Step_Size (steps_per_second);
	step_round = (steps_per_second + 1) / 2;

	//---- get the time of day format ----
	
	str_ptr = Get_Control_String (TIME_OF_DAY_FORMAT);

	if (str_ptr == NULL) {
		str_ptr = "24_HOUR_CLOCK";
	}
	if (!time_step.Format (str_ptr)) {
		Error ("Time of Day Format %s was Unrecognized", str_ptr);
	}
	Print (2, "Time of Day Format = %s", str_ptr);
	
	//---- get the start time ----
	
	str_ptr = Get_Control_String (SIMULATION_START_TIME);

	if (str_ptr != NULL) {
		start_time_step = time_step.Step (str_ptr);

		if (start_time_step < 0) {
			Error ("Simulation Start Time %s was Unrecognized", str_ptr);
		}
	}
    Print (1, "Simulation Start Time = %s", time_step.Format_Step (start_time_step));

	//---- get the end time ----
	
	str_ptr = Get_Control_String (SIMULATION_END_TIME);

	if (str_ptr != NULL) {
		end_time_step = time_step.Step (str_ptr);

		if (end_time_step < 0) {
			Error ("Simulation End Time %s was Unrecognized", str_ptr);
		}
	}
	Print (1, "Simulation End Time = %s", time_step.Format_Step (end_time_step));

	if (start_time_step >= end_time_step) {
		Error ("Simulation Start and End Times are Inconsistent");
	}

	//---- speed calculation method ----

	str_ptr = Get_Control_String (SPEED_CALCULATION_METHOD);

	if (str_ptr != NULL) {
		if (str_cmp (str_ptr, method [0]) == 0) {
			speed_method = false;
		} else if (str_cmp (str_ptr, method [1]) == 0) {
			speed_method = true;
		} else {
			Error ("Speed Calculation Method %s is Not Recognized", str_ptr);
		}
	}
	Print (2, "Speed Calculation Method = %s", ((speed_method) ? method [1] : method [0]));

	//---- plan following distance ----

	str_ptr = Get_Control_String (PLAN_FOLLOWING_DISTANCE);

	if (str_ptr != NULL) {
		Get_Integer (str_ptr, &lvalue);

		if (lvalue < 0 || lvalue > 2000) {
			Error ("Plan Following Distance %d is Out of Range (0..2000)", lvalue);
		}
		plan_follow = lvalue;
	}
	Print (2, "Plan Following Distance = %d meter%c", plan_follow,
		((plan_follow != 1) ? 's' : ' '));

	plan_follow = (Round (plan_follow) + cell_round) / cell_size; 

	//---- look ahead distance ----

	str_ptr = Get_Control_String (LOOK_AHEAD_DISTANCE);

	if (str_ptr != NULL) {
		Get_Integer (str_ptr, &lvalue);

		if (lvalue < 0 || lvalue > 2000) {
			Error ("Look Ahead Distance %d is Out of Range (0..2000)", lvalue);
		}
		look_ahead = lvalue;
	}
	Print (1, "Look Ahead Distance = %d meter%c", look_ahead,
		((look_ahead != 1) ? 's' : ' '));

	look_ahead = (Round (look_ahead) + cell_round) / cell_size; 

	look_ahead_flag = (look_ahead > 0);

	if (look_ahead_flag) {

		//---- look ahead lane factor ----

		str_ptr = Get_Control_String (LOOK_AHEAD_LANE_FACTOR);

		if (str_ptr != NULL) {
			Get_Double (str_ptr, &dvalue);

			if (dvalue < 1.0 || dvalue > 25.0) {
				Error ("Look Ahead Lane Factor %.2lf is Out of Range (1..25)", dvalue);
			}
		} else {
			dvalue = UnRound (lane_factor);
		}
		Print (1, "Look Ahead Lane Factor = %.2lf", dvalue);

		if (speed_method) {
			lane_factor = (int) (dvalue * cell_size + 0.5);
		} else {
			lane_factor = Round (dvalue);
		}

		//---- look ahead time factor ----

		str_ptr = Get_Control_String (LOOK_AHEAD_TIME_FACTOR);

		if (str_ptr != NULL) {
			Get_Double (str_ptr, &dvalue);

			if (dvalue < 0.0 || dvalue > 5.0) {
				Error ("Look Ahead Time Factor %.2lf is Out of Range (0..5)", dvalue);
			}
			time_factor = Round (dvalue);
		} else {
			dvalue = UnRound (time_factor);
		}
		Print (1, "Look Ahead Time Factor = %.2lf", dvalue);

		if (lane_factor == 0 && time_factor == 0) {
			look_ahead_flag = false;
		}
	}

	//---- maximum swapping speed ----

	str_ptr = Get_Control_String (MAXIMUM_SWAPPING_SPEED);

	if (str_ptr != NULL) {
		Get_Double (str_ptr, &dvalue);

		if (dvalue < 0.0 || dvalue > 40.0) {
			Error ("Maximum Swapping Speed %.2lf is Out of Range (0..40)", dvalue);
		}
		max_swap_speed = Round (dvalue);
	} else {
		dvalue = UnRound (max_swap_speed);
	}
	Print (1, "Maximum Swapping Speed = %.1lf meters/second", dvalue);

	if (!speed_method) {
		max_swap_speed = (max_swap_speed + cell_round) / cell_size;
	}

	//---- maximum speed difference ----

	str_ptr = Get_Control_String (MAXIMUM_SPEED_DIFFERENCE);

	if (str_ptr != NULL) {
		Get_Double (str_ptr, &dvalue);

		if (dvalue < 0.0 || dvalue > 10.0) {
			Error ("Maximum Speed Difference %.2lf is Out of Range (0..10)", dvalue);
		}
		max_swap_diff = Round (dvalue);
	} else {
		dvalue = UnRound (max_swap_diff);
	}
	Print (1, "Maximum Speed Difference = %.1lf meters/second", dvalue);

	if (!speed_method) {
		max_swap_diff = (max_swap_diff + cell_round) / cell_size;
	}

	//---- enforce parking lanes ----

	str_ptr = Get_Control_String (ENFORCE_PARKING_LANES);

	if (str_ptr != NULL) {
		parking_flag = Get_Control_Flag (ENFORCE_PARKING_LANES);
	}
	if (parking_flag) {
		Print (1, "Parking Lanes will be Enforced");
	} else {
		Print (1, "Parking Lanes will be Relaxed");
	}

	//---- fix vehicle locations ----

	str_ptr = Get_Control_String (FIX_VEHICLE_LOCATIONS);

	if (str_ptr != NULL) {
		fix_flag = Get_Control_Flag (FIX_VEHICLE_LOCATIONS);

		if (fix_flag) {
			Print (1, "Vehicle Locations will be Fixed if Necessary");
		}
	}

	//---- reaction time ----

	str_ptr = Get_Control_String (DRIVER_REACTION_TIME);

	Print (2, "Driver Reaction Time = ");
	dvalue = reaction_time [0];

	if (str_ptr != NULL) {
		for (i=1; i < MAX_FACILITY_TYPE; i++) {
			str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));
			if (buffer [0] == '\0') break;

			Get_Double (buffer, &dvalue);

			if (dvalue < 0.0 || dvalue > 5.0) {
				Error ("Driver Reaction Time %.2lf is Out of Range (0..5)", dvalue);
			}
			reaction_time [i] = dvalue;
			if (dvalue > 0.0) reaction_flag = true;
			if (i > 1) {
				Print (0, ", %.2lf", dvalue);
			} else {
				Print (0, "%.2lf", dvalue);
			}
		}
	} else {
		i = 1;
		if (dvalue > 0.0) reaction_flag = true;
		Print (0, ".2lf", dvalue);
	}
	for (; i < MAX_FACILITY_TYPE; i++) {
		reaction_time [i] = dvalue;
	}
	Print (0, " seconds");

	//---- permission probability ----

	str_ptr = Get_Control_String (PERMISSION_PROBABILITY);

	Print (1, "Permission Probability = ");
	dvalue = permission_prob [0];
	permit_none = permit_all = true;

	if (str_ptr != NULL) {
		for (i=1; i < MAX_FACILITY_TYPE; i++) {
			str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));
			if (buffer [0] == '\0') break;

			Get_Double (buffer, &dvalue);

			if (dvalue < 0.0 || dvalue > 100.0) {
				Error ("Permission Probability %.1lf is Out of Range (0..100)", dvalue);
			}
			if (i > 1) {
				Print (0, ", %.1lf", dvalue);
			} else {
				Print (0, "%.1lf", dvalue);
			}
			dvalue /= 100.0;
			permission_prob [i] = dvalue;
			if (dvalue != 0.0) permit_none = false;
			if (dvalue != 1.0) permit_all = false;
		}
	} else {
		i = 1;
		Print (0, "%.1lf", dvalue);
		dvalue /= 100.0;
		if (dvalue != 0.0) permit_none = false;
		if (dvalue != 1.0) permit_all = false;
	}
	for (; i < MAX_FACILITY_TYPE; i++) {
		permission_prob [i] = dvalue;
	}
	Print (0, " percent");

	//---- slow down probability ----

	str_ptr = Get_Control_String (SLOW_DOWN_PROBABILITY);

	Print (1, "Slow Down Probability = ");
	dvalue = slow_down_prob [0];

	if (str_ptr != NULL) {
		for (i=1; i < MAX_FACILITY_TYPE; i++) {
			str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));
			if (buffer [0] == '\0') break;

			Get_Double (buffer, &dvalue);

			if (dvalue < 0.0 || dvalue > 100.0) {
				Error ("Slow Down Probability %.1lf is Out of Range (0..100)", dvalue);
			}
			if (i > 1) {
				Print (0, ", %.1lf", dvalue);
			} else {
				Print (0, "%.1lf", dvalue);
			}
			dvalue /= 100.0;
			slow_down_prob [i] = dvalue;
			if (dvalue != 0.0) slow_down_flag = true;
		}
	} else {
		i = 1;
		Print (0, "%.1lf", dvalue);
		dvalue /= 100.0;
		if (dvalue != 0.0) slow_down_flag = true;
	}
	for (; i < MAX_FACILITY_TYPE; i++) {
		slow_down_prob [i] = dvalue;
	}
	Print (0, " percent");
	if (slow_down_flag) {

		//---- slow down percentage ----

		str_ptr = Get_Control_String (SLOW_DOWN_PERCENTAGE);

		lvalue = slow_down_percent [0];

		if (str_ptr != NULL) {
			Print (1, "Slow Down Percentage = ");

			for (i=1; i < MAX_FACILITY_TYPE; i++) {
				str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));
				if (buffer [0] == '\0') break;

				Get_Integer (buffer, &lvalue);

				if (lvalue < 0 || lvalue > 50) {
					Error ("Slow Down Percentage %d is Out of Range (0..50)", lvalue);
				}
				if (i > 1) {
					Print (0, ", %d", lvalue);
				} else {
					Print (0, "%d", lvalue);
				}
				slow_down_percent [i] = lvalue;
			}
			Print (0, " percent");
		} else {
			i = 1;

			if (lvalue > 0) {
				Print (1, "Slow Down Percentage = %d percent", lvalue);
			} else {
				Print (1, "Slow Down by Maximum Deceleration Rate");
			}
		}
		for (; i < MAX_FACILITY_TYPE; i++) {
			slow_down_percent [i] = lvalue;
		}
	}

	if (slow_down_flag || reaction_flag) {

		//---- random number seed ----

		str_ptr = Get_Control_String (RANDOM_NUMBER_SEED);

		if (str_ptr != NULL) {
			random.Seed (atoi (str_ptr));
		}
		Print (1, "Random Number Seed = %d", random.Seed ());
	}

	//---- min waiting time ----

	str_ptr = Get_Control_String (MINIMUM_WAITING_TIME);

	if (str_ptr != NULL) {
		Get_Integer (str_ptr, &lvalue);

		if (lvalue < 0 || lvalue > 6000) {
			Error ("Minimum Waiting Time %d is Out of Range (0..6000)", lvalue);
		}
		min_wait_time = lvalue;
	}

	if (min_wait_time == 0) {
		Print (2, "Minimum Waiting Time is Disabled");
		min_wait_time = MAX_INTEGER;
	} else {
		Print (2, "Minimum Waiting Time = %d second%c", min_wait_time,
			((min_wait_time != 1) ? 's' : ' '));

		min_wait_time *= steps_per_second;
	}

	//---- max waiting time ----

	str_ptr = Get_Control_String (MAXIMUM_WAITING_TIME);

	if (str_ptr != NULL) {
		Get_Integer (str_ptr, &lvalue);

		if (lvalue < 0 || lvalue > 9000) {
			Error ("Maximum Waiting Time %d is Out of Range (0..9000)", lvalue);
		}
		max_wait_time = lvalue;
	}

	if (max_wait_time == 0) {
		Print (1, "Maximum Waiting Time is Disabled");
		max_wait_time = MAX_INTEGER;
	} else {
		Print (1, "Maximum Waiting Time = %d second%c", max_wait_time,
			((max_wait_time != 1) ? 's' : ' '));

		max_wait_time *= steps_per_second;
	}

	//---- max arrival time variance ----

	str_ptr = Get_Control_String (MAX_ARRIVAL_TIME_VARIANCE);

	if (str_ptr != NULL) {
		Get_Integer (str_ptr, &lvalue);

		if (lvalue < 0 || lvalue > 180) {
			Error ("Arrival Time Variance %d is Out of Range (0..180)", lvalue);
		}
		max_end_variance = lvalue;
	}
	Print (1, "Maximum Arrival Time Variance = %d minute%c", max_end_variance,
		((max_end_variance != 1) ? 's' : ' '));

	max_end_variance *= 60 * steps_per_second;

	//---- max departure time variance ----

	str_ptr = Get_Control_String (MAX_DEPARTURE_TIME_VARIANCE);

	if (str_ptr != NULL) {
		Get_Integer (str_ptr, &lvalue);

		if (lvalue < 0 || lvalue > 180) {
			Error ("Departure Time Variance %d is Out of Range (0..180)", lvalue);
		}
		max_start_variance = lvalue;
	}
	Print (1, "Maximum Departure Time Variance = %d minute%c", max_start_variance,
		((max_start_variance != 1) ? 's' : ' '));

	max_start_variance *= 60 * steps_per_second;

	//---- open the problem file ----
	
	str_ptr = Get_Control_String (NEW_PROBLEM_FILE);

	if (str_ptr != NULL) {
		Print (1);
		format_ptr = Get_Control_String (NEW_PROBLEM_FORMAT);

		if (format_ptr != NULL) {
			problem_file.Dbase_Format (format_ptr);
		}
		problem_file.Microsimulator_Flag ();
		problem_file.Create (Project_Filename (str_ptr, Extension ()));
		problem_flag = true;
	}

	//---- max simulation errors ----
	
	str_ptr = Get_Control_String (MAX_SIMULATION_PROBLEMS);

	if (str_ptr != NULL) {
		Get_Integer (str_ptr, &lvalue);

		if (lvalue < 0) {
			Error ("Maximum Simulation Errors %d is Out of Range", lvalue);
		}
		Max_Problems (lvalue);
	}
	if (Max_Problems () > 0) {
		Print (1, "Maximum Number of Simulation Errors = %d", Max_Problems ());
	}

	//---- print error messages ----
	
	error_flag = Get_Control_Flag (PRINT_PROBLEM_MESSAGES);

	if (error_flag) {
		Print (1, "Error Messages will be Printed");
	} else if (Get_Control_String (PRINT_PROBLEM_MESSAGES) != NULL) {
		Print (1, "Error Messages will Not be Printed");
	}

	//---- read the output traveler data ----

	traveler_output.Read_Control ();

	//---- read the output snapshot data ----

	snapshot_output.Read_Control (steps_per_second);

	//---- read the output summary data ----

	summary_output.Read_Control (steps_per_second);

	//---- read the output occupancy data ----

	occupancy_output.Read_Control ();

	//---- read the output problem data ----

	problem_output.Read_Control (steps_per_second);

	//---- read the new system event data ----

	system_event.Read_Control (steps_per_second);

	//---- read the output event data ----

	event_output.Read_Control (steps_per_second);

	//---- read the output turn movement data ----

	turn_output.Read_Control (steps_per_second);

	//---- read the output ridership data ----

	ridership_output.Read_Control (steps_per_second);

	//---- read the output speed bin data ----

	speed_bin_output.Read_Control (steps_per_second, UnRound (cell_size));

	return;

control_error:
	Error ("Missing Control Key = %s", Current_Key ());
} 
