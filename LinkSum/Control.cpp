//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "LinkSum.hpp"

#include "Location_File.hpp"
#include "Utility.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void LinkSum::Program_Control (void)
{
	int i, field, ngroup;
	bool default_flag, binary, delay_flag, index_flag;
	char *str_ptr, *format_ptr, buffer [FIELD_BUFFER], *field_ptr;
	Format_Type default_format;

	Location_File *location_file;
	Db_Field *fld;	

	default_flag = delay_flag = false;

	//---- open network files ----

	Demand_Service::Program_Control ();

	delay_flag = Demand_File_Flag (PERFORMANCE);

	if (First_Report () > 0 && !delay_flag) goto delay_error;

	//---- open the previous link delay ----
	
	str_ptr = Get_Control_String (PREVIOUS_LINK_DELAY_FILE);

	if (str_ptr != NULL) {
		previous_delay.File_Type ("Previous Link Delay File");
		Print (1);

		format_ptr = Get_Control_String (PREVIOUS_LINK_DELAY_FORMAT);

		if (format_ptr != NULL) {
			previous_delay.Dbase_Format (format_ptr);
		}
		previous_delay.Open (Project_Filename (str_ptr));
		previous_flag = true;
	}

	//---- get summary increment ----
	
	str_ptr = Get_Control_String (SUMMARY_TIME_INCREMENT);

	if (str_ptr != NULL) {
		increment = atol (str_ptr);

		if (increment < 0 || increment > 240) {
			Error ("Summary Time Increment %d is Out of Range (0-240)", increment);
		}
		increment *= 60;
	}
	if (increment == 0) {
		performance_data.Period_Size (MIDNIGHT);

		if (Network_File_Flag (LANE_CONNECTIVITY)) {
			connect_time.Period_Size (MIDNIGHT);
		}
	} else {
		time_periods.Period_Flag (true);
		performance_data.Period_Size (increment);

		if (Network_File_Flag (LANE_CONNECTIVITY)) {
			connect_time.Period_Size (increment);
		}
	}
	time_periods.Increment (increment);
	time_periods.Format ("24_HOUR_CLOCK");

	//---- get the summary time periods ----
	
	str_ptr = Get_Control_String (SUMMARY_TIME_PERIODS);

	if (str_ptr == NULL) {
		str_ptr = (char *) "All";
	}
	Print (2, "Summary Time Periods = %s", str_ptr);

	if (!time_periods.Add_Ranges (str_ptr)) {
		File_Error ("Time Period Range", str_ptr);
	}
	num_inc = time_periods.Num_Ranges ();
	if (num_inc < 1) num_inc = 1;

	cap_factor = (double) time_periods.Max_Count () / (3600.0 * num_inc);

	performance_data.Time_Periods (&time_periods);
	if (Network_File_Flag (LANE_CONNECTIVITY)) {
		connect_time.Time_Periods (&time_periods);
	}
	if (previous_flag) {
		performance_data.Layers (2);
		if (Network_File_Flag (LANE_CONNECTIVITY)) {
			connect_time.Layers (4);
		}
	}
	if (increment > 0) {
		Print (1, "Summary Time Increment = %d minutes", (increment / 60));
	} else {
		Print (1, "One Summary Increment will be Output");
	}

	//---- get minimum volume ----
	
	str_ptr = Get_Control_String (MINIMUM_LINK_VOLUME);

	if (str_ptr != NULL) {

		min_volume = atol (str_ptr);

		if (min_volume < 1) {
			Error ("Minimum Link Volume %d is Out of Range (> 0)", min_volume);
		}
	}
	Print (2, "Minimum Link Volume = %d vehicles/period", min_volume);

	//---- get congested time ratio ----
	
	str_ptr = Get_Control_String (CONGESTED_TIME_RATIO);

	if (str_ptr != NULL) {

		congested_ratio = atof (str_ptr);

		if (congested_ratio != 0.0 && congested_ratio <= 1.0 || congested_ratio > 10.0) {
			Error ("Congested Time Ratio %.2lf is Out of Range (1.0..10.0)", congested_ratio);
		}
	}
	if (congested_ratio == 0.0) {
		Print (2, "Congested Summaries are Disabled");
	} else {
		Print (2, "Congested Time Ratio = %.2lf loaded time / free flow time", congested_ratio);
	}
	performance_data.Congested_Ratio (congested_ratio);

	//---- get facility selection ----
	
	str_ptr = Get_Control_String (SELECT_FACILITY_TYPES);

	if (str_ptr != NULL) {
		Print (2, "Select Facility Types = %s", str_ptr);

		memset (funcl_flag, '\0', sizeof (funcl_flag));
		select_flag = true;

		while (str_ptr != NULL) {
			str_ptr = Get_Token (str_ptr, buffer);

			if (buffer [0] == '\0') break;

			funcl_flag [Facility_Code (buffer)] = true;
		}
	} else {
		for (i=0; i <= EXTERNAL; i++) {
			funcl_flag [i] = true;
		}
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
		subarea_flag = select_flag = true;

		if (!Network_File_Flag (NODE)) {
			Error ("Network Node file is Required for Select Subarea Polygon");
		}
	}

	//---- get the select by link group flag ----

	str_ptr = Get_Control_String (SELECT_BY_LINK_GROUP);

	if (str_ptr != NULL) {
		Print (2, "Select by Link Group = %s", str_ptr);

		group_select = Get_Control_Flag (SELECT_BY_LINK_GROUP);

		if (group_select) select_flag = true;
	}

	//---- default output format ----

	str_ptr = Get_Control_String (DEFAULT_OUTPUT_FORMAT);

	if (str_ptr != NULL) {
		default_format = Format_Code (str_ptr);
		default_flag = true;
		Print (2, "Default Output Format = %s", str_ptr);
	}

	//---- create link activity file ----

	str_ptr = Get_Control_String (NEW_LINK_ACTIVITY_FILE);

	if (str_ptr != NULL) {
		if (!Network_File_Flag (ACTIVITY_LOCATION)) {
			Error ("An Activity Location File is needed for the Link Activity File");
		}
		location_file = (Location_File *) Network_Db_File (ACTIVITY_LOCATION);
		binary = (location_file->Record_Format () == BINARY);

		Print (1);
		activity_file.File_Type ("New Link Activity File");
		
		format_ptr = Get_Control_String (NEW_LINK_ACTIVITY_FORMAT);

		if (format_ptr != NULL) {
			activity_file.Dbase_Format (format_ptr);
		} else if (default_flag) {
			activity_file.Dbase_Format (default_format);
		}
		activity_file.Create (Project_Filename (str_ptr));

		str_ptr = Get_Control_String (ACTIVITY_LOCATION_FIELDS);

		if (str_ptr == NULL) {
			Error ("Activity Location Fields are needed for the Link Activity File");
		}
		Print (1, "Activity Location Fields = %s", str_ptr);

		link_db.File_Type ("Household Database");

		//---- set required fields ----

		link_db.Add_Field ("LINK", INTEGER, 10);
		activity_file.Add_Field ("LINK", INTEGER, 10);

		//---- copy selected fields ----

		while (str_ptr) {
			str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));

			field = location_file->Field_Number (buffer);
			if (field == 0) {
				Error ("Field %s was Not Found in the Activity Location File", buffer);
			}
			field_list.Add (field);

			fld = location_file->Field (field);

			link_db.Add_Field (fld->Name (), fld->Type (), fld->Size (), fld->Decimal (), -1, binary);
			activity_file.Add_Field (fld->Name (), fld->Type (), fld->Size (), fld->Decimal (), -1, binary);
		}
		activity_file.Write_Header ();
		activity_flag = true;
	}

	//---- create zone travel file ----

	str_ptr = Get_Control_String (NEW_ZONE_TRAVEL_FILE);

	if (str_ptr != NULL) {
		if (!Network_File_Flag (ACTIVITY_LOCATION)) {
			Error ("An Activity Location File is needed for the Zone Travel File");
		}
		Print (1);
		zone_file.File_Type ("New Zone Travel File");
		
		format_ptr = Get_Control_String (NEW_ZONE_TRAVEL_FORMAT);

		if (format_ptr != NULL) {
			zone_file.Dbase_Format (format_ptr);
		} else if (default_flag) {
			zone_file.Dbase_Format (default_format);
		}
		zone_file.Create (Project_Filename (str_ptr));
		zone_flag = true;
	}

	//---- create zone group travel file ----

	str_ptr = Get_Control_String (NEW_GROUP_TRAVEL_FILE);

	if (str_ptr != NULL) {
		if (!Network_File_Flag (ACTIVITY_LOCATION)) {
			Error ("An Activity Location File is needed for the Group Travel File");
		}
		Print (1);
		group_file.File_Type ("New Group Travel File");
		
		format_ptr = Get_Control_String (NEW_GROUP_TRAVEL_FORMAT);

		if (format_ptr != NULL) {
			group_file.Dbase_Format (format_ptr);
		} else if (default_flag) {
			group_file.Dbase_Format (default_format);
		}
		group_file.Create (Project_Filename (str_ptr));
		group_flag = true;

		//---- get the zone equivalence file ----

		str_ptr = Get_Control_String (ZONE_EQUIVALENCE_FILE);

		if (str_ptr == NULL) {
			Error ("A Zone Equivalence File is Required for Zone Group File");
		}
		Print (1);
		zone_equiv.Open (Project_Filename (str_ptr));
	}

	if (!activity_flag && !zone_flag && !group_flag) {
		Network_File_False (ACTIVITY_LOCATION);
	}

	//---- open the link equivalence ----

	if (group_select || Report_Flag (LINK_GROUP) || Report_Flag (TRAVEL_TIME)) {
		str_ptr = Get_Control_String (LINK_EQUIVALENCE_FILE);

		if (str_ptr == NULL) {
			Error ("A Link Equivalence File is Required for Link Group Reports");
		}
		Print (1);
		link_equiv.Open (Project_Filename (str_ptr));
	}

	//---- get the number of direction groups ----

	ngroup = Highest_Control_Group (NEW_LINK_DIRECTION_FILE_x, 0);

	if (ngroup > 0) {
		if (!delay_flag) goto delay_error;

		//---- process each group ----

		for (i=1; i <= ngroup; i++) {
			str_ptr = Get_Control_String (NEW_LINK_DIRECTION_FILE_x, i);

			if (str_ptr != NULL) {
				str_fmt (buffer, sizeof (buffer), "New Link Direction File #%d", i);

				format_ptr = Get_Control_String (NEW_LINK_DIRECTION_FORMAT_x, i);

				field_ptr = Get_Control_String (NEW_LINK_DIRECTION_FIELD_x, i);
				if (field_ptr == NULL) {
					Error ("New Link Direction Field #%d is Missing", i);
				}
				index_flag = Get_Control_Flag (NEW_LINK_DIRECTION_INDEX_x, i);

				Link_Dir_Control (buffer, str_ptr, format_ptr, field_ptr, index_flag);

				Print (1, "New Link Direction Field #%d = %s", i, field_ptr);
				if (index_flag) {
					Print (1, "Link Direction Index Field was Added");
				}
			}
		}
	}

	//---- get the number of groups ----

	ngroup = Highest_Control_Group (NEW_LINK_DATA_FILE_x, 0);

	if (ngroup > 0) {
		if (!delay_flag) goto delay_error;		

		//---- process each group ----

		for (i=1; i <= ngroup; i++) {
			str_ptr = Get_Control_String (NEW_LINK_DATA_FILE_x, i);

			if (str_ptr != NULL) {
				str_fmt (buffer, sizeof (buffer), "New Link Data File #%d", i);

				format_ptr = Get_Control_String (NEW_LINK_DATA_FORMAT_x, i);

				field_ptr = Get_Control_String (NEW_LINK_DATA_FIELD_x, i);
				if (field_ptr == NULL) {
					Error ("New Link Data Field #%d is Missing", i);
				}
				Link_Data_Control (buffer, str_ptr, format_ptr, field_ptr);

				Print (1, "New Link Data Field #%d = %s", i, field_ptr);
			}
		}
	}

	//---- performance data file ----

	str_ptr = Get_Control_String (NEW_PERFORMANCE_DATA_FILE);

	if (str_ptr != NULL) {
		Print (1);
		detail_file.File_Type ("New Performance Data File");

		format_ptr = Get_Control_String (NEW_PERFORMANCE_DATA_FORMAT);

		if (format_ptr != NULL) {
			detail_file.Dbase_Format (format_ptr);
		} else if (default_flag) {
			detail_file.Dbase_Format (default_format);
		}
		detail_file.Add_Field ("MEASURE", STRING, 40);
		detail_file.Add_Field ("VALUE", DOUBLE, 12, 2);

		detail_file.Create (Project_Filename (str_ptr));
		detail_flag = true;
	}

	//---- the following keys are for backward compatibility only ----
	//---- create output volume file ----

	str_ptr = Get_Control_String (NEW_LINK_VOLUME_FILE);

	if (str_ptr != NULL) {
		if (!delay_flag) goto delay_error;

		format_ptr = Get_Control_String (NEW_LINK_VOLUME_FORMAT);

		Link_Data_Control ("New Link Volume File", str_ptr, format_ptr, "VOLUME");
	}

	//---- create link speed file ----
	
	str_ptr = Get_Control_String (NEW_LINK_SPEED_FILE);

	if (str_ptr != NULL) {
		if (!delay_flag) goto delay_error;

		format_ptr = Get_Control_String (NEW_LINK_SPEED_FORMAT);

		Link_Data_Control ("New Link Speed File", str_ptr, format_ptr, "SPEED");
	}

	//---- create link travel time file ----
	
	str_ptr = Get_Control_String (NEW_LINK_TRAVEL_TIME_FILE);

	if (str_ptr != NULL) {
		if (!delay_flag) goto delay_error;

		format_ptr = Get_Control_String (NEW_LINK_TRAVEL_TIME_FORMAT);

		Link_Data_Control ("New Link Travel Time File", str_ptr, format_ptr, "TRAVEL_TIME");
	}

	//---- create link vc file ----

	str_ptr = Get_Control_String (NEW_LINK_VC_RATIO_FILE);

	if (str_ptr != NULL) {
		if (!delay_flag) goto delay_error;
		
		format_ptr = Get_Control_String (NEW_LINK_VC_RATIO_FORMAT);

		Link_Data_Control ("New Link V/C Ratio File", str_ptr, format_ptr, "VC_RATIO");
	}

	//---- create link time ratio file ----

	str_ptr = Get_Control_String (NEW_LINK_TIME_RATIO_FILE);

	if (str_ptr != NULL) {
		if (!delay_flag) goto delay_error;

		format_ptr = Get_Control_String (NEW_LINK_TIME_RATIO_FORMAT);

		Link_Data_Control ("New Link Time Ratio File", str_ptr, format_ptr, "TIME_RATIO");
	}

	//---- read report types ----

	List_Reports ();

	if (!previous_flag && (Report_Flag (TIME_CHANGE) || Report_Flag (VOLUME_CHANGE) ||
		Report_Flag (TOP_TIME_CHANGE) || Report_Flag (TOP_VOL_CHANGE) || Report_Flag (RELATIVE_GAP))) {

		Error ("A Previous Link Delay File is Required for Change Reports");
	}

	//---- process support data ----

	if (group_select || Report_Flag (LINK_GROUP) || Report_Flag (TRAVEL_TIME)) {
		link_equiv.Read (Report_Flag (LINK_EQUIV));
	}

	if (group_flag) {
		zone_equiv.Read (Report_Flag (ZONE_EQUIV));
	}

	//---- allocate work space ----

	sum_bin = new double * [num_inc + 1];
	if (sum_bin == NULL) goto mem_error;

	for (i=0; i <= num_inc; i++) {
		sum_bin [i] = new double [NUM_SUM_BINS];
		if (sum_bin [i] == NULL) goto mem_error;
	}
	return;

	//---- error message ----

delay_error:
	Error ("A Link Delay File is Required");

mem_error:
	Error ("Insufficient Memory for Summary Bins");
} 
