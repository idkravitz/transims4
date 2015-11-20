//*********************************************************
//	Output_Plan.cpp - Write Output Plans and Index
//*********************************************************

#include "PlanPrep.hpp"

//---------------------------------------------------------
//	Output_Plan
//---------------------------------------------------------

void PlanPrep::Output_Plan (Plan_Data *plan_ptr, bool label)
{
	off_t offset;

	//---- get the current file offset ----

	if (!first_out && size_flag) {
		offset = output_plans.Offset ();
	} else {
		offset = 0;
	}

	//---- check max output plan size ----

	if (first_out || (size_flag && offset > max_size)) {
		if (nout) {
			output_plans.Close ();
		}
		if (!output_plans.Open (nout++)) {
			File_Error ("Creating Output Plan File", output_plans.Filename ());
		}
		first_out = false;

		if (label) {
			if (Progress_Count () > 0) {
				End_Progress ();
			}
			if (output_plans.Extend ()) {
				Show_Message ("Writing Plan File %s -- Plan", output_plans.Extension ());
			} else {
				Show_Message ("Writing Plan File -- Plan");
			}
			Set_Progress (10000);
		}
	}

	//---- write the plan records ----

	if (!output_plans.Write (plan_ptr)) {
		Error ("Writing Plan File");
	}
	return;
}
