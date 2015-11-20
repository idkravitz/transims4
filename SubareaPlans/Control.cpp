//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "SubareaPlans.hpp"

#include "Delay_File.hpp"
#include "Utility.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void SubareaPlans::Program_Control (void)
{
	int num_zone, num_time;
	char *str_ptr;

	//---- open the plan file ----
	
	if (Get_Control_String (NODE_LIST_PATHS) != NULL) {
		type_flag = Get_Control_Flag (NODE_LIST_PATHS);
	}
	str_ptr = Get_Control_String (PLAN_FILE);

	if (str_ptr == NULL) goto control_error;

	input_plans.File_Type ("Input Plan File");

	str_ptr = Project_Filename (str_ptr, Extension ());

	Print_Filename (2, input_plans.File_Type (), str_ptr);

	input_plans.Filename (str_ptr);

	str_ptr = Get_Control_String (PLAN_FORMAT);

	if (str_ptr != NULL) {
		input_plans.File_Format (str_ptr);
		Print (1, "%s Format = %s", input_plans.File_Type (), str_ptr);
	}
	input_plans.Node_Based_Flag (type_flag);

	if (!input_plans.Open (0)) {
		File_Error ("Opening Input Plan File", input_plans.Filename ());
	}
	type_flag = input_plans.Node_Based_Flag ();

	//---- node list paths ----

	if (type_flag) {
		Print (1, "Plan File contains Node List Paths");
	} else {
		Print (1, "Plan File contains Link List Paths");
	}
	
	//---- open network files ----

	Demand_Service::Program_Control ();

	if (Network_File_Flag (TRANSIT_STOP) || Network_File_Flag (TRANSIT_ROUTE) ||
		Network_File_Flag (TRANSIT_SCHEDULE) || Network_File_Flag (TRANSIT_DRIVER) ||
		Network_File_Flag (NEW_TRANSIT_STOP) || Network_File_Flag (NEW_TRANSIT_ROUTE) ||
		Network_File_Flag (NEW_TRANSIT_SCHEDULE) || Network_File_Flag (NEW_TRANSIT_DRIVER)) {

		transit_flag = (Network_File_Flag (TRANSIT_STOP) && Network_File_Flag (TRANSIT_ROUTE) &&
			Network_File_Flag (TRANSIT_SCHEDULE) && Network_File_Flag (TRANSIT_DRIVER));

		output_flag = (Network_File_Flag (NEW_TRANSIT_STOP) && Network_File_Flag (NEW_TRANSIT_ROUTE) &&
			Network_File_Flag (NEW_TRANSIT_SCHEDULE) && Network_File_Flag (NEW_TRANSIT_DRIVER));

		if (!transit_flag) {
			Error ("Existing Transit Stop, Route, Schedule, and Driver Files are Required");
		}
		if (!output_flag) {
			if (Network_File_Flag (NEW_TRANSIT_STOP) || Network_File_Flag (NEW_TRANSIT_ROUTE) ||
				Network_File_Flag (NEW_TRANSIT_SCHEDULE) || Network_File_Flag (NEW_TRANSIT_DRIVER)) {
				Error (" New Transit Stop, Route, Schedule, and Driver Files are Required");
			}
		}
	}
	boundary_flag = Demand_File_Flag (NEW_BOUNDARY_SPEED);

	//---- get the subarea boundary ----

	str_ptr = Get_Control_String (SUBAREA_BOUNDARY_POLYGON);

	if (str_ptr == NULL) goto control_error;
	Print (1);
	subarea_boundary.File_Type ("Subarea Boundary Polygon");

	subarea_boundary.Open (Project_Filename (str_ptr));
	
	if (!subarea_boundary.Read_Record ()) {
		Error ("Reading the Boundary Polygon");
	}

	//---- open the new plans ----

	str_ptr = Get_Control_String (NEW_PLAN_FILE);

	if (str_ptr == NULL) goto control_error;

	str_ptr = Project_Filename (str_ptr, Extension ());

	subarea_plans.File_Type ("New Plan File");
	subarea_plans.File_Access (Db_Code::CREATE);
	subarea_plans.Filename (str_ptr);

	Print_Filename (2, subarea_plans.File_Type (), str_ptr);

	str_ptr = Get_Control_String (NEW_PLAN_FORMAT);

	if (str_ptr != NULL) {
		subarea_plans.File_Format (str_ptr);
		Print (1, "%s Format = %s", subarea_plans.File_Type (), str_ptr);
	}
	subarea_plans.Node_Based_Flag (type_flag);

	if (!subarea_plans.Open (0)) {
		File_Error ("Opening Subarea Plan File", subarea_plans.Filename ());
	}
	extend_flag = (subarea_plans.Extend () > 0);

	//---- copy new file fields ----

	if (Network_File_Flag (NEW_PARKING)) {
		parking_flag = true;
		Network_Copy_Header (PARKING, NEW_PARKING);
		parking_file = (Parking_File *) Network_Db_Base (NEW_PARKING);
	}

	if (Network_File_Flag (NEW_ACTIVITY_LOCATION)) {
		location_flag = true;
		Network_Copy_Header (ACTIVITY_LOCATION, NEW_ACTIVITY_LOCATION);
		location_file = (Location_File *) Network_Db_Base (NEW_ACTIVITY_LOCATION);
	}

	if (Network_File_Flag (NEW_PROCESS_LINK)) {
		access_flag = true;
		Network_Copy_Header (PROCESS_LINK, NEW_PROCESS_LINK);
		access_file = (Access_File *) Network_Db_Base (NEW_PROCESS_LINK);
	}

	if (output_flag) {
		Network_Copy_Header (TRANSIT_STOP, NEW_TRANSIT_STOP);
		Network_Copy_Header (TRANSIT_ROUTE, NEW_TRANSIT_ROUTE);
		Network_Copy_Header (TRANSIT_SCHEDULE, NEW_TRANSIT_SCHEDULE);
		Network_Copy_Header (TRANSIT_DRIVER, NEW_TRANSIT_DRIVER);
		stop_file = (Stop_File *) Network_Db_Base (NEW_TRANSIT_STOP);
		line_file = (Line_File *) Network_Db_Base (NEW_TRANSIT_ROUTE);
		schedule_file = (Schedule_File *) Network_Db_Base (NEW_TRANSIT_SCHEDULE);
		driver_file = (Driver_File *) Network_Db_Base (NEW_TRANSIT_DRIVER);
	}

	Demand_Copy_Header (VEHICLE, NEW_VEHICLE);

	//---- get the household list ----

	str_ptr = Get_Control_String (NEW_HOUSEHOLD_LIST);

	if (str_ptr != NULL) {
		str_ptr = Project_Filename (str_ptr, Extension ());

		hhlist_file.File_Type ("New Household List");
		hhlist_file.File_Access (Db_Code::CREATE);
		hhlist_file.Filename (str_ptr);		

		if (subarea_plans.Extend () > 0 && hhlist_file.Extend () == 0) {
			hhlist_file.Extend (subarea_plans.Extend ());
		}
		Print_Filename (2, hhlist_file.File_Type (), str_ptr);

		if (!hhlist_file.Open (0)) {
			File_Error ("Opening New Household List", hhlist_file.Filename ());
		}
		hhlist_flag = true;
	}

	//---- early arrival flag ----

	early_flag = Get_Control_Flag (RETIME_EARLY_ARRIVALS);

	if (early_flag) {
		Print (2, "Activity Schedules will be Adjusted for Trips that Arrive Early");
	} else {
		Print (2, "Activity Schedules will Not be Adjusted for Trips that Arrive Early");
	}

	//---- open the zone and time period equivalence files ----
	
	if (Report_Flag (TRIP_REPORT)) {
		trip_flag = true;

		str_ptr = Get_Control_String (ZONE_EQUIVALENCE_FILE);

		if (str_ptr != NULL) {
			Print (1);
			zone_equiv.Open (Project_Filename (str_ptr));
			zone_flag = true;
		}

		str_ptr = Get_Control_String (TIME_PERIOD_EQUIVALENCE);

		if (str_ptr != NULL) {
			Print (1);
			time_equiv.Open (Project_Filename (str_ptr));
			time_flag = true;
		}
	}

	//---- get the time of day format ----

	str_ptr = Get_Control_String (TIME_OF_DAY_FORMAT);

	if (str_ptr == NULL) {
		str_ptr = "24_HOUR_CLOCK";
	}
	if (!time_step.Format (str_ptr) || !time_range.Format (str_ptr)) {
		Error ("Time of Day Format %s was Unrecognized", str_ptr);
	}
	Print (2, "Time of Day Format = %s", str_ptr);

	//---- get the select time periods ----
	
	str_ptr = Get_Control_String (SELECT_TIME_PERIODS);

	if (str_ptr != NULL) {
		Print (2, "Select Time Periods = %s", str_ptr);
		range_flag = true;

		if (!time_range.Add_Ranges (str_ptr)) {
			File_Error ("Time Period Range", str_ptr);
		}
	}

	//---- read report types ----

	List_Reports ();

	if (zone_flag) {
		zone_equiv.Read (Report_Flag (ZONE_EQUIV));
		num_zone = zone_equiv.Num_Groups ();
	} else {
		num_zone = 2;
	}
	if (time_flag) {
		time_equiv.Read (Report_Flag (TIME_EQUIV));
		num_time = time_equiv.Num_Periods ();
	} else {
		num_time = 1;
	}

	//---- allocate space ----

	if (trip_flag) {
		trips = new int ** [num_time+1];

		for (int p=1; p <= num_time; p++) {
			trips [p] = new int * [num_zone+1];

			for (int i=1; i <= num_zone; i++) {
				trips [p] [i] = new int [num_zone+1];

				memset (trips [p] [i], '\0', sizeof (int) * (num_zone + 1));
			}
		}
	}

	//---- check for a link delay file ----

	if (Demand_File_Flag (LINK_DELAY)) {
		delay_flag = true;

		Delay_File *file = (Delay_File *) Demand_Db_File (LINK_DELAY);

		if (file->Dbase_Format () != VERSION3) {
			if (!Network_File_Flag (LANE_CONNECTIVITY)) {
				Warning ("Lane Connectivity is needed for Turn Delay processing");
				Show_Message (1);
			} else {
				turn_flag = true;
			}
		} else {
			Network_File_False (LANE_CONNECTIVITY);
		}
	} else {
		Network_File_False (LANE_CONNECTIVITY);
	}
	return;

	//---- error message ----

control_error:
	Error ("Missing Control Key = %s", Current_Key ());
} 
