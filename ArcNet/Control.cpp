//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "ArcNet.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void ArcNet::Program_Control (void)
{
	int i;
	char *str_ptr, arcdir [FILE_NAME_SIZE];
	bool z_flag, m_flag;
	Db_Base *file;
	Db_Field *fld;

	//---- open network files ----

	Network_Service::Program_Control ();

	//---- open subzone files ----

	str_ptr = Get_Control_String (SUBZONE_DATA_FILE);

	if (str_ptr != NULL) {
		subzone_flag = true;

		Print (1);
		subzone_file.Open (Project_Filename (str_ptr));
	}

	//---- set the transit options ----

	route_flag = Network_File_Flag (TRANSIT_ROUTE);
	schedule_flag = Network_File_Flag (TRANSIT_SCHEDULE);
	driver_flag = Network_File_Flag (TRANSIT_DRIVER);

	if (!route_flag && !driver_flag) {
		route_node_flag = (Network_File_Flag (ROUTE_HEADER) || Network_File_Flag (ROUTE_NODES));

		if (route_node_flag) {
			if (!Network_File_Flag (ROUTE_HEADER)) {
				Error ("Route Header File is Required for Route Nodes Processing");
			} else if (!Network_File_Flag (ROUTE_NODES)) {
				Error ("Route Nodes File is Required for Route Header Processing");
			}
		}
	}

	//---- get the projection data ----

	projection.Read_Control ();

	//---- get the z coordinate flag ----

	m_flag = projection.M_Flag ();
	z_flag = projection.Z_Flag ();
	points.Z_Flag (z_flag);

	//---- get the arcview directory ----

	str_ptr = Get_Control_String (ARCVIEW_DIRECTORY);

	if (str_ptr == NULL) goto control_error;

	str_cpy (arcdir, sizeof (arcdir), str_ptr);

	Print_Filename (2, "ArcView Output Directory", arcdir);

	Fix_Directory (arcdir);

	//---- open the arcview node file ----

	if (Network_File_Flag (NODE)) {
		file = Network_Db_Base (NODE);

		arcview_node.File_Type ("ArcView Node File");
		arcview_node.File_Access (CREATE);
		arcview_node.Shape_Type (DOT);
		arcview_node.Z_Flag (z_flag);
		arcview_node.M_Flag (m_flag);

		if (!arcview_node.Open (Shapename (arcdir, file->Filename ()))) {
			File_Error ("Opening ArcView Node File", arcview_node.Shape_Filename ());
		}
		arcview_node.Replicate_Fields (file);

		arcview_node.Write_Header ();

		arcview_node.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- open the arcview zone file ----

	if (Network_File_Flag (ZONE)) {
		file = Network_Db_Base (ZONE);

		arcview_zone.File_Type ("ArcView Zone File");
		arcview_zone.File_Access (CREATE);
		arcview_zone.Shape_Type (DOT);
		arcview_zone.Z_Flag (z_flag);
		arcview_zone.M_Flag (m_flag);

		if (!arcview_zone.Open (Shapename (arcdir, file->Filename ()))) {
			File_Error ("Opening ArcView Zone File", arcview_zone.Shape_Filename ());
		}
		arcview_zone.Replicate_Fields (file);

		arcview_zone.Write_Header ();

		arcview_zone.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- open the arcview link file ----

	if (Network_File_Flag (LINK)) {
		file = Network_Db_Base (LINK);

		arcview_link.File_Type ("ArcView Link File");
		arcview_link.File_Access (CREATE);
		arcview_link.Shape_Type (VECTOR);
		arcview_link.Z_Flag (z_flag);
		arcview_link.M_Flag (m_flag);

		if (!arcview_link.Open (Shapename (arcdir, file->Filename ()))) {
			File_Error ("Opening ArcView Link File", arcview_link.Shape_Filename ());
		}
		arcview_link.Replicate_Fields (file);

		arcview_link.Write_Header ();

		arcview_link.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- open the arcview pocket file ----

	if (Network_File_Flag (POCKET_LANE)) {
		file = Network_Db_Base (POCKET_LANE);

		arcview_pocket.File_Type ("ArcView Pocket Lane File");
		arcview_pocket.File_Access (CREATE);
		arcview_pocket.Shape_Type (VECTOR);
		arcview_pocket.Z_Flag (z_flag);
		arcview_pocket.M_Flag (m_flag);

		if (!arcview_pocket.Open (Shapename (arcdir, file->Filename ()))) {
			File_Error ("Opening ArcView Pocket Lane File", arcview_pocket.Shape_Filename ());
		}
		arcview_pocket.Replicate_Fields (file);

		arcview_pocket.Write_Header ();

		arcview_pocket.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- open the arcview lane use file ----

	if (Network_File_Flag (LANE_USE)) {
		file = Network_Db_Base (LANE_USE);

		arcview_lane_use.File_Type ("ArcView Lane Use File");
		arcview_lane_use.File_Access (CREATE);
		arcview_lane_use.Shape_Type (VECTOR);
		arcview_lane_use.Z_Flag (z_flag);
		arcview_lane_use.M_Flag (m_flag);

		if (!arcview_lane_use.Open (Shapename (arcdir, file->Filename ()))) {
			File_Error ("Opening ArcView Lane Use File", arcview_lane_use.Shape_Filename ());
		}
		arcview_lane_use.Replicate_Fields (file);

		arcview_lane_use.Write_Header ();

		arcview_lane_use.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- open the arcview activity location file ----

	if (Network_File_Flag (ACTIVITY_LOCATION)) {
		file = Network_Db_Base (ACTIVITY_LOCATION);

		arcview_location.File_Type ("ArcView Activity Location File");
		arcview_location.File_Access (CREATE);
		arcview_location.Shape_Type (DOT);
		arcview_location.Z_Flag (z_flag);
		arcview_location.M_Flag (m_flag);

		if (!arcview_location.Open (Shapename (arcdir, file->Filename ()))) {
			File_Error ("Opening ArcView Activity Location File", arcview_location.Shape_Filename ());
		}
		arcview_location.Replicate_Fields (file);

		arcview_location.Write_Header ();

		arcview_location.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- open the arcview parking file ----

	if (Network_File_Flag (PARKING)) {
		file = Network_Db_Base (PARKING);

		arcview_parking.File_Type ("ArcView Parking File");
		arcview_parking.File_Access (CREATE);
		arcview_parking.Shape_Type (DOT);
		arcview_parking.Z_Flag (z_flag);
		arcview_parking.M_Flag (m_flag);

		if (!arcview_parking.Open (Shapename (arcdir, file->Filename ()))) {
			File_Error ("Opening ArcView Parking File", arcview_parking.Shape_Filename ());
		}
		arcview_parking.Replicate_Fields (file);

		arcview_parking.Write_Header ();

		arcview_parking.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- open the arcview process link file ----

	if (Network_File_Flag (PROCESS_LINK)) {
		file = Network_Db_Base (PROCESS_LINK);

		arcview_access.File_Type ("ArcView Process Link File");
		arcview_access.File_Access (CREATE);
		arcview_access.Shape_Type (LINE);
		arcview_access.Z_Flag (z_flag);
		arcview_access.M_Flag (m_flag);

		if (!arcview_access.Open (Shapename (arcdir, file->Filename ()))) {
			File_Error ("Opening ArcView Process Link File", arcview_access.Shape_Filename ());
		}
		arcview_access.Replicate_Fields (file);

		arcview_access.Write_Header ();

		arcview_access.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- open the arcview lane connectivity file ----

	if (Network_File_Flag (LANE_CONNECTIVITY)) {
		file = Network_Db_Base (LANE_CONNECTIVITY);

		arcview_connect.File_Type ("ArcView Lane Connectivity File");
		arcview_connect.File_Access (CREATE);
		arcview_connect.Shape_Type (VECTOR);
		arcview_connect.Z_Flag (z_flag);
		arcview_connect.M_Flag (m_flag);

		if (!arcview_connect.Open (Shapename (arcdir, file->Filename ()))) {
			File_Error ("Opening ArcView Lane Connectivity File", arcview_connect.Shape_Filename ());
		}
		arcview_connect.Replicate_Fields (file);

		arcview_connect.Write_Header ();

		arcview_connect.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- open the arcview turn prohibition file ----

	if (Network_File_Flag (TURN_PROHIBITION)) {
		file = Network_Db_Base (TURN_PROHIBITION);

		arcview_turn.File_Type ("ArcView Turn Prohibition File");
		arcview_turn.File_Access (CREATE);
		arcview_turn.Shape_Type (VECTOR);
		arcview_turn.Z_Flag (z_flag);
		arcview_turn.M_Flag (m_flag);

		if (!arcview_turn.Open (Shapename (arcdir, file->Filename ()))) {
			File_Error ("Opening ArcView Turn Prohibition File", arcview_turn.Shape_Filename ());
		}
		arcview_turn.Replicate_Fields (file);

		arcview_turn.Write_Header ();

		arcview_turn.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- open the arcview toll file ----

	if (Network_File_Flag (TOLL)) {
		file = Network_Db_Base (TOLL);

		arcview_toll.File_Type ("ArcView Toll File");
		arcview_toll.File_Access (CREATE);
		arcview_toll.Shape_Type (VECTOR);
		arcview_toll.Z_Flag (z_flag);
		arcview_toll.M_Flag (m_flag);

		if (!arcview_toll.Open (Shapename (arcdir, file->Filename ()))) {
			File_Error ("Opening ArcView Toll File", arcview_toll.Shape_Filename ());
		}
		arcview_toll.Replicate_Fields (file);

		arcview_toll.Write_Header ();

		arcview_toll.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- open the arcview unsignalized node file ----

	if (Network_File_Flag (UNSIGNALIZED_NODE)) {
		file = Network_Db_Base (UNSIGNALIZED_NODE);

		arcview_sign.File_Type ("ArcView Unsignalized Node File");
		arcview_sign.File_Access (CREATE);
		arcview_sign.Shape_Type (DOT);
		arcview_sign.Z_Flag (z_flag);
		arcview_sign.M_Flag (m_flag);

		if (!arcview_sign.Open (Shapename (arcdir, file->Filename ()))) {
			File_Error ("Opening ArcView Unsignalized Node File", arcview_sign.Shape_Filename ());
		}
		arcview_sign.Replicate_Fields (file);

		arcview_sign.Write_Header ();

		arcview_sign.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- open the arcview signalized node file ----

	if (Network_File_Flag (SIGNALIZED_NODE)) {
		file = Network_Db_Base (SIGNALIZED_NODE);

		arcview_signal.File_Type ("ArcView Signalized Node File");
		arcview_signal.File_Access (CREATE);
		arcview_signal.Shape_Type (DOT);
		arcview_signal.Z_Flag (z_flag);
		arcview_signal.M_Flag (m_flag);

		if (!arcview_signal.Open (Shapename (arcdir, file->Filename ()))) {
			File_Error ("Opening ArcView Signalized Node File", arcview_signal.Shape_Filename ());
		}
		arcview_signal.Replicate_Fields (file);

		arcview_signal.Write_Header ();

		arcview_signal.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- open the arcview timing plan file ----

	if (Network_File_Flag (TIMING_PLAN)) {
		if (!Network_File_Flag (PHASING_PLAN)) {
			Error ("A Phasing Plan is required to Draw Timing Plans");
		}
		file = Network_Db_Base (TIMING_PLAN);

		arcview_timing.File_Type ("ArcView Timing Plan File");
		arcview_timing.File_Access (CREATE);
		arcview_timing.Shape_Type (VECTOR);
		arcview_timing.Z_Flag (z_flag);
		arcview_timing.M_Flag (m_flag);

		if (!arcview_timing.Open (Shapename (arcdir, file->Filename ()))) {
			File_Error ("Opening ArcView Timing Plan File", arcview_timing.Shape_Filename ());
		}
		arcview_timing.Replicate_Fields (file);

		arcview_timing.Write_Header ();

		arcview_timing.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());

		timing_db.Replicate_Fields (file, true);
		timing_db.File_ID ("Timing");
		timing_flag = true;
	}

	//---- open the arcview phasing plan file ----

	if (Network_File_Flag (PHASING_PLAN)) {
		file = Network_Db_Base (PHASING_PLAN);

		arcview_phasing.File_Type ("ArcView Phasing Plan File");
		arcview_phasing.File_Access (CREATE);
		arcview_phasing.Shape_Type (VECTOR);
		arcview_phasing.Z_Flag (z_flag);
		arcview_phasing.M_Flag (m_flag);

		if (!arcview_phasing.Open (Shapename (arcdir, file->Filename ()))) {
			File_Error ("Opening ArcView Phasing Plan File", arcview_phasing.Shape_Filename ());
		}
		arcview_phasing.Replicate_Fields (file);

		arcview_phasing.Write_Header ();

		arcview_phasing.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- open the arcview detector file ----

	if (Network_File_Flag (DETECTOR)) {
		file = Network_Db_Base (DETECTOR);

		arcview_detector.File_Type ("ArcView Detector File");
		arcview_detector.File_Access (CREATE);
		arcview_detector.Shape_Type (VECTOR);
		arcview_detector.Z_Flag (z_flag);
		arcview_detector.M_Flag (m_flag);

		if (!arcview_detector.Open (Shapename (arcdir, file->Filename ()))) {
			File_Error ("Opening ArcView Detector File", arcview_detector.Shape_Filename ());
		}
		arcview_detector.Replicate_Fields (file);

		arcview_detector.Write_Header ();

		arcview_detector.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- open the arcview signal coordinator file ----

	if (Network_File_Flag (SIGNAL_COORDINATOR)) {
		if (!Network_File_Flag (SIGNALIZED_NODE)) {
			Error ("Signalized Nodes are required to Draw Signal Coordinators");
		}
		file = Network_Db_Base (SIGNAL_COORDINATOR);

		arcview_coordinator.File_Type ("ArcView Signal Coordinator File");
		arcview_coordinator.File_Access (CREATE);
		arcview_coordinator.Shape_Type (MDOTS);
		arcview_coordinator.Z_Flag (z_flag);
		arcview_coordinator.M_Flag (m_flag);

		if (!arcview_coordinator.Open (Shapename (arcdir, file->Filename ()))) {
			File_Error ("Opening ArcView Signal Coordinator File", arcview_coordinator.Shape_Filename ());
		}
		arcview_coordinator.Replicate_Fields (file);

		arcview_coordinator.Write_Header ();

		arcview_coordinator.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
		
		coordinator_db.Replicate_Fields (file, true);
		coordinator_db.File_ID ("Coordinator");
		coordinator_flag = true;
	}

	//---- open the arcview transit stop file ----

	if (Network_File_Flag (TRANSIT_STOP)) {
		file = Network_Db_Base (TRANSIT_STOP);

		arcview_stop.File_Type ("ArcView Transit Stop File");
		arcview_stop.File_Access (CREATE);
		arcview_stop.Shape_Type (DOT);
		arcview_stop.Z_Flag (z_flag);
		arcview_stop.M_Flag (m_flag);

		if (!arcview_stop.Open (Shapename (arcdir, file->Filename ()))) {
			File_Error ("Opening ArcView Transit Stop File", arcview_stop.Shape_Filename ());
		}
		arcview_stop.Replicate_Fields (file);

		arcview_stop.Write_Header ();

		arcview_stop.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- open the arcview transit route file ----

	if (route_flag) {
		if (!Network_File_Flag (TRANSIT_STOP)) {
			Error ("Transit Stops are required for Transit Route Processing");
		}
		file = Network_Db_Base (TRANSIT_ROUTE);

		arcview_route.File_Type ("ArcView Transit Route File");
		arcview_route.File_Access (CREATE);
		arcview_route.Shape_Type (VECTOR);
		arcview_route.Z_Flag (z_flag);
		arcview_route.M_Flag (m_flag);

		if (!arcview_route.Open (Shapename (arcdir, file->Filename ()))) {
			File_Error ("Opening ArcView Transit Route File", arcview_route.Shape_Filename ());
		}

		//---- copy the existing header fields ----

		for (i=1; i <= file->Num_Fields (); i++) {
			fld = file->Field (i);
			if (!fld->Nested ()) {
				arcview_route.Add_Field (fld->Name (), fld->Type (), fld->Size (), fld->Decimal ());
			}
		}
		if (schedule_flag) {
			arcview_route.Add_Field ("NUM_RUNS", INTEGER, 5);
		}
		if (driver_flag) {
			arcview_route.Add_Field ("NUM_LINKS", INTEGER, 5);
			arcview_route.Add_Field ("VEH_TYPE", INTEGER, 3);
			arcview_route.Add_Field ("SUB_TYPE", INTEGER, 2);
		}
		if (schedule_flag) {
			str_ptr = Get_Control_String (TRANSIT_TIME_PERIODS);

			if (str_ptr != NULL) {
				if (!time_period.Add_Breaks (str_ptr)) {
					Error ("Adding Transit Time Period Breaks");
				}
				Print (1, "Transit Time Period Breaks = %s", str_ptr);

				time_period.Format (Time_Step::CLOCK24);
				run_field = arcview_route.Num_Fields ();

				char *ptr, buffer [20];

				for (i=1; i <= time_period.Num_Ranges (); i++) {
					str_ptr = time_period.Range_Format (i);

					ptr = strstr (str_ptr, "..");
					if (ptr != NULL) *ptr = '\0';

					str_cpy (buffer, sizeof (buffer), "RUNS_");
					str_cat (buffer, sizeof (buffer), str_ptr);

					arcview_route.Add_Field (buffer, INTEGER, 4);
				}
			} else {
				schedule_flag = false;
			}
		}

	} else if (driver_flag) {

		file = Network_Db_Base (TRANSIT_DRIVER);
		Network_File_False (TRANSIT_ROUTE);
		Network_Option_False (TRANSIT_ROUTE);

		arcview_route.File_Type ("ArcView Transit Driver File");
		arcview_route.File_Access (CREATE);
		arcview_route.Shape_Type (VECTOR);
		arcview_route.Z_Flag (z_flag);
		arcview_route.M_Flag (m_flag);

		if (!arcview_route.Open (Shapename (arcdir, file->Filename ()))) {
			File_Error ("Opening ArcView Transit Driver File", arcview_route.Shape_Filename ());
		}

		//---- copy the existing header fields ----

		for (i=1; i <= file->Num_Fields (); i++) {
			fld = file->Field (i);
			if (!fld->Nested ()) {
				arcview_route.Add_Field (fld->Name (), fld->Type (), fld->Size (), fld->Decimal ());
			}
		}

	} else if (route_node_flag) {

		file = Network_Db_Base (ROUTE_NODES);

		arcview_route.File_Type ("ArcView Route Nodes File");
		arcview_route.File_Access (CREATE);
		arcview_route.Shape_Type (VECTOR);
		arcview_route.Z_Flag (z_flag);
		arcview_route.M_Flag (m_flag);

		if (!arcview_route.Open (Shapename (arcdir, file->Filename ()))) {
			File_Error ("Opening ArcView Route Nodes File", arcview_route.Shape_Filename ());
		}
		file = Network_Db_Base (ROUTE_HEADER);

		arcview_route.Replicate_Fields (file);

		AB_Key_Flag (true);

		route_header_db.Replicate_Fields (file, true);
		route_header_db.File_ID ("RouteHeader");
	}

	//---- open the arcview subzone file ----

	if (subzone_flag) {
		arcview_subzone.File_Type ("ArcView Subzone File");
		arcview_subzone.File_Access (CREATE);
		arcview_subzone.Shape_Type (DOT);
		arcview_subzone.Z_Flag (z_flag);
		arcview_subzone.M_Flag (m_flag);

		if (!arcview_subzone.Open (Shapename (arcdir, subzone_file.Filename ()))) {
			File_Error ("Opening ArcView Subzone File", arcview_subzone.Shape_Filename ());
		}
		arcview_subzone.Replicate_Fields (&subzone_file);

		arcview_subzone.Write_Header ();

		arcview_subzone.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- open the visualizer guideway file ----

	str_ptr = Get_Control_String (VISUALIZER_GUIDEWAY_FILE);

	if (str_ptr != NULL) {
		Print (1);
		guideway_flag = true;

		guideway_file.File_Type ("Visualizer Guideway File");
		guideway_file.File_Access (CREATE);
		guideway_file.Shape_Type (VECTOR);
		guideway_file.Z_Flag (z_flag);
		guideway_file.M_Flag (m_flag);

		if (!guideway_file.Open (Shapename (arcdir, str_ptr))) {
			File_Error ("Opening Visualizer Guideway File", guideway_file.Shape_Filename ());
		}
		guideway_file.Add_Field ("LINKID", INTEGER, 10);
		guideway_file.Add_Field ("DIRECTION", CHAR, 1);

		guideway_file.Write_Fields ();
		guideway_file.Write_Header ();

		guideway_file.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- offset parameters ----

	//---- draw lanes ----

	Print (1);
	str_ptr = Get_Control_String (DRAW_NETWORK_LANES);

	if (str_ptr != NULL) {
		Print (1, "Draw Network Lanes = %s", str_ptr);

		lanes_flag = Get_Control_Flag (DRAW_NETWORK_LANES);
	} else if (guideway_flag) {
		Error ("Visualizer Guideways require Lane Drawing");
	}

	//---- lane width ----

	if (lanes_flag || Network_File_Flag (LANE_CONNECTIVITY)) {
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

	//---- center oneway links ----

	str_ptr = Get_Control_String (CENTER_ONEWAY_LINKS);

	if (str_ptr != NULL) {
		Print (1, "Center Oneway Links = %s", str_ptr);

		center_flag = Get_Control_Flag (CENTER_ONEWAY_LINKS);
	}

	if (!lanes_flag) {

		//---- link direction offset ----

		if (Network_File_Flag (LINK)) {
			str_ptr = Get_Control_String (LINK_DIRECTION_OFFSET);

			if (str_ptr != NULL) {
				Get_Double (str_ptr, &link_offset);

				if (link_offset < 0.0 || link_offset > 15.0) {
					Error ("Link Direction Offset %.2lf is Out of Range (0..15)", link_offset);
				}
			}
			Print (1, "Link Direction Offset = %.2lf meters", link_offset);

			if (link_offset == 0.0) {
				str_ptr = Get_Control_String (DRAW_AB_DIRECTION);

				if (str_ptr != NULL) {
					direction_flag = Get_Control_Flag (DRAW_AB_DIRECTION);

					Print (1, "Draw AB Direction = %s", str_ptr);
				}
			}
		}

		//---- pocket side offset ----

		if (Network_File_Flag (POCKET_LANE)) {
			str_ptr = Get_Control_String (POCKET_LANE_SIDE_OFFSET);

			if (str_ptr != NULL) {
				Get_Double (str_ptr, &pocket_side);

				if (pocket_side < 0.0 || pocket_side > 25.0) {
					Error ("Pocket Lane Side Offset %.2lf is Out of Range (0..25)", pocket_side);
				}
			}
			Print (1, "Pocket Lane Side Offset = %.2lf meters", pocket_side);
		}
	}

	//---- activity location side offset ----

	str_ptr = Get_Control_String (ACTIVITY_LOCATION_SIDE_OFFSET);

	if (str_ptr != NULL) {
		Get_Double (str_ptr, &location_side);

		if (location_side < 0.0 || location_side > 100.0) {
			Error ("Activity Location Side Offset %.2lf is Out of Range (0..100)", location_side);
		}
	}
	Print (2, "Activity Location Side Offset = %.2lf meters", location_side);

	//---- parking side offset ----

	str_ptr = Get_Control_String (PARKING_SIDE_OFFSET);

	if (str_ptr != NULL) {
		Get_Double (str_ptr, &parking_side);

		if (parking_side < 0.0 || parking_side > 50.0) {
			Error ("Parking Side Offset %.2lf is Out of Range (0..50)", parking_side);
		}
	}
	Print (1, "Parking Side Offset = %.2lf meters", parking_side);

	//---- unsignalized node offsets ----

	str_ptr = Get_Control_String (UNSIGNALIZED_NODE_SIDE_OFFSET);

	if (str_ptr != NULL) {
		Get_Double (str_ptr, &sign_side);

		if (sign_side < 0.0 || sign_side > 75.0) {
			Error ("Unsignalized Node Side Offset %.2lf is Out of Range (0..75)", sign_side);
		}
	}
	Print (1, "Unsignalized Node Side Offset = %.2lf meters", sign_side);

	str_ptr = Get_Control_String (UNSIGNALIZED_NODE_SETBACK);

	if (str_ptr != NULL) {
		Get_Double (str_ptr, &sign_setback);

		if (sign_setback < 0.0 || sign_setback > 100.0) {
			Error ("Unsignalized Node Setback %.2lf is Out of Range (0..100)", sign_setback);
		}
	}
	Print (1, "Unsignalized Node Setback = %.2lf meters", sign_setback);

	//---- transit stop offset ----

	str_ptr = Get_Control_String (TRANSIT_STOP_SIDE_OFFSET);

	if (str_ptr != NULL) {
		Get_Double (str_ptr, &stop_side);

		if (stop_side < 0.0 || stop_side > 50.0) {
			Error ("Transit Stop Side Offset %.2lf is Out of Range (0..50)", stop_side);
		}
	}
	Print (2, "Transit Stop Side Offset = %.2lf meters", stop_side);

	//---- transit direction offset ----

	if (route_flag || driver_flag || route_node_flag) {

		arcview_route.Write_Header ();

		arcview_route.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());

		str_ptr = Get_Control_String (TRANSIT_DIRECTION_OFFSET);

		if (str_ptr != NULL) {
			Get_Double (str_ptr, &route_offset);

			if (route_offset < 0.0 || route_offset > 15.0) {
				Error ("Transit Direction Offset %.2lf is Out of Range (0..15)", route_offset);
			}
		}
		Print (1, "Transit Direction Offset = %.2lf meters", route_offset);

		//---- transit overlap ----

		str_ptr = Get_Control_String (TRANSIT_OVERLAP_FLAG);

		if (str_ptr != NULL) {
			Print (1, "Transit Overlap Flag = %s", str_ptr);

			overlap_flag = Get_Control_Flag (TRANSIT_OVERLAP_FLAG);

			if (!overlap_flag) {
				if (route_offset == 0.0) {
					Error ("Transit Direction Offset is Required for Overlap Processing");
				}
				if (route_flag && !driver_flag) {
					Error ("Transit Driver file is Required for Overlap Processing");
				}
			}
		}
	}

	//---- oneway arrow ----

	str_ptr = Get_Control_String (DRAW_ONEWAY_ARROWS);

	if (str_ptr != NULL) {
		Print (2, "Draw Oneway Arrows = %s", str_ptr);

		arrow_flag = Get_Control_Flag (DRAW_ONEWAY_ARROWS);
		
		str_ptr = Get_Control_String (ONEWAY_ARROW_LENGTH);

		if (str_ptr != NULL) {
			Get_Double (str_ptr, &arrow_length);

			if (arrow_length < 0.0 ) {
				Error ("Arrow Length %.2lf is Out of Range (>0)", arrow_length);
			}
		} else if (lane_width > 0) {
			arrow_length = lane_width * 2.0;
		}
		Print (1, "Oneway Arrow Length = %.2lf meters", arrow_length);

		str_ptr = Get_Control_String (ONEWAY_ARROW_SIDE_OFFSET);

		if (str_ptr != NULL) {
			Get_Double (str_ptr, &arrow_side);

			if (arrow_side <= 0.0 || arrow_side > 50.0) {
				Error ("Arrow Side Offset %.2lf is Out of Range (>0.0..50)", arrow_side);
			}
		} else if (lane_width > 0) {
			arrow_side = lane_width / 2.0;
		}
		Print (1, "Oneway Arrow Side Offset = %.2lf meters", arrow_side);
	}

	//---- set rounded connectivity ----
	
	str_ptr = Get_Control_String (USE_CONNECTIVITY_ARCS);

	if (str_ptr != NULL) {
		Print (2, "Draw Connectivity as Arcs = %s", str_ptr);

		rounded_connectivity = Get_Control_Flag (USE_CONNECTIVITY_ARCS);
		
		str_ptr = Get_Control_String (USE_FIXED_SETBACK_FOR_ARCS);

		if (str_ptr != NULL) {
			setback_override_flag = Get_Control_Flag (USE_FIXED_SETBACK_FOR_ARCS);
			Print (2, "Use Fixed Setback Distance for Arcs = %s", str_ptr);
			setback_distance = 18.0;
		} else {
			setback_override_flag = false;
		}
		

		str_ptr = Get_Control_String (FIXED_SETBACK_DISTANCE);
		if (str_ptr != NULL) {
			Get_Double (str_ptr, &setback_distance);
		}

		Print (1, "Fixed Setback Distance = %.2lf meters", setback_distance);
	}

	//---- get the select time period ----
	
	str_ptr = Get_Control_String (SELECT_TIME_PERIOD);

	if (str_ptr != NULL) {
		Print (2, "Select Time Period = %s", str_ptr);

		if (!time_range.Add_Ranges (str_ptr)) {
			File_Error ("Time Period Range", str_ptr);
		}
		select_time = true;
	}
	return;
	
control_error:
	Error ("Missing Control Key = %s", Current_Key ());
}
