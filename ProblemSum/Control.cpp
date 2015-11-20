//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "ProblemSum.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void ProblemSum::Program_Control (void)
{
	int type, increment;
	char *str_ptr, *format_ptr, buffer [FIELD_BUFFER];

	//---- open network files ----

	Demand_Service::Program_Control ();

	net_flag = (Network_File_Flag (LINK) && Network_File_Flag (NODE));

	//---- open the plan file ----

	str_ptr = Get_Control_String (PROBLEM_FILE);

	if (str_ptr == NULL) goto control_error;

	format_ptr = Get_Control_String (PROBLEM_FORMAT);

	if (format_ptr != NULL) {
		problem_file.File_Format (format_ptr);
	}
	if (Partition ()) {
		problem_file.Filename (Project_Filename (str_ptr), Partition_Number ());
	} else {
		problem_file.Filename (Project_Filename (str_ptr));
	}
	Print_Filename (2, problem_file.File_Type (), problem_file.Groupname ());

	if (!problem_file.Open (0)) {
		File_Error (problem_file.File_Type (), problem_file.Filename ());
	}

	problem_db.Replicate_Fields (&problem_file, false);
	problem_db.File_ID ("Problem");

	//---- get the time of day format ----
	
	str_ptr = Get_Control_String (TIME_OF_DAY_FORMAT);

	if (str_ptr == NULL) {
		str_ptr = "24_HOUR_CLOCK";
	}
	if (!times.Format (str_ptr)) {
		Error ("Time of Day Format %s was Unrecognized", str_ptr);
	}
	Print (2, "Time of Day Format = %s", str_ptr);

	//---- get summary time increment ----
	
	str_ptr = Get_Control_String (SUMMARY_TIME_INCREMENT);

	if (str_ptr != NULL) {
		increment = atol (str_ptr);

		if (increment < 0 || increment > 240) {
			Error ("Summary Time Increment %d is Out of Range (0-240)", increment);
		}
		increment *= 60;
	} else {
		increment = 0;
	}
	times.Increment (increment);
	if (increment > 0) {
		times.Period_Flag (true);
	}

	//---- get the summary time periods ----
	
	str_ptr = Get_Control_String (SUMMARY_TIME_PERIODS);

	if (str_ptr != NULL) {
		time_flag = true;

		if (!times.Add_Ranges (str_ptr)) {
			File_Error ("Time Period Range", str_ptr);
		}
	} else {
		str_ptr = (char *) "0:00..24:00";
		times.Add_Ranges (str_ptr);
	}
	Print (2, "Summary Time Periods = %s", str_ptr);

	if (increment > 0) {
		Print (1, "Summary Time Increment = %d minutes", increment / 60);
	} else {
		Print (1, "One Summary Increment will be Output");
	}

	//---- get the select links ----
	
	str_ptr = Get_Control_String (SELECT_LINKS);

	if (str_ptr != NULL) {
		if (!net_flag) {
			Error ("Link and Node Files are Required for Link Selection");
		}
		Print (2, "Select Links = %s", str_ptr);

		if (!links.Add_Ranges (str_ptr)) {
			File_Error ("Link Range", str_ptr);
		}
		link_flag = true;
	}

	//---- get the select problem types ----
	
	str_ptr = Get_Control_String (SELECT_PROBLEM_TYPES);

	if (str_ptr != NULL) {
		Print (2, "Select Problem Types = %s", str_ptr);

		while (str_ptr != NULL) {
			str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));

			if (buffer [0] != '\0') {
				type = Problem_Code (buffer);
				str_fmt (buffer, sizeof (buffer), "%d", type);

				if (!types.Add_Ranges (buffer)) {
					File_Error ("Problem Types", str_ptr);
				}
				type_flag = true;
			}
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
		subarea_flag = location_flag = true;

		if (problem_file.Link_Field () > 0) {
			if (!net_flag) {
				Error ("Link and Node Files are Required for Select Subarea Polygon");
			}
		}
		if (!Network_File_Flag (ACTIVITY_LOCATION)) {
			Error ("An Activity Location file is Required for Select Subarea Polygon");
		}
	}

	//---- problem time file ----

	str_ptr = Get_Control_String (NEW_PROBLEM_TIME_FILE);

	if (str_ptr != NULL) {
		Print (1);
		time_file.File_Type ("New Problem Time File");
		problem_time_flag = true;

		format_ptr = Get_Control_String (NEW_PROBLEM_TIME_FORMAT);

		if (format_ptr != NULL) {
			time_file.Dbase_Format (format_ptr);
		} else {
			time_file.Dbase_Format (DEFAULT_FORMAT);
		}
		time_file.Create (Project_Filename (str_ptr));

		//---- add the zone index and coordinate fields ----

		time_file.Add_Field ("PROBLEM", INTEGER, 5);
		time_file.Add_Field ("START", STRING, TIME_BUFFER);
		time_file.Add_Field ("END", STRING, TIME_BUFFER);
		time_file.Add_Field ("COUNT", INTEGER, 10);
		time_file.Add_Field ("PERCENT", DOUBLE, 10, 2);

		time_file.Write_Header ();
	}

	//---- problem link file ----

	str_ptr = Get_Control_String (NEW_PROBLEM_LINK_FILE);

	if (str_ptr != NULL) {
		if (!net_flag) {
			Error ("Link and Node Files are Required for Problem Link Files");
		}
		Print (1);
		problem_link_flag = true;

		format_ptr = Get_Control_String (NEW_PROBLEM_LINK_FORMAT);

		if (format_ptr != NULL) {
			link_file.Dbase_Format (format_ptr);
		} else {
			link_file.Dbase_Format (DEFAULT_FORMAT);
		}
		link_file.Create (Project_Filename (str_ptr));
	}

	//---- check reports ----

	List_Reports ();

	if (Report_Flag (TIME_REPORT) && !times.Period_Flag ()) {
		Error ("Problem Time Reports required Time Increments");
	}

	if (Report_Flag (DENSITY_REPORT) || problem_link_flag) {
		for (type=First_Report (); type != 0; type=Next_Report ()) {
			if (type == DENSITY_REPORT) {
				if (Report_Data () < 1.0) {
					Error ("Problem Density Value %.1lf is Out of Range (> 1.0)", Report_Data ());
				}
			}
		}
		if (problem_file.Link_Field () > 0) {
			if (!net_flag) {
				Error ("Link and Node Files are Required for Link Summaries");
			}
			if (!location_flag) {
				location_flag = Network_File_Flag (ACTIVITY_LOCATION);
			}
		} else {
			if (!Network_File_Flag (ACTIVITY_LOCATION)) {
				Error ("An Activity Location file is Required for Problem Summaries");
			}
			location_flag = true;
		}
		dir_array = (Dir_Array *) new TTime_Array ();

		ttime_data.Periods (times.Num_Ranges ());
	}
	return;

	//---- error message ----

control_error:
	Error ("Missing Control Key = %s", Current_Key ());
} 
