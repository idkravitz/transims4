//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "LineSum.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void LineSum::Program_Control (void)
{
	int i, num;
	char *str_ptr, *inc_ptr;

	Select_Set *set_ptr;

	int period_code [] = { RUN_START, RUN_MID, RUN_END, SCHED_START, 
			SCHED_MID, SCHED_END };

	char *period_method [] = { "RUN_START", "RUN_MIDPOINT", "RUN_END", "SCHEDULE_START", 
			"SCHEDULE_MIDPOINT", "SCHEDULE_END", NULL };

	//---- open network files ----

	Demand_Service::Program_Control ();

	if (!Demand_File_Flag (RIDERSHIP)) {
		for (i=First_Report (); i != 0; i=Next_Report ()) {
			if (i != LINK_GROUP && i != LINK_EQUIV && i != LINE_EQUIV && i != STOP_EQUIV) {
				Error ("Selected Report Options require a Transit Ridership File");
			}
		}
	}

	//---- create each selection set ----

	set_ptr = select_set.New_Record (true);
	if (set_ptr == NULL) goto set_error;

	if (!select_set.Add ()) goto set_error;

	for (num = First_Report (); num != 0; num = Next_Report ()) {
		num = (int) Report_Data ();
		if (num == 0) continue;

		for (set_ptr = select_set.First (); set_ptr; set_ptr = select_set.Next ()) {
			if (set_ptr->Number () == num) break;
		}
		if (set_ptr != NULL) continue;

		set_ptr = select_set.New_Record (true);
		if (set_ptr == NULL) goto set_error;
		set_ptr->Number (num);

		if (!select_set.Add ()) goto set_error;
	}

	//---- get the selection set keys ----

	for (set_ptr = select_set.First (); set_ptr; set_ptr = select_set.Next ()) {

		num = set_ptr->Number ();

		if (num == 0) {
			Print (2, "Default Selection Set");
		} else {
			Print (2, "Selection Set #%d", num);
		}

		//---- get the selection description ----
		
		if (num == 0) {
			str_ptr = Get_Control_String (SELECTION_DESCRIPTION);
		} else {
			str_ptr = Get_Control_String (SELECTION_DESCRIPTION_x, num);
		}
		if (str_ptr != NULL) {
			Print (1, "Selection Description = %s", str_ptr);

			if (!set_ptr->Select_Label (str_ptr)) goto set_error;
		}

		//---- get the summary time periods ----
		
		if (num == 0) {
			str_ptr = Get_Control_String (SUMMARY_TIME_PERIODS);

			if (str_ptr == NULL) {
				str_ptr = (char *) "All";
			}
		} else {
			str_ptr = Get_Control_String (SUMMARY_TIME_PERIODS_x, num);
		}

		if (str_ptr != NULL) {
			Print (1, "Summary Time Periods = %s", str_ptr);

			if (num == 0) {
				inc_ptr = Get_Control_String (SUMMARY_TIME_INCREMENT);
			} else {
				inc_ptr = Get_Control_String (SUMMARY_TIME_INCREMENT_x, num);
			}
			if (!set_ptr->Time_Periods (str_ptr, inc_ptr)) goto set_error;
		}

		//---- get the time period method ----
		
		if (num == 0) {
			str_ptr = Get_Control_String (TIME_PERIOD_METHOD);

			if (str_ptr == NULL) {
				str_ptr = "RUN_MIDPOINT";
			}
		} else {
			str_ptr = Get_Control_String (TIME_PERIOD_METHOD_x, num);
		}
		if (str_ptr != NULL) {
			Print (1, "Time Period Method = %s", str_ptr);

			for (i=0; period_method [i] != NULL; i++) {
				if (str_cmp (str_ptr, period_method [i]) == 0) {
					set_ptr->Time_Method (period_code [i]);
					break;
				}
			}
			if (set_ptr->Time_Method () == 0) {
				Error ("Time Period Method %s was Unrecognized", str_ptr);
			}
		}

		//---- get the select transit modes ----
		
		if (num == 0) {
			str_ptr = Get_Control_String (SELECT_TRANSIT_MODES);
		} else {
			str_ptr = Get_Control_String (SELECT_TRANSIT_MODES_x, num);
		}
		if (str_ptr != NULL) {
			Print (1, "Select Transit Modes = %s", str_ptr);

			if (!set_ptr->Select_Modes (str_ptr)) goto set_error;
		}
		
		//---- get the select transit routes ----
		
		if (num == 0) {
			str_ptr = Get_Control_String (SELECT_TRANSIT_ROUTES);
		} else {
			str_ptr = Get_Control_String (SELECT_TRANSIT_ROUTES_x, num);
		}
		if (str_ptr != NULL) {
			Print (1, "Select Transit Routes = %s", str_ptr);

			if (!set_ptr->Select_Routes (str_ptr)) goto set_error;
		}
			
		//---- get the select subarea polygon ----

		if (num == 0) {
			str_ptr = Get_Control_String (SELECT_SUBAREA_POLYGON);
		} else {
			str_ptr = Get_Control_String (SELECT_SUBAREA_POLYGON_x, num);
		}
		if (str_ptr != NULL) {
			if (!set_ptr->Select_Subarea (str_ptr)) goto set_error;
			offset_flag = true;
		}
	}

	//---- open the run schedule file ----

	str_ptr = Get_Control_String (NEW_RUN_SCHEDULE_FILE);

	if (str_ptr != NULL) {
		run_flag = true;
		run_file.File_Type ("Run Schedule File");

		run_file.Dbase_Format ("TAB_DELIMITED");

		run_file.Add_Field ("PERIOD", STRING, TIME_BUFFER);
		run_file.Add_Field ("RUN_START", INTEGER, 10);
		run_file.Add_Field ("RUN_END", INTEGER, 10);
		run_file.Add_Field ("RUN_MID", INTEGER, 10);
		run_file.Add_Field ("SCHED_START", INTEGER, 10);
		run_file.Add_Field ("SCHED_END", INTEGER, 10);
		run_file.Add_Field ("SCHED_MID", INTEGER, 10);
		
		Print (1);
		run_file.Create (Project_Filename (str_ptr));
	}

	//---- open the run capacity file ----

	str_ptr = Get_Control_String (NEW_RUN_CAPACITY_FILE);

	if (str_ptr != NULL) {
		if (!Demand_File_Flag (VEHICLE_TYPE)) {
			Error ("A Vehicle Type File is required for Capacity File");
		}
		cap_flag = true;
		cap_file.File_Type ("Run Capacity File");

		cap_file.Dbase_Format ("TAB_DELIMITED");

		cap_file.Add_Field ("MODE", STRING, 16);
		cap_file.Add_Field ("ROUTE", INTEGER, 10);
		cap_file.Add_Field ("PERIOD", STRING, TIME_BUFFER);
		cap_file.Add_Field ("AVG_RUNS", INTEGER, 4);
		cap_file.Add_Field ("AVG_LOAD", INTEGER, 6);
		cap_file.Add_Field ("AVG_FAC", DOUBLE, 6, 2);
		cap_file.Add_Field ("CAP_RUNS", INTEGER, 4);
		cap_file.Add_Field ("CAP_LOAD", INTEGER, 6);
		cap_file.Add_Field ("CAP_FAC", DOUBLE, 6, 2);
		cap_file.Add_Field ("MAX_RUN", INTEGER, 4);
		cap_file.Add_Field ("MAX_LOAD", INTEGER, 6);
		cap_file.Add_Field ("MAX_FAC", DOUBLE, 6, 2);
		
		Print (1);
		cap_file.Create (Project_Filename (str_ptr));
	}

	//---- stop equivalance ----

	if (Report_Flag (STOP_GROUP) || Report_Flag (STOP_DETAIL)) {

		//---- open the stop equivalence ----

		str_ptr = Get_Control_String (STOP_EQUIVALENCE_FILE);

		if (str_ptr == NULL) {
			Error ("A Stop Equivalance File is needed for Stop Group Reports");
		}
		Print (1);
		stop_equiv.Open (Project_Filename (str_ptr));
	}

	//---- line equivalance ----

	if (Report_Flag (GROUP_SUM) || Report_Flag (GROUP_PERF)) {

		//---- open the line equivalence ----

		str_ptr = Get_Control_String (LINE_EQUIVALENCE_FILE);

		if (str_ptr == NULL) {
			Error ("A Line Equivalance File is needed for Line Group Reports");
		}
		Print (1);
		line_equiv.Open (Project_Filename (str_ptr));
	}

	//---- link equivalance ----

	if (Report_Flag (LINK_GROUP)) {

		//---- open the link equivalence ----

		str_ptr = Get_Control_String (LINK_EQUIVALENCE_FILE);

		if (str_ptr == NULL) {
			Error ("A Link Equivalance File is needed for Link Group Reports");
		}
		Print (1);
		link_equiv.Open (Project_Filename (str_ptr));
	}

	//---- read report types ----

	List_Reports ();

	for (i=First_Report (); i != 0; i=Next_Report ()) {
		if (i == PERF_REPORT || i == PERF_REPORT_x || i == GROUP_PERF) {
			distance_flag = true;
			break;
		}
	}
	for (i=First_Report (); i != 0; i=Next_Report ()) {
		if (i == CAP_REPORT || i == CAP_REPORT_x) {
			if (!Demand_File_Flag (VEHICLE_TYPE)) {
				Error ("A Vehicle Type File is required for Capacity Reports");
			}
			break;
		}
	}

	//---- print the equiv report ----

	if (Report_Flag (STOP_GROUP) || Report_Flag (STOP_DETAIL)) {
		stop_equiv.Read (Report_Flag (STOP_EQUIV));
	}
	if (Report_Flag (GROUP_SUM) || Report_Flag (GROUP_PERF)) {
		line_equiv.Read (Report_Flag (LINE_EQUIV));
	}
	if (Report_Flag (LINK_GROUP)) {
		link_equiv.Read (Report_Flag (LINK_EQUIV));
	}
	return;

set_error:
	Error ("Adding Records to the Selection Set Array");
} 
