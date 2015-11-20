//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "TripSum.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void TripSum::Execute (void)
{
	int nfile;

	//---- read the network ----

	Demand_Service::Execute ();

	//---- read the household list ----

	if (hhlist_flag) {
		Household_List (0);
	}

	//---- read the household person counts ----

	if (count_flag) {
		Read_Count ();
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

	//---- save the trip length files ----

	if (time_length_flag) {
		trip_time_length.Write_Trip_Length_File ();
	}
	if (distance_length_flag) {
		trip_distance_length.Write_Trip_Length_File ();
	}

	//---- write the link trip end file ----

	if (volume_flag) {
		Write_Volume ();
	}

	//---- write the location trip end file ----

	if (loc_end_flag) {
		Write_Locations ();
	}

	//---- write the zone trip end file ----

	if (zone_end_flag) {
		Write_Zones ();
	}

	//---- write the trip time file ----

	if (time_flag) {
		Diurnal_Distribution ();
	}

	//---- write the trip table ----

	if (table_flag) {
		Write_Trips ();
	}

	//---- print reports ----

	Show_Message ("Writing Performance Reports");	

	for (int i=First_Report (); i != 0; i=Next_Report ()) {
		switch (i) {
			case TOP_100_LINKS:		//---- Top 100 Link Trip Ends ----
				Top_100_Report (TOP_100_LINKS);
				break;
			case TOP_100_LANES:		//---- Top 100 Lane Trip Ends ----
				Top_100_Report (TOP_100_LANES);
				break;
			case TOP_100_VC_RATIOS:	//---- Top 100 Trip Capacity Ratios ----
				Top_100_Report (TOP_100_VC_RATIOS);
				break;
			case TRIP_TIME:		//---- Trip Time Report ----
				Trip_Time ();
				break;
			case TRIP_LENGTH:
				trip_length.Report (TRIP_LENGTH, "Trip Length Summary", "Purpose");
				break;
			case TRIP_PURPOSE:
				trip_purpose.Report (TRIP_PURPOSE, "Trip Purpose Summary", " ", "Purpose");
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
	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void TripSum::Page_Header (void)
{
	switch (Header_Number ()) {
		case TOP_100_LINKS:		//---- Top 100 Link Trip End Report ----
			Top_100_Header (TOP_100_LINKS);
			break;
		case TOP_100_LANES:		//---- Top 100 Lanes Trip End Report ----
			Top_100_Header (TOP_100_LANES);
			break;
		case TRIP_TIME:		//---- Trip Time Report ----
			Trip_Time_Header ();
			break;
		case TRIP_LENGTH:			//---- Trip Length Summary ----
			trip_length.Header ();
			break;
		case TRIP_PURPOSE:			//---- Trip Purpose Summary ----
			trip_purpose.Header ();
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
