//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "PlanPrep.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void PlanPrep::Execute (void)
{
	int i;

	//---- read household lists ----

	if (hhlist_flag) {
		Read_HHList ();
	}

	//---- read the delete household lists ----

	if (delete_flag) {
		Read_Deletes ();
	}

	//---- read fix plan file ----

	if (fix_flag) {
		Read_Fix_Plans ();
	}

	//---- read the plan files ----

	if (sort_flag || merge_flag || subarea_flag || (!combine_flag && output_flag)) {

		size_flag = (max_size_flag && !(combine_flag && combine == FILE_COMBINE));

		for (nfile=0; ; nfile++) {
			first_out = true;

			//---- open the input plans ----

			if (!input_plans.Open (nfile)) break;

			//---- open the merge plans ----

			if (merge_flag) {
				if (!merge_plans.Open (nfile)) {
					File_Error ("Opening Merge Plan File", merge_plans.Filename ());
				}
			} else if (subarea_flag) {
				if (subpart_flag) {
					i = nfile;
				} else {
					i = 0;
					subarea_plans.Reset_Counters ();
				}
				if (!subarea_plans.Open (i)) {
					File_Error ("Opening Subarea Plan File", subarea_plans.Filename ());
				}
			}

			//---- process the plan files ----

			if (combine_flag && combine == MEMORY_COMBINE) {

				//---- sort the input plans ----

				Sort_Plans ();

			} else if (sort_flag) {

				//---- sort the input plans ----

				Sort_Plans ();

				//---- merge and/or write plans ----

				if (merge_flag) {
					Sort_Merge ();
				} else {
					Write_Plans ();
				}
				plan_list.Reset ();

			} else if (hhlist_flag) {

				Partition_Plans ();

			} else {

				//---- merge sorted files or copy to output file ----

				if (merge_flag) {
					File_Merge ();
				} else if (subarea_flag) {
					Subarea_Merge ();
				} else {
					Copy_Plans ();
				}
			}
		}
		if (nfile == 0) {
			File_Error ("No Plan Files were Found", input_plans.Filename ());
		}
	}

	//---- create combined output file ----

	if (combine_flag) {
		nout = 0;
		first_out = true;
		size_flag = max_size_flag;
		input_sort_flag = (sort_flag || input_sort_flag);

		output_plans.Filename (output_name);
		output_plans.Extend (extend);
		output_plans.Reset_Counters ();
		output_plans.File_Format ((binary_flag) ? BINARY : UNFORMATED);

		if (combine == MEMORY_COMBINE) {
			Write_Plans ();
			plan_list.Clear ();
		} else {
			File_Combine ();
		}

	} else if (hhlist_flag) {

		//---- close the files and clear the memory -----

		output_plans.Close ();

		for (i=0; i < nout; i++) {
			plan_file [i]->Close ();

			output_plans.Reset_Counters (output_plans.Num_Records () + plan_file [i]->Num_Records (),
										output_plans.Num_Plans () + plan_file [i]->Num_Plans (),
										output_plans.Num_Travelers () + plan_file [i]->Num_Travelers (),
										output_plans.Num_Trips () + plan_file [i]->Num_Trips ());
			delete plan_file [i];
		}
		delete [] plan_file;
	}

	//---- write summary statistics ----

	Break_Check (6);

	Write (2, "Number of Input Files = %d", nfile);
	Write (1, "Number of Input Plans = %d", input_plans.Num_Plans ());
	Write (1, "Number of Input Records = %d", input_plans.Num_Records ());
	Write (1, "Number of Input Travelers = %d", input_plans.Num_Travelers ());
	Write (1, "Number of Input Trips = %d", input_plans.Num_Trips ());

	if (one_leg_flag) {
		Write (2, "Number of One Leg Plans Deleted = %d", num_one_leg);
	}
	if (merge_flag) {
		Break_Check (7);

		Write (2, "Number of Merge Plans = %d", merge_plans.Num_Plans ());
        Write (1, "Number of Merge Records = %d", merge_plans.Num_Records ());
		Write (1, "Number of Merge Travelers = %d", merge_plans.Num_Travelers ());
		Write (1, "Number of Merge Trips = %d", merge_plans.Num_Trips ());
		Write (2, "Number of Replaced Travelers = %d", replaced);
	} else if (subarea_flag) {
		Break_Check (6);

		Write (2, "Number of Subarea Plans = %d", subarea_plans.Num_Plans ());
        Write (1, "Number of Subarea Records = %d", subarea_plans.Num_Records ());
		Write (1, "Number of Subarea Travelers = %d", subarea_plans.Num_Travelers ());
		Write (1, "Number of Subarea Trips = %d", subarea_plans.Num_Trips ());
		Write (1, "Number of Updated Travelers = %d", updated);
	}
	if (output_flag) {
		Break_Check (6);

		Write (2, "Number of Output Files = %d", nout);
		Write (1, "Number of Output Plans = %d", output_plans.Num_Plans ());
		Write (1, "Number of Output Records = %d", output_plans.Num_Records ());
		Write (1, "Number of Output Travelers = %d", output_plans.Num_Travelers ());
		Write (1, "Number of Output Trips = %d", output_plans.Num_Trips ());
	}
	
	//---- print reports ----

	for (i=First_Report (); i != 0; i=Next_Report ()) {
		switch (i) {
			case PATH_CHANGE:		//---- percent path change ----
				Path_Change ();
				break;
			case TIME_CHANGE:		//---- percent travel time change ---
				Travel_Time_Change ();
				break;
			case FIX_PLAN:			//---- fix plan list ----
				Fix_Plan_Report ();
				break;
			default:
				break;
		}
	}
	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void PlanPrep::Page_Header (void)
{
	switch (Header_Number ()) {
		case PATH_CHANGE:
			Path_Change_Header ();
			break;
		case TIME_CHANGE:
			Time_Change_Header ();
			break;
		case FIX_PLAN:
			Fix_Plan_Header ();
			break;
		default:
			break;
	}
}
