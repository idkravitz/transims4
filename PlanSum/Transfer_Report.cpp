//*********************************************************
//	Transfer_Report.cpp - Report Transit Transfers
//*********************************************************

#include "PlanSum.hpp"

//---------------------------------------------------------
//	Transfer_Report
//---------------------------------------------------------

void PlanSum::Transfer_Report (int num)
{
	int i, j, k, total, bus_total [MAX_BOARD], num_board;

	int *board_ptr = boardings [num];

	//---- get the dimensions ----

	total = max_rail = num_board = 0;

	for (i=0; i < MAX_BOARD; i++) {
		k = i * MAX_BOARD;

		for (j=MAX_BOARD-1; j >= 0; j--) {	
			if (board_ptr [k + j] > 0) {
				if (j > max_rail) max_rail = j;
				break;
			}
		}
		if (j >= 0) total++;
	}

	//---- print the header ----

	if (num > 0) {
		if (total == 0) return;

		Header_Number (XFER_DETAILS);
		i = (num - 1) / 2 + 1;
		j = (num - 1) % 2;

		str_fmt (xfer_label, sizeof (xfer_label), "%s %s", ((j) ? "Drive Access" : "Walk Access"),
			time_periods.Range_Format (i));
	} else {
		Header_Number (SUM_TRANSFERS);
		xfer_label [0] = '\0';
	}

	if (!Break_Check (total + 10)) {
		Print (1);
		Transfer_Header ();
	}

	//---- print each row ----

	memset (bus_total, '\0', sizeof (bus_total));

	for (i=0; i < MAX_BOARD; i++) {
		k = i * MAX_BOARD;

		for (j=0; j <= max_rail; j++) {
			if (board_ptr [k + j] > 0) break;
		}
		if (j > max_rail) continue;
		
		Print (1, "%3d      ", i);

		if (max_rail > 0) {
			total = 0;

			for (j=0; j <= max_rail; j++, k++) {
				Print (0, " %7d", board_ptr [k]);
				total += board_ptr [k];
				bus_total [j] += board_ptr [k];

				num_board += board_ptr [k] * (i + j);
			}
		} else {
			bus_total [0] = total = board_ptr [k];

			num_board += board_ptr [k] * i;
		}
		Print (0, " %7d", total);
	}
	
	//---- bus totals ----

	Print (2, "Total    ");

	if (max_rail > 0) {
		total = 0;

		for (j=0; j <= max_rail; j++) {
			Print (0, " %7d", bus_total [j]);
			total += bus_total [j];
		}
	} else {
		total = bus_total [0];
	}
	Print (0, " %7d", total);

	Print (2, "Total Boardings = %d, Boardings per Trip = %.2lf", num_board,
		((total > 0) ? (double) num_board / total : 0.0));

	Header_Number (0);
}

//---------------------------------------------------------
//	Transfer_Header
//---------------------------------------------------------

void PlanSum::Transfer_Header (void)
{
	int i, len, len1;
	char *dash = "----------------------------------------";

	if (xfer_label [0] == '\0') {
		Print (1, "Transit Transfer Summary");
	} else {
		Print (1, "Transit Transfer Details -- %s", xfer_label);
	}
	Print (2, "Bus      ");

	if (max_rail > 0) {
		len = ((max_rail + 2) * 8) - 16;
		len1 = len / 2;
		len = len - len1;
		len1--;

		if (len > 2 && len1 > 2) {
			Print (0, " %*.*s Rail Boardings %*.*s", len1, len1, dash, len, len, dash);
		} else {
			Print (0, "%*cRail Boardings", len1+1, BLANK);
		}
	}
	Print (1, "Boardings");

	if (max_rail > 0) {
		for (i=0; i <= max_rail; i++) {
			Print (0, " %7d", i);
		}
		Print (0, "   Total");
	}
	Print (1);
}

/*********************************************|***********************************************

	Transit Transfer Summary

	Bus       ------------------------------ Rail Boardings ---------------------------------
	Boardings       0       1       2       3       4       5       6       7      8+   Total

	  0       ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd
	  1       ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd
	  2       ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd
	  3       ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd

	Total     ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd


	Total Boardings = %d,  Boardings per Trip = %f.ff

**********************************************|***********************************************/ 
