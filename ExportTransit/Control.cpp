//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "ExportTransit.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void ExportTransit::Program_Control (void)
{
	char *str_ptr, buffer [40];

	//---- open network files ----

	Network_Service::Program_Control ();
	
	header_file = (Route_Header_File *) Network_Db_File (NEW_ROUTE_HEADER);
	nodes_file = (Route_Nodes_File *) Network_Db_File (NEW_ROUTE_NODES);

	//---- write dwell field ----

	Print (1);

	str_ptr = Get_Control_String (WRITE_DWELL_FIELD);

	if (str_ptr != NULL) {
		Print (1, "Write Dwell Field = %s", str_ptr);
		dwell_flag = Get_Control_Flag (WRITE_DWELL_FIELD);
	}

	//---- write time field ----

	str_ptr = Get_Control_String (WRITE_TIME_FIELD);

	if (str_ptr != NULL) {
		Print (1, "Write Time Field = %s", str_ptr);
		ttime_flag = Get_Control_Flag (WRITE_TIME_FIELD);
	}

	//---- write speed field ----

	str_ptr = Get_Control_String (WRITE_SPEED_FIELD);

	if (str_ptr != NULL) {
		Print (1, "Write Speed Field = %s", str_ptr);
		speed_flag = Get_Control_Flag (WRITE_SPEED_FIELD);
	}

	//---- dwell times ----

	if (dwell_flag) {
		str_ptr = Get_Control_String (MINIMUM_DWELL_TIME);

		if (str_ptr != NULL) {
			min_dwell = atoi (str_ptr);
			if (min_dwell < 1 || min_dwell > 120) {
				Error ("Minimum Dwell Time %d is Out of Range (1..120)", min_dwell);
			}
		}
		Print (2, "Minimum Dwell Time = %d seconds", min_dwell);

		str_ptr = Get_Control_String (MAXIMUM_DWELL_TIME);

		if (str_ptr != NULL) {
			max_dwell = atoi (str_ptr);
			if (max_dwell < min_dwell || max_dwell > 300) {
				Error ("Maximum Dwell Time %d is Out of Range (%d..300)", max_dwell);
			}
		}
		Print (1, "Maximum Dwell Time = %d seconds", max_dwell);

		min_dwell = Round (min_dwell);
		max_dwell = Round (max_dwell);
	}

	//---- get the time periods ----

	str_ptr = Get_Control_String (TRANSIT_TIME_PERIODS);

	if (str_ptr == NULL) {
		str_ptr = "None";
	}
	if (!time_period.Add_Breaks (str_ptr)) {
		Error ("Adding Transit Time Period Breaks");
	}
	num_periods = time_period.Num_Ranges ();

	if (num_periods >= MAX_PERIODS) {
		Error ("Number of Time Periods %d are Out of Range (1..%d)", num_periods, MAX_PERIODS-1);
	}
	Print (2, "Transit Time Period Breaks = %s", str_ptr);

	//---- get the period travel time flag ----

	str_ptr = Get_Control_String (PERIOD_TRAVEL_TIMES);

	if (str_ptr != NULL) {
		Print (1, "Period Travel Times = %s", str_ptr);

		time_flag = Get_Control_Flag (PERIOD_TRAVEL_TIMES);
	}

	//---- update the file header ----

	header_file->Create_Fields (num_periods, time_flag);
	header_file->Write_Header ();

	nodes_file->Dwell_Flag (dwell_flag);
	nodes_file->Time_Flag (ttime_flag);
	nodes_file->Speed_Flag (speed_flag);

	nodes_file->Clear_Fields ();
	nodes_file->Create_Fields ();
	nodes_file->Write_Header ();

	//---- get the select transit modes ----

	str_ptr = Get_Control_String (SELECT_TRANSIT_MODES);

	if (str_ptr != NULL) {
		Print (1, "Select Transit Modes = %s", str_ptr);

		memset (select_modes, '\0', sizeof (select_modes));

		while (str_ptr != NULL) {
			str_ptr = Get_Token (str_ptr, buffer);

			if (buffer [0] == '\0') break;

			select_modes [Transit_Code (buffer)] = true;
		}
	} else {
		for (int i=0; i <= REGIONRAIL; i++) {
			select_modes [i] = true;
		}
	}
	
	//---- get the select transit routes ----
	
	str_ptr = Get_Control_String (SELECT_TRANSIT_ROUTES);

	if (str_ptr != NULL) {
		Print (1, "Select Transit Routes = %s", str_ptr);

		if (!select_routes.Add_Ranges (str_ptr)) {
			Error ("Transit Route Range");
		}
	}
} 
