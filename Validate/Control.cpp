//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "Validate.hpp"

#include "Utility.hpp"
#include "Scanf.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void Validate::Program_Control (void)
{
	int i, increment;
	char *str_ptr, buffer [FIELD_BUFFER];
	Range_Data *range_ptr;
	Label_Type type_rec;
	Db_File label_file;

	char *method_code [] = {
		"VOLUME", "VMT"
	};

	//---- open network files ----

	Demand_Service::Program_Control ();

	//---- check for traffic counts ----

	str_ptr = Get_Control_String (TRAFFIC_COUNT_FILE);

	if (str_ptr != NULL) {
		link_flag = true;

		if (Report_Flag (AREA_TYPE) || Report_Flag (ZONE_GROUP)) {
			if (!Network_File_Flag (NODE) || !Network_File_Flag (ZONE)) {
				Error ("Node and Zone Files are Required for Area Type and Zone Group Reports");
			}
			zone_flag = true;
		}
	} else {
		if (Report_Flag (VOLUME_LEVEL) || Report_Flag (FACILITY_TYPE) ||
			Report_Flag (AREA_TYPE) || Report_Flag (ZONE_GROUP) ||
			Report_Flag (LINK_GROUP) || Report_Flag (GROUP_DETAILS)) {

			Error ("Traffic Count file is Required for Link-Based Reports");
		}
	}

	//---- check for turning movements ----

	str_ptr = Get_Control_String (TURN_COUNT_FILE);

	if (str_ptr != NULL) {
		if ((Report_Flag (TURN_MOVEMENT) || Report_Flag (TURN_LOS)) && 
			!Network_File_Flag (LANE_CONNECTIVITY)) {

			Error ("Lane Connectivity File is Required for Turning Movement Reports");
		}
		if (Report_Flag (TURN_LOS) && !(Network_File_Flag (SIGNALIZED_NODE) &&
			Network_File_Flag (TIMING_PLAN) && Network_File_Flag (PHASING_PLAN))) {

			Error ("Signal Data is Required for Turn Level of Service Analysis");
		}
		turn_flag = true;
	} else {
		if (Report_Flag (TURN_MOVEMENT) || Report_Flag (TURN_LOS)) {
			Error ("Turn Count File is Required for Turning Movement Reports");
		}
		Network_File_False (POCKET_LANE);
		Network_File_False (LANE_CONNECTIVITY);
		Network_File_False (SIGNALIZED_NODE);
		Network_File_False (TIMING_PLAN);
		Network_File_False (PHASING_PLAN);
	}

	//---- check for transit reports ----

	line_flag = Report_Flag (LINE_GROUP);
	stop_flag = (Report_Flag (STOP_GROUP) || Report_Flag (BOARD_GROUP) || Report_Flag (ALIGHT_GROUP));

	if (line_flag || stop_flag) {
		if (!Network_File_Flag (TRANSIT_STOP) || !Network_File_Flag (TRANSIT_ROUTE) ||
			!Network_File_Flag (TRANSIT_SCHEDULE) || !Network_File_Flag (TRANSIT_DRIVER) ||
			!Demand_File_Flag (RIDERSHIP)) {

			Error ("Transit Network and Ridership are Required for Transit Reports");
		}
		if (line_flag && Get_Control_String (LINE_EQUIVALENCE_FILE) == NULL) {
			Error ("Line Group Equivalance is Required for Line Group Reports");
		}
		if (line_flag && Get_Control_String (LINE_GROUP_COUNT_FILE) == NULL) {
			Error ("Line Group Counts are Required for Line Group Reports");
		}
		if (stop_flag && Get_Control_String (STOP_EQUIVALENCE_FILE) == NULL) {
			Error ("Stop Group Equivalance is Required for Stop Group Reports");
		}
		if (stop_flag && Get_Control_String (STOP_GROUP_COUNT_FILE) == NULL) {
			Error ("Stop Group Counts are Required for Stop Group Reports");
		}
	} else {
		Network_File_False (TRANSIT_STOP);
		Network_File_False (TRANSIT_ROUTE);
		Network_File_False (TRANSIT_SCHEDULE);
		Network_File_False (TRANSIT_DRIVER);
		Demand_File_False (RIDERSHIP);
	}

	//---- check for a link delay file ----

	if (!Demand_File_Flag (LINK_DELAY)) {

		//---- open the link volume file ----

		if (link_flag) {
			str_ptr = Get_Control_String (INPUT_VOLUME_FILE);

			if (str_ptr == NULL) goto control_error;

			volume_file.File_Type ("Input Volume File");
			Print (1);

			volume_file.Open (Project_Filename (str_ptr));
		}

		//---- open the turn movement file ----

		if (turn_flag) {
			str_ptr = Get_Control_String (TURN_VOLUME_FILE);

			if (str_ptr == NULL) goto control_error;
			Print (1);
			turn_volume.File_Type ("Turn Volume File");

			turn_volume.Open (Project_Filename (str_ptr));
		}
	} else {
		delay_flag = true;
	}

	//---- open the traffic count file ----

	if (link_flag) {
		str_ptr = Get_Control_String (TRAFFIC_COUNT_FILE);

		Print (1);
		count_file.File_Type ("Traffic Count File");

		count_file.Open (Project_Filename (str_ptr));
	}

	//---- open the turn count file ----

	if (turn_flag) {
		str_ptr = Get_Control_String (TURN_COUNT_FILE);
		Print (1);
		turn_count.File_Type ("Turn Count File");

		turn_count.Open (Project_Filename (str_ptr));
	}

	//---- open the transit line group count file ----

	if (line_flag) {
		str_ptr = Get_Control_String (LINE_GROUP_COUNT_FILE);

		Print (1);
		line_count_file.File_Type ("Transit Line Group Count File");

		line_count_file.Open (Project_Filename (str_ptr));

		line_count_file.Required_Field ("GROUP", "LINE", "LINEGROUP", "LINEGRP", "ID");
		line_count_file.Required_Field ("COUNT", "RIDERS", "PASSENGERS", "TOTAL", "VOLUME");
	}

	//---- open the transit stop group count file ----

	if (stop_flag) {
		str_ptr = Get_Control_String (STOP_GROUP_COUNT_FILE);

		Print (1);
		stop_count_file.File_Type ("Transit Stop Group Count File");

		stop_count_file.Open (Project_Filename (str_ptr));

		stop_count_file.Required_Field ("GROUP", "STOP", "STOPGROUP", "STOPGRP", "ID");

		if (Report_Flag (STOP_GROUP)) {
			stop_count_file.Required_Field ("TOTAL",  "ONOFF", "ON_OFF", "PASSENGERS", "COUNT");
		}
		if (Report_Flag (BOARD_GROUP)) {
			stop_count_file.Required_Field ("BOARDINGS", "BOARD", "ON", "PASSENGERS", "COUNT");
		}
		if (Report_Flag (ALIGHT_GROUP)) {
			stop_count_file.Required_Field ("ALIGHTINGS", "ALIGHT", "OFF", "PASSENGERS", "COUNT");
		}
	}

	//---- create output volume file ----

	str_ptr = Get_Control_String (OUTPUT_VOLUME_FILE);

	if (str_ptr != NULL) {
		if (!delay_flag && !link_flag) {
			Error ("Link Delay or Volume File is Required to Output Volume Data");
		}
		output_file.File_Type ("Output Volume File");
		Print (1);

		output_file.Create (Project_Filename (str_ptr));
		output_flag = true;
	}

	//---- get the summary time increment ----
	
	str_ptr = Get_Control_String (SUMMARY_TIME_INCREMENT);

	if (str_ptr == NULL) {
		increment = -1;
	} else {
		increment = atoi (str_ptr);

		if (increment < 0 || increment > MIDNIGHT / 60) {
			Error ("Summary Time Increment %d is Out of Range (0..%d)", increment, MIDNIGHT / 60);
		}
		if ((increment % 15) > 0) {
			Error ("Summary Time Increment %d must be a multiple of 15 minutes", increment);
		}
		increment *= 60;
	}
	if (increment > 0) {
		time_periods.Increment (increment);
		time_periods.Period_Flag (true);
	}
	time_periods.Format ("24_HOUR_CLOCK");

	//---- get the summary time range ----

	str_ptr = Get_Control_String (SUMMARY_TIME_PERIODS);

	if (str_ptr == NULL) {
		str_ptr = "0:00..24:00";
	}
	Print (2, "Summary Time Periods = %s", str_ptr);

	if (!time_periods.Add_Ranges (str_ptr)) {
		File_Error ("Summary Time Periods", str_ptr);
	}
	if (time_periods.Num_Ranges () == 0) {
		Error ("Summary Time Periods is Zero");
	}
	if (increment >= 0) {
		Print (1, "Summary Time Increment = %d (minutes)", increment / 60);
	}

	//---- adjust the volume ranges ----

	hours = 24.0 * time_periods.Max_Count () / MIDNIGHT;

	if (hours < 12.0) {
		double adjust = (hours <= 4.0) ? 0.1 : 0.5;

		for (i=0; volume_level [i] > 0; i++) {
			volume_level [i] = (int) (volume_level [i] * adjust + 0.5);
		}
	}
	hours = hours / time_periods.Num_Ranges ();

	//---- check the data fields ----

	if (link_flag) {
		str_ptr = "Traffic Count";

		if (count_file.Num_Ranges () < time_periods.Num_Ranges ()) goto field_error;

		for (range_ptr = time_periods.First (); range_ptr; range_ptr = time_periods.Next ()) {
			if (!count_file.In_Range (range_ptr->Low ()) ||
				!count_file.In_Range ((range_ptr->Low () + range_ptr->High () + 1) / 2)) goto field_error;
		}
		if (!delay_flag) {
			str_ptr = "Input Volume";

			if (volume_file.Num_Ranges () < time_periods.Num_Ranges ()) goto field_error;

			for (range_ptr = time_periods.First (); range_ptr; range_ptr = time_periods.Next ()) {
				if (!volume_file.In_Range (range_ptr->Low ()) ||
					!volume_file.In_Range ((range_ptr->Low () + range_ptr->High () + 1) / 2)) goto field_error;
			}
		}
	}

	//---- read the analysis method ----

	str_ptr = Get_Control_String (ANALYSIS_METHOD);

	if (str_ptr != NULL) {
		if (str_cmp (str_ptr, method_code [1]) == 0) {
			method = true;
		} else if (str_cmp (str_ptr, method_code [0]) != 0) {
			Error ("Analysis Method = %s was Unrecognized", str_ptr);
		}
	}
	Print (2, "Analysis Method = %s Based", method_code [((method) ? 1 : 0)]);
	
	//---- read the adjustment factor ----

	str_ptr = Get_Control_String (ADJUSTMENT_FACTOR);

	if (str_ptr != NULL) {
		factor = atof (str_ptr);

		if (factor < 0.05 || factor > 10.0) {
			Error ("Adjustment Factor %.2lf is Out of Range (0.05..10.0)", factor);
		}
		Print (1, "Adjustment Factor = %.2lf", factor);
	}

	//---- open the link equivalence ----

	if (Report_Flag (LINK_GROUP)) {
		str_ptr = Get_Control_String (LINK_EQUIVALENCE_FILE);

		if (str_ptr == NULL) {
			Error ("A Link Equivalence File is Required for Link Group Reports");
		}
		Print (1);
		link_equiv.Open (Project_Filename (str_ptr));
	}

	//---- open the zone equivalence ----

	if (Report_Flag (ZONE_GROUP)) {
		str_ptr = Get_Control_String (ZONE_EQUIVALENCE_FILE);

		if (str_ptr == NULL) {
			Error ("A Zone Equivalence File is Required for Zone Group Reports");
		}
		Print (1);
		zone_equiv.Open (Project_Filename (str_ptr));
	}

	//---- open the line equivalence ----

	if (line_flag || stop_flag) {
		Print (1);

		if (line_flag) {
			str_ptr = Get_Control_String (LINE_EQUIVALENCE_FILE);
			line_equiv.Open (Project_Filename (str_ptr));
		}

		//---- open the stop equivalence ----

		if (stop_flag) {
			str_ptr = Get_Control_String (STOP_EQUIVALENCE_FILE);
			stop_equiv.Open (Project_Filename (str_ptr));
		}
	}

	//---- open the facility type labels ----

	if (Report_Flag (FACILITY_TYPE)) {
		str_ptr = Get_Control_String (FACILITY_TYPE_LABELS);

		if (str_ptr != NULL) {
			label_file.File_Type ("Facility Type Label File");
			Print (1);

			label_file.Open (Project_Filename (str_ptr));

			while (label_file.Read ()) {

				str_ptr = label_file.Clean_Record ();

				if (*str_ptr == '\0') continue;

				str_ptr = Get_Integer (str_ptr, &i);

				type_rec.type = i;

				i = (int) (strlen (str_ptr)) + 1;

				type_rec.label = new char [i];

				if (type_rec.label == NULL) goto memory_error;

				str_cpy (type_rec.label, i, str_ptr);

				if (!facility_type.Add (&type_rec)) goto memory_error;
			}
			label_file.Close ();
		}
	}

	//---- open the area type labels ----

	if (Report_Flag (AREA_TYPE)) {
		str_ptr = Get_Control_String (AREA_TYPE_LABELS);

		if (str_ptr != NULL) {
			label_file.File_Type ("Area Type Label File");
			Print (1);

			label_file.Open (Project_Filename (str_ptr));

			while (label_file.Read ()) {

				str_ptr = label_file.Clean_Record ();

				if (*str_ptr == '\0') continue;

				str_ptr = Get_Integer (str_ptr, &i);

				type_rec.type = i;

				i = (int) (strlen (str_ptr)) + 1;

				type_rec.label = new char [i];

				if (type_rec.label == NULL) goto memory_error;

				str_cpy (type_rec.label, i, str_ptr);

				if (!area_type.Add (&type_rec)) goto memory_error;
			}
			label_file.Close ();
		}
	}

	//---- get facility selection ----
	
	str_ptr = Get_Control_String (SELECT_FACILITY_TYPES);

	if (str_ptr != NULL) {
		Print (2, "Select Facility Types = %s", str_ptr);

		memset (type_flag, '\0', sizeof (type_flag));
		select_flag = true;

		while (str_ptr != NULL) {
			str_ptr = Get_Token (str_ptr, buffer);

			if (buffer [0] == '\0') break;

			type_flag [Facility_Code (buffer)] = true;
		}
	} else {
		for (i=0; i <= EXTERNAL; i++) {
			type_flag [i] = true;
		}
	}

	//---- write the report names ----

	List_Reports ();

	//---- read the zone equiv ----

	if (Report_Flag (ZONE_GROUP)) {
		zone_equiv.Read (Report_Flag (ZONE_EQUIV));
	}

	//---- process support data ----

	if (Report_Flag (LINK_GROUP) || Report_Flag (GROUP_DETAILS)) {
		link_equiv.Read (Report_Flag (LINK_EQUIV));
	}

	//---- read the line equiv ----

	if (line_flag) {
		line_equiv.Read (Report_Flag (LINE_EQUIV));
	}

	//---- read the stop equiv ----

	if (stop_flag) {
		stop_equiv.Read (Report_Flag (STOP_EQUIV));
	}
	return;

	//---- error message ----

field_error:
	Error ("The %s File has insufficient data for %d Time Increments", str_ptr, time_periods.Num_Ranges ());

memory_error:
	Error ("Insufficient Memory for Label Data");

control_error:
	Error ("Missing Control Key = %s", Current_Key ());
} 
