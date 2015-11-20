//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "ActGen.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void ActGen::Execute (void)
{
	int nproblem;
	HHold_Data *household_ptr;
	
	//---- create the purpose map ----

	max_purpose = purpose_group.Max_Purpose ();

	if (max_purpose >= MAX_PURPOSE) {
		Error ("Too Many Purpose Codes (%d > %d)", max_purpose, MAX_PURPOSE-1);
	}

	for (int i=1; i <= max_purpose; i++) {
		Purpose_Group *group_ptr;

		for (group_ptr = purpose_group.First (); group_ptr; group_ptr = purpose_group.Next ()) {
			if (group_ptr->In_Range (i)) {
				if (purpose_map [i] != 0) {
					Error ("Purpose %d was assigned to Multiple Groups", i);
				}
				purpose_map [i] = purpose_group.Record_Index ();
			}
		}
	}

	//---- read the user program scripts ----

	if (hhold_type_flag || survey_type_flag || choice_flag) {
		Read_Scripts ();
		Flush ();
	}
	
	//---- read the survey files ----

	Read_Survey ();

	//---- read the household list ----

	if (hhlist_flag) {
		Household_List ();
	}

	//---- read the network and travel data ----

	if (update_flag) {
		Demand_File_False (ACTIVITY);
	}

	Demand_Service::Execute ();

	if (zero_zone > 0) {
		Print (1);
		Warning ("%d Activity Locations have a Zone Number of Zero", zero_zone);
		Show_Message (1);
	}
	Flush ();

	//---- household type summary report ----

	if (Report_Flag (HOUSEHOLD_TYPE)) {
		Header_Number (HOUSEHOLD_TYPE);

		if (!Break_Check (type_data.Num_Records () + 4)) {
			Print (1);
			Page_Header ();
		}
		double ratio;
		Type_Data *type_ptr;

		for (type_ptr = type_data.First_Key (); type_ptr; type_ptr = type_data.Next_Key ()) {
			ratio = (double) type_ptr->Num_HHold () / type_ptr->Num_Survey ();

			Print (1, "%4d  %8d   %8d   %10.1lf", 
				type_ptr->Type (), type_ptr->Num_HHold (), type_ptr->Num_Survey (), ratio);
		}
		Header_Number (0);
		Flush ();
	}

	//---- read the balancing factor file ----

	if (balance_flag) {
		Read_Factors ();
	}

	//---- read skim data ----

	if (skim_flag) {
		Skim_Group *group;

		for (group = skim_group.First (); group; group = skim_group.Next ()) {
			if (zone_flag) {
				group->Num_Zones (zone_data.Num_Records ());
			}
			if (!group->Read_Skim (Report_Flag (TIME_EQUIV))) {
				Error ("Reading Zone Skim #%d", group->Group ());
			}
		}
		Flush ();
	}

	//---- generate activity ----

	Generate_Activity ();

	if (match_flag) {
		match_file.Close ();
	}
	if (count_flag) {
		Write_Count ();
	}

	//---- save the trip length files ----

	if (time_length_flag) {
		trip_time_length.Write_Trip_Length_File ();
	}
	if (distance_length_flag) {
		trip_distance_length.Write_Trip_Length_File ();
	}

	//---- print reports ----

	for (int i=First_Report (); i != 0; i=Next_Report ()) {
		switch (i) {
			case TRIP_LENGTH:
				trip_length.Report (TRIP_LENGTH, "Trip Length Summary", "Purpose");
				break;
			case TOUR_LENGTH:
				tour_length.Report (TOUR_LENGTH, "Tour Length Summary", "Purpose");
				break;
			case TRIP_PURPOSE:
				trip_purpose.Report (TRIP_PURPOSE, "Trip Purpose Summary", " ", "Purpose");
				break;
			case TOUR_PURPOSE:	
				tour_purpose.Report (TOUR_PURPOSE, "Tour Purpose Summary", "Purpose", "-Stops");
				break;
			case MODE_LENGTH:
				mode_length.Report (MODE_LENGTH, "Mode Length Summary", " Mode");
				break;
			case MODE_PURPOSE:	
				mode_purpose.Report (MODE_PURPOSE, "Mode Purpose Summary", " Mode-", "Purpose");
				break;
			case ACT_PROBLEM:
				act_problem_data.Report (ACT_PROBLEM, "Activity Problem Report", " Mode-", "Purpose");
				break;
			default:
				break;
		}
	}

	//---- purpose warnings ----

	Print (1);
	for (int i=1; i <= max_purpose; i++) {
		if (purpose_map [i] < 0) {
			Warning ("Purpose %d was Not Assigned to a Processing Group", i);
		}
	}
	
	//---- end the program ----

	if (new_pattern_flag) {
		Write (2, "Number of New Activity Pattern Records = %d", num_pattern);
	}
	nproblem = 0;

	for (household_ptr = household_data.First (); household_ptr; household_ptr = household_data.Next ()) {
		if (household_ptr->Problem_Flag ()) nproblem++;
	}
	if (nproblem) {
		Write (2, "Number of Households with Problems = %d (%.1lf%%)", nproblem,
			(100.0 * nproblem / household_data.Num_Records ()));
	}
	Report_Problems ();

	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void ActGen::Page_Header (void)
{
	switch (Header_Number ()) {
		case HOUSEHOLD_SCRIPT:		//---- Household Type Script ----
			Print (1, "Household Type Script");
			Print (1);
			break;
		case HOUSEHOLD_STACK:		//---- Household Type Stack ----
			Print (1, "Household Type Stack");
			Print (1);
			break;
		case HOUSEHOLD_TYPE:		//---- Household Type Summary ----
			Print (1, "Household Type Summary");
			Print (2, "Type  Households  Surveys  Households/Survey");
			Print (1);
			break;		
		case SURVEY_SCRIPT:			//---- Survey Type Script ----
			Print (1, "Survey Type Script");
			Print (1);
			break;
		case SURVEY_STACK:			//---- Survey Type Stack ----
			Print (1, "Survey Type Stack");
			Print (1);
			break;
		case SURVEY_TYPE:			//---- Survey Type Summary ----
			Print (1, "Survey Type Summary");
			Print (2, "Type  Households  Cumulative Weight  Average Weight");
			Print (1);
			break;
		case LOCATION_SCRIPT:		//---- Location Choice Script ----
			Print (1, "Location Choice Script #%d", purpose_number);
			Print (1);
			break;
		case LOCATION_STACK:		//---- Location Choice Stack ----
			Print (1, "Location Choice Stack #%d", purpose_number);
			Print (1);
			break;
		case HOUSEHOLD_MATCH:		//---- Household Match Report ----
			Print (1, "                                   Household Match Report");
			Print (1, "      ------------- Household ----------------  --------------- Survey -----------------");
			Print (1, "Type  Household Person Age Group Worker Gender  Household Person Age Group Worker Gender");
			Print (1);
			break;
		case TRIP_LENGTH:			//---- Trip Length Summary ----
			trip_length.Header ();
			break;
		case TOUR_LENGTH:			//---- Tour Length Summary ----
			tour_length.Header ();
			break;
		case TRIP_PURPOSE:			//---- Trip Purpose Summary ----
			trip_purpose.Header ();
			break;
		case TOUR_PURPOSE:			//---- Tour Purpose Summary ----
			tour_purpose.Header ();
			break;
		case MODE_LENGTH:			//---- Mode Length Summary ----
			mode_length.Header ();
			break;
		case MODE_PURPOSE:			//---- Mode Purpose Summary ----
			mode_purpose.Header ();
			break;
		case ACT_PROBLEM:			//---- Activity Problem Report ----
			act_problem_data.Header ();
		default:
			break;
	}
}
