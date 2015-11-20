//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "ArcDelay.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void ArcDelay::Program_Control (void)
{
	int i, len, increment, nfield, n, num, type;
	char *str_ptr, buffer [STRING_BUFFER];
	double dvalue;
	bool binary, ignore_flag, delay_in, z_flag, m_flag;

	Db_Field *fld;
	Image_Data image_rec;

	//---- open network files ----

	Demand_Service::Program_Control ();

	delay_in = Demand_File_Flag (PERFORMANCE);
	index_flag = Get_Control_Flag ("ADD_LINK_DIRECTION_INDEX");
	ignore_flag = Get_Control_Flag ("IGNORE_TIME_RANGE_FIELDS");
	width_flag = (Get_Control_String (BANDWIDTH_FIELD) != NULL);
	event_flag = (Get_Control_String (SYSTEM_EVENT_FILE) != NULL);

	//---- get the projection data ----

	projection.Read_Control ();

	//---- get the z coordinate flag ----

	m_flag = projection.M_Flag ();
	z_flag = projection.Z_Flag ();
	points.Z_Flag (z_flag);

	//---- get the link datafile ----

	str_ptr = Get_Control_String (LINK_DATA_FILE);

	if (str_ptr != NULL) {
		link_data_flag = true;

		Print (1);
		link_data_file.Open (Project_Filename (str_ptr));
	}

	//---- get the link direction file ----

	str_ptr = Get_Control_String (LINK_DIRECTION_FILE);

	if (str_ptr != NULL) {
		link_dir_flag = true;
		link_dir_file.Ignore_Periods (ignore_flag);

		Print (1);
		link_dir_file.Open (Project_Filename (str_ptr));

		if (ignore_flag) {
			Print (1, "Time Range Fields are Ignored");
		}
	}
	if (!delay_in && !link_data_flag && !link_dir_flag) goto delay_error;

	//---- open the system event file ----

	if (event_flag) {
		str_ptr = Get_Control_String (SYSTEM_EVENT_FILE);

		Print (1);
		event_file.Open (Project_Filename (str_ptr));

		if (!Network_File_Flag (PHASING_PLAN)) {
			Error ("A Phasing Plan file is required for System Event Processing");
		}
	}

	//---- get the arcview link data file ----

	str_ptr = Get_Control_String (ARCVIEW_LINK_DATA_FILE);

	if (str_ptr != NULL) {
		if (!link_data_flag) {
			Error ("The Input Link Data File is Missing");
		}
		Print (1);
		arcview_link_data.File_Type ("New ArcView Link Data File");
		arcview_link_data.File_Access (CREATE);
		if (width_flag) {
			arcview_link_data.Shape_Type (POLYGON);
		} else {
			arcview_link_data.Shape_Type (VECTOR);
		}
		arcview_link_data.Z_Flag (z_flag);
		arcview_link_data.M_Flag (m_flag);

		arcview_link_data.Replicate_Fields (&link_data_file);

		arcview_link_data.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());

		if (!arcview_link_data.Open (Project_Filename (str_ptr))) {
			File_Error ("Opening ArcView Link Data File", arcview_link_data.Shape_Filename ());
		}
		arcview_link_data.Write_Header ();

	} else if (link_data_flag) {
		Error ("The Output Arcview Link Data File is Missing");
	}

	//---- get the arcview link dir file ----

	str_ptr = Get_Control_String (ARCVIEW_LINK_DIR_FILE);

	if (str_ptr != NULL) {
		if (!link_dir_flag) {
			Error ("The Input Link Direction File is Missing");
		}
		Print (1);
		arcview_link_dir.File_Type ("New ArcView Link Dir File");
		arcview_link_dir.File_Access (CREATE);
		if (width_flag) {
			arcview_link_dir.Shape_Type (POLYGON);
		} else {
			arcview_link_dir.Shape_Type (VECTOR);
		}
		arcview_link_dir.Z_Flag (z_flag);
		arcview_link_dir.M_Flag (m_flag);

		if (index_flag) {
			arcview_link_dir.Add_Field ("INDEX", INTEGER, 10);
		}
		arcview_link_dir.Replicate_Fields (&link_dir_file);

		arcview_link_dir.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());

		if (!arcview_link_dir.Open (Project_Filename (str_ptr))) {
			File_Error ("Opening ArcView Link Dir File", arcview_link_dir.Shape_Filename ());
		}
		arcview_link_dir.Write_Header ();

		if (index_flag) {
			Print (1, "Link Direction Index Field was Added");
		}
	} else if (link_dir_flag) {
		Error ("The Output Arcview Link Dir File is Missing");
	}

	//---- get the arcview delay file ----

	str_ptr = Get_Control_String (ARCVIEW_DELAY_FILE);

	if (str_ptr != NULL) {
		if (!delay_in) goto delay_error;
		delay_flag = true;

		arcview_delay.File_Type ("New ArcView Delay File");
		arcview_delay.File_Access (CREATE);
		if (width_flag && !link_dir_flag) {
			arcview_delay.Shape_Type (POLYGON);
		} else {
			arcview_delay.Shape_Type (VECTOR);
		}
		arcview_delay.Z_Flag (z_flag);
		arcview_delay.M_Flag (m_flag);

		//---- copy the link fields ----

		delay_file = (Delay_File *) Demand_Db_File (PERFORMANCE);

		binary = (delay_file->Record_Format () == BINARY);
		nfield = delay_file->Num_Fields ();

		if (index_flag) {
			arcview_delay.Add_Field ("INDEX", INTEGER, 10);
		}

		for (i=1; i <= nfield; i++) {
			fld = delay_file->Field (i);
			if (fld != NULL && !fld->Nested ()) {
				arcview_delay.Add_Field (fld->Name (), fld->Type (), fld->Size (), fld->Decimal (), -1, binary, false);
			}
		}
		arcview_delay.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());

		//---- set the file name ----

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

		Print_Filename (2, arcview_delay.File_Type (), buffer);

		if (index_flag) {
			Print (1, "Link Direction Index Field was Added");
		}
	}

	//---- get the arcview intersection file ----

	str_ptr = Get_Control_String (ARCVIEW_INTERSECTION_FILE);

	if (str_ptr != NULL) {
		if (!delay_in) goto delay_error;
		intersection_flag = true;

		arcview_node.File_Type ("New ArcView Intersection File");
		arcview_node.File_Access (CREATE);
		arcview_node.Shape_Type (DOT);
		arcview_node.Z_Flag (z_flag);
		arcview_node.M_Flag (m_flag);

		//---- copy the delay fields ----

		delay_file = (Delay_File *) Demand_Db_File (PERFORMANCE);

		binary = (delay_file->Record_Format () == BINARY);
		nfield = delay_file->Num_Fields ();
			
		arcview_node.Add_Field ("NODE", INTEGER, 10);

		for (i=1; i <= nfield; i++) {
			fld = delay_file->Field (i);
			if (fld != NULL && !fld->Nested ()) {

				if (!str_cmp (fld->Name (), "LINK") || !str_cmp (fld->Name (), "NODE") ||
					!str_cmp (fld->Name (), "DIR") || !str_cmp (fld->Name (), "LANE")) continue;

				arcview_node.Add_Field (fld->Name (), fld->Type (), fld->Size (), fld->Decimal (), -1, binary, false);
			}
		}
		arcview_node.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());

		//---- set the file name ----

		len = (int) strlen (str_ptr);

		if (len > 4) {
			char *ptr = strrchr (str_ptr + len - 4, '.');

			if (ptr != NULL) {
				*ptr = '\0';
			}
		}
		str_cpy (nodename, sizeof (nodename), Project_Filename (str_ptr));

		str_cpy (buffer, sizeof (buffer), nodename);
		str_cat (buffer, sizeof (buffer), "_time_of_day.shp");

		Print_Filename (2, arcview_node.File_Type (), buffer);
	}

	//---- get the arcview turn file ----

	str_ptr = Get_Control_String (ARCVIEW_TURN_FILE);

	if (str_ptr != NULL) {
		if (!delay_in) goto delay_error;

		if (!Network_File_Flag (LANE_CONNECTIVITY)) {
			Error ("Turning Movement Delays required a Lane Connectivity File");
		}

		//---- initialize the turn file ----

		arcview_turn.File_Type ("New ArcView Turn File");
		arcview_turn.File_Access (CREATE);
		arcview_turn.Shape_Type (VECTOR);
		arcview_turn.Z_Flag (z_flag);
		arcview_turn.M_Flag (m_flag);

		delay_file = (Delay_File *) Demand_Db_File (PERFORMANCE);
		
		binary = (delay_file->Record_Format () == BINARY);
		nfield = delay_file->Num_Fields ();

		for (i=1; i <= nfield; i++) {
			fld = delay_file->Field (i);
			if (fld == NULL) continue;
			if (!fld->Nested ()) {
				if (strcmp (fld->Name (), "LINK") != 0 &&
					strcmp (fld->Name (), "NODE") != 0 &&
					strcmp (fld->Name (), "DIR") != 0) {
					continue;
				}
			}
			arcview_turn.Add_Field (fld->Name (), fld->Type (), fld->Size (), fld->Decimal (), -1, binary, false);
		}
		if (event_flag) {
			arcview_turn.Add_Field ("GREEN", INTEGER, 6);
			arcview_turn.Add_Field ("YELLOW", INTEGER, 6);
			arcview_turn.Add_Field ("RED", INTEGER, 6);
			arcview_turn.Add_Field ("SPLIT", INTEGER, 4);

			green_field = arcview_turn.Field_Number ("GREEN");
			yellow_field = arcview_turn.Field_Number ("YELLOW");
			red_field = arcview_turn.Field_Number ("RED");
			split_field = arcview_turn.Field_Number ("SPLIT");
		}
		arcview_turn.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());

		len = (int) strlen (str_ptr);

		if (len > 4) {
			char *ptr = strrchr (str_ptr + len - 4, '.');

			if (ptr != NULL) {
				*ptr = '\0';
			}
		}
		str_cpy (turnname, sizeof (turnname), Project_Filename (str_ptr));

		str_cpy (buffer, sizeof (buffer), turnname);
		str_cat (buffer, sizeof (buffer), "_time_of_day.shp");

		Print_Filename (2, arcview_turn.File_Type (), buffer);
		turn_flag = true;

	} else {
		if (event_flag) {
			Error ("System Event File Processing required an ArcView Turn File");
		}
		Network_File_False (PHASING_PLAN);
	}

	//---- get the arcview delay file ----
		
	shape_flag = Get_Control_Flag (DRAW_VEHICLE_SHAPES);

	num = Highest_Control_Group (ARCVIEW_TRAFFIC_IMAGE_x, 0);

	for (n=0; n <= num; n++) {
		if (n) {
			str_ptr = Get_Control_String (ARCVIEW_TRAFFIC_IMAGE_x, n);
			str_fmt (buffer, sizeof (buffer), "ArcView Traffic Image #%d", n);
		} else {
			str_ptr = Get_Control_String (ARCVIEW_TRAFFIC_IMAGE);
			str_cpy (buffer, sizeof (buffer), "ArcView Traffic Image");
		}
		if (str_ptr == NULL) continue;
		if (!delay_in) goto delay_error;

		image_flag = true;
		image_rec.number = n;
		image_rec.file = new Arcview_File (CREATE);
		image_rec.file->File_Type (buffer);
		image_rec.file->Shape_Type ((shape_flag) ? POLYGON : DOT);
		image_rec.file->Z_Flag (z_flag);
		image_rec.file->M_Flag (m_flag);

		//---- set the file name ----

		len = (int) strlen (str_ptr);

		if (len > 4) {
			char *ptr = strrchr (str_ptr + len - 4, '.');

			if (ptr != NULL) {
				*ptr = '\0';
			}
		}
		str_cpy (image_rec.name, sizeof (image_rec.name), Project_Filename (str_ptr));

		str_cpy (buffer, sizeof (buffer), image_rec.name);
		str_cat (buffer, sizeof (buffer), "_time_of_day.shp");

		Print_Filename (2, image_rec.file->File_Type (), buffer);

		//---- copy the link fields ----

		delay_file = (Delay_File *) Demand_Db_File (PERFORMANCE);

		binary = (delay_file->Record_Format () == BINARY);
		nfield = delay_file->Num_Fields ();

		for (i=1; i <= nfield; i++) {
			fld = delay_file->Field (i);
			if (fld != NULL && !fld->Nested ()) {
				image_rec.file->Add_Field (fld->Name (), fld->Type (), fld->Size (), fld->Decimal (), -1, binary, false);
			}
		}
		image_rec.file->Set_Projection (projection.Input_Projection (), projection.Output_Projection ());

		//---- image attribute ----

		if (n) {
			str_ptr = Get_Control_String (TRAFFIC_IMAGE_ATTRIBUTE_x, n);
			Print (1, "Traffic Image Attribute #%d = %s", n, str_ptr);
		} else {
			str_ptr = Get_Control_String (TRAFFIC_IMAGE_ATTRIBUTE);
			Print (1, "Traffic Image Attribute = %s", str_ptr);
		}
		if (str_ptr == NULL) goto control_error;

		image_rec.type = type = Performance_Code (str_ptr);

		if (type != VOLUME_DATA && type != DENSITY_DATA && 
			type != MAX_DENSITY_DATA && type != QUEUE_DATA &&
			type != MAX_QUEUE_DATA && type != FAILURE_DATA) {

			Error ("Traffic Image Attribute %s is Not an Image Option", str_ptr);
		}

		if (!image_data.Add (&image_rec)) {
			Error ("Adding Image Record");
		}
	}

	//---- check support file flags ----

	if (image_flag) {
		if (!Demand_File_Flag (VEHICLE_TYPE)) {
			Error ("A Vehicle Type File is required for Traffic Images");
		}
		if (!Network_File_Flag (POCKET_LANE)) {
			Error ("A Pocket Lane File is required for Traffic Images");
		}
		if (!Network_File_Flag (LANE_USE)) {
			Write (2, "Warning: A Lane Use File is desirable for Traffic Images");
			Show_Message (1);
		}

		//---- vehicle type distribution ----
		
		str_ptr = Get_Control_String (VEHICLE_TYPE_DISTRIBUTION);

		Print (2, "Vehicle Type Distribution = ");

		do {
			if (str_ptr != NULL) {
				str_ptr = Get_Double (str_ptr, &dvalue);
			} else {
				dvalue = 100.0;
			}
			if (dvalue < 0.0 || dvalue > 100.0) {
				Error ("Vehicle Type Distribution Value %.1lf is Out of Range (0..100)", dvalue);
			}
			Print (0, " %.1lf ", dvalue);

			if (!type_share.Add (Round (dvalue))) {
				Error ("Adding Vehicle Type Share");
			}
		} while (str_ptr != NULL);

		Print (0, "percent");

		//---- random number seed ----

		str_ptr = Get_Control_String (RANDOM_NUMBER_SEED);

		if (str_ptr != NULL) {
			random.Seed (atoi (str_ptr));
		}
		Print (2, "Random Number Seed = %d", random.Seed ());

		//---- print the control key ----

		str_ptr = Get_Control_String (DRAW_VEHICLE_SHAPES);

		if (str_ptr != NULL) {
			Print (1, "Draw Vehicle Shapes = %s", str_ptr);
		}
	} else {
		Network_File_False (POCKET_LANE);
		Network_File_False (LANE_USE);
		Demand_File_False (VEHICLE_TYPE);
	}

	//---- draw lanes ----

	Print (1);
	str_ptr = Get_Control_String (DRAW_NETWORK_LANES);

	if (str_ptr != NULL) {
		Print (1, "Draw Network Lanes = %s", str_ptr);

		lanes_flag = Get_Control_Flag (DRAW_NETWORK_LANES);
	}

	//---- center oneway links ----

	str_ptr = Get_Control_String (CENTER_ONEWAY_LINKS);

	if (str_ptr != NULL) {
		Print (1, "Center Oneway Links = %s", str_ptr);

		center_flag = Get_Control_Flag (CENTER_ONEWAY_LINKS);
	}

	//---- lane width ----

	if (lanes_flag || image_flag) {
		str_ptr = Get_Control_String (LANE_WIDTH);

		if (str_ptr != NULL) {
			Get_Double (str_ptr, &lane_width);

			if (lane_width < 0.0 || lane_width > 25.0) {
				Error ("Lane Width %.2lf is Out of Range (0..25)", lane_width);
			}
			if (lanes_flag && lane_width == 0.0) {
				Error ("Lane Width must be > 0.0 to Draw Lanes");
			}
		}
		Print (1, "Lane Width = %.2lf meters", lane_width);

	}

	//---- link direction offset ----

	if (!lanes_flag) {
		str_ptr = Get_Control_String (LINK_DIRECTION_OFFSET);

		if (str_ptr != NULL) {
			Get_Double (str_ptr, &link_offset);

			if (link_offset < 0.0 || link_offset > 15.0) {
				Error ("Link Direction Offset %.2lf is Out of Range (0..15)", link_offset);
			}
		}
		if (delay_flag || image_flag || link_dir_flag || link_data_flag ) {
			Print (1, "Link Direction Offset = %.2lf meters", link_offset);
		}
	}

	//---- bandwidth processing ----

	if (width_flag) {
		str_ptr = Get_Control_String (BANDWIDTH_FIELD);

		if (link_data_flag) {
			width_field = arcview_link_data.Required_Field (str_ptr);
		} else if (link_dir_flag) {
			width_field = arcview_link_dir.Required_Field (str_ptr);
		} else if (delay_flag) {
			width_field = arcview_delay.Required_Field (str_ptr);
		} else {
			Error ("Bandwidths required a Arcview Link or Delay File");
		}
		Print (2, "Bandwidth Field = %s, Number = %d", str_ptr, width_field);

		//---- bandwidth scaling factor ----

		str_ptr = Get_Control_String (BANDWIDTH_SCALING_FACTOR);

		if (str_ptr != NULL) {
			Get_Double (str_ptr, &width_factor);

			if (width_factor < 0.01 || width_factor > 100000.0) {
				Error ("Bandwidth Scaling Factor %.2lf is Out of Range (0.01..100000.0)", width_factor);
			}
		}
		Print (1, "Bandwidth Scaling Factor = %.2lf per meter", width_factor);

		//---- minimum bandwidth value ----

		str_ptr = Get_Control_String (MINIMUM_BANDWIDTH_VALUE);

		if (str_ptr != NULL) {
			Get_Double (str_ptr, &min_value);

			if (min_value < 0.0 || min_value > 100000.0) {
				Error ("Minimum Bandwidth Value %.2lf is Out of Range (0.0..100000.0)", min_value);
			}
			Print (1, "Minimum Bandwidth Value = %.2lf", min_value);
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
		increment = time_period.Duration (str_ptr);

		if ((increment < 5 || increment > 7200) && increment != 0) {
			Error ("Select Time Increment %d is Out of Range (5..120)", increment);
		}
		increment *= 60;
	} else {
		increment = 900;
	}
	time_period.Increment (increment);
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
	if (increment > 0) {
		Print (1, "Select Time Increment = %d minutes", increment / 60);
	} else {
		Print (1, "One Summary Increment will be Output");
	}

	//---- set the period range ----

	performance_data.Time_Periods (&time_period);

	if (Network_File_Flag (LANE_CONNECTIVITY)) {
		connect_time.Time_Periods (&time_period);
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
	return;

control_error:
	Error ("Missing Control Key = %s", Current_Key ());

delay_error:
	Error ("The Input Link Delay File is Missing");
}
