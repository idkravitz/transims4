//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "ModeChoice.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void ModeChoice::Execute (void)
{
	int nfile;
	
	//---- read the model scripts ----

	Read_Scripts ();
	
	//---- read the household list ----

	if (hhlist_flag) {
		Household_List ();
	}

	//---- read the network ----

	Demand_Service::Execute ();

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

	//---- process each activity file ----

	if (activity_flag) {
		for (nfile=0; ; nfile++) {
			if (!Read_Activity (nfile)) break;
		}
		if (nfile == 0) {
			Error ("No Input Activity Files were Found");
		}
	}

	//---- process each trip file ----

	if (trip_flag) {
		for (nfile=0; ; nfile++) {
			if (!Read_Trip (nfile)) break;
		}
		if (nfile == 0) {
			Error ("No Input Trip Files were Found");
		}
	}

	//---- summarize results ----

	if (activity_flag) {
		if (!Break_Check (3)) {
			Print (1);
		}
		Print (1, "Total Number of Activity Records Read = %d", tot_act_in);
		if (new_act_flag) {
			Print (1, "Total Number of Activity Records Written = %d", tot_act_out);
		}
		Print (1, "Number of Activity Records with New Modes = %d", tot_act_mode);
	}
	if (trip_flag) {
		if (!Break_Check (3)) {
			Print (1);
		}
		Print (1, "Total Number of Trip Records Read = %d", tot_trip_in);
		if (new_trip_flag) {
			Print (1, "Total Number of Trip Records Written = %d", tot_trip_out);
		}
		Print (1, "Number of Trip Records with New Modes = %d", tot_trip_mode);
		Print (1, "Number of IntraZonal Trips = %d", tot_intra);
	}
	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void ModeChoice::Page_Header (void)
{
	switch (Header_Number ()) {
		case TRIP_SCRIPT:		//---- Trip Mode Script ----
			Print (1, "Trip Mode Model #%d Script", report_code);
			Print (1);
			break;
		case TRIP_STACK:		//---- Trip Mode Stack ----
			Print (1, "Trip Mode Model #%d Stack", report_code);
			Print (1);
			break;
		case TOUR_SCRIPT:		//---- Tour Mode Script ----
			Print (1, "Tour Mode Model #%d Script", report_code);
			Print (1);
			break;
		case TOUR_STACK:		//---- Tour Mode Stack ----
			Print (1, "Tour Mode Model #%d Stack", report_code);
			Print (1);
			break;
		case SHARE_DETAILS:		//---- Mode Share Details -----
			Share_Details_Header ();
			break;
		default:
			break;
	}
}
