//*********************************************************
//	Partition_Plans.cpp - Partition Plan Files
//*********************************************************

#include "PlanPrep.hpp"

//---------------------------------------------------------
//	Partition_Plans
//---------------------------------------------------------

void PlanPrep::Partition_Plans (void)
{
	int file, hhold;
	bool check;

	Plan_Data *plan_ptr;
	Partition *part_ptr;
	Plan_File temp_plan;

	if (input_plans.Extend ()) {
		if (partition_flag) {
			Show_Message ("Partitioning Plan File %s -- Plan", input_plans.Extension ());
		} else {
			Show_Message ("Selecting Plans from File %s -- Plan", input_plans.Extension ());
		}
	} else {
		if (partition_flag) {
			Show_Message ("Partitioning Plan File -- Plan");
		} else {
			Show_Message ("Selecting Plans from Plan File -- Plan");
		}
	}
	Set_Progress (10000);
	check = false;

	while (input_plans.Read ()) {
		Show_Progress ();

		hhold = input_plans.Household ();

		part_ptr = (Partition *) partition.Get (&hhold);

		if (part_ptr == NULL) {
			if (partition_flag) {
				Warning ("A Partition was Not Found for Traveler %d", input_plans.Traveler ());
			}
			continue;
		} else {
			file = part_ptr->partition;
		}
		plan_ptr = input_plans.Plan ();

		if (fix_flag) {
			plan_ptr = Fix_Plan (plan_ptr);
		}

		//---- check for one leg plans ----

		if (one_leg_flag) {
			if (plan_ptr->leg == 1) {
				if (check) num_one_leg++;
				temp_plan.Plan (plan_ptr);
				check = true;
				continue;
			} else if (plan_ptr->leg == 2) {
				Output_Plan (temp_plan.Plan ());
				check = false;
			}
		}
		if (!plan_file [file]->Write (plan_ptr)) {
			Error ("Writing Plan File");
		}
	}
	End_Progress ();

	input_plans.Close ();
}
