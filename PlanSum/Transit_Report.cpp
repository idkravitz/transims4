//*********************************************************
//	Transit_Report.cpp - Report Transit Ridership
//*********************************************************

#include "PlanSum.hpp"

//---------------------------------------------------------
//	Transit_Report
//---------------------------------------------------------

void PlanSum::Transit_Report (void)
{
	int nrun, nstop, run, stop, board, alight, load;
	int sum_board, tot_board, max_board, min_board, avg_board;
	int high_load, tot_load, max_load, min_load, avg_load;
	int tot_routes, tot_runs, tot_stops;
	int tot_tot_board, max_max_board, min_min_board;
	int tot_tot_load, max_max_load, min_min_load;

	Rider_Data *rider_ptr;

	Show_Message ("Transit Ridership Report -- Route");
	Set_Progress (100);

	Header_Number (SUM_RIDERS);

	tot_routes = rider_data.Num_Records ();

	if (!Break_Check (tot_routes + 8)) {
		Print (1);
		Transit_Header ();
	}
	tot_runs = tot_stops = avg_board = avg_load = 0;
	tot_tot_board = max_max_board = tot_tot_load = max_max_load = 0;
	min_min_load = min_min_board = -1;

	//---- process each transit route ----

	for (rider_ptr = rider_data.First_Key (); rider_ptr; rider_ptr = rider_data.Next_Key ()) {
		Show_Progress ();

		tot_board = max_board = min_board = 0;
		tot_load = max_load = min_load = 0;
		
		nrun = rider_ptr->Runs ();
		nstop = rider_ptr->Stops ();

		for (run=1; run <= nrun; run++) {
			load = sum_board = high_load = 0;

			for (stop=1; stop <= nstop; stop++) {

				board = rider_ptr->Board (run, stop);
				alight = rider_ptr->Alight (run, stop);

				load += board - alight;

				if (load == 0 && board == 0 && alight == 0) continue;

				if (load > high_load) high_load = load;
				sum_board += board;
			}
			if (high_load > max_load) max_load = high_load;
			if (high_load < min_load || min_load == 0) min_load = high_load;
			tot_load += high_load;

			if (sum_board > max_board) max_board = sum_board;
			if (sum_board < min_board || min_board == 0) min_board = sum_board;
			tot_board += sum_board;
		}

		//---- print report ----

		if (nrun > 0) {
			avg_board = (tot_board + nrun / 2) / nrun;
			avg_load = (tot_load + nrun / 2) / nrun;
		}

		Print (1, "%-12.12s%7d %6d %5d  %7d %7d %7d %7d  %7d %7d %7d", 
			Transit_Code ((Transit_Type) rider_ptr->Mode ()),
			rider_ptr->Route (), nrun, nstop, 
			tot_board, max_board, min_board, avg_board,
			max_load, min_load, avg_load);

		tot_runs += nrun;
		tot_stops += nstop;
		tot_tot_board += tot_board;
		tot_tot_load += tot_load;

		if (max_load > max_max_load) max_max_load = max_load;
		if (min_load < min_min_load || min_min_load < 0) min_min_load = min_load;
		if (max_board > max_max_board) max_max_board = max_board;
		if (min_board < min_min_board || min_min_board < 0) min_min_board = min_board;
	}
	End_Progress ();

	//---- system total ----

	if (tot_runs > 0) {
		avg_board = (tot_tot_board + tot_runs / 2) / tot_runs;
		avg_load = (tot_tot_load + tot_runs / 2) / tot_runs;
	}

	Print (2, "%-12.12s%7d %6d %5d  %7d %7d %7d %7d  %7d %7d %7d", 
		"Total", tot_routes, tot_runs, tot_stops, 
		tot_tot_board, max_max_board, min_min_board, avg_board,
		max_max_load, min_min_load, avg_load);

	Header_Number (0);
}

//---------------------------------------------------------
//	Transit_Header
//---------------------------------------------------------

void PlanSum::Transit_Header (void)
{
	Print (1, "Transit Ridership Summary");
	Print (2, "%22cNum.  Num.  -----------Boardings-----------  -----Max.Load Point----", BLANK);
	Print (1, "Mode          Route   Runs Stops    Total Maximum Minimum Average  Maximum Minimum Average");
	Print (1);
}

/*********************************************|***********************************************

	Transit Ridership Summary

                          Num.  Num.  -----------Boardings-----------  -----Max.Load Point----
	Mode          Route   Runs Stops    Total Maximum Minimum Average  Maximum Minimum Average

	ssss12ssssssddddddd dddddd ddddd  ddddddd ddddddd ddddddd ddddddd  ddddddd ddddddd ddddddd

	Total       ddddddd dddddd ddddd  ddddddd ddddddd ddddddd ddddddd  ddddddd ddddddd ddddddd

**********************************************|***********************************************/ 
