//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "EventSum.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void EventSum::Execute (void)
{

	//---- read the network and demand files ----

	Demand_Service::Execute ();

	max_hhold = MAX_INTEGER / Traveler_Scale ();

	//---- read the household list ----

	if (hhlist_flag) {
		Read_HHList ();
	}

	//---- read input travel times ----

	if (time_in_flag) {
		Read_Times ();
	}

	//---- read activity file ----

	if (activity_flag) {
		Read_Activity ();
	}

	//---- read trip file ----

	if (trip_file_flag) {
		Read_Trips ();
	}

	//---- read the event file ----

	if (combine_flag) {
		Combine_Events ();
	} else {
		Read_Events ();
	}

	//---- select travelers ----

	if (select_flag) {
		Select_Travelers ();
	}

	//---- write travel time file ----

	if (time_file_flag) {
		Write_Events ();
	}

	//---- write time summary file ----

	if (time_sum_flag) {
		Write_Times ();
	}

	//---- write the link events file ----

	if (link_flag) {
		Write_Links ();
	}

	//---- write the time distribution file ----

	if (distrib_flag) {
		time_diff.Write_Distribution ();
	}

	//---- update plans ----

	if (update_flag) {
		Update_Plans ();
	}

	//---- write new household list ----

	if (new_hhold_flag) {
		Write_HHList ();
	}

	//---- print the comparison summary ----

	time_diff.Total_Summary ();

	//---- print reports ----

	Show_Message ("Writing Summary Reports");	

	for (int i=First_Report (); i != 0; i=Next_Report ()) {
		switch (i) {
			case TOP_100_TTIME:		//---- top 100 travel time differences ----
				Top_100_TTime_Report ();
				break;
			case TOTAL_DISTRIB:		//---- total difference distribution ----
			case TTIME_DISTRIB:		//---- time difference distributions ----
				time_diff.Time_Distribution (i);
				break;
			case PERIOD_SUM:		//---- time period summary ----
				time_diff.Period_Summary ();
				break;
			case TOP_100_LINK:
				Top_100_Link_Report ();
				break;
			case TOP_100_PERIOD:	//---- top 100 period events ----
				Top_100_Period_Report ();
				break;
			case LINK_EVENT:		//---- link event report -----
				Link_Event_Report ((int) Report_Data ());
				break;
			case GROUP_EVENT:		//---- group event report ----
				Group_Event_Report ((int) Report_Data ());
				break;
			case TRAVELER_SUM:		//---- traveler summary report ----
				Traveler_Sum_Report ();
				break;
			case EVENT_SUM:			//---- event summary report ----
				Event_Sum_Report ();
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

void EventSum::Page_Header (void)
{
	switch (Header_Number ()) {
		case TOP_100_TTIME:		//---- top 100 travel time differences ----
			Top_100_TTime_Header ();
			break;
		case TOTAL_DISTRIB:		//---- total difference distribution ----
		case TTIME_DISTRIB:		//---- time difference distributions ----
		case PERIOD_SUM:		//---- time period summary ----
			time_diff.Report_Header (Header_Number ());
			break;
		case TOP_100_LINK:
			Top_100_Link_Header ();
			break;
		case TOP_100_PERIOD:	//---- top 100 period events ----
			Top_100_Period_Header ();
			break;
		case LINK_EVENT:		//---- link event report -----
			Link_Event_Header ();
			break;
		case GROUP_EVENT:		//---- group event report ----
			Group_Event_Header ();
			break;
		case TRAVELER_SUM:		//---- traveler summary report ----
			Traveler_Sum_Header ();
			break;
		case EVENT_SUM:			//---- event summary report ----
			Event_Sum_Header ();
			break;
		default:
			break;
	}
}
