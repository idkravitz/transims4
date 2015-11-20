//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "PlanTrips.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void PlanTrips::Execute (void)
{
	int nfile = 0;

	//---- read the network ----

	Demand_Service::Execute ();

	//---- read the household list ----

	if (hhlist_flag) {
		Read_Households ();
	}

	//---- read the plan file ----

	for (nfile=0; ; nfile++) {
		if (!plan_file.Open (nfile)) break;

		if (plan_flag) {
			if (!new_plan_file.Open (nfile)) {
				Error ("Creating New Plan File");
			}
		}
		if (hhold_flag) {
			if (!hhold_file.Open (nfile)) {
				Error ("Creating New Household List");
			}
		}

		//---- process the plan files ----

		Read_Plans ();
	}

	if (nfile == 0) {
		File_Error ("No Plan Files were Found", plan_file.Filename ());
	}

	//---- write output trip file ----

	if (trip_flag) {
		Write_Trips ();
	} else if (new_trip_flag) {
		trip_file->Close ();
	}

	//---- write output activity file ----

	if (act_flag) {
		Write_Activity ();
	} else if (new_act_flag) {
		act_file->Close ();
	}

	//---- write summary statistics ----

	Break_Check (6);

	Write (2, "Number of Plan Files = %d", nfile);
	Write (1, "Number of Input Plans = %d", plan_file.Num_Plans ());
	Write (1, "Number of Input Records = %d", plan_file.Num_Records ());
	Write (1, "Number of Input Travelers = %d", plan_file.Num_Travelers ());
	Write (1, "Number of Input Trips = %d", plan_file.Num_Trips ());

	if (plan_flag) {
		Break_Check (5);

		Write (2, "Number of Output Plans = %d", new_plan_file.Num_Plans ());
		Write (1, "Number of Output Records = %d", new_plan_file.Num_Records ());
		Write (1, "Number of Output Travelers = %d", new_plan_file.Num_Travelers ());
		Write (1, "Number of Output Trips = %d", new_plan_file.Num_Trips ());
	}
	if (new_trip_flag) {
		Write (2, "Number of Trip Records Written = %d", num_trips);
	}
	if (new_act_flag) {
		Write (2, "Number of Activity Records Written = %d", num_act);
	}
	if (num_adjust > 0) {
		Write (2, "Number of Plan Time Adjustments = %d", num_adjust);
		Write (1, "Average Time Adjustment = %.2lf minutes", tot_adjust / (num_adjust * 60.0));
	}

	//---- write the resport ----

	if (Report_Flag (TIME_SHIFT)) {
		int i, min, max;

		New_Page ();
		Print (1, "Time Shift Summary Report");
		Print (2, "Time Shift   -------- Number of Trips -------");
		Print (1, " (minutes)   Start Time   End Time   Mid Time");

		for (i=0, min=-250; i <= 100; i++, min = max) {
			if (i == 50) {
				max = min;
			} else {
				max = min + 5;
			}
			if (start_offset [i] != 0 || end_offset [i] != 0 || mid_offset [i] != 0) {
				Print (1, "%4d  %4d     %8d   %8d   %8d", min, max, start_offset [i], end_offset [i], mid_offset [i]);
			}
		}
	}
	Exit_Stat (DONE);
}
