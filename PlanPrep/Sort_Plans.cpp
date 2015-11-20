//*********************************************************
//	Sort_Plans.cpp - Sorts Input Plan Files
//*********************************************************

#include "PlanPrep.hpp"

//---------------------------------------------------------
//	Sort_Plans
//---------------------------------------------------------

void PlanPrep::Sort_Plans (void)
{
	bool skip;
	int current_traveler, traveler;
	Plan_Data *plan_ptr;

	current_traveler = 0;
	skip = false;

	if (input_plans.Extend ()) {
		Show_Message ("Sorting Plan File %s -- Plan", input_plans.Extension ());
	} else {
		Show_Message ("Sorting Plan File -- Plan");
	}
	Set_Progress (10000);

	while (input_plans.Read ()) {
		Show_Progress ();

		//---- check the time period criteria ----

		if (time_flag) {
			if (!time_period.In_Range (input_plans.Time ())) continue;
		}
		if (hhold_flag) {
			if (!hhold_range.In_Range (input_plans.Traveler ())) continue;
		}
		if (delete_flag) {
			skip = (delete_list.Get_Index (input_plans.Household ()) != 0);
		}
		if (select_flag) {
			traveler = input_plans.Traveler ();

			if (current_traveler != traveler) {
				current_traveler = traveler;
				skip = (random.Probability () >= percent);
			}
			if (skip) continue;
		}
		plan_ptr = input_plans.Plan ();

		if (plan_ptr->key1 < 0 || plan_ptr->key2 < 0) {
			Error ("Negative Key values are Not Permitted (%d, %d)", plan_ptr->key1, plan_ptr->key2);
		}
		if (fix_flag) {
			plan_ptr = Fix_Plan (plan_ptr);
		}
		if (!plan_list.Add (&plan_ptr)) {
			Error ("Insufficient Memory for Plan Sort");
		}
	}
	End_Progress ();

	input_plans.Close ();
}
