 //*********************************************************
//	Sort_Merge.cpp - Merge Sorted Plan Files
//*********************************************************

#include "PlanPrep.hpp"

//---------------------------------------------------------
//	Sort_Merge
//---------------------------------------------------------

void PlanPrep::Sort_Merge (void)
{
	int next_in, next_merge, next_in_trip, next_merge_trip, last_merge, replace_id, replace_trip, current_in;
	bool read_merge, read_in, save_merge, save_in;
	Plan_Data **input_ptr, *merge_ptr, *temp_ptr;

	if (input_plans.Extend ()) {
		Show_Message ("Merging Plan Files %s -- Plan", input_plans.Extension ());
	} else {
		Show_Message ("Merging Plan Files -- Plan");
	}
	Set_Progress (10000);

	last_merge = replace_id = replace_trip = current_in = 0;
	read_merge = read_in = save_merge = save_in = false;

	temp_ptr = 0;

	for (input_ptr = (Plan_Data **) plan_list.First_Key (); ; input_ptr = (Plan_Data **) plan_list.Next_Key ()) {
		if (input_ptr == NULL) {
			next_in = MAX_INTEGER;
		} else {
			if (one_leg_flag && (*input_ptr)->leg == 1) {
				if (temp_ptr && temp_ptr->leg == 1) num_one_leg++;
				temp_ptr = *input_ptr;
				continue;
			}
			next_in = (*input_ptr)->key1;
			next_in_trip = (*input_ptr)->trip;
		}
		break;
	}

	//---- read the first record in the merge file ----

	if (merge_plans.Read ()) {
		merge_ptr = merge_plans.Plan ();
		next_merge = merge_ptr->key1;
		next_merge_trip = merge_ptr->trip;
	} else {
		next_merge = MAX_INTEGER;
	}

	//---- interleaf plan records based on traveler ID ----

	for (;;) {
		if (next_in == MAX_INTEGER && next_merge == MAX_INTEGER) break;

		Show_Progress ();

		//---- set the processing flags ----

		if (next_merge < next_in) {
			if (next_merge == replace_id && (!trip_flag || next_merge_trip == replace_trip)) {
				save_merge = false;
			} else {
				save_merge = true;
			}
			read_merge = true;
			read_in = save_in = false;
		} else if (next_merge == next_in) {

			if (trip_flag) {
				if (next_merge_trip < next_in_trip) {
					read_merge = save_merge = true;
					read_in = save_in = false;					
				} else if (next_merge_trip == next_in_trip) {
					if (replace_id != next_in) {
						replaced++;
						replace_id = next_in;
					}
					replace_trip = next_in_trip;

					if (compare_flag) {
						Compare (*input_ptr, merge_ptr);
					}
					read_merge = read_in = save_in = true;
					save_merge = false;
				} else {
					read_in = save_in = true;
					read_merge = save_merge = false;
				}
			} else {
				if (replace_id != next_in) {
					replaced++;
					replace_id = next_in;
					replace_trip = next_in_trip;
				}
				if (compare_flag) {
					Compare (*input_ptr, merge_ptr);
				}
				read_merge = read_in = save_in = true;
				save_merge = false;
			}
		} else {
			read_in = save_in = true;
			read_merge = save_merge = false;
		}
		
		//---- write the merge plan to the output file ----
		
		if (save_merge) {
			if (next_merge < last_merge) {
				Error ("The Merge Plan File is Not Sorted by %s", 
					((sort == TIME_SORT) ? "Time" : "Traveler"));
			}
			last_merge = next_merge;

			Output_Plan (merge_ptr);
		}

		//---- get the next merge plan ----

		if (read_merge) {
			if (merge_plans.Read ()) {
				merge_ptr = merge_plans.Plan ();
				next_merge = merge_ptr->key1;
				next_merge_trip = merge_ptr->trip;
			} else {
				next_merge = MAX_INTEGER;
			}
		}

		//---- write the input plan in the output file ----

		if (save_in) {
			if (one_leg_flag && (*input_ptr)->leg == 2) {
				Output_Plan (temp_ptr);
				temp_ptr = 0;
			}
			Output_Plan (*input_ptr);
		}

		//---- get the next input plan ----

		if (read_in) {
			for (;;) {
				input_ptr = (Plan_Data **) plan_list.Next_Key ();

				if (input_ptr == NULL) {
					next_in = MAX_INTEGER;
				} else {
					if (one_leg_flag && (*input_ptr)->leg == 1) {
						if (temp_ptr && temp_ptr->leg == 1) num_one_leg++;
						temp_ptr = *input_ptr;
						continue;
					}
					next_in = (*input_ptr)->key1;
					next_in_trip = (*input_ptr)->trip;
				}
				break;
			}
		}
	}
	End_Progress ();

	merge_plans.Close ();
	output_plans.Close ();
}
