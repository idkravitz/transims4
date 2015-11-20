//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void Router::Execute (void)
{

	//---- read the household list ----

	if (hhlist_flag || hhrec_flag || hhold_flag) {
		Read_Household ();
	}

	//---- read the network ----

	Demand_Service::Execute ();

	//---- print the transit fares ----

	if (Report_Flag (FARE_DATA)) {
		Fare_Report ();
	}

	//---- prepare path building data ----

	Data_Processing ();

	//---- process each trip ----

	if (trip_flag) {
		Read_Trip ();

		Write (2, "Number of Trip Records = %d", nrecord);
		Write (1, "Number of Trips Processed = %d", nprocess);
	}

	//---- process each activity ----

	if (activity_flag) {
		Read_Activity ();

		Write (2, "Number of Activity Records = %d", nrecord);
		Write (1, "Number of Activities Processed = %d", nprocess);
	}

	//---- build specified routes ----

	if (!trip_flag && !activity_flag && select_od) {
		int mode = DRIVE_ALONE;

		if (mode_flag) {
			mode = new_mode;
		}
		if (mode == DRIVE_ALONE || mode == CARPOOL2 || mode == CARPOOL3 || mode == CARPOOL4) {
			Drive_Vines ();
		} else {
			Build_Paths ();
		}
		Write (2, "Number of Specified Paths = %d", nprocess);
	}

	//---- copy old plans ----

	if (old_plan_flag) {
		if (skim_flag) {
			Show_Message (2, "Updating Plans -- Record");
		} else {
			Show_Message (2, "Coping Remaining Plans -- Record");
		}
		Set_Progress (10000);

		do {
			if (old_plan_file.Traveler () > 0) {
				if (!skim_flag || (skim_flag && Update_Plan ())) {
					if (!plan_file.Write (old_plan_file.Plan ())) {
						Error ("Writing Plan File");
					}
				}
				Show_Progress ();
			}
		} while (old_plan_file.Read ());

		if (attribute_flag) {
			Write_Attribute ();
		}
		End_Progress ();
		old_plan_file.Close ();
	}

	//---- print the results ----

	Write (2, "Number of Households Processed = %d", nhh_proc);
	Write (1, "Number of Vehicle Trips Saved = %d", ntrips);

	if (update_flag) {
		Write (1, "Number of Travel Time Updates = %d", nupdates);
	}
	if (skim_flag) {
		Write (2, "Number of Updated Start Times = %d", nlegs);
		Write (1, "Number of Updated Transit Legs = %d", nlegs_transit);
		Write (1, "Number of Updated Auto Legs = %d", nlegs_auto);
	}
	if (old_plan_file.Num_Records () > 0) {
		Break_Check (5);

		Write (2, "Number of Input Plans = %d", old_plan_file.Num_Plans ());
		Write (1, "Number of Input Records = %d", old_plan_file.Num_Records ());
		Write (1, "Number of Input Travelers = %d", old_plan_file.Num_Travelers ());
		Write (1, "Number of Input Trips = %d", old_plan_file.Num_Trips ());
	}
	Break_Check (5);

	Write (2, "Number of Output Plans = %d", plan_file.Num_Plans ());
	Write (1, "Number of Output Records = %d", plan_file.Num_Records ());
	Write (1, "Number of Output Travelers = %d", plan_file.Num_Travelers ());
	Write (1, "Number of Output Trips = %d", plan_file.Num_Trips ());

	if (ignore_errors) {
		Write (2, "Number of Trips Replaced by Magic Moves = %d", nmagic);
	}
	if (ntransit) {
		Write (2, "Number of Illogical Transit Adjustments = %d", ntransit);
	}
	if (tot_trips > 0 && Num_Problems () > 0) {
		Write (2, "Percent of Total Trips with Problems = %.1lf%%", 100.0 * Num_Problems () / tot_trips);
	}
	
	//---- print the error report ----

	if (dump_flag) {
		dump_file.Close ();
	}
	Report_Problems ();

	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void Router::Page_Header (void)
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
		case FARE_DATA:				//---- Fare Data Report ----
			Fare_Header ();
			break;
		default:
			break;
	}
}
