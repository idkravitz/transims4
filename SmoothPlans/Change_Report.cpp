//*********************************************************
//	Change_Report.cpp - time change summary report
//*********************************************************

#include "SmoothPlans.hpp"

//---------------------------------------------------------
//	Change_Report
//---------------------------------------------------------

void SmoothPlans::Change_Report (void)
{
	int hour, time;
	double average;
	char buffer [20];

	Period_Index *period_ptr;

	//---- print the report header ----

	if (!Break_Check (32)) {
		Print (1);
	}
	Print (1, "Time Change Summary Report");
	Print (1);
	Print (1, "            ------ Trips -------     Average     Minimum    Maximum");
	Print (1, "      Hour   Original   Smoothed      Change      Change     Change");
	Print (1);

	for (hour=1; hour < 24; hour++) {
		period_ptr = period_index [hour];

		Print (1, "%10.10s %10d %10d", time_step.Format_Step (hour * 3600),
			period_ptr->Original (), period_ptr->Smoothed ());

		average = period_ptr->Average_Change ();

		if (time_step.Format () == Time_Step::HOURS) {
			if (average >= 0) {
				time = (int) (average + 0.5);
			} else {
				time = -((int) (-average + 0.5));
			}
			Print (0, "  %11.11s", time_step.Format_Duration (time));
		} else {
			if (average >= 0) {
				time = (int) average;
				average = average - time;
			} else {
				time = -((int) (-average));
				average = time - average;
			}
			str_fmt (buffer, sizeof (buffer), "%.1lf", average);

			Print (0, " %10.10s%s", time_step.Format_Duration (time), buffer+1);
		}
		Print (0, " %10.10s", time_step.Format_Duration (period_ptr->Min_Change ()));
		Print (0, " %10.10s", time_step.Format_Duration (period_ptr->Max_Change ()));
	}
}
	 
/*********************************************|***********************************************

	Time Change Summary Report

	            ------ Trips -------     Average     Minimum    Maximum 
	      Hour   Original   Smoothed      Change      Change     Change

	ssssssssss dddddddddd dddddddddd ssssssssss.f ssssssssss ssssssssss


**********************************************|***********************************************/ 
