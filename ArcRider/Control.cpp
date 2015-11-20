//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "ArcRider.hpp"

#include "Utility.hpp"
#include "Problem_Service.hpp"
#include "Delay_File.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void ArcRider::Program_Control (void)
{
	int i, nsel, increment;
	char *str_ptr, buffer [20];
	bool z_flag, m_flag;

	//---- open network files ----

	Demand_Service::Program_Control ();

	//---- get the projection data ----

	projection.Read_Control ();
	Print (1);

	//---- get the z coordinate flag ----

	m_flag = projection.M_Flag ();
	z_flag = projection.Z_Flag ();
	points.Z_Flag (z_flag);

	//---- set the load flag ----

	str_ptr = Get_Control_String (RIDERSHIP_SCALING_FACTOR);

	if (str_ptr != NULL) {
		Get_Double (str_ptr, &load_factor);

		load_flag = (load_factor != 0.0);

		if (load_flag) {
			factor_flag = Get_Control_Flag (SET_WIDTH_USING_LOAD_FACTOR);

			if (!factor_flag) {
				runs_flag = Get_Control_Flag (SET_WIDTH_USING_RUNS);
			}
		}
	}

	//---- get the arcview line demand file ----

	str_ptr = Get_Control_String (ARCVIEW_LINE_DEMAND_FILE);

	if (str_ptr != NULL) {
		arcview_line.File_Type ("ArcView Line Demand File");
		arcview_line.File_Access (CREATE);
		arcview_line.Shape_Type (VECTOR);
		arcview_line.Z_Flag (z_flag);
		arcview_line.M_Flag (m_flag);

		if (!arcview_line.Open (Project_Filename (str_ptr))) {
			File_Error ("Opening ArcView Line Demand File", arcview_line.Shape_Filename ());
		}
		line_flag = true;

		arcview_line.Add_Field ("ROUTE", Db_Code::INTEGER, 10);
		arcview_line.Add_Field ("NAME", Db_Code::STRING, 40);
		arcview_line.Add_Field ("MODE", Db_Code::STRING, 16);
		arcview_line.Add_Field ("STOPS", Db_Code::INTEGER, 10);
		arcview_line.Add_Field ("RUNS", Db_Code::INTEGER, 10);
		arcview_line.Add_Field ("RIDERS", Db_Code::INTEGER, 10);
		arcview_line.Add_Field ("MAX_LOAD", Db_Code::INTEGER, 10);
		arcview_line.Add_Field ("MAX_BOARD", Db_Code::INTEGER, 10);
		arcview_line.Add_Field ("MAX_ALIGHT", Db_Code::INTEGER, 10);

		arcview_line.Write_Header ();

		arcview_line.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- get the arcview line group file ----

	str_ptr = Get_Control_String (ARCVIEW_LINE_GROUP_FILE);

	if (str_ptr != NULL) {
		arcview_sum.File_Type ("ArcView Line Group File");
		arcview_sum.File_Access (CREATE);
		arcview_sum.Shape_Type (VECTOR);
		arcview_sum.Z_Flag (z_flag);
		arcview_sum.M_Flag (m_flag);

		if (!arcview_sum.Open (Project_Filename (str_ptr))) {
			File_Error ("Opening ArcView Line Group File", arcview_sum.Shape_Filename ());
		}
		sum_flag = true;

		arcview_sum.Add_Field ("GROUP", Db_Code::INTEGER, 10);
		arcview_sum.Add_Field ("NAME", Db_Code::STRING, 40);
		arcview_sum.Add_Field ("ROUTES", Db_Code::INTEGER, 10);
		arcview_sum.Add_Field ("STOPS", Db_Code::INTEGER, 10);
		arcview_sum.Add_Field ("RUNS", Db_Code::INTEGER, 10);
		arcview_sum.Add_Field ("RIDERS", Db_Code::INTEGER, 10);
		arcview_sum.Add_Field ("MAX_LOAD", Db_Code::INTEGER, 10);
		arcview_sum.Add_Field ("MAX_BOARD", Db_Code::INTEGER, 10);
		arcview_sum.Add_Field ("MAX_ALIGHT", Db_Code::INTEGER, 10);

		arcview_sum.Write_Header ();

		arcview_sum.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- get the arcview ridership file ----

	str_ptr = Get_Control_String (ARCVIEW_RIDERSHIP_FILE);

	if (str_ptr != NULL) {
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
		arcview_rider.Add_Field ("RUNS", Db_Code::INTEGER, 10);
		arcview_rider.Add_Field ("FACTOR", Db_Code::DOUBLE, 10, 2);

		arcview_rider.Write_Header ();

		arcview_rider.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- get the arcview stop demand file ----

	str_ptr = Get_Control_String (ARCVIEW_STOP_DEMAND_FILE);

	if (str_ptr != NULL) {
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

	//---- get the arcview run capacity file ----

	str_ptr = Get_Control_String (ARCVIEW_RUN_CAPACITY_FILE);

	if (str_ptr != NULL) {
		if (!Demand_File_Flag (VEHICLE_TYPE)) {
			Error ("A Vehicle Type File is required for Run Capacity");
		}
		arcview_cap.File_Type ("ArcView Run Capacity File");
		arcview_cap.File_Access (CREATE);
		arcview_cap.Shape_Type (VECTOR);
		arcview_cap.Z_Flag (z_flag);
		arcview_cap.M_Flag (m_flag);

		if (!arcview_cap.Open (Project_Filename (str_ptr))) {
			File_Error ("Opening ArcView Run Capacity File", arcview_cap.Shape_Filename ());
		}
		cap_flag = true;

		arcview_cap.Add_Field ("MODE", Db_Code::STRING, 16);		
		arcview_cap.Add_Field ("ROUTE", Db_Code::INTEGER, 10);
		arcview_cap.Add_Field ("TIME", Db_Code::STRING, TIME_BUFFER);
		arcview_cap.Add_Field ("PERIOD", Db_Code::INTEGER, 4);
		arcview_cap.Add_Field ("AVG_RUNS", INTEGER, 4);
		arcview_cap.Add_Field ("AVG_LOAD", INTEGER, 6);
		arcview_cap.Add_Field ("AVG_FAC", DOUBLE, 6, 2);
		arcview_cap.Add_Field ("CAP_RUNS", INTEGER, 4);
		arcview_cap.Add_Field ("CAP_LOAD", INTEGER, 6);
		arcview_cap.Add_Field ("CAP_FAC", DOUBLE, 6, 2);
		arcview_cap.Add_Field ("MAX_RUN", INTEGER, 4);
		arcview_cap.Add_Field ("MAX_LOAD", INTEGER, 6);
		arcview_cap.Add_Field ("MAX_FAC", DOUBLE, 6, 2);

		arcview_cap.Write_Header ();

		arcview_cap.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- check for output ----

	if (!line_flag && !sum_flag && !rider_flag && !on_off_flag && !cap_flag) {
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

	//---- get select increment ----
	
	str_ptr = Get_Control_String (SELECT_TIME_INCREMENT);

	if (str_ptr != NULL) {
		increment = atol (str_ptr);

		if (increment < 0 || increment > 240) {
			Error ("Select Time Increment %d is Out of Range (0-240)", increment);
		}
		increment *= 60;
	}
	if (increment > 0) {
		times.Period_Flag (true);
	}
	times.Increment (increment);

	//---- get the select time periods ----
	
	str_ptr = Get_Control_String (SELECT_TIME_PERIODS);

	if (str_ptr != NULL) {
		Print (2, "Select Time Periods = %s", str_ptr);

		times.Format (Time_Step::HOURS);

		if (!times.Add_Ranges (str_ptr)) {
			File_Error ("Time Period Range", str_ptr);
		}
	}
	if (increment > 0) {
		Print (1, "Select Time Increment = %d minutes", (increment / 60));
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

	Print (1);

	//---- transit stop offset ----

	str_ptr = Get_Control_String (TRANSIT_STOP_SIDE_OFFSET);

	if (str_ptr != NULL) {
		Get_Double (str_ptr, &stop_side);

		if (stop_side < 0.0 || stop_side > 50.0) {
			Error ("Transit Stop Side Offset %.2lf is Out of Range (0..50)", stop_side);
		}
		Print (1, "Transit Stop Side Offset = %.2lf meters", stop_side);
	}

	//---- transit route offset ----

	if (line_flag || rider_flag) {
		str_ptr = Get_Control_String (TRANSIT_DIRECTION_OFFSET);

		if (str_ptr != NULL) {
			Get_Double (str_ptr, &route_offset);

			if (route_offset < 0.0 || route_offset > 15.0) {
				Error ("Transit Direction Offset %.2lf is Out of Range (0..15)", route_offset);
			}
			Print (1, "Transit Direction Offset = %.2lf meters", route_offset);
		}
	}

	//---- polygon smoothing keys ----

	if (load_flag) {

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

	//---- ridership scaling factor ----

	if (load_flag) {
		if (load_factor < 0.01 || load_factor > 100000.0) {
			Error ("Ridership Scaling Factor %.2lf is Out of Range (0.01..100000.0)", load_factor);
		}
		if (factor_flag) {
			Print (2, "Ridership Scaling Factor = %.2lf riders / run / meter", load_factor);
		} else if (runs_flag) {
			Print (2, "Ridership Scaling Factor = %.2lf runs / meter", load_factor);
		} else {
			Print (2, "Ridership Scaling Factor = %.2lf riders / meter", load_factor);
		}

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

		//---- set width using runs ----

		str_ptr = Get_Control_String (SET_WIDTH_USING_RUNS);

		if (str_ptr != NULL) {
			Print (1, "Set Width Using Runs = %s", str_ptr);
		}

		//---- set width using load factor ----

		str_ptr = Get_Control_String (SET_WIDTH_USING_LOAD_FACTOR);

		if (str_ptr != NULL) {
			Print (1, "Set Width Using Load Factor = %s", str_ptr);
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

	//---- line equivalance ----

	if (line_flag) {

		//---- open the line equivalence ----

		str_ptr = Get_Control_String (LINE_EQUIVALENCE_FILE);

		if (str_ptr == NULL) {
			Error ("A Line Equivalance File is needed for Line Group Output");
		}
		Print (1);
		line_equiv.Open (Project_Filename (str_ptr));

		//---- read the equiv ----

		line_equiv.Read (false);
	}
}
