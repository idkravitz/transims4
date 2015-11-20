//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "ArcTraveler.hpp"

#include "Activity_File.hpp"
#include "Trip_File.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void ArcTraveler::Program_Control (void)
{
	char *str_ptr, *format_ptr;
	bool z_flag, m_flag;
	Db_Base *file;

	//---- open network files ----

	Demand_Service::Program_Control ();

	//---- set the application type ----

	if (Demand_File_Flag (TRIP) || Demand_File_Flag (ACTIVITY)) {
		traveler_flag = false;

		if (!Network_File_Flag (ACTIVITY_LOCATION)) {
			Error ("Activity Locations are Required for Trip or Activity File Processing");
		}

		Network_File_False (NODE);
		Network_File_False (LINK);
		Network_File_False (SHAPE);

		if (Demand_File_Flag (TRIP)) {
			trip_flag = true;
			file = Demand_Db_Base (TRIP);

			if (Demand_Db_File (ACTIVITY)) {
				Warning ("Trip and Activity Files can not be Processed Simultaneously");
			}
		} else {
			activity_flag = true;
			file = Demand_Db_Base (ACTIVITY);
		}
	} else {
		traveler_flag = true;
		Network_File_False (ACTIVITY_LOCATION);

		str_ptr = Get_Control_String (TRAVELER_FILE);

		if (str_ptr == NULL) goto control_error;

		format_ptr = Get_Control_String (TRAVELER_FORMAT);

		if (format_ptr != NULL) {
			traveler_file.Dbase_Format (format_ptr);
		}
		Print (1);
		traveler_file.Open (Project_Filename (str_ptr));

		if (!Network_File_Flag (NODE) || !Network_File_Flag (LINK)) {
			Error ("Link and Node Files are required for Traveler Processing");
		}
	}
	Demand_File_False (TRIP);
	Demand_File_False (ACTIVITY);

	//---- get the projection data ----

	projection.Read_Control ();

	//---- get the z coordinate flag ----

	m_flag = projection.M_Flag ();
	z_flag = projection.Z_Flag ();
	points.Z_Flag (z_flag);

	//---- get the arcview traveler file ----

	str_ptr = Get_Control_String (ARCVIEW_TRAVELER_FILE);

	if (str_ptr == NULL) goto control_error;

	Print (1);
	arcview_traveler.File_Type ("ArcView Traveler File");
	arcview_traveler.File_Access (CREATE);
	arcview_traveler.Shape_Type (VECTOR);
	arcview_traveler.Z_Flag (z_flag);
	arcview_traveler.M_Flag (m_flag);

	if (!arcview_traveler.Open (Project_Filename (str_ptr))) {
		File_Error ("Creating ArcView Traveler File", arcview_traveler.Shape_Filename ());
	}
	if (traveler_flag) {
		arcview_traveler.Add_Field ("TRAVELER", INTEGER, 10);
		arcview_traveler.Add_Field ("VEHICLE", INTEGER, 10);
		arcview_traveler.Add_Field ("STARTTIME", STRING, TIME_BUFFER);
		arcview_traveler.Add_Field ("ENDTIME", STRING, TIME_BUFFER);
		arcview_traveler.Add_Field ("DISTANCE", DOUBLE, 10, 1);
		arcview_traveler.Add_Field ("TTIME", DOUBLE, 10, 1);
		arcview_traveler.Add_Field ("SPEED", DOUBLE, 10, 1);
	} else {
		arcview_traveler.Replicate_Fields (file);
	}
	arcview_traveler.Write_Header ();

	arcview_traveler.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());

	//---- draw detail traveler paths ----

	if (traveler_flag) {

		//---- draw lanes ----

		Print (1);
		str_ptr = Get_Control_String (DRAW_NETWORK_LANES);

		if (str_ptr != NULL) {
			Print (1, "Draw Network Lanes = %s", str_ptr);

			lanes_flag = Get_Control_Flag (DRAW_NETWORK_LANES);
		}

		//---- lane width ----

		if (lanes_flag) {
			str_ptr = Get_Control_String (LANE_WIDTH);

			if (str_ptr != NULL) {
				Get_Double (str_ptr, &lane_width);

				if (lane_width < 0.1 || lane_width > 25.0) {
					Error ("Lane Width %.2lf is Out of Range (0.1..25)", lane_width);
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

	//---- get the select travelers ----
	
	str_ptr = Get_Control_String (SELECT_TRAVELERS);

	if (str_ptr != NULL) {
		Print (2, "Select Traveler IDs = %s", str_ptr);

		if (!travelers.Add_Ranges (str_ptr)) {
			File_Error ("Traveler ID Range", str_ptr);
		}
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
}
