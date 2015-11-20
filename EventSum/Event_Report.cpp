//*********************************************************
//	Event_Report.cpp - Print the Event Summary Report
//*********************************************************

#include "EventSum.hpp"

//---------------------------------------------------------
//	Event_Sum_Report
//---------------------------------------------------------

void EventSum::Event_Sum_Report (void)
{
	//int i, start_total, mid_total, end_total;

	//Show_Message ("Event Summary Report");

	////---- get the total number of trips ----

	//start_total = mid_total = end_total = 0;

	//for (i=0; i < num_inc; i++) {
	//	start_total += start_time [i];
	//	mid_total += mid_time [i];
	//	end_total += end_time [i];
	//}

	//if (start_total == 0 && mid_total == 0 && end_total == 0) {
	//	Write (2, "Trip Time Data is Zero");
	//	return;
	//}

	////---- print the report ----

	//Header_Number (EVENT_SUM);

	//if (!Break_Check (num_inc + 7)) {
	//	Print (1);
	//	Event_Sum_Header ();
	//}		

	//for (i=1; i <= time_periods.Num_Ranges (); i++) {
	//	Print (1, "%-12.12s   %10d   %6.2lf   %10d   %6.2lf   %10d   %6.2lf", 
	//		time_periods.Range_Format (i), 
	//		start_time [i], 100.0 * start_time [i] / (double) start_total,
	//		mid_time [i], 100.0 * mid_time [i] / (double) mid_total,
	//		end_time [i], 100.0 * end_time [i] / (double) end_total);
	//}
	//Print (2, "Total          %10d   100.00   %10d   100.00   %10d   100.00", start_total, mid_total, end_total);
	//	
	//Header_Number (0);
}

//---------------------------------------------------------
//	Event_Sum_Header
//---------------------------------------------------------

void EventSum::Event_Sum_Header (void)
{
	Print (1, "Event Summary Report");
	Print (2, "Time-of-Day    Start-Trip   Percent    Mid-Trip  Percent     End-Trip  Percent");
	Print (1);
}
	 
/*********************************************|***********************************************

	Event Summary Report

	Time-of-Day    Start-Trip   Percent    Mid-Trip  Percent     End-Trip  Percent

	dd:dd..dd:dd   dddddddddd   ddd.dd   dddddddddd   ddd.dd   dddddddddd   ddd.dd

	Total          dddddddddd   ddd.dd   dddddddddd   ddd.dd   dddddddddd   ddd.dd

**********************************************|***********************************************/ 
