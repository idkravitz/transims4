//*********************************************************
//	Trip_Time.cpp - Print the Trip Start Time Report
//*********************************************************

#include "TripSum.hpp"

//---------------------------------------------------------
//	Trip_Time
//---------------------------------------------------------

void TripSum::Trip_Time (void)
{
	int i, start_total, mid_total, end_total;
	double start, mid, end;

	Show_Message ("Trip Time Report");

	//---- get the total number of trips ----

	start_total = mid_total = end_total = 0;

	for (i=1; i <= num_inc; i++) {
		start_total += start_time [i];
		mid_total += mid_time [i];
		end_total += end_time [i];
	}
	if (start_total == 0 && mid_total == 0 && end_total == 0) {
		Write (2, "Trip Time Data is Zero");
		return;
	}
	if (start_total == 0) {
		start = 1.0;
	} else {
		start = start_total;
	}
	if (mid_total == 0) {
		mid = 1.0;
	} else {
		mid = mid_total;
	}
	if (end_total == 0) {
		end = 1.0;
	} else {
		end = end_total;
	}

	//---- print the report ----

	Header_Number (TRIP_TIME);

	if (!Break_Check (num_inc + 7)) {
		Print (1);
		Trip_Time_Header ();
	}		

	for (i=1; i <= num_inc; i++) {
		Print (1, "%-12.12s   %10d   %6.2lf   %10d   %6.2lf   %10d   %6.2lf", 
			time_periods.Range_Format (i), 
			start_time [i], 100.0 * start_time [i] / start,
			mid_time [i], 100.0 * mid_time [i] / mid,
			end_time [i], 100.0 * end_time [i] / end);
	}
	Print (2, "Total          %10d   100.00   %10d   100.00   %10d   100.00", start_total, mid_total, end_total);
		
	Header_Number (0);
}

//---------------------------------------------------------
//	Trip_Time_Header
//---------------------------------------------------------

void TripSum::Trip_Time_Header (void)
{
	Print (1, "Trip Start Time Report");
	Print (2, "Time-of-Day    Start-Trip   Percent    Mid-Trip  Percent     End-Trip  Percent");
	Print (1);
}
	 
/*********************************************|***********************************************

	Trip Start Time Report

	Time-of-Day    Start-Trip   Percent    Mid-Trip  Percent     End-Trip  Percent

	dd:dd..dd:dd   dddddddddd   ddd.dd   dddddddddd   ddd.dd   dddddddddd   ddd.dd

	Total          dddddddddd   ddd.dd   dddddddddd   ddd.dd   dddddddddd   ddd.dd

**********************************************|***********************************************/ 
