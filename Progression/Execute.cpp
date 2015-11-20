//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "Progression.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void Progression::Execute (void)
{
	int group, ngroup, best_group, best_perf, perf, nperiod, lvalue;
	Range_Data *range_ptr;

	//---- read the network files ----

	Demand_Service::Execute ();

	//---- group period weights ----

	if (weight_flag) {
		Read_Weights ();
	}

	//---- prepare signal data ----

	Signal_Setup ();

	//---- find the best group for setting offsets ----

	old_thru = tot_thru = 0;
	nperiod = progress_time.Num_Records ();

	for (period=1; period <= nperiod; period++) {
		Show_Message ("Progressing Time Period %d", period);
		Show_Message (1);

		range_ptr = progress_time [period];
		progression_time = range_ptr->High ();
		mid_period = ttime_data.Period ((range_ptr->High () + range_ptr->Low () + 1) / 2);

		lvalue = progress_speed.Best (period);
		if (lvalue == 0) {
			speed_flag = false;
		} else {
			speed_flag = true;
			progression_speed = lvalue / 100.0;
		}

		//---- prepare the group data ----

		group_data.Reset ();
		signal_offset.Reset ();

		if (equiv_flag) {
			User_Groups ();
		} else {
			Signal_Groups ();
		}
		ngroup = group_data.Num_Records ();

		//---- evaluate the existing offsets ----

		if (eval_flag) {
			Set_Offset (1, true, true);
		}
		if (order_flag) {

			//---- use link group order ----

			Set_Offset (1, true);

		} else {

			Show_Message ("Setting Signal Offsets -- Group");
			Set_Progress (1);

			best_group = 0;
			best_perf = -(MAX_INTEGER-1);

			for (group=1; group <= ngroup; group++) {
				Show_Progress (group);

				perf = Set_Offset (group);

				if (perf > best_perf) {
					best_perf = perf;
					best_group = group;
				}
			}
			End_Progress ();
	
			//---- reset the offsets for the best group ----

			Set_Offset (best_group, true);
		}
	}
	Write_Signals ();

	//---- summarize results ----

	Break_Check (5);
	Write (2, "Number of Fixed Time Signals = %d", fixed);	
	Write (1, "Number of Updated Offsets = %d", updated);
	Write (1, "Number of Signal Groups = %d", ngroup);
	Write (1, "Number of Time Periods = %d", nperiod);
	if (nperiod > 0) {
		Write (1, "Non-Stop Thru Movements = %.1lf%%", ((double) tot_thru / (10.0 * nperiod)));

		if (eval_flag) {
			Write (1, "Original Non-Stop Thru Movements = %.1lf%%", ((double) old_thru / (10.0 * nperiod)));
		}
	}

	//---- end the program ----

	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void Progression::Page_Header (void)
{
	switch (Header_Number ()) {
		case WEIGHT_REPORT:		//---- group period weights ----
			Weight_Header ();
			break;
		default:
			break;
	}
}
