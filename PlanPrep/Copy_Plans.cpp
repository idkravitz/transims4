//*********************************************************
//	Copy_Plans.cpp - Copy Plan Files
//*********************************************************

#include "PlanPrep.hpp"

//---------------------------------------------------------
//	Copy_Plans
//---------------------------------------------------------

void PlanPrep::Copy_Plans (void)
{
	bool skip, check;
	int current_traveler, traveler;

	Plan_Data *plan_ptr;
	Plan_File temp_plan;
	Partition *part_ptr, part_rec;

	if (input_plans.Extend ()) {
		Show_Message ("Copying Plan File %s -- Plan", input_plans.Extension ());
	} else {
		Show_Message ("Copying Plan File -- Plan");
	}
	Set_Progress (10000);

	current_traveler = 0;
	skip = check = false;

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
			if (delete_list.Get_Index (input_plans.Household ()) != 0) continue;
		}
		if (select_flag) {
			traveler = input_plans.Traveler ();

			if (current_traveler != traveler) {
				current_traveler = traveler;
				skip = (random.Probability () >= percent);
			}
			if (skip) continue;
		}

		//---- check the partition ----

		if (check_flag) {
			part_rec.traveler = input_plans.Traveler ();

			part_ptr = (Partition *) partition.Get (&part_rec);

			if (part_ptr == NULL) {
				part_rec.partition = nfile;

				if (!partition.Add (&part_rec)) {
					Error ("Adding Partition Record");
				}
			} else if (part_ptr->partition != nfile) {
				Warning ("Traveler %d was in Partition %d and %d", 
					part_ptr->traveler, part_ptr->partition, nfile);
				continue;
			}
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
		Output_Plan (plan_ptr);
	}
	End_Progress ();

	input_plans.Close ();
	output_plans.Close ();
}
