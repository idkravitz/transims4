//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "TripPrep.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void TripPrep::Execute (void)
{
	int nfile;

	//---- read the network ----

	Demand_Service::Execute ();

	//---- set the data files ----

	if (script_flag) {
		Set_Files ();
	}

	//---- read the household list ----

	if (hhlist_flag) {
		Household_List (0);
	}

	//---- read the probability factors ----

	if (factor_flag) {
		Read_Factor ();
	}

	//---- read the zone target file ----

	if (synod_flag) {
		Read_Target ();
	}

	//---- process each activity file ----

	if (activity_flag) {
		for (nfile=0; ; nfile++) {
			if (!Read_Activity (nfile)) break;
		}
		if (nfile == 0) {
			Error ("No Input Activity Files were Found");
		}
		if (nfile > 1) {
			Print (2, "Total Number of Activity Records Read = %d", total_in);
			if (output_flag) {
				Print (1, "Total Number of Activity Records Written = %d", total_out);
			}
			Print (1, "Total Number of Trips Used for Reports = %d", total_used);
		}
	}

	//---- process each trip file ----

	if (trip_flag) {
		total_in = total_out = total_used = 0;

		if (activity_flag && hhlist_flag && hhlist_file.Extend ()) {
			Household_List (0);
		}
		for (nfile=0; ; nfile++) {
			if (!Read_Trip (nfile)) break;
		}
		if (nfile == 0) {
			Error ("No Input Trip Files were Found");
		}
		if (nfile > 1) {
			Print (2, "Total Number of Trip Records Read = %d", total_in);
			if (output_flag) {
				Print (1, "Total Number of Trip Records Written = %d", total_out);
			}
			Print (1, "Total Number of Trips Used for Reports = %d", total_used);
		}
	}

	//---- create the synthetic od ----

	if (synod_flag) {
		Synthetic_OD ();
	}

	//---- process the vehicle file ----

	if (vehicle_flag) {
		Read_Vehicle ();
	}

	//---- print reports ----

	//Show_Message ("Writing Performance Reports");	

	//for (int i=First_Report (); i != 0; i=Next_Report ()) {
	//	switch (i) {
	//		default:
	//			break;
	//	}
	//}
	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void TripPrep::Page_Header (void)
{
	switch (Header_Number ()) {
		case PRINT_SCRIPT:		//---- Data Processing Script ----
			Print (1, "Data Processing Script");
			Print (1);
			break;
		case PRINT_STACK:		//---- Data Processing Stack ----
			Print (1, "Data Processing Stack");
			Print (1);
			break;
		default:
			break;
	}
}
