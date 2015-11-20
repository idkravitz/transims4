//*********************************************************
//	Passenger_Report.cpp - travel summary report
//*********************************************************

#include "PlanSum.hpp"

//---------------------------------------------------------
//	Passenger_Report
//---------------------------------------------------------

void PlanSum::Passenger_Report (void)
{
	double factor;

	Trip_Sum_Data *pass_sum_ptr;

	Show_Message ("Transit Passenger Summary");

	//---- print the period reports ----

	if (num_inc > 1) {
		Header_Number (SUM_PASSENGERS);

		if (!Break_Check (num_inc + 4)) {
			Print (1);
			Passenger_Header ();
		}

		//---- print each time period ---

		for (pass_sum_ptr = pass_sum_data.First (); pass_sum_ptr; pass_sum_ptr = pass_sum_data.Next ()) {
			if (pass_sum_ptr->Count () == 0) continue;

			Print (1, " %3d %7d %7.0lf %8.0lf %7.2lf %7.2lf %6.2lf %6.2lf %7.2lf %7.2lf %6.2lf %6.2lf",
				pass_sum_ptr->Group (), pass_sum_ptr->Count (),
				pass_sum_ptr->Time () / 3600.0, pass_sum_ptr->Distance () / MILETOMETER,
				pass_sum_ptr->Average_Time () / 60.0, pass_sum_ptr->Average_Distance () / MILETOMETER, 	
				pass_sum_ptr->Average_Speed () * 3600.0 / MILETOMETER, pass_sum_ptr->Average_Turns (), 	
				pass_sum_ptr->StdDev_Time () / 60.0, pass_sum_ptr->StdDev_Distance () / MILETOMETER,
				pass_sum_ptr->StdDev_Speed () * 3600.0 / MILETOMETER, pass_sum_ptr->StdDev_Turns ());
		}
		Header_Number (0);
	}

	//---- print the total report ----

	if (Break_Check (12) || num_inc <= 1) {
		Print (2, "Transit Passenger Summary Report");
	}
	Print (2, "Total Passenger Trips = %d", pass_sum_total.Count ());
	Print (1, "Total Passenger Hours of Travel = %.1lf hours", pass_sum_total.Time () / 3600.0);
	Print (1, "Total Passenger Miles of Travel = %.1lf miles", pass_sum_total.Distance () / MILETOMETER);
	Print (1, "Total Number of Transfers = %.0lf", pass_sum_total.Turns ());

	Print (2, "                        Minimum  Maximum  Average   StdDev");
	
	factor = 1.0 / 60.0;			//---- seconds to minutes ----
	Print (1, "Travel Time (minutes)   %7.2lf  %7.2lf  %7.2lf  %7.2lf", 
		pass_sum_total.Min_Time () * factor, pass_sum_total.Max_Time () * factor,
		pass_sum_total.Average_Time () * factor, pass_sum_total.StdDev_Time () * factor);

	factor = 1.0 / MILETOMETER;		//---- meters to miles ----
	Print (1, "Trip Length (miles)     %7.2lf  %7.2lf  %7.2lf  %7.2lf", 
		pass_sum_total.Min_Distance () * factor, pass_sum_total.Max_Distance () * factor,
		pass_sum_total.Average_Distance () * factor, pass_sum_total.StdDev_Distance () * factor);

	factor = 3600.0 / MILETOMETER;	//---- meters per second to miles per hour ----
	Print (1, "Travel Speed (mph)      %7.2lf  %7.2lf  %7.2lf  %7.2lf", 
		pass_sum_total.Min_Speed () * factor, pass_sum_total.Max_Speed () * factor,
		pass_sum_total.Average_Speed () * factor, pass_sum_total.StdDev_Speed () * factor);

	Print (1, "Number of Transfers     %7.2lf  %7.2lf  %7.2lf  %7.2lf", 
		(double) pass_sum_total.Min_Turns (), (double) pass_sum_total.Max_Turns (),
		pass_sum_total.Average_Turns (), pass_sum_total.StdDev_Turns ());
}

//---------------------------------------------------------
//	Passenger_Header
//---------------------------------------------------------

void PlanSum::Passenger_Header (void)
{
	Print (1, "Transit Passenger Summary Report");
	Print (1);
	Print (1, "             ---- Total ----- --------- Average ----------- ----------- StdDev ----------");
	Print (1, "Period Trips ----Passenger---   Time  Length   Speed  Xfers   Time  Length   Speed  Xfers");
	Print (1, "              Hours    Miles (minutes)(miles)  (mph)   (#) (minutes)(miles)  (mph)   (#)");
	Print (1);
}
	 
/*********************************************|***********************************************

	Transit Passenger Summary Report

	             ---- Total ----- --------- Average ----------- ----------- StdDev ----------
	Period Trips ----Passenger---   Time  Length   Speed  Xfers   Time  Length   Speed  Xfers
	              Hours    Miles (minutes)(miles)  (mph)   (#) (minutes)(miles)  (mph)   (#)

	 ddd ddddddd fffffff ffffffff ffff.ff ffff.ff fff.ff fff.ff ffff.ff ffff.ff fff.ff fff.ff

	Total Passenger Trips = dddddd
	Total Passenger Hours of Travel = fffffff.f hours
	Total Passenger Miles of Travel = ffffffff.f miles;
	Total Number of Transfers = ddddd

	                        Minimum  Maximum  Average   StdDev   
	Travel Time (minutes)    fff.ff   fff.ff   fff.ff   fff.ff
	Trip Length (miles)      fff.ff   fff.ff   fff.ff   fff.ff
	Travel Speed (mph)       fff.ff   fff.ff   fff.ff   fff.ff
	Number of Transfers      fff.ff   fff.ff   fff.ff   fff.ff

**********************************************|***********************************************/ 
