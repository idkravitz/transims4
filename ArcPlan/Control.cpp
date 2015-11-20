//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "ArcPlan.hpp"

#include "Utility.hpp"
#include "Problem_Service.hpp"
#include "Delay_File.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void ArcPlan::Program_Control (void)
{
	int i, nsel, increment, index;
	char *str_ptr, buffer [20];
	bool flag, z_flag, m_flag;

	//---- open network files ----

	Demand_Service::Program_Control ();

	//---- set the transit options ----

	stop_flag = Network_File_Flag (TRANSIT_STOP);
	route_flag = Network_File_Flag (TRANSIT_ROUTE);
	driver_flag = Network_File_Flag (TRANSIT_DRIVER);

	if ((route_flag || driver_flag) && !stop_flag) {
		Error ("Transit Stops are required for Transit Route Processing");
	}

	//---- get the projection data ----

	projection.Read_Control ();

	//---- get the z coordinate flag ----

	m_flag = projection.M_Flag ();
	z_flag = projection.Z_Flag ();
	points.Z_Flag (z_flag);

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

	//---- open the household list file ----
	
	str_ptr = Get_Control_String (HOUSEHOLD_LIST);

	if (str_ptr != NULL) {
		hhlist_file.File_Type ("Household List File");
						
		str_ptr = Project_Filename (str_ptr);

		if (Partition ()) {
			hhlist_file.Filename (str_ptr, Partition_Number ());
		} else {
			hhlist_file.Filename (str_ptr);
		}
		Print_Filename (2, hhlist_file.File_Type (), hhlist_file.Groupname ());

		if (!hhlist_file.Open (0)) {
			File_Error (hhlist_file.File_Type (), hhlist_file.Filename ());
		}
		hhlist_flag = true;
	}

	//---- open the problem file ----

	str_ptr = Get_Control_String (PROBLEM_FILE);

	if (str_ptr != NULL) {
		Print (1);
		str_ptr = Get_Control_String (PROBLEM_FORMAT);

		if (str_ptr != NULL) {
			problem_file.File_Format (str_ptr);
		}
		str_ptr = Get_Control_String (PROBLEM_FILE);

		problem_file.Open (Project_Filename (str_ptr));
		problem_flag = true;
	}

	Print (1);

	//---- get the arcview plan file ----

	str_ptr = Get_Control_String (ARCVIEW_PLAN_FILE);

	if (str_ptr != NULL) {
		arcview_plan.File_Type ("ArcView Plan File");
		arcview_plan.File_Access (CREATE);
		arcview_plan.Shape_Type (VECTOR);
		arcview_plan.Z_Flag (z_flag);
		arcview_plan.M_Flag (m_flag);

		if (!arcview_plan.Open (Project_Filename (str_ptr))) {
			File_Error ("Opening ArcView Plan File", arcview_plan.Shape_Filename ());
		}
		plan_flag = true;

		arcview_plan.Add_Field ("TRAVELER", Db_Code::INTEGER, 10);
		arcview_plan.Add_Field ("TRIP", Db_Code::INTEGER, 5);
		arcview_plan.Add_Field ("LEG", Db_Code::INTEGER, 5);
		arcview_plan.Add_Field ("TIME", Db_Code::STRING, 14);
		arcview_plan.Add_Field ("DURATION", Db_Code::STRING, 14);
		arcview_plan.Add_Field ("COST", Db_Code::INTEGER, 5);
		arcview_plan.Add_Field ("IMPEDANCE", Db_Code::INTEGER, 10);
		arcview_plan.Add_Field ("MODE", Db_Code::STRING, 12);
		arcview_plan.Add_Field ("MODE_ID", Db_Code::INTEGER, 10);

		arcview_plan.Write_Header ();

		arcview_plan.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());

		traveler_field = arcview_plan.Field_Number ("TRAVELER");
		trip_field = arcview_plan.Field_Number ("TRIP");
		leg_field = arcview_plan.Field_Number ("LEG");
		time_field = arcview_plan.Field_Number ("TIME");
		duration_field = arcview_plan.Field_Number ("DURATION");
		cost_field = arcview_plan.Field_Number ("COST");
		impedance_field = arcview_plan.Field_Number ("IMPEDANCE");
		mode_field = arcview_plan.Field_Number ("MODE");
		mode_id_field = arcview_plan.Field_Number ("MODE_ID");
	}

	//---- open the arcview problem file ----

	if (problem_flag) {
		str_ptr = Get_Control_String (ARCVIEW_PROBLEM_FILE);

		if (str_ptr != NULL) {
			arcview_problem.File_Type ("ArcView Problem File");
			arcview_problem.File_Access (CREATE);
			arcview_problem.Shape_Type (DOT);
			//arcview_problem.Shape_Type (POLYGON);
			arcview_problem.Z_Flag (z_flag);
			arcview_problem.M_Flag (m_flag);

			if (!arcview_problem.Open (Project_Filename (str_ptr))) {
				File_Error ("Opening ArcView Problem File", arcview_problem.Shape_Filename ());
			}
			arcview_problem.Replicate_Fields (&problem_file);

			arcview_problem.Write_Header ();

			arcview_problem.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
			problem_out = true;
		}
	}

	//---- get the arcview bandwidth file ----

	str_ptr = Get_Control_String (ARCVIEW_BANDWIDTH_FILE);

	if (str_ptr != NULL) {
		arcview_width.File_Type ("ArcView Bandwidth File");
		arcview_width.File_Access (CREATE);
		arcview_width.Shape_Type (POLYGON);
		arcview_width.Z_Flag (z_flag);
		arcview_width.M_Flag (m_flag);

		if (!arcview_width.Open (Project_Filename (str_ptr))) {
			File_Error ("Opening ArcView Bandwidth File", arcview_width.Shape_Filename ());
		}
		width_flag = true;

		arcview_width.Add_Field ("LINK", Db_Code::INTEGER, 10);
		arcview_width.Add_Field ("DIR", Db_Code::INTEGER, 1);
		arcview_width.Add_Field ("VOLUME", Db_Code::INTEGER, 10);

		arcview_width.Write_Header ();

		arcview_width.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- get the arcview time contour file ----

	str_ptr = Get_Control_String (ARCVIEW_TIME_CONTOUR);

	if (str_ptr != NULL) {
		arcview_time.File_Type ("ArcView Time Contour File");
		arcview_time.File_Access (CREATE);
		arcview_time.Shape_Type (VECTOR);
		arcview_time.Z_Flag (z_flag);
		arcview_time.M_Flag (m_flag);

		if (!arcview_time.Open (Project_Filename (str_ptr))) {
			File_Error ("Opening ArcView Time Contour File", arcview_time.Shape_Filename ());
		}
		time_flag = true;

		arcview_time.Add_Field ("CONTOUR", Db_Code::INTEGER, 10);
		arcview_time.Add_Field ("TIME", Db_Code::INTEGER, 10);

		arcview_time.Write_Header ();

		arcview_time.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- get the arcview distance contour file ----

	str_ptr = Get_Control_String (ARCVIEW_DISTANCE_CONTOUR);

	if (str_ptr != NULL) {
		arcview_distance.File_Type ("ArcView Distance Contour File");
		arcview_distance.File_Access (CREATE);
		arcview_distance.Shape_Type (VECTOR);
		arcview_distance.Z_Flag (z_flag);
		arcview_distance.M_Flag (m_flag);

		if (!arcview_distance.Open (Project_Filename (str_ptr))) {
			File_Error ("Opening ArcView Distance Contour File", arcview_distance.Shape_Filename ());
		}
		distance_flag = true;

		arcview_distance.Add_Field ("CONTOUR", Db_Code::INTEGER, 10);
		arcview_distance.Add_Field ("DISTANCE", Db_Code::INTEGER, 10);

		arcview_distance.Write_Header ();

		arcview_distance.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- get the arcview accessibilityr file ----

	str_ptr = Get_Control_String (ARCVIEW_ACCESSIBILITY_FILE);

	if (str_ptr != NULL) {
		arcview_access.File_Type ("ArcView Accessibility File");
		arcview_access.File_Access (CREATE);
		arcview_access.Shape_Type (DOT);
		arcview_access.Z_Flag (z_flag);
		arcview_access.M_Flag (m_flag);

		if (!arcview_access.Open (Project_Filename (str_ptr))) {
			File_Error ("Opening ArcView Accessibility File", arcview_access.Shape_Filename ());
		}
		access_flag = true;

		arcview_access.Add_Field ("ORIGIN", Db_Code::INTEGER, 10);
		arcview_access.Add_Field ("START", Db_Code::STRING, 20);
		arcview_access.Add_Field ("LOCATION", Db_Code::INTEGER, 10);
		arcview_access.Add_Field ("TTIME", Db_Code::INTEGER, 10);
		arcview_access.Add_Field ("DISTANCE", Db_Code::INTEGER, 10);

		arcview_access.Write_Header ();

		arcview_access.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- set the load flag ----

	str_ptr = Get_Control_String (RIDERSHIP_SCALING_FACTOR);

	if (str_ptr != NULL) {
		Get_Double (str_ptr, &load_factor);

		load_flag = (load_factor != 0.0);
	}

	//---- get the arcview ridership file ----

	str_ptr = Get_Control_String (ARCVIEW_RIDERSHIP_FILE);

	if (str_ptr != NULL) {
		if (!stop_flag || !route_flag || !driver_flag) {
			Error ("Transit Data are need for Ridership Output");
		}
		arcview_rider.File_Type ("ArcView Ridership File");
		arcview_rider.File_Access (CREATE);

		if (load_flag) {
			arcview_rider.Shape_Type (POLYGON);
		} else {
			arcview_rider.Shape_Type (VECTOR);
		}
		arcview_rider.Z_Flag (z_flag);
		arcview_rider.M_Flag (m_flag);

		if (!arcview_rider.Open (Project_Filename (str_ptr))) {
			File_Error ("Opening ArcView Ridership File", arcview_rider.Shape_Filename ());
		}
		rider_flag = true;

		arcview_rider.Add_Field ("LINK", Db_Code::INTEGER, 10);
		arcview_rider.Add_Field ("DIR", Db_Code::INTEGER, 1);
		arcview_rider.Add_Field ("SEGMENT", Db_Code::INTEGER, 2);
		arcview_rider.Add_Field ("BOARD", Db_Code::INTEGER, 10);
		arcview_rider.Add_Field ("RIDERS", Db_Code::INTEGER, 10);
		arcview_rider.Add_Field ("ALIGHT", Db_Code::INTEGER, 10);

		arcview_rider.Write_Header ();

		arcview_rider.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- get the arcview stop demand file ----

	str_ptr = Get_Control_String (ARCVIEW_STOP_DEMAND_FILE);

	if (str_ptr != NULL) {
		if (!stop_flag) {
			Error ("Transit Stops are need for Stop Demand Output");
		}
		arcview_stop.File_Type ("ArcView Stop Demand File");
		arcview_stop.File_Access (CREATE);
		arcview_stop.Shape_Type (DOT);
		arcview_stop.Z_Flag (z_flag);
		arcview_stop.M_Flag (m_flag);

		if (!arcview_stop.Open (Project_Filename (str_ptr))) {
			File_Error ("Opening ArcView Stop Demand File", arcview_stop.Shape_Filename ());
		}
		on_off_flag = demand_flag = true;

		arcview_stop.Add_Field ("STOP", Db_Code::INTEGER, 10);
		arcview_stop.Add_Field ("BOARD", Db_Code::INTEGER, 10);
		arcview_stop.Add_Field ("ALIGHT", Db_Code::INTEGER, 10);
		arcview_stop.Add_Field ("TOTAL", Db_Code::INTEGER, 10);

		arcview_stop.Write_Header ();

		arcview_stop.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- get the arcview stop group file ----

	str_ptr = Get_Control_String (ARCVIEW_STOP_GROUP_FILE);

	if (str_ptr != NULL) {
		if (!stop_flag) {
			Error ("Transit Stops are need for Stop Group Output");
		}
		arcview_group.File_Type ("ArcView Stop Group File");
		arcview_group.File_Access (CREATE);
		arcview_group.Shape_Type (DOT);
		arcview_group.Z_Flag (z_flag);
		arcview_group.M_Flag (m_flag);

		if (!arcview_group.Open (Project_Filename (str_ptr))) {
			File_Error ("Opening ArcView Stop Group File", arcview_group.Shape_Filename ());
		}
		on_off_flag = group_flag = true;

		arcview_group.Add_Field ("GROUP", Db_Code::INTEGER, 10);
		arcview_group.Add_Field ("NAME", Db_Code::STRING, 40);
		arcview_group.Add_Field ("BOARD", Db_Code::INTEGER, 10);
		arcview_group.Add_Field ("ALIGHT", Db_Code::INTEGER, 10);
		arcview_group.Add_Field ("TOTAL", Db_Code::INTEGER, 10);

		arcview_group.Write_Header ();

		arcview_group.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- get the arcview parking demand file ----

	str_ptr = Get_Control_String (ARCVIEW_PARKING_DEMAND_FILE);

	if (str_ptr != NULL) {
		arcview_parking.File_Type ("ArcView Parking Demand File");
		arcview_parking.File_Access (CREATE);
		arcview_parking.Shape_Type (DOT);
		arcview_parking.Z_Flag (z_flag);
		arcview_parking.M_Flag (m_flag);

		if (!arcview_parking.Open (Project_Filename (str_ptr))) {
			File_Error ("Opening ArcView Parking Demand File", arcview_parking.Shape_Filename ());
		}
		parking_flag = true;

		arcview_parking.Add_Field ("PARKING", Db_Code::INTEGER, 10);
		arcview_parking.Add_Field ("DEPART", Db_Code::INTEGER, 10);
		arcview_parking.Add_Field ("ARRIVE", Db_Code::INTEGER, 10);
		arcview_parking.Add_Field ("TOTAL", Db_Code::INTEGER, 10);

		arcview_parking.Write_Header ();

		arcview_parking.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- check for output ----

	if (!plan_flag && !problem_flag && !width_flag && !time_flag && !distance_flag && !access_flag &&
		!rider_flag && !on_off_flag && !parking_flag) {
		Error ("An Output ArcView File was Not Specified");
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

	//---- get the select travelers ----
	
	str_ptr = Get_Control_String (SELECT_TRAVELERS);

	if (str_ptr != NULL) {
		Print (2, "Select Traveler IDs = %s", str_ptr);

		if (!travelers.Add_Ranges (str_ptr)) {
			File_Error ("Traveler ID Range", str_ptr);
		}
	}
	
	//---- get the select nodes ----

	nsel = Highest_Control_Group (SELECT_NODES_x, 0);

	if (nsel > 0) {

		nodes.Initialize (sizeof (Data_Range), nsel);

		for (i=1; i <= nsel; i++) {
			str_ptr = Get_Control_String (SELECT_NODES_x, i);

			if (str_ptr == NULL) continue;

			Print (2, "Select Nodes for Group #%d = %s", i, str_ptr);

			Data_Range *ptr = new Data_Range ();

			if (!ptr->Add_Ranges (str_ptr)) {
				File_Error ("Path Node Numbers", str_ptr);
			}
			if (!nodes.Add (ptr)) {
				Error ("Adding Node Ranges to the List");
			}
		}
	}
	
	//---- get the select links ----

	nsel = Highest_Control_Group (SELECT_LINKS_x, 0);

	if (nsel > 0) {

		links.Initialize (sizeof (Data_Range), nsel);

		for (i=1; i <= nsel; i++) {
			str_ptr = Get_Control_String (SELECT_LINKS_x, i);

			if (str_ptr == NULL) continue;

			Print (2, "Select Links for Group #%d = %s", i, str_ptr);

			Data_Range *ptr = new Data_Range ();

			if (!ptr->Add_Ranges (str_ptr)) {
				File_Error ("Path Link Numbers", str_ptr);
			}
			if (!links.Add (ptr)) {
				Error ("Adding Link Ranges to the List");
			}
		}
	}

	//---- get the select time periods ----
	
	str_ptr = Get_Control_String (SELECT_TIME_PERIODS);

	if (str_ptr != NULL) {
		Print (2, "Select Time Periods = %s", str_ptr);

		times.Format (Time_Step::HOURS);

		if (!times.Add_Ranges (str_ptr)) {
			File_Error ("Time Period Range", str_ptr);
		}
	}

	//---- get the select locations ----
	
	str_ptr = Get_Control_String (SELECT_LOCATIONS);

	if (str_ptr != NULL) {
		Print (2, "Select Activity Locations = %s", str_ptr);

		if (!locations.Add_Ranges (str_ptr)) {
			File_Error ("Activity Location Range", str_ptr);
		}
	}

	//---- get the select parking lots ----
	
	str_ptr = Get_Control_String (SELECT_PARKING_LOTS);

	if (str_ptr != NULL) {
		Print (2, "Select Parking Lots = %s", str_ptr);

		if (!parking_lots.Add_Ranges (str_ptr)) {
			File_Error ("Parking Lot Range", str_ptr);
		}
	}

	//---- get the select transit stops ----
	
	str_ptr = Get_Control_String (SELECT_TRANSIT_STOPS);

	if (str_ptr != NULL) {
		Print (2, "Select Transit Stops = %s", str_ptr);

		if (!transit_stops.Add_Ranges (str_ptr)) {
			File_Error ("Transit Stop Range", str_ptr);
		}
	}

	//---- get the select transit routes ----
	
	str_ptr = Get_Control_String (SELECT_TRANSIT_ROUTES);

	if (str_ptr != NULL) {
		Print (2, "Select Transit Routes = %s", str_ptr);

		if (!transit_routes.Add_Ranges (str_ptr)) {
			File_Error ("Transit Route Range", str_ptr);
		}
	}

	//---- get the select transit modes ----
	
	str_ptr = Get_Control_String (SELECT_TRANSIT_MODES);

	if (str_ptr != NULL) {
		if (!route_flag) {
			Error ("Transit Routes are Needed to Select Modes");
		}
		Print (2, "Select Transit Modes = %s", str_ptr);

		mode_flag = true;
		memset (travel_modes, '\0', (REGIONRAIL+1) * sizeof (bool));

		while (str_ptr != NULL) {
			str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));

			if (buffer [0] != '\0') {
				travel_modes [Transit_Code (buffer)] = true;
			}
		}
	}

	//---- get the select problem types ----

	if (problem_flag) {
		Problem_Service problem;
		char buffer [STRING_BUFFER];

		max_problem = problem.MAX_PROBLEM;

		problem_type = new bool [max_problem];

		memset (problem_type, '\0', max_problem * sizeof (bool));

		str_ptr = Get_Control_String (SELECT_PROBLEM_TYPES);

		if (str_ptr != NULL) {
			Print (2, "Select Problem Types = %s", str_ptr);

			while (str_ptr != NULL) {
				str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));

				if (buffer [0] != '\0') {
					problem_type [problem.Problem_Code (buffer)] = true;
				}
			}
		} else {
			for (i=0; i < max_problem; i++) {
				problem_type [i] = true;
			}
		}
	}

	//---- get the select random percentage ----
	
	str_ptr = Get_Control_String (SELECT_RANDOM_PERCENTAGE);

	if (str_ptr != NULL) {
		Print (2, "Select Random Percentage = %s", str_ptr);

		random_flag = true;

		Get_Double (str_ptr, &random_share);

		if (random_share < 0.01 || random_share > 100.0) {
			Error ("Random Percentage %.2lf is Out of Range (0.01..100.0)", random_share);
		}
		random_share /= 100.0;

		//---- random number seed ----

		str_ptr = Get_Control_String (RANDOM_NUMBER_SEED);

		if (str_ptr != NULL) {
			random.Seed (atoi (str_ptr));
		}
		Print (1, "Random Number Seed = %d", random.Seed ());
	}

	//---- get offset parameters ----

	Print (1);
	str_ptr = Get_Control_String (LINK_DIRECTION_OFFSET);

	if (str_ptr != NULL) {
		Get_Double (str_ptr, &link_offset);

		if (link_offset < 0.0 || link_offset > 15.0) {
			Error ("Link Direction Offset %.2lf is Out of Range (0..15)", link_offset);
		}
		Print (1, "Link Direction Offset = %.2lf meters", link_offset);
	}

	//---- activity location offset ----

	str_ptr = Get_Control_String (ACTIVITY_LOCATION_SIDE_OFFSET);

	if (str_ptr != NULL) {
		Get_Double (str_ptr, &location_side);

		if (location_side < 0.0 || location_side > 100.0) {
			Error ("Activity Location Side Offset %.2lf is Out of Range (0..100)", location_side);
		}
		Print (1, "Activity Location Side Offset = %.2lf meters", location_side);
	}

	//---- parking offset ----

	str_ptr = Get_Control_String (PARKING_SIDE_OFFSET);

	if (str_ptr != NULL) {
		Get_Double (str_ptr, &parking_side);

		if (parking_side < 0.0 || parking_side > 50.0) {
			Error ("Parking Side Offset %.2lf is Out of Range (0..50)", parking_side);
		}
		Print (1, "Parking Side Offset = %.2lf meters", parking_side);
	}

	//---- transit stop offset ----

	if (stop_flag) {
		str_ptr = Get_Control_String (TRANSIT_STOP_SIDE_OFFSET);

		if (str_ptr != NULL) {
			Get_Double (str_ptr, &stop_side);

			if (stop_side < 0.0 || stop_side > 50.0) {
				Error ("Transit Stop Side Offset %.2lf is Out of Range (0..50)", stop_side);
			}
			Print (1, "Transit Stop Side Offset = %.2lf meters", stop_side);
		}
	}

	//---- transit route offset ----

	if (route_flag || driver_flag) {
		str_ptr = Get_Control_String (TRANSIT_DIRECTION_OFFSET);

		if (str_ptr != NULL) {
			Get_Double (str_ptr, &route_offset);

			if (route_offset < 0.0 || route_offset > 15.0) {
				Error ("Transit Direction Offset %.2lf is Out of Range (0..15)", route_offset);
			}
			Print (1, "Transit Direction Offset = %.2lf meters", route_offset);
		}
	}

	//---- bandwidth scaling factor ----

	if (width_flag) {
		str_ptr = Get_Control_String (BANDWIDTH_SCALING_FACTOR);

		if (str_ptr != NULL) {
			Get_Double (str_ptr, &width_factor);

			if (width_factor < 0.01 || width_factor > 100000.0) {
				Error ("Bandwidth Scaling Factor %.2lf is Out of Range (0.01..100000.0)", width_factor);
			}
		}
		Print (2, "Bandwidth Scaling Factor = %.2lf trips / meter", width_factor);

		//---- minimum bandwidth value ----

		str_ptr = Get_Control_String (MINIMUM_BANDWIDTH_VALUE);

		if (str_ptr != NULL) {
			Get_Integer (str_ptr, &min_value);

			if (min_value < 0 || min_value > 100000) {
				Error ("Minimum Bandwidth Value %d is Out of Range (0..100000)", min_value);
			}
			Print (1, "Minimum Bandwidth Value = %d trips", min_value);
		}

		//---- minimum bandwidth size ----

		str_ptr = Get_Control_String (MINIMUM_BANDWIDTH_SIZE);

		if (str_ptr != NULL) {
			Get_Double (str_ptr, &min_width);

			if (min_width < 0.001 || min_width > 10.0) {
				Error ("Minimum Bandwidth Size %.3lf meters is Out of Range (0.001..10.0)", min_width);
			}
			Print (1, "Minimum Bandwidth Size = %.3lf meters", min_width);
		}

		//---- maximum bandwidth size ----

		str_ptr = Get_Control_String (MAXIMUM_BANDWIDTH_SIZE);

		if (str_ptr != NULL) {
			Get_Double (str_ptr, &max_width);

			if (max_width < 1.0 || max_width > 10000.0) {
				Error ("Maximum Bandwidth Size %.2lf meters is Out of Range (1.0..10000.0)", max_width);
			}
			Print (1, "Maximum Bandwidth Size = %.2lf meters", max_width);
		}
	}

	//---- polygon smoothing keys ----

	if (width_flag || load_flag) {

		//---- maximum shape angle ----

		str_ptr = Get_Control_String (MAXIMUM_SHAPE_ANGLE);

		if (str_ptr != NULL) {
			max_angle = atoi (str_ptr);

			if (max_angle != 0 && (max_angle < 5 || max_angle > 120)) {
				Error ("Maximum Shape Angle %d is Out of Range (5..120)", max_angle);
			}
		}
		if (max_angle > 0) {
			Print (1, "Maximum Shape Angle = %d degrees", max_angle);
		} else {
			Print (1, "Shape Smoothing is Disabled");
		}

		//---- minimum shape length ----

		str_ptr = Get_Control_String (MINIMUM_SHAPE_LENGTH);

		if (str_ptr != NULL) {
			min_length = atoi (str_ptr);

			if (min_length != 0 && (min_length < 1 || min_length > 50)) {
				Error ("Minimum Shape Length %d is Out of Range (1..50)", min_length);
			}
		}
		if (min_length > 0) {
			Print (1, "Minimum Shape Length = %d meters", min_length);
		} else if (max_angle != 0) {
			Print (1, "Shape Smoothing is Disabled");
		}
	}

	//---- time contour increments ----

	if (time_flag) {

		//---- get the time increments ----

		str_ptr = Get_Control_String (CONTOUR_TIME_INCREMENTS);
		flag = false;

		while (str_ptr != NULL) {
			str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));

			if (buffer [0] == '*') {
				flag = true;
				index = time_points.Num_Records ();
				increment = atoi (buffer + 1);
			} else {
				increment = atoi (buffer);
			}
			if (increment < 1 || increment > MIDNIGHT) {
				Error ("Time Increment %d is Out of Range (1..%d)", increment, MIDNIGHT);
			}
			if (flag) {
				for (i=time_points.Last_Key () + increment; i < MIDNIGHT; i += increment) {
					if (!time_points.Add (i)) goto time_error;
				}
				break;
			} else {
				if (!time_points.Add (increment)) goto time_error;
			}
		}
		if (time_points.Num_Records () == 0) {
			Error ("Time Increments are Needed for Time Contours");
		}

		//---- print the time increments ----

		Print (2, "Contour Time Increments =");

		nsel = 0;

		for (i=time_points.First_Key (); i > 0; i = time_points.Next_Key (), nsel++) {
			Print (0, " %d", i);
			if (flag && nsel == index + 2) {
				Print (0, "...");
				break;
			}
		}
		Print (0, " seconds");

		//---- check for a link delay file ----

		if (Demand_File_Flag (LINK_DELAY)) {
			Delay_File *file = (Delay_File *) Demand_Db_File (LINK_DELAY);

			if (file->Dbase_Format () == VERSION3) {
				turn_flag = false;
				Network_File_False (LANE_CONNECTIVITY);
			} else if (!Network_File_Flag (LANE_CONNECTIVITY)) {
				Warning ("Lane Connectivity is required for Turning Delays");
				Show_Message (1);
				turn_flag = false;
			} else {
				turn_flag = true;
			}
		} else {
			Network_File_False (LANE_CONNECTIVITY);
		}
	} else {
		Network_File_False (LANE_CONNECTIVITY);
	}

	//---- distance contour increments ----

	if (distance_flag) {

		//---- get the distance increments ----

		str_ptr = Get_Control_String (CONTOUR_DISTANCE_INCREMENTS);
		flag = false;

		while (str_ptr != NULL) {
			str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));

			if (buffer [0] == '*') {
				flag = true;
				index = distance_points.Num_Records ();
				increment = atoi (buffer + 1);
			} else {
				increment = atoi (buffer);
			}
			if (increment < 100 || increment > 1000000) {
				Error ("Distance Increment %d is Out of Range (100..1000000)", increment);
			}
			if (flag) {
				for (i=distance_points.Last_Key () + increment; i < 1000000; i += increment) {
					if (!distance_points.Add (i)) goto distance_error;
				}
				break;
			} else {
				if (!distance_points.Add (increment)) goto distance_error;
			}
		}
		if (distance_points.Num_Records () == 0) {
			Error ("Distance Increments are Needed for Distance Contours");
		}

		//---- print the distance increments ----

		Print (2, "Contour Distance Increments =");

		nsel = 0;

		for (i=distance_points.First_Key (); i > 0; i = distance_points.Next_Key (), nsel++) {
			Print (0, " %d", i);
			if (flag && nsel == index + 2) {
				Print (0, "...");
				break;
			}
		}
		Print (0, " meters");
	}

	//---- ridership scaling factor ----

	if (load_flag) {
		if (load_factor < 0.01 || load_factor > 100000.0) {
			Error ("Ridership Scaling Factor %.2lf is Out of Range (0.01..100000.0)", load_factor);
		}
		Print (2, "Ridership Scaling Factor = %.2lf riders / meter", load_factor);

		//---- minimum ridership value ----

		str_ptr = Get_Control_String (MINIMUM_RIDERSHIP_VALUE);

		if (str_ptr != NULL) {
			Get_Integer (str_ptr, &min_load);

			if (min_load < 0 || min_load > 100000) {
				Error ("Minimum Ridership Value %d is Out of Range (0..100000)", min_load);
			}
			Print (1, "Minimum Ridership Value = %d trips", min_load);
		}

		//---- minimum ridership size ----

		str_ptr = Get_Control_String (MINIMUM_RIDERSHIP_SIZE);

		if (str_ptr != NULL) {
			Get_Double (str_ptr, &min_rider);

			if (min_rider < 0.001 || min_rider > 10.0) {
				Error ("Minimum Ridership Size %.3lf meters is Out of Range (0.001..10.0)", min_rider);
			}
			Print (1, "Minimum Ridership Size = %.3lf meters", min_rider);
		}

		//---- maximum ridership size ----

		str_ptr = Get_Control_String (MAXIMUM_RIDERSHIP_SIZE);

		if (str_ptr != NULL) {
			Get_Double (str_ptr, &max_rider);

			if (max_rider < 1.0 || max_rider > 10000.0) {
				Error ("Maximum Ridership Size %.2lf meters is Out of Range (1.0..10000.0)", max_rider);
			}
			Print (1, "Maximum Ridership Size = %.2lf meters", max_rider);
		}
	} else if (rider_flag) {
		Print (2, "Ridership Scaling Factor = Zero (Vector-Based)");
	}

	//---- stop equivalance ----

	if (group_flag) {

		//---- open the stop equivalence ----

		str_ptr = Get_Control_String (STOP_EQUIVALENCE_FILE);

		if (str_ptr == NULL) {
			Error ("A Stop Equivalance File is needed for Stop Group Output");
		}
		Print (1);
		stop_equiv.Open (Project_Filename (str_ptr));

		//---- read the equiv ----

		stop_equiv.Read (false);
	}
	return;
	
control_error:
	Error ("Missing Control Key = %s", Current_Key ());

time_error:
	Error ("Adding Time Increment");

distance_error:
	Error ("Adding Distance Increment");
}
