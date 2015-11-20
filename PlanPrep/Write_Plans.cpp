//*********************************************************
//	Write_Plans.cpp - Write Sorted Plan Files
//*********************************************************

#include "PlanPrep.hpp"

//---------------------------------------------------------
//	Write_Plans
//---------------------------------------------------------

void PlanPrep::Write_Plans (void)
{
    Plan_Data **plan_ptr, *temp_ptr;

	Set_Progress (10000);
	temp_ptr = 0;

	//---- output the plans ----

	for (plan_ptr = (Plan_Data **) plan_list.First_Key ();
		plan_ptr != NULL;
		plan_ptr = (Plan_Data **) plan_list.Next_Key ()) {

		if (one_leg_flag) {
			if ((*plan_ptr)->leg == 1) {
				if (temp_ptr && temp_ptr->leg == 1) num_one_leg++;
				temp_ptr = *plan_ptr;
				continue;
			} else if ((*plan_ptr)->leg == 2) {
				Output_Plan (temp_ptr);
				temp_ptr = 0;
			}
		}
		Output_Plan (*plan_ptr, true);

		Show_Progress ();
	}
	End_Progress ();

	output_plans.Close ();
}
