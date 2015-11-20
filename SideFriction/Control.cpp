//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "SideFriction.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void SideFriction::Program_Control (void)
{
	int i, num, size;
	double dvalue;
	char *str_ptr, buffer [FIELD_BUFFER];

	//---- open network files ----

	Demand_Service::Program_Control ();

	//---- get the new lane use file ----

	new_file = (Lane_Use_File *) Network_Db_Base (NEW_LANE_USE);

	if (new_file->Dbase_Format () == VERSION3) {
		Error ("The %s cannot be in VERSION3 Format");
	}

	//---- get the time of day format ----
	
	str_ptr = Get_Control_String (TIME_OF_DAY_FORMAT);

	if (str_ptr == NULL) {
		str_ptr = "24_HOUR_CLOCK";
	}
	if (!time_step.Format (str_ptr)) {
		Error ("Time of Day Format %s was Unrecognized", str_ptr);
	}
	Print (2, "Time of Day Format = %s", str_ptr);

	//---- get the transit time periods ----
	
	str_ptr = Get_Control_String (TRANSIT_TIME_PERIODS);

	if (str_ptr == NULL) {
		str_ptr = (char *) "All";
	}
	Print (2, "Transit Time Periods = %s", str_ptr);

	if (!transit_periods.Add_Ranges (str_ptr)) {
		File_Error ("Time Period Range", str_ptr);
	}

	//---- initialize the time period delays ----

	num = transit_periods.Num_Ranges ();

	stop_times = new int [num + 1];

	if (stop_times == NULL) {
		Error ("Insufficient Memory for %d Time Factors", num);
	}
	for (i=0; i <= num; i++) {
		stop_times [i] = Round (5.0);
	}

	//---- time period delays ----

	str_ptr = Get_Control_String (TRANSIT_LANE_BLOCK_TIMES);

	if (str_ptr != NULL) {
		Print (1, "Transit Lane Block Times = %s seconds", str_ptr);
		i = 1;

		while (str_ptr != NULL) {
			str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));
			if (buffer [0] == '\0') break;

			if (i > num) {
				Warning ("Too Many Lane Block Times");
				break;
			}
			dvalue = atof (buffer);

			if (dvalue < 1.0 || dvalue > 300.0) {
				Error ("Lane Block Time %.1lf is Out of Range (1..300)", dvalue);
			}
			stop_times [i++] = Round (dvalue);
		}
	}

	//---- lane block length ----

	str_ptr = Get_Control_String (TRANSIT_LANE_BLOCK_LENGTH);

	if (str_ptr != NULL) {
		dvalue = atof (str_ptr);

		if (dvalue < 10 || dvalue > 100) {
			Error ("Lane Block Length %.2lf is Out of Range (10..100)", dvalue);
		}
		Print (1, "Transit Lane Block Length = %.2lf meters", dvalue);

		stop_length = Round (dvalue);
	}

	//---- get the select transit modes ----
	
	str_ptr = Get_Control_String (SELECT_TRANSIT_MODES);

	if (str_ptr != NULL) {
		Print (1, "Select Transit Modes = %s", str_ptr);

		size = REGIONRAIL + 1;

		memset (select_modes, '\0', size * sizeof (bool));

		while (str_ptr != NULL) {
			str_ptr = Get_Token (str_ptr, buffer);

			if (buffer [0] == '\0') break;

			select_modes [Transit_Code (buffer)] = true;
		}
	} else {
		for (i=1; i <= REGIONRAIL; i++) {
			select_modes [i] = true;
		}
	}
	
	//---- get the select transit routes ----

	str_ptr = Get_Control_String (SELECT_TRANSIT_ROUTES);

	if (str_ptr != NULL) {
		Print (1, "Select Transit Routes = %s", str_ptr);

		if (!select_routes.Add_Ranges (str_ptr)) {
			File_Error ("Transit Route Range", str_ptr);
		}
		route_flag = true;
	}
		
	//---- get the select subarea polygon ----

	str_ptr = Get_Control_String (SELECT_SUBAREA_POLYGON);

	if (str_ptr != NULL) {
		select_subarea.File_Type ("Select Subarea Polygon");

		select_subarea.Open (Project_Filename (str_ptr));
		
		if (!select_subarea.Read_Record ()) {
			Error ("Reading %s", select_subarea.File_Type ());
		}
		subarea_flag = true;
	}

	//---- read report types ----

	List_Reports ();

	return;
} 
