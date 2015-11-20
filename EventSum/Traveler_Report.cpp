//*********************************************************
//	Traveler_Report.cpp - traveler summary report
//*********************************************************

#include "EventSum.hpp"

//---------------------------------------------------------
//	Traveler_Sum_Report
//---------------------------------------------------------

void EventSum::Traveler_Sum_Report (void)
{
	int i;

	Show_Message ("Traveler Summary Report");

	//---- print the report ----

	Header_Number (TRAVELER_SUM);

	if (!Break_Check (num_inc + 7)) {
		Print (1);
		Traveler_Sum_Header ();
	}		

	for (i=1; i <= time_periods.Num_Ranges (); i++) {
		Print (1, "%-12.12s   %10d   %6.2lf   %10d   %6.2lf   %10d   %6.2lf", 
			time_periods.Range_Format (i));
	}
	Print (2, "Total          %10d   100.00   %10d   100.00   %10d   100.00");
		
	Header_Number (0);
}

//---------------------------------------------------------
//	Travel_Header
//---------------------------------------------------------

void EventSum::Traveler_Sum_Header (void)
{
	Print (1, "Traveler Summary Report");
	Print (1);
	Print (1, "             ---- Total ----- --------- Average ----------- ----------- StdDev ----------");
	Print (1, "Period Trips Vehicle  Vehicle   Time  Length   Speed  Turns   Time  Length   Speed  Turns");
	Print (1, "              Hours    Miles (minutes)(miles)  (mph)   (#) (minutes)(miles)  (mph)   (#)");
	Print (1);
}
	 
/*********************************************|***********************************************

	Traveler Summary Report

	             ---- Total ----- --------- Average ----------- ----------- StdDev ----------
	Period Trips Vehicle  Vehicle   Time  Length   Speed  Turns   Time  Length   Speed  Turns
	              Hours    Miles (minutes)(miles)  (mph)   (#) (minutes)(miles)  (mph)   (#)

	 ddd ddddddd fffffff ffffffff ffff.ff ffff.ff fff.ff fff.ff ffff.ff ffff.ff fff.ff fff.ff


**********************************************|***********************************************/ 
