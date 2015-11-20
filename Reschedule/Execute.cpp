//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "Reschedule.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void Reschedule::Execute (void)
{

	//---- read the network ----

	Demand_Service::Execute ();

	//---- factor transit service ----

	if (service_flag) {
		Factor_Service ();

		Exit_Stat (DONE);
	}

	//---- segment schedule factor ----

	if (equiv_flag) {
		Segment_Schedule ();

		Exit_Stat (DONE);
	}

	//---- read the run start time file ----

	if (run_flag) {
		Read_Run_Start ();
	}

	//---- calculate and write the new schedules ----

	Write_Schedule ();

	//---- write the distribution file ----

	if (diff_flag) {
		time_diff.Write_Distribution ();
	}

	//---- print reports ----

	for (int i=First_Report (); i != 0; i=Next_Report ()) {
		switch (i) {
			case TOTAL_CHANGE:		//---- total change distribution ----
			case PERIOD_CHANGE:		//---- period change distributions ----
				time_diff.Time_Distribution (i);
				break;
			case PERIOD_SUM:		//---- time period summaries ----
				time_diff.Period_Summary ();
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

void Reschedule::Page_Header (void)
{
	switch (Header_Number ()) {
		case TOTAL_CHANGE:		//---- total change distribution ----
		case PERIOD_CHANGE:		//---- period change distributions ----
		case PERIOD_SUM:		//---- time period summary ----
			time_diff.Report_Header (Header_Number ());
		default:
			break;
	}
}
