//*********************************************************
//	Change_Report.cpp - Create a Change Report
//*********************************************************

#include "PlanCompare.hpp"

//---------------------------------------------------------
//	Change_Report
//---------------------------------------------------------

void PlanCompare::Change_Report (int type)
{
	int i, j, k, k1, time1, time2, percent, total, sum, round;
	int percentile [NUM_PERCENTILES];
	double factor;
	int *changes;

	Show_Message ("%s Change Report", change_text [type]);

	//---- print the report ----

	Header_Number (type);
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
		if (type == PATH_CHANGE) {
			changes = path_changes [i];
		} else if (type == TIME_CHANGE) {
			changes = time_changes [i];
		} else if (type == COST_CHANGE) {
			changes = cost_changes [i];
		}

		//---- get the total observations ----

		total = 0;

		for (j=0; j < NUM_CHANGE_BINS; j++) {
			total += changes [j];
		}
		if (total == 0) continue;

		//---- identify the percentile values ----

		memset (percentile, '\0', sizeof (percentile));

		k1 = 0;
		sum = 0;
		round = (total >> 1);

		for (j=0; j < NUM_CHANGE_BINS; j++) {
			sum += changes [j];

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
//	Change_Header
//---------------------------------------------------------

void PlanCompare::Change_Header (int type)
{
	int i;
	char *type_text;

	if (type == PATH_CHANGE) {
		type_text = "Link";
	} else if (type == TIME_CHANGE) {
		type_text = "Time";
	} else if (type == COST_CHANGE) {
		type_text = "Cost";
	} else {
		Error ("Change Report Type %d was Not Recognized", type);
	}
	Print (1, "%s Change Report", change_text [type]);
	Print (1);
	Print (1, "%27c%s Change * 100 / Previous %s%s", BLANK, type_text, type_text, ((type == PATH_CHANGE) ? "s" : " "));
	Print (1, "%12c-------------Percentile Distribution by Time Period----------     Num.", BLANK);
	Print (1, "Time Period");

	for (i=0; i < NUM_PERCENTILES; i++) {
		Print (0, "  %3d%% ", percent_break [i]);
	}
	Print (0, "   Plans");
	Print (1);
}

/*********************************************|***********************************************

	sssssssssss Change Report

                                ssss Change * 100 / Previous sssss
	              -------------Percentile Distribution by Time Period----------    Num.
	Time Period    50%    65%    70%    75%    80%    85%    90%    95%    99%    Plans
	
	xx:xx-xx:xx   ddd.d  ddd.d  ddd.d  ddd.d  ddd.d  ddd.d  ddd.d  ddd.d  ddd.d  ddddddd

	Total

**********************************************|***********************************************/ 
