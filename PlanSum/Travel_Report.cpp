//*********************************************************
//	Travel_Report.cpp - travel summary report
//*********************************************************

#include "PlanSum.hpp"

//---------------------------------------------------------
//	Travel_Report
//---------------------------------------------------------

void PlanSum::Travel_Report (void)
{
	double factor;

	Trip_Sum_Data *trip_sum_ptr;

	Show_Message ("Travel Summary Report");

	//---- print the period reports ----

	if (num_inc > 1) {
		Header_Number (SUM_TRAVEL);

		if (!Break_Check (num_inc + 4)) {
			Print (1);
			Travel_Header ();
		}

		//---- print each time period ---

		for (trip_sum_ptr = trip_sum_data.First (); trip_sum_ptr; trip_sum_ptr = trip_sum_data.Next ()) {
			if (trip_sum_ptr->Count () == 0) continue;

			Print (1, " %3d %7d %7.0lf %8.0lf %7.2lf %7.2lf %6.2lf %6.2lf %7.2lf %7.2lf %6.2lf %6.2lf",
				trip_sum_ptr->Group (), trip_sum_ptr->Count (),
				trip_sum_ptr->Time () / 3600.0, trip_sum_ptr->Distance () / MILETOMETER,
				trip_sum_ptr->Average_Time () / 60.0, trip_sum_ptr->Average_Distance () / MILETOMETER, 	
				trip_sum_ptr->Average_Speed () * 3600.0 / MILETOMETER, trip_sum_ptr->Average_Turns (), 	
				trip_sum_ptr->StdDev_Time () / 60.0, trip_sum_ptr->StdDev_Distance () / MILETOMETER,
				trip_sum_ptr->StdDev_Speed () * 3600.0 / MILETOMETER, trip_sum_ptr->StdDev_Turns ());
		}
		Header_Number (0);
	}

	//---- print the total report ----

	if (Break_Check (12) || num_inc <= 1) {
		Print (2, "Travel Summary Report");
	}
	Print (2, "Total Vehicle Trips = %d", trip_sum_total.Count ());
	Print (1, "Total Vehicle Hours of Travel = %.1lf hours", trip_sum_total.Time () / 3600.0);
	Print (1, "Total Vehicle Miles of Travel = %.1lf miles", trip_sum_total.Distance () / MILETOMETER);
	Print (1, "Total Number of Turns = %.0lf", trip_sum_total.Turns ());

	Print (2, "                        Minimum  Maximum  Average   StdDev");
	
	factor = 1.0 / 60.0;			//---- seconds to minutes ----
	Print (1, "Travel Time (minutes)   %7.2lf  %7.2lf  %7.2lf  %7.2lf", 
		trip_sum_total.Min_Time () * factor, trip_sum_total.Max_Time () * factor,
		trip_sum_total.Average_Time () * factor, trip_sum_total.StdDev_Time () * factor);

	factor = 1.0 / MILETOMETER;		//---- meters to miles ----
	Print (1, "Trip Length (miles)     %7.2lf  %7.2lf  %7.2lf  %7.2lf", 
		trip_sum_total.Min_Distance () * factor, trip_sum_total.Max_Distance () * factor,
		trip_sum_total.Average_Distance () * factor, trip_sum_total.StdDev_Distance () * factor);

	factor = 3600.0 / MILETOMETER;	//---- meters per second to miles per hour ----
	Print (1, "Travel Speed (mph)      %7.2lf  %7.2lf  %7.2lf  %7.2lf", 
		trip_sum_total.Min_Speed () * factor, trip_sum_total.Max_Speed () * factor,
		trip_sum_total.Average_Speed () * factor, trip_sum_total.StdDev_Speed () * factor);

	Print (1, "Number of Turns         %7.2lf  %7.2lf  %7.2lf  %7.2lf", 
		(double) trip_sum_total.Min_Turns (), (double) trip_sum_total.Max_Turns (),
		trip_sum_total.Average_Turns (), trip_sum_total.StdDev_Turns ());
}

//---------------------------------------------------------
//	Travel_Header
//---------------------------------------------------------

void PlanSum::Travel_Header (void)
{
	Print (1, "Travel Summary Report");
	Print (1);
	Print (1, "             ---- Total ----- --------- Average ----------- ----------- StdDev ----------");
	Print (1, "Period Trips Vehicle  Vehicle   Time  Length   Speed  Turns   Time  Length   Speed  Turns");
	Print (1, "              Hours    Miles (minutes)(miles)  (mph)   (#) (minutes)(miles)  (mph)   (#)");
	Print (1);
}
	 
/*********************************************|***********************************************

	Travel Summary Report

	             ---- Total ----- --------- Average ----------- ----------- StdDev ----------
	Period Trips Vehicle  Vehicle   Time  Length   Speed  Turns   Time  Length   Speed  Turns
	              Hours    Miles (minutes)(miles)  (mph)   (#) (minutes)(miles)  (mph)   (#)

	 ddd ddddddd fffffff ffffffff ffff.ff ffff.ff fff.ff fff.ff ffff.ff ffff.ff fff.ff fff.ff

	Total Vehicle Trips = dddddd
	Total Vehicle Hours of Travel = fffffff.f hours
	Total Vehicle Miles of Travel = ffffffff.f miles;
	Total Number of Turns = ddddd

	                        Minimum  Maximum  Average   StdDev   
	Travel Time (minutes)    fff.ff   fff.ff   fff.ff   fff.ff
	Trip Length (miles)      fff.ff   fff.ff   fff.ff   fff.ff
	Travel Speed (mph)       fff.ff   fff.ff   fff.ff   fff.ff
	Number of Turns          fff.ff   fff.ff   fff.ff   fff.ff

**********************************************|***********************************************/ 
