//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "CheckSurvey.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void CheckSurvey::Execute (void)
{
	
	//---- read the household type script ----

	if (survey_type_flag) {
		Type_Script ();
		Flush ();
	}

	//---- read the household list ----

	if (hhlist_flag) {
		Household_List ();
	}	

	//---- read the survey files ----

	Survey_Households ();

	if (weight_flag) {
		Survey_Weights ();
	}
	Survey_Population ();

	Survey_Activity ();

	//---- print reports ----

	for (int i=First_Report (); i != 0; i=Next_Report ()) {
		switch (i) {
			case SURVEY_TYPE:		//---- Survey Type Summary ----
				Type_Report ();
				break;
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
			default:
				break;
		}
	}

	//---- end the program ----

	Print (2, "Number of Households = %d", nhhold);
	Print (1, "Number of Persons = %d", nperson);
	Print (1, "Number of Workers = %d", nwork);

	if (nhhold == 0) nhhold = 1;

	Print (2, "Persons per Household = %.2lf", (double) nperson / nhhold);
	Print (1, "Workers per Household = %.2lf", (double) nwork / nhhold);

	if (nperson == 0) nperson = 1;

	if (num_problem > 0 || num_act_gen != num_activity) {
		Print (2, "Number of Activities Generated = %d", num_act_gen);

		if (num_problem > 0) {
			Print (1, "Number of Activities with Problems = %d (%.1lf%%)", num_problem,
				(100.0 * num_problem / num_act_gen));
		}
		Print (1, "Number of Activities Written = %d (%.1lf%%)", num_activity,
			(100.0 * num_activity / num_act_gen));
	} else {
		Print (2, "Number of Activities = %d", num_activity);
	}
	Print (2, "Activities per Household = %.2lf", (double) num_activity / nhhold);
	Print (1, "Activities per Person = %.2lf", (double) num_activity / nperson);

	if (nproblem) {
		Write (2, "Number of Households with Problems = %d (%.1lf%%)", nproblem,
			(100.0 * nproblem / survey_hh_data.Num_Records ()));
	}
	Report_Problems ();

	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void CheckSurvey::Page_Header (void)
{
	switch (Header_Number ()) {
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
		default:
			break;
	}
}
