//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "LineSum.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void LineSum::Execute (void)
{

	//---- read the network ----

	Demand_Service::Execute ();

	if (run_flag) {
		Run_Schedule ();
	}
	if (cap_flag) {
		Write_Capacity ();
	}

	//---- calculate transit leg length ----

	if (distance_flag) {
		Build_Legs ();
	}

	//---- print reports ----

	for (int i=First_Report (); i != 0; i=Next_Report ()) {
		switch (i) {
			case LINE_RIDER:		//---- Line Ridership Report ----
			case LINE_RIDER_x:
				Line_Rider_Report ((int) Report_Data ());
				break;
			case LINE_SUM:			//---- Line Summary Report ----
			case LINE_SUM_x:
				Line_Sum_Report ((int) Report_Data ());
				break;
			case PERF_REPORT:		//---- Transit Performance Report ----
			case PERF_REPORT_x:
				Performance_Report ((int) Report_Data ());
				break;
			case CAP_REPORT:		//---- Capacity Constrained Runs ----
			case CAP_REPORT_x:
				Capacity_Report ((int) Report_Data ());
				break;
			case STOP_GROUP:		//---- Transit Stop Group Summary ----
				Stop_Group_Report ();
				break;
			case STOP_DETAIL:		//---- Transit Stop Group Detail ----
				Stop_Detail_Report ();
				break;
			case STOP_STOP:			//---- Transit Stop Group by Stop ----
				Stop_Stop_Report ();
				break;
			case GROUP_SUM:			//---- Line Group Summary Report ----
				Line_Group_Report ();
				break;
			case GROUP_PERF:		//---- Group Performance Report ----
				Group_Performance_Report ();
				break;
			case LINK_GROUP:		//---- Transit Link Group Report ----
				Link_Group_Report ();
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

void LineSum::Page_Header (void)
{
	switch (Header_Number ()) {
		case LINE_RIDER:			//---- Line Ridership Report ----
			Line_Rider_Header ();
			break;
		case LINE_SUM:				//---- Line Summary Report ----
			Line_Sum_Header ();
			break;
		case PERF_REPORT:			//---- Transit Performance Report ----
			Performance_Header ();
			break;
		case CAP_REPORT:			//---- Capacity Constrained Report ----
			Capacity_Header ();
			break;
		case STOP_GROUP:			//---- Transit Stop Group Summary ----
			Stop_Group_Header ();
			break;
		case STOP_DETAIL:			//---- Transit Stop Group Detail ----
			Stop_Detail_Header ();
			break;
		case STOP_STOP:				//---- Transit Stop Group by Stop ----
			Stop_Stop_Header ();
			break;
		case GROUP_SUM:				//---- Line Group Summary Report ----
			Line_Group_Header ();
			break;
		case GROUP_PERF:			//---- Group Performance Report ----
			Group_Performance_Header ();
			break;
		case LINK_GROUP:			//---- Transit Link Group Report ----
			Link_Group_Header ();
			break;
		default:
			break;
	}
}
