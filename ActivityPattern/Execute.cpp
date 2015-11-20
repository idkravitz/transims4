//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "ActivityPattern.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void ActivityPattern::Execute (void)
{
	int nproblem;
	HHold_Data *household_ptr;

	//---- read the user program scripts ----

	if (hhold_type_flag || survey_type_flag) {
		Read_Scripts ();
		Flush ();
	}
	
	//---- read the survey files ----

	Read_Survey ();

	//---- read the household list ----

	if (hhlist_flag) {
		Household_List ();
	}

	Demand_Service::Execute ();
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

	//---- generate activity ----

	Generate_Activity ();

	if (match_flag) {
		match_file.Close ();
	}
	if (count_flag) {
		Write_Count ();
	}
		
	//---- end the program ----

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

void ActivityPattern::Page_Header (void)
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
		case HOUSEHOLD_MATCH:		//---- Household Match Report ----
			Print (1, "                                   Household Match Report");
			Print (1, "      ------------- Household ----------------  --------------- Survey -----------------");
			Print (1, "Type  Household Person Age Group Worker Gender  Household Person Age Group Worker Gender");
			Print (1);
			break;
		default:
			break;
	}
}
