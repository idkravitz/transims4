//*********************************************************
//	Path_Change.cpp - Create Path Change Report
//*********************************************************

#include "PlanPrep.hpp"

//---------------------------------------------------------
//	Path_Change
//---------------------------------------------------------

void PlanPrep::Path_Change (void)
{
	int i, j, k, k1, time1, time2, percent, total, sum, round;
	int percentile [NUM_PERCENTILES];
	double factor;

	Show_Message ("\n\tPath Change Report");

	//---- print the report ----

	Header_Number (PATH_CHANGE);
	New_Page ();

	time1 = 0;
	factor = NUM_CHANGE_BINS / 100.0;

	for (i=0; i < NUM_TIME_PERIOD; i++, time1 = time2) {
		time2 = time1 + increment;

		if (i == TOTAL_TIME_PERIOD) {
			Print (2, "Total     ");
		} else {
			Print (1, "%2d%02d-%2d%02d ", (time1 / 3600), (time1 % 3600) / 60, (time2 / 3600), (time2 % 3600) / 60);
		}

		//---- get the total observations ----

		total = 0;

		for (j=0; j < NUM_CHANGE_BINS; j++) {
			total += path_changes [i] [j];
		}
		if (total == 0) continue;

		//---- identify the percentile values ----

        memset (percentile, '\0', sizeof (percentile));

		k1 = 0;
		sum = 0;
		round = (total >> 1);

		for (j=0; j < NUM_CHANGE_BINS; j++) {
			sum += path_changes [i] [j];

			percent = (sum * 100 + round) / total;

			for (k=k1; k < NUM_PERCENTILES; k++) {
				if (percent_break [k] <= percent) {
					percentile [k] = j;
					k1 = k + 1;
				} else {
					break;
				}
			}
		}

		//---- print the percentiles ----

		for (k=0; k < NUM_PERCENTILES; k++) {
			Print (0, " %6.1lf", (double) percentile [k] / factor);
		}
		Print (0, "  %7d", total);
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Path_Change_Header
//---------------------------------------------------------

void PlanPrep::Path_Change_Header (void)
{
	int i;

	Print (1, "Path Change Report");
	Print (1);
	Print (1, "%27cLink Changes * 100 / Previous Links", BLANK);
	Print (1, "%12c-------------Percentile Distribution by Time Period----------     Num.", BLANK);
	Print (1, "Time Period");

	for (i=0; i < NUM_PERCENTILES; i++) {
		Print (0, "  %3d%% ", percent_break [i]);
	}
	Print (0, "   Plans");
	Print (1);
}

/*********************************************|***********************************************

	Path Change Report

                                Link Changes * 100 / Previous Links
	              -------------Percentile Distribution by Time Period----------    Num.
	Time Period    50%    65%    70%    75%    80%    85%    90%    95%    99%    Plans
	
	xx:xx-xx:xx   ddd.d  ddd.d  ddd.d  ddd.d  ddd.d  ddd.d  ddd.d  ddd.d  ddd.d  ddddddd

	Total       
**********************************************|***********************************************/ 
