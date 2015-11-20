//*********************************************************
//	Trip_Gap.cpp - Create a Trip Gap Report
//*********************************************************

#include "PlanCompare.hpp"

//---------------------------------------------------------
//	Trip_Gap_Report
//---------------------------------------------------------

void PlanCompare::Trip_Gap_Report (void)
{
	int i, count;
	double input, compare, diff;
	Period_Diff *period_ptr;

	Show_Message ("Trip Gap Report");

	//---- print the report ----

	Header_Number (TRIP_GAP);
	New_Page ();

	count = 0;
	input = compare = diff = 0;

	for (i=1; i <= num_inc; i++) {
		period_ptr = (Period_Diff *) period_diff.Record (i);

		count += period_ptr->count;
		input += period_ptr->input;
		compare += period_ptr->compare;
		diff += period_ptr->diff;

		Print (1, " %3d  %10d  %12.0lf  %12.0lf  %10.0lf  %8.4lf", i, period_ptr->count,
			period_ptr->input, period_ptr->compare, period_ptr->diff, 
			((period_ptr->input > 0) ? (-period_ptr->diff / period_ptr->input) : 0.0));
	}
	Print (2, "Total %10d  %12.0lf  %12.0lf  %10.0lf  %8.4lf", count,
		input, compare, diff, ((input > 0) ? (-diff / input) : 0.0));

	Header_Number (0);
}

//---------------------------------------------------------
//	Trip_Gap_Header
//---------------------------------------------------------

void PlanCompare::Trip_Gap_Header (void)
{
	Print (1, "Trip Gap Report");
	if (cost_flag) {
		Print (2, "PERIOD     COUNT    INPUT_COST  COMPARE_COST  DIFFERENCE  TRIP_GAP");
	} else {
		Print (2, "PERIOD     COUNT    INPUT_TIME  COMPARE_TIME  DIFFERENCE  TRIP_GAP");
	}
	Print (1);
}

/*********************************************|***********************************************

	Trip Gap Report

	PERIOD     COUNT    INPUT_COST  COMPARE_COST  DIFFERENCE  TRIP_GAP
	 ddd   ddddddddd  ffffffffffff  ffffffffffff  ffffffffff  fff.ffff

	Total  ddddddddd  ffffffffffff  ffffffffffff  ffffffffff  fff.ffff

**********************************************|***********************************************/ 
