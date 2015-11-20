//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "PlanSelect.hpp"

#include "Delay_File.hpp"
#include "Utility.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void PlanSelect::Program_Control (void)
{
	int i, nsel, lvalue;
	char *str_ptr, buffer [FIELD_BUFFER];
	double dvalue;
	Box box;

	//---- open network files ----

	Demand_Service::Program_Control ();

	//---- open the plan file ----
	
	if (Get_Control_String (NODE_LIST_PATHS) != NULL) {
		type_flag = Get_Control_Flag (NODE_LIST_PATHS);
	}
	str_ptr = Get_Control_String (PLAN_FILE);

	if (str_ptr == NULL) goto control_error;

	str_ptr = Project_Filename (str_ptr, Extension ());

	Print_Filename (2, plan_file.File_Type (), str_ptr);

	plan_file.Filename (str_ptr);

	str_ptr = Get_Control_String (PLAN_FORMAT);

	if (str_ptr != NULL) {
		plan_file.File_Format (str_ptr);
		Print (1, "%s Format = %s", plan_file.File_Type (), str_ptr);
	}
	plan_file.Node_Based_Flag (type_flag);

	if (!plan_file.Open (0)) {
		File_Error ("Opening Plan File", plan_file.Filename ());
	}
	type_flag = plan_file.Node_Based_Flag ();

	//---- node list paths ----

	if (type_flag) {
		Print (1, "Plan File contains Node List Paths");
	} else {
		Print (1, "Plan File contains Link List Paths");
	}

	//---- check for a link delay file ----

	if (Demand_File_Flag (LINK_DELAY)) {
		Delay_File *file = (Delay_File *) Demand_Db_File (LINK_DELAY);

		if (!Network_File_Flag (LINK) || !Network_File_Flag (NODE)) {
			Error ("Link and Node files are Required for Link Delay Processing");
		}
		if (file->Dbase_Format () == VERSION3) {
			turn_flag = false;
			Network_File_False (LANE_CONNECTIVITY);
		} else if (!Network_File_Flag (LANE_CONNECTIVITY)) {
			Warning ("Lane Connectivity is required for Turning Delays");
			Show_Message (1);
			turn_flag = false;
		} else {
			turn_flag = true;
		}
		delay_flag = true;
	} else {
		Network_File_False (LANE_CONNECTIVITY);
	}

	//---- open the population file ----

	str_ptr = Get_Control_String (POPULATION_FILE);

	if (str_ptr != NULL) {
		pop_file.File_Type ("Population File");
		Print (1);

		pop_file.Open (str_ptr);
		pop_flag = true;
	}

	//---- get the household list ----

	str_ptr = Get_Control_String (NEW_HOUSEHOLD_LIST);

	if (str_ptr != NULL) {
		hhlist_file.File_Type ("New Household List");
		hhlist_file.File_Access (CREATE);

		str_ptr = Project_Filename (str_ptr, Extension ());
		hhlist_file.Filename (str_ptr);

		Print_Filename (2, hhlist_file.File_Type (), str_ptr);

		if (!hhlist_file.Open (0)) {
			File_Error ("Opening Household List File", hhlist_file.Filename ());
		}
		if (hhlist_file.Extend () == 0 && plan_file.Extend () != 0) {
			Warning ("Household List File is Not Partitioned");
		}
	} else {
		str_ptr = Get_Control_String (NEW_HOUSEHOLD_RECORD_FILE);

		if (str_ptr == NULL) {
			Error ("A New Household List or Household Record File is Required");
		}
		hhrec_flag = true;
		hhrec_file.File_Access (CREATE);

		str_ptr = Project_Filename (str_ptr, Extension ());
		hhrec_file.Filename (str_ptr);

		Print_Filename (2, hhrec_file.File_Type (), str_ptr);

		str_ptr = Get_Control_String (NEW_HOUSEHOLD_RECORD_FORMAT);

		if (str_ptr != NULL) {
			hhrec_file.Dbase_Format (str_ptr);
			Print (1, "%s Format = %s", hhrec_file.File_Type (), str_ptr);
		}
		if (!hhrec_file.Open (0)) {
			File_Error ("Opening Household Record file", hhrec_file.Filename ());
		}
		if (hhrec_file.Extend () == 0 && plan_file.Extend () != 0) {
			Warning ("Household Record File is Not Partitioned");
		}
	}

	//---- get the select travelers ----
	
	str_ptr = Get_Control_String (SELECT_TRAVELERS);

	if (str_ptr != NULL) {
		Print (2, "Select Traveler IDs = %s", str_ptr);

		if (!travelers.Add_Ranges (str_ptr)) {
			File_Error ("Traveler ID Range", str_ptr);
		}
	}
	
	//---- get the select nodes ----

	nsel = Highest_Control_Group (SELECT_NODES_x, 0);

	if (nsel > 0) {

		nodes.Max_Records (nsel);

		for (i=1; i <= nsel; i++) {
			str_ptr = Get_Control_String (SELECT_NODES_x, i);

			if (str_ptr == NULL) continue;

			Print (2, "Select Nodes for Group #%d = %s", i, str_ptr);

			Data_Range *ptr = new Data_Range ();

			if (!ptr->Add_Ranges (str_ptr)) {
				File_Error ("Path Node Numbers", str_ptr);
			}
			if (!nodes.Add (ptr)) {
				Error ("Adding Node Ranges to the List");
			}
		}
	}
	
	//---- get the select links ----

	nsel = Highest_Control_Group (SELECT_LINKS_x, 0);

	if (nsel > 0) {

		links.Max_Records (nsel);

		for (i=1; i <= nsel; i++) {
			str_ptr = Get_Control_String (SELECT_LINKS_x, i);

			if (str_ptr == NULL) continue;

			Print (2, "Select Links for Group #%d = %s", i, str_ptr);

			Data_Range *ptr = new Data_Range (1, -MAX_INTEGER, MAX_INTEGER, 0);

			if (!ptr->Add_Ranges (str_ptr)) {
				File_Error ("Path Link Numbers", str_ptr);
			}
			if (!links.Add (ptr)) {
				Error ("Adding Link Ranges to the List");
			}
		}
	}

	//---- get the select time periods ----
	
	str_ptr = Get_Control_String (SELECT_TIME_PERIODS);

	if (str_ptr != NULL) {
		Print (2, "Select Time Periods = %s", str_ptr);

		times.Format (Time_Step::HOURS);

		if (!times.Add_Ranges (str_ptr)) {
			File_Error ("Time Period Range", str_ptr);
		}
	}
	
	//---- get the select travel modes ----

	str_ptr = Get_Control_String (SELECT_TRAVEL_MODES);

	if (str_ptr != NULL) {

		Print (2, "Select Travel Modes = %s", str_ptr);

		memset (mode_flag, '\0', sizeof (mode_flag));
		nsel = 0;

		while (str_ptr != NULL) {
			char *old_ptr = str_ptr;

			str_ptr = Get_Integer (str_ptr, &i);

			if (i < 0 || i > DRIVER_MODE || (i == 0 && *old_ptr != '0')) {
				Error ("Selected Travel Mode %s is Out of Range (0..%d)", old_ptr, DRIVER_MODE);
			}
			mode_flag [i] = true;
			nsel++;
		}
		if (nsel == 0) {
			Error ("No Modes were Selected for Processing");
		}
	} else {
		for (i=0; i <= DRIVER_MODE; i++) {
			mode_flag [i] = true;
		}
	}

	//---- get the select facility types ----

	str_ptr = Get_Control_String (SELECT_FACILITY_TYPES);

	if (str_ptr != NULL) {

		Print (2, "Select Facility Types = %s", str_ptr);

		factype_flag = true;
		memset (facility_flag, '\0', sizeof (facility_flag));
		nsel = 0;

		while (str_ptr != NULL) {
			str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));
			if (buffer [0] == '\0') break;

			i = Facility_Code (buffer);
			facility_flag [i] = true;
			nsel++;
		}
		if (nsel == 0) {
			Error ("No Facility Types were Selected for Processing");
		}
	}

	//---- get the select locations ----
	
	str_ptr = Get_Control_String (SELECT_LOCATIONS);

	if (str_ptr != NULL) {
		Print (2, "Select Activity Locations = %s", str_ptr);

		if (!locations.Add_Ranges (str_ptr)) {
			File_Error ("Activity Location Range", str_ptr);
		}
	}

	//---- get the select parking lots ----
	
	str_ptr = Get_Control_String (SELECT_PARKING_LOTS);

	if (str_ptr != NULL) {
		Print (2, "Select Parking Lots = %s", str_ptr);

		if (!parking_lots.Add_Ranges (str_ptr)) {
			File_Error ("Parking Lot Range", str_ptr);
		}
	}

	//---- get the select transit stops ----
	
	str_ptr = Get_Control_String (SELECT_TRANSIT_STOPS);

	if (str_ptr != NULL) {
		Print (2, "Select Transit Stops = %s", str_ptr);

		if (!transit_stops.Add_Ranges (str_ptr)) {
			File_Error ("Transit Stop Range", str_ptr);
		}
	}

	//---- get the select transit routes ----
	
	str_ptr = Get_Control_String (SELECT_TRANSIT_ROUTES);

	if (str_ptr != NULL) {
		Print (2, "Select Transit Routes = %s", str_ptr);

		if (!transit_routes.Add_Ranges (str_ptr)) {
			File_Error ("Transit Route Range", str_ptr);
		}
	}

	//---- get the select vc ratio ----

	str_ptr = Get_Control_String (SELECT_VC_RATIOS);

	if (str_ptr != NULL) {
		if (!delay_flag) {
			Error ("Select V/C Ratios requires a Link Delay file");
		}
		vc_flag = true;
		Get_Double (str_ptr, &vc_ratio);

		if (vc_ratio < 1.0) {
			Error ("Select V/C Ratios %.2lf is Out of Range (>= 1.0)", vc_ratio);
		}
		Print (2, "Select V/C Ratios >= %.2lf", vc_ratio);
	}

	//---- get the select time ratio ----

	str_ptr = Get_Control_String (SELECT_TIME_RATIOS);

	if (str_ptr != NULL) {
		if (!delay_flag) {
			Error ("Select Travel Time Ratios requires a Link Delay file");
		}
		ratio_flag = true;
		Get_Double (str_ptr, &time_ratio);

		if (time_ratio < 1.0) {
			Error ("Select Time Ratios %.2lf is Out of Range (>= 1.0)", time_ratio);
		}
		Print (2, "Select Time Ratios >= %.2lf", time_ratio);
	}

	//---- get the select coordinates ----

	nsel = Highest_Control_Group (SELECT_COORDINATES_x, 0);

	for (i=0; i <= nsel; i++) {
		if (i == 0) {
			str_ptr = Get_Control_String (SELECT_COORDINATES);
		} else {
			str_ptr = Get_Control_String (SELECT_COORDINATES_x, i);
		}
		if (str_ptr == NULL) continue;

		if (!Network_File_Flag (NODE)) {
			Error ("Select Coordinates requires a Node file");
		}
		coord_flag = true;

		if (i == 0) {
			Print (2, "Select Coordinates = %s", str_ptr);
		} else {
			if (coord_box.Num_Records () == 0) Print (1);

			Print (1, "Select Coordinates #%d = %s", i, str_ptr);
		}
		str_ptr = Get_Double (str_ptr, &dvalue);
		box.x1 = Round (dvalue);

		if (str_ptr == NULL) goto coord_error;

		str_ptr = Get_Double (str_ptr, &dvalue);
		box.y1 = Round (dvalue);

		if (str_ptr == NULL) goto coord_error;

		str_ptr = Get_Double (str_ptr, &dvalue);
		box.x2 = Round (dvalue);

		if (box.x2 < box.x1) {
			lvalue = box.x2;
			box.x2 = box.x1;
			box.x1 = lvalue;
		}
		if (str_ptr == NULL) goto coord_error;

		str_ptr = Get_Double (str_ptr, &dvalue);
		box.y2 = Round (dvalue);

		if (box.y2 < box.y1) {
			lvalue = box.y2;
			box.y2 = box.y1;
			box.y1 = lvalue;
		}
		if (!coord_box.Add (&box)) goto mem_error;
	}

	//---- get the select od coordinates ----

	nsel = Highest_Control_Group (SELECT_OD_COORDINATES_x, 0);

	for (i=0; i <= nsel; i++) {
		if (i == 0) {
			str_ptr = Get_Control_String (SELECT_OD_COORDINATES);
		} else {
			str_ptr = Get_Control_String (SELECT_OD_COORDINATES_x, i);
		}
		if (str_ptr == NULL) continue;

		if (!Network_File_Flag (NODE)) {
			Error ("Select OD Coordinates requires a Node file");
		}
		in_flag = true;

		if (i == 0) {
			Print (2, "Select OD Coordinates = %s", str_ptr);
		} else {
			if (od_box.Num_Records () == 0) Print (1);

			Print (1, "Select OD Coordinates #%d = %s", i, str_ptr);
		}
		str_ptr = Get_Double (str_ptr, &dvalue);
		box.x1 = Round (dvalue);

		if (str_ptr == NULL) goto coord_error;

		str_ptr = Get_Double (str_ptr, &dvalue);
		box.y1 = Round (dvalue);

		if (str_ptr == NULL) goto coord_error;

		str_ptr = Get_Double (str_ptr, &dvalue);
		box.x2 = Round (dvalue);

		if (box.x2 < box.x1) {
			lvalue = box.x2;
			box.x2 = box.x1;
			box.x1 = lvalue;
		}
		if (str_ptr == NULL) goto coord_error;

		str_ptr = Get_Double (str_ptr, &dvalue);
		box.y2 = Round (dvalue);

		if (box.y2 < box.y1) {
			lvalue = box.y2;
			box.y2 = box.y1;
			box.y1 = lvalue;
		}
		if (!od_box.Add (&box)) goto mem_error;
	}

	//---- get the exclude od coordinates ----

	nsel = Highest_Control_Group (EXCLUDE_OD_COORDINATES_x, 0);

	for (i=0; i <= nsel; i++) {
		if (i == 0) {
			str_ptr = Get_Control_String (EXCLUDE_OD_COORDINATES);
		} else {
			str_ptr = Get_Control_String (EXCLUDE_OD_COORDINATES_x, i);
		}
		if (str_ptr == NULL) continue;

		if (!Network_File_Flag (NODE)) {
			Error ("Excludet OD Coordinates requires a Node file");
		}
		out_flag =  true;

		if (i == 0) {
			Print (2, "Exclude OD Coordinates = %s", str_ptr);
		} else {
			if (exclude_box.Num_Records () == 0) Print (1);

			Print (1, "Excludet OD Coordinates #%d = %s", i, str_ptr);
		}
		str_ptr = Get_Double (str_ptr, &dvalue);
		box.x1 = Round (dvalue);

		if (str_ptr == NULL) goto od_coord_error;

		str_ptr = Get_Double (str_ptr, &dvalue);
		box.y1 = Round (dvalue);

		if (str_ptr == NULL) goto od_coord_error;

		str_ptr = Get_Double (str_ptr, &dvalue);
		box.x2 = Round (dvalue);

		if (box.x2 < box.x1) {
			lvalue = box.x2;
			box.x2 = box.x1;
			box.x1 = lvalue;
		}
		if (str_ptr == NULL) goto od_coord_error;

		str_ptr = Get_Double (str_ptr, &dvalue);
		box.y2 = Round (dvalue);

		if (box.y2 < box.y1) {
			lvalue = box.y2;
			box.y2 = box.y1;
			box.y1 = lvalue;
		}
		if (!exclude_box.Add (&box)) goto mem_error;
	}

	//---- get the select subarea polygon ----

	str_ptr = Get_Control_String (SELECT_SUBAREA_POLYGON);

	if (str_ptr != NULL) {
		if (!Network_File_Flag (LINK) || !Network_File_Flag (NODE)) {
			Error ("Link and Node files are Required for Subarea Polygon Selection");
		}
		Print (1);
		select_subarea.File_Type ("Select Subarea Polygon");

		select_subarea.Open (Project_Filename (str_ptr));
		
		if (!select_subarea.Read_Record ()) {
			Error ("Reading %s", select_subarea.File_Type ());
		}
		subarea_flag = true;
	}

	//---- get the select vehicle types ----
	
	str_ptr = Get_Control_String (SELECT_VEHICLE_TYPES);

	if (str_ptr != NULL) {
		Print (2, "Select Vehicle Types = %s", str_ptr);

		if (!vehicle_types.Add_Ranges (str_ptr)) {
			File_Error ("Vehicle Type Range", str_ptr);
		}
		vehicle_flag = true;
		if (!Demand_File_Flag (VEHICLE)) {
			Error ("A Vehicle File is Required for Vehicle Type Selection");
		}
	} else {
		Demand_File_False (VEHICLE);
	}

	//---- get the percent time difference ----

	str_ptr = Get_Control_String (PERCENT_TIME_DIFFERENCE);

	if (str_ptr != NULL) {
		if (!delay_flag) {
			Error ("Percent Time Difference requires a Link Delay file");
		}
		diff_flag = true;
        Get_Double (str_ptr, &percent_diff);

		if (percent_diff < 0.1 || percent_diff > 100.0) {
			Error ("Percent Time Difference %.2lf is Out of Range (0.1-100.0)", percent_diff);
		}
		Print (2, "Percent Time Difference = %.1lf%%", percent_diff);

		percent_diff /= 100.0;

		//---- get the minimum time difference ----

		str_ptr = Get_Control_String (MINIMUM_TIME_DIFFERENCE);

		if (str_ptr != NULL) {
			Get_Integer (str_ptr, &min_time);

			if (min_time < 0 || min_time > 120) {
				Error ("Minimum Time Difference %d is Out of Range (0-120)", min_time);
			}
		}
		Print (1, "Minimum Time Difference = %d minute%s", 
			min_time, ((min_time != 1) ? "s" : " "));

		//---- get the maximum time difference ----

		str_ptr = Get_Control_String (MAXIMUM_TIME_DIFFERENCE);

		if (str_ptr != NULL) {
			Get_Integer (str_ptr, &max_time);

			if (max_time < min_time || max_time > 1440) {
				Error ("Maximum Time Difference %d is Out of Range (%d-1440)", max_time, min_time);
			}
		}
		Print (1, "Maximum Time Difference = %d minutes", max_time);

		min_time *= 60;
		max_time *= 60;
	}

	//---- get the selection percentage ----

	str_ptr = Get_Control_String (SELECTION_PERCENTAGE);

	if (str_ptr != NULL) {
		Get_Double (str_ptr, &percent);

		if (percent < 0.1 || percent > 100.0) {
			Error ("Selection Percentage %.2lf is Out of Range (0.1-100.0)", percent);
		}
		select_flag = (percent != 100.0);
	}
	Print (2, "Selection Percentage = %.1lf%%", percent);

	percent /= 100.0;

	//---- get the max percent selected ----

	str_ptr = Get_Control_String (MAXIMUM_PERCENT_SELECTED);

	if (str_ptr != NULL) {
		Get_Double (str_ptr, &max_percent);

		if (max_percent < 1.0 || max_percent > 100.0) {
			Error ("Maximum Percent Selected %.1lf is Out of Range (1.0-100.0)", max_percent);
		}
	}
	Print (1, "Maximum Percent Selected = %.1lf%%", max_percent);

	max_percent /= 100.0;

	//---- random number seed ----

	str_ptr = Get_Control_String (RANDOM_NUMBER_SEED);

	if (str_ptr != NULL) {
		random.Seed (atoi (str_ptr));
	}
	if (str_ptr != NULL || percent != 1.0 || max_percent != 1.0) {
		Print (1, "Random Number Seed = %d", random.Seed ());
	}

	//---- reset the network flags ----

	if (!vc_flag && !diff_flag && !ratio_flag) {
		delay_flag = parking_flag = turn_flag = false;
		Demand_File_False (LINK_DELAY);
		Network_File_False (LANE_USE);
		Network_File_False (PARKING);
		Network_File_False (LANE_CONNECTIVITY);

		if (!coord_flag && !out_flag && !in_flag && !subarea_flag && !factype_flag && 
			nodes.Num_Records () == 0 && links.Num_Records () == 0) {
			Network_File_False (LINK);
			Network_File_False (NODE);
			Network_File_False (TRANSIT_STOP);
			Network_File_False (TRANSIT_DRIVER);
		} else if (!factype_flag && type_flag && links.Num_Records () == 0) {
			Network_File_False (LINK);
		}
	} else {
		parking_flag = Network_File_Flag (PARKING);

		if (!diff_flag) {
			turn_flag = false;
			Network_File_False (LANE_CONNECTIVITY);
		}
	}
	transit_flag = (Network_File_Flag (TRANSIT_STOP) && Network_File_Flag (TRANSIT_DRIVER));

	return;

	//---- error message ----

control_error:
	Error ("Missing Control Key = %s", Current_Key ());

coord_error:
	Error ("Select Coordinates are Incomplete");

od_coord_error:
	Error ("Exclude OD Coordinates are Incomplete");

mem_error:
	Error ("Insufficient Memory for Coordinate Groups");
} 
