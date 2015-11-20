//*********************************************************
//	Save_Plans.cpp - Save the Subarea Plan Record
//*********************************************************

#include "SubareaPlans.hpp"

//---------------------------------------------------------
//	Save_Plans
//---------------------------------------------------------

bool SubareaPlans::Save_Plans (void)
{
	if (range_flag) {
		int start, end;

		start = subarea_plans.Time ();
		end = subarea_plans.Stop_Time ();

		if (!time_range.Span_Range (start, end)) return (false);
	}
	if (!subarea_plans.Write ()) {
		Error ("Writing Plan File");
	}
	return (true);
}
