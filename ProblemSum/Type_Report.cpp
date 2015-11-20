//*********************************************************
//	Type_Report.cpp - Print the Problem Type Summary
//*********************************************************

#include "ProblemSum.hpp"

//---------------------------------------------------------
//	Type_Report
//---------------------------------------------------------

void ProblemSum::Type_Report (void)
{
	int i, total;

	//---- print the report ----

	Header_Number (TYPE_REPORT);

	if (!Break_Check (num_types + 6)) {
		Print (1);
		Type_Header ();
	}
	total = num_problems [0];

	for (i=1; i < MAX_PROBLEM; i++) {
		if (num_problems [i] == 0) continue;

		Print (1, "%2d  %-20.20s   %10d   %6.2lf", i, Problem_Code ((Problem_Type) i),
			num_problems [i], ((100.0 * num_problems [i]) / total));
	}
	Print (2, "Total                      %10d   100.00", total);

	Header_Number (0);
}

//---------------------------------------------------------
//	Type_Header
//---------------------------------------------------------

void ProblemSum::Type_Header (void)
{
	Print (1, "Problem Type Summary");
	Print (2, "Problem Type                    Count   Percent");
	Print (1);
}
	 
/*********************************************|***********************************************

	Problem Type Summary

	Problem Type                    Count   Percent

	dd  ssssssssssssssssssss   dddddddddd   ddd.dd

	Total                      dddddddddd   ddd.dd

**********************************************|***********************************************/ 
