//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "ArcProblem.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void ArcProblem::Program_Control (void)
{
	int len, increment, type;
	char *str_ptr, *format_ptr, buffer [STRING_BUFFER];
	bool z_flag, m_flag;

	//---- open network files ----

	Demand_Service::Program_Control ();

	//---- get the projection data ----

	projection.Read_Control ();

	//---- get the z coordinate flag ----

	m_flag = projection.M_Flag ();
	z_flag = projection.Z_Flag ();
	points.Z_Flag (z_flag);

	//---- open the problem file ----

	str_ptr = Get_Control_String (PROBLEM_FILE);

	if (str_ptr == NULL) goto control_error;
	Print (1);

	format_ptr = Get_Control_String (PROBLEM_FORMAT);

	if (format_ptr != NULL) {
		problem_file.File_Format (format_ptr);
	}

	if (!problem_file.Open (Project_Filename (str_ptr))) {
		File_Error ("Opening Problem File", problem_file.Filename ());
	}

	//---- get the arcview problem file ----

	str_ptr = Get_Control_String (ARCVIEW_PROBLEM_FILE);

	if (str_ptr == NULL) goto control_error;

	len = (int) strlen (str_ptr);

	if (len > 4) {
		char *ptr = strrchr (str_ptr + len - 4, '.');

		if (ptr != NULL) {
			*ptr = '\0';
		}
	}
	str_cpy (shapename, sizeof (shapename), Project_Filename (str_ptr));

	str_cpy (buffer, sizeof (buffer), shapename);
	str_cat (buffer, sizeof (buffer), "_time_of_day.shp");

	Print_Filename (2, "ArcView Problem File", buffer);

	//---- lane width ----

	str_ptr = Get_Control_String (LANE_WIDTH);

	if (str_ptr != NULL) {
		Get_Double (str_ptr, &lane_width);

		if (lane_width < 0.1 || lane_width > 25.0) {
			Error ("Lane Width %.2lf is Out of Range (0.1..25)", lane_width);
		}
	}
	Print (2, "Lane Width = %.2lf meters", lane_width);

	//---- center oneway links ----

	str_ptr = Get_Control_String (CENTER_ONEWAY_LINKS);

	if (str_ptr != NULL) {
		Print (1, "Center Oneway Links = %s", str_ptr);

		center_flag = Get_Control_Flag (CENTER_ONEWAY_LINKS);
	}

	//---- draw vehicle shapes ----

	str_ptr = Get_Control_String (DRAW_VEHICLE_SHAPES);

	if (str_ptr != NULL) {
		Print (1, "Draw Vehicle Shapes = %s", str_ptr);

		shape_flag = Get_Control_Flag (DRAW_VEHICLE_SHAPES);
	}

	//---- draw trip od links ----

	str_ptr = Get_Control_String (DRAW_TRIP_OD_LINKS);

	if (str_ptr != NULL) {
		Print (1, "Draw Trip OD Links = %s", str_ptr);

		od_flag = Get_Control_Flag (DRAW_TRIP_OD_LINKS);

		if (od_flag && problem_file.Destination_Field () == 0) {
			Error ("The Problem File does not Include OD Information");
		}
	}
	if (shape_flag && od_flag) {
		Error ("Drawing Vehicle Shapes and Trip OD Links are Incompatible");
	}

	//---- get the time of day format ----
	
	str_ptr = Get_Control_String (TIME_OF_DAY_FORMAT);

	if (str_ptr == NULL) {
		str_ptr = "24_HOUR_CLOCK";
	}
	if (!times.Format (str_ptr)) {
		Error ("Time of Day Format %s was Unrecognized", str_ptr);
	}
	Print (2, "Time of Day Format = %s", str_ptr);

	//---- get select time increment ----
	
	str_ptr = Get_Control_String (SELECT_TIME_INCREMENT);

	if (str_ptr != NULL) {
		increment = atol (str_ptr);

		if (increment < 0 || increment > 240) {
			Error ("Summary Time Increment %d is Out of Range (0-240)", increment);
		}
		increment *= 60;
	} else {
		increment = 0;
	}
	times.Increment (increment);
	if (increment > 0) {
		times.Period_Flag (true);
	}

	//---- get the select time periods ----
	
	str_ptr = Get_Control_String (SELECT_TIME_PERIODS);

	if (str_ptr != NULL) {
		time_flag = true;

		if (!times.Add_Ranges (str_ptr)) {
			File_Error ("Time Period Range", str_ptr);
		}
	} else {
		str_ptr = (char *) "All";
	}
	Print (2, "Select Time Periods = %s", str_ptr);

	if (increment > 0) {
		Print (1, "Select Time Increment = %d minutes", increment / 60);
	} else {
		Print (1, "One Time Increment will be Output");
	}

	//---- get the select links ----
	
	str_ptr = Get_Control_String (SELECT_LINKS);

	if (str_ptr != NULL) {
		Print (2, "Select Links = %s", str_ptr);

		if (!links.Add_Ranges (str_ptr)) {
			File_Error ("Link Range", str_ptr);
		}
		link_flag = true;
	}

	//---- get the select problem types ----
	
	str_ptr = Get_Control_String (SELECT_PROBLEM_TYPES);

	if (str_ptr != NULL) {
		Print (2, "Select Problem Types = %s", str_ptr);

		while (str_ptr != NULL) {
			str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));

			if (buffer [0] != '\0') {
				type = Problem_Code (buffer);
				str_fmt (buffer, sizeof (buffer), "%d", type);

				if (!types.Add_Ranges (buffer)) {
					File_Error ("Problem Types", str_ptr);
				}
				type_flag = true;
			}
		}
	}

	//---- get the select subarea polygon ----

	str_ptr = Get_Control_String (SELECT_SUBAREA_POLYGON);

	if (str_ptr != NULL) {
		Print (1);
		select_subarea.File_Type ("Select Subarea Polygon");

		select_subarea.Open (Project_Filename (str_ptr));
		
		if (!select_subarea.Read_Record ()) {
			Error ("Reading %s", select_subarea.File_Type ());
		}
		subarea_flag = true;
	}

	//---- check for location information ----

	if (problem_file.Link_Field () == 0) {
		if (!Network_File_Flag (ACTIVITY_LOCATION)) {
			Error ("An Activity Location file is Required");
		} else {
			location_flag = true;
		}
	}

	//---- initialize the shape file ----

	arcview_problem.File_Type ("ArcView Problem File");
	arcview_problem.File_Access (CREATE);
	arcview_problem.Shape_Type ((od_flag) ? VECTOR : ((shape_flag) ? POLYGON : DOT));
	arcview_problem.Z_Flag (z_flag);
	arcview_problem.M_Flag (m_flag);

	arcview_problem.Replicate_Fields (&problem_file);
	arcview_problem.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());

	return;

control_error:
	Error ("Missing Control Key = %s", Current_Key ());
}
