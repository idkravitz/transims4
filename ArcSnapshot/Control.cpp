//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "ArcSnapshot.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void ArcSnapshot::Program_Control (void)
{
	int i, len, increment;
	char *str_ptr, *format_ptr, buffer [STRING_BUFFER];
	bool z_flag, m_flag;

	char *method_text [] = { "AT_INCREMENT", "TOTAL", "MAXIMUM", NULL };

	//---- open network files ----

	Demand_Service::Program_Control ();

	//---- get the projection data ----

	project_flag = projection.Read_Control ();

	//---- get the z coordinate flag ----

	m_flag = projection.M_Flag ();
	z_flag = projection.Z_Flag ();
	points.Z_Flag (z_flag);

	//---- open the snapshot file ----

	str_ptr = Get_Control_String (SNAPSHOT_FILE);
	Print (1);

	if (str_ptr != NULL) {
		format_ptr = Get_Control_String (SNAPSHOT_FORMAT);

		if (format_ptr != NULL) {
			snapshot_file.Dbase_Format (format_ptr);
		}
		if (!snapshot_file.Open (Project_Filename (str_ptr))) {
			File_Error ("Opening Snapshot File", snapshot_file.Filename ());
		}
	} else {
		str_ptr = Get_Control_String (OCCUPANCY_FILE);

		if (str_ptr == NULL) {
			Error ("A Input Snapshot or Occupancy file are Required");
		}
		format_ptr = Get_Control_String (OCCUPANCY_FORMAT);

		if (format_ptr != NULL) {
			occupancy_file.Dbase_Format (format_ptr);
		}
		if (!occupancy_file.Open (Project_Filename (str_ptr))) {
			File_Error ("Opening Occupancy File", occupancy_file.Filename ());
		}
		occupancy_flag = true;
	}

	//---- get the arcview snapshot file ----

	str_ptr = Get_Control_String (ARCVIEW_SNAPSHOT_FILE);
	
	if (str_ptr != NULL) {
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

		Print_Filename (2, "ArcView Snapshot File", buffer);
		snapshot_flag = true;
	}

	//---- new snapshot file ----

	str_ptr = Get_Control_String (NEW_SNAPSHOT_FILE);

	if (str_ptr != NULL) {
		Print (1);
		format_ptr = Get_Control_String (NEW_SNAPSHOT_FORMAT);

		if (format_ptr != NULL) {
			new_snapshot_file.Dbase_Format (format_ptr);
		}
		new_snapshot_file.Location_Flag (true);
		output_flag = true;

		if (!new_snapshot_file.Create (Project_Filename (str_ptr))) {
			Error ("Creating %s", new_snapshot_file.File_Type ());
		}
		if (project_flag) {
			project_coord.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
		}
	}

	//---- new link summary file ----

	str_ptr = Get_Control_String (NEW_LINK_SUMMARY_FILE);

	if (str_ptr != NULL) {
		if (occupancy_flag) {
			Error ("New Link Summary File requires a Snapshot File");
		}
		Print (1);
		summary_file.File_Type ("Link Summary File");

		format_ptr = Get_Control_String (NEW_LINK_SUMMARY_FORMAT);

		if (format_ptr != NULL) {
			summary_file.Dbase_Format (format_ptr);
		}
		sum_flag = true;

		if (!summary_file.Create (Project_Filename (str_ptr))) {
			Error ("Creating %s", summary_file.File_Type ());
		}
	}
	if (!snapshot_flag && !output_flag && !sum_flag) {
		Error ("ArcView Snapshot, New Snapshot, or New Link Summary File are Required");
	}

	//---- cell size ----

	str_ptr = Get_Control_String (CELL_SIZE);

	if (str_ptr != NULL) {
		Get_Double (str_ptr, &cell_size);

		if (cell_size < 4.0 || cell_size > 9.0) {
			Error ("Cell Size %.2lf is Out of Range (4.0..9.0)", cell_size);
		}
		Print (2, "Cell Size = %.2lf meters", cell_size);
	}

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

	if (snapshot_flag) {
		str_ptr = Get_Control_String (DRAW_VEHICLE_SHAPES);

		if (str_ptr != NULL) {
			Print (1, "Draw Vehicle Shapes = %s", str_ptr);

			shape_flag = Get_Control_Flag (DRAW_VEHICLE_SHAPES);
		}

		if (!shape_flag) {
			str_ptr = Get_Control_String (ADD_PASSENGER_CIRCLE_SIZE);

			if (str_ptr != NULL) {
				Print (1, "Add Passenger Circle Size Field = %s", str_ptr);

				circle_flag = Get_Control_Flag (ADD_PASSENGER_CIRCLE_SIZE);
			}
			str_ptr = Get_Control_String (ADD_PASSENGER_SQUARE_SIZE);

			if (str_ptr != NULL) {
				Print (1, "Add Passenger Square Size Field = %s", str_ptr);

				square_flag = Get_Control_Flag (ADD_PASSENGER_SQUARE_SIZE);
			}
		}
	}

	//---- time step ----

	str_ptr = Get_Control_String (TIME_STEPS_PER_SECOND);

	if (str_ptr != NULL) {
		Get_Integer (str_ptr, &steps_per_second);

		if (steps_per_second < 1 || steps_per_second > 25) {
			Error ("%d Time Steps per Second is Out of Range (1..25)", steps_per_second);
		}
		Print (1, "Time Steps per Second = %d", steps_per_second);
	}
	time_period.Step_Size (steps_per_second);

	//---- pad file time labels ----

	str_ptr = Get_Control_String (PAD_FILE_TIME_LABEL);

	if (str_ptr != NULL) {
		Print (1, "Pad File Time Label = %s", str_ptr);

		pad_flag = Get_Control_Flag (PAD_FILE_TIME_LABEL);
	}

	//---- get the time of day format ----

	str_ptr = Get_Control_String (TIME_OF_DAY_FORMAT);

	if (str_ptr == NULL) {
		str_ptr = "24_HOUR_CLOCK";
	}
	if (!time_period.Format (str_ptr)) {
		Error ("Time of Day Format %s was Unrecognized", str_ptr);
	}
	Print (2, "Time of Day Format = %s", str_ptr);

	//---- get select time increment ----
	
	str_ptr = Get_Control_String (SELECT_TIME_INCREMENT);

	if (str_ptr != NULL) {
		time_period.Increment (str_ptr);
		increment = time_period.Increment ();

		if (increment < 0 || increment > 3600 * steps_per_second) {
			Error ("Select Time Increment %d is Out of Range (0..3600)", increment / steps_per_second);
		}
	} else {
		increment = 60;
		time_period.Increment (increment);
	}
	time_period.Period_Flag (true);

	//---- get the select time periods ----
	
	str_ptr = Get_Control_String (SELECT_TIME_PERIODS);

	if (str_ptr == NULL) {
		str_ptr = (char *) "All";
	}
	Print (2, "Select Time Periods = %s", str_ptr);

	if (!time_period.Add_Ranges (str_ptr)) {
		File_Error ("Time Period Range", str_ptr);
	}
	Print (1, "Select Time Increment = %g seconds", (double) increment / steps_per_second);

	max_time = time_period.Max_Value ();
		
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

	//---- get the veh types ----

	str_ptr = Get_Control_String (SELECT_VEHICLE_TYPES);

	if (str_ptr != NULL) {
		Print (2, "Select Vehicle Types = %s", str_ptr);

		if (!veh_types.Add_Ranges (str_ptr)) {
			Error ("Select Vehicle Types %s", str_ptr);
		}
		veh_type_flag = true;
	}

	//---- get the time processing method ----

	str_ptr = Get_Control_String (TIME_PROCESSING_METHOD);

	if (str_ptr != NULL) {
		Print (2, "Time Processing Method = %s", str_ptr);

		for (i=0; method_text [i] != NULL; i++) {
			if (!str_cmp (str_ptr, method_text [i])) {
				method = i;
				break;
			}
		}
		if (method_text [i] == NULL) {
			Error ("Unrecognized Time Processing Method = %s", str_ptr);
		}
		if (!occupancy_flag && method == TOTAL) {
			Error ("Snapshot Files cannot be Totaled");
		}
	}

	//---- initialize the shape file ----

	arcview_snapshot.File_Type ("ArcView Snapshot File");
	arcview_snapshot.File_Access (CREATE);
	arcview_snapshot.Shape_Type ((shape_flag) ? POLYGON : DOT);
	arcview_snapshot.Z_Flag (z_flag);
	arcview_snapshot.M_Flag (m_flag);

	if (occupancy_flag) {
		arcview_snapshot.Replicate_Fields (&occupancy_file);
		arcview_snapshot.LinkDir_Type ((Db_Code::Direction_Type) occupancy_file.LinkDir_Type ());

		link_field = occupancy_file.Link_Field ();
		dir_field = occupancy_file.Dir_Field ();
		lane_field = occupancy_file.Lane_Field ();
		offset_field = occupancy_file.Offset_Field ();
		occ_field = occupancy_file.Occupancy_Field ();
		start_field = occupancy_file.Start_Field ();
		end_field = occupancy_file.End_Field ();
		type_field = 0;
	} else {
		arcview_snapshot.Replicate_Fields (&snapshot_file);
		arcview_snapshot.LinkDir_Type ((Db_Code::Direction_Type) snapshot_file.LinkDir_Type ());

		link_field = snapshot_file.Link_Field ();
		dir_field = snapshot_file.Dir_Field ();
		lane_field = snapshot_file.Lane_Field ();
		offset_field = snapshot_file.Offset_Field ();
		type_field = snapshot_file.Type_Field ();
		occ_field = start_field = end_field = 0;

		if (circle_flag) {
			arcview_snapshot.Add_Field ("RADIUS", DOUBLE, 14, 2);
			circle_field = arcview_snapshot.Field_Number ("RADIUS");
		}
		if (square_flag) {
			arcview_snapshot.Add_Field ("SQUARE", DOUBLE, 14, 2);
			square_field = arcview_snapshot.Field_Number ("SQUARE");
		}
	}
	arcview_snapshot.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
}
