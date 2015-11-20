//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "TransitNet.hpp"

#include "Parking_File.hpp"
#include "Location_File.hpp"
#include "Access_File.hpp"

#include "Route_Nodes_File.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void TransitNet::Program_Control (void)
{
	int i;
	char *str_ptr, buffer [FIELD_BUFFER];
	double dvalue;

	Route_Nodes_File *nodes_file;

	//---- open the park & ride file ----

	str_ptr = Get_Control_String (PARK_AND_RIDE_FILE);

	if (str_ptr != NULL) {
		parkride_flag = true;
		parkride_file.File_Format ((Db_Code::Format_Type) Default_Format ());
		parkride_file.File_Type ("Park and Ride File");

		parkride_file.Open (Project_Filename (str_ptr));

		Network_File_True (PARKING);
		Network_File_True (NEW_PARKING);
	} else {
		Network_Option_False (PARKING);
		Network_Option_False (NEW_PARKING);
	}

	//---- open the zone equivalence ----

	str_ptr = Get_Control_String (ZONE_EQUIVALENCE_FILE);

	if (str_ptr != NULL) {
		Print (1);
		zone_equiv.Open (Project_Filename (str_ptr));
		equiv_flag = true;
	}

	//---- open network files ----

	Network_Service::Program_Control ();
	
	connect_flag = Network_File_Flag (LANE_CONNECTIVITY);

	nodes_file = (Route_Nodes_File *) Network_Db_File (ROUTE_NODES);

	dwell_flag = nodes_file->Dwell_Flag ();
	time_flag = nodes_file->Time_Flag ();
	speed_flag = nodes_file->Speed_Flag ();

	//---- copy headers ----

	if (parkride_flag) {
		Network_Copy_Header (PARKING, NEW_PARKING);
	}
	Network_Copy_Header (ACTIVITY_LOCATION, NEW_ACTIVITY_LOCATION);
	Network_Copy_Header (PROCESS_LINK, NEW_PROCESS_LINK);
	Network_Copy_Header (TRANSIT_STOP, NEW_TRANSIT_STOP);

	//---- get the min spacing length ----

	memset (min_stop_spacing, '\0', (MAX_AREA_TYPE+1) * sizeof (int));
	Print (1);

	str_ptr = Get_Control_String (STOP_SPACING_BY_AREATYPE);

	if (str_ptr != NULL) {
		for (i=1; i <= MAX_AREA_TYPE && str_ptr != NULL; i++) {
			str_ptr = Get_Integer (str_ptr, &(min_stop_spacing [i]));

			if (min_stop_spacing [i] == 0) continue;

			if (min_stop_spacing [i] < MIN_LENGTH || min_stop_spacing [i] > 3200) {
				Error ("Minimum Stop Spacing %d is Out of Range (%.0lf-3200)", 
					min_stop_spacing [i], MIN_LENGTH);
			}
			Print (1, "Minimum Stop Spacing in Area Type %d = %d meters", i, min_stop_spacing [i]);

			min_stop_spacing [i] = Round (min_stop_spacing [i]);
		}
	}

	//---- get the time periods ----

	str_ptr = Get_Control_String (TRANSIT_TIME_PERIODS);

	if (str_ptr != NULL) {
		Print (2, "Transit Time Period Breaks = %s", str_ptr);
	} else {
		str_ptr = "None";
		Warning ("Only One Transit Time Period 0:00..24:00");
	}
	if (!time_period.Add_Breaks (str_ptr)) {
		Error ("Adding Transit Time Period Breaks");
	}
	num_periods = time_period.Num_Ranges ();

	//---- set the time factors ----

	time_factor = new double [num_periods];

	if (time_factor == NULL) {
		Error ("Insufficient Memory for %d Time Factors", num_periods);
	}
	for (i=0; i < num_periods; i++) {
		time_factor [i] = 1.0;
	}

	//---- read the time factor key ----

	str_ptr = Get_Control_String (TRANSIT_TRAVEL_TIME_FACTOR);

	if (str_ptr != NULL) {
		Print (1, "Transit Travel Time Factors = %s", str_ptr);
		i = 0;

		while (str_ptr != NULL) {
			str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));
			if (buffer [0] == '\0') break;

			if (i >= num_periods) break;

			dvalue = atof (buffer);

			if (dvalue < 0.5 || dvalue > 3.0) {
				Error ("Travel Time Factor %.2lf is Out of Range (0.5-3.0)", dvalue);
			}
			time_factor [i++] = dvalue;
		}
	}

	//---- minimum dwell time ----

	str_ptr = Get_Control_String (MINIMUM_DWELL_TIME);

	if (str_ptr != NULL) {
		Get_Integer (str_ptr, &min_dwell);

		if (min_dwell < 1 || min_dwell > 300) {
			Error ("Minimum Dwell Time %d is Out of Range (1..300)", min_dwell);
		}
	}
	Print (2, "Minimum Dwell Time = %d seconds", min_dwell);

	//---- intersection stop type ----

	str_ptr = Get_Control_String (INTERSECTION_STOP_TYPE);

	if (str_ptr != NULL) {
		stop_type = Stop_Code (str_ptr);

		if (stop_type != NEARSIDE && stop_type != FARSIDE) {
			Error ("Intersection Stop Type %s is not NEARSIDE or FARSIDE", str_ptr);
		}
	}
	Print (2, "Intersection Stop Type = %s", Stop_Code ((Stop_Type) stop_type));

	//---- intersection stop offset ----

	str_ptr = Get_Control_String (INTERSECTION_STOP_OFFSET);

	if (str_ptr != NULL) {
		stop_offset = atoi (str_ptr);

		if (stop_offset < 0 || stop_offset > 100) {
			Error ("Intersection Stop Offset %d is Out of Range", stop_offset);
		} else if (stop_type == FARSIDE && stop_offset < 10) {
			Warning ("Farside Stops and Stop Offset %d < 10 meters", stop_offset);
		}
	}
	Print (1, "Intersection Stop Offset = %d meters", stop_offset);	

	stop_offset = Round (stop_offset);

	//---- process link time ----

	str_ptr = Get_Control_String (TRANSIT_PROCESS_LINK_TIME);

	if (str_ptr != NULL) {
		access_time = atoi (str_ptr);

		if (access_time < 0 || access_time > 900) {
			Error ("Transit Process Link Time %d is Out of Range (0..900)", access_time);
		}
	}
	Print (2, "Transit Process Link Time = %d seconds", access_time);

	//---- random number seed ----

	str_ptr = Get_Control_String (RANDOM_NUMBER_SEED);

	if (str_ptr != NULL) {
		random.Seed (atoi (str_ptr));
	}
	Print (2, "Random Number Seed = %d", random.Seed ());

	//---- write the report names ----

	List_Reports ();

	//---- read the zone equiv ----

	if (equiv_flag) {
		zone_equiv.Read (Report_Flag (ZONE_EQUIV));
	}
} 
