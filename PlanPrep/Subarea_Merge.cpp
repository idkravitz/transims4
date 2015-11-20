//*********************************************************
//	Subarea_Merge.cpp - Merge Plan and Subarea Files
//*********************************************************

#include "PlanPrep.hpp"

//---------------------------------------------------------
//	Subarea_Merge
//---------------------------------------------------------

void PlanPrep::Subarea_Merge (void)
{
	int i, j, k, num_in, num_sub, num, *path_in, *path_sub, *in, *sub;

	int next_in, next_sub, last_in, last_sub, update_id, current_in;
	bool read_sub, read_in;
	Plan_Data *input_ptr, *subarea_ptr;

	if (input_plans.Extend ()) {
		Show_Message ("Merging Subarea Plan File %s -- Plan", input_plans.Extension ());
	} else {
		Show_Message ("Merging Subarea Plan File -- Plan");
	}
	last_in = last_sub = update_id = current_in = 0;
	read_sub = read_in = false;

	Set_Progress (10000);

	//---- read the first record in the input file ----

	if (input_plans.Read ()) {
        input_ptr = input_plans.Plan ();
		next_in = input_ptr->key1;
	} else {
		next_in = MAX_INTEGER;
	}

	//---- read the first record in the merge file ----

	if (subarea_plans.Read ()) {
		subarea_ptr = subarea_plans.Plan ();
		next_sub = subarea_ptr->key1;
	} else {
		next_sub = MAX_INTEGER;
	}

	//---- interleaf plan records based on traveler ID ----

	for (;;) {
		if (next_in == MAX_INTEGER && next_sub == MAX_INTEGER) break;

		Show_Progress ();

		if (next_sub > next_in || (next_sub == next_in && 
			(input_ptr->start_type != PARKING_ID || input_ptr->end_type != PARKING_ID))) {

			//---- save the input plan ---

			Output_Plan (input_ptr);

			//---- get the next input plan ----

			if (input_plans.Read ()) {
				input_ptr = input_plans.Plan ();
				next_in = input_ptr->key1;

				if (next_in < last_in) {
					Error ("The Input Plan File is Not Sorted by Traveler (%d < %d)", next_in, last_in);
				}
				last_in = next_in;
			} else {
				next_in = MAX_INTEGER;
			}

		} else {

			if (next_sub == next_in) {
				if (update_id != next_in) {
					updated++;
					update_id = next_in;
				}

				//---- check for drive travel legs ----

				if (subarea_ptr->start_type == PARKING_ID && subarea_ptr->end_type == PARKING_ID) {

					//---- merge subarea plan with input plan ----

					num_in = input_ptr->tokens - 2; // - input_ptr->data [1];
					path_in = input_ptr->data + 2;

					num_sub = subarea_ptr->tokens - 2; // - subarea_ptr->data [1];
					path_sub = subarea_ptr->data + 2;

					//---- check the origin parking lots ----

					if (subarea_ptr->start_id == input_ptr->start_id) {
						i = 0;
					} else {

						//---- find the first leg ----

						for (i=0; i < num_in; i++, path_in++) {
							if (*path_sub == *path_in) break;
						}
					}
					if (i == num_in) {
						Warning ("Traveler %d Plans do Not Match", next_in);
					} else {

						//---- check the destination parking lots ----

						if (subarea_ptr->end_id == input_ptr->end_id) {
							j = num_in - 1;
							k = j - i + 1;
							in = path_in + k - 1;
						} else {

							//---- find the end leg ----

							in = path_in;
							sub = path_sub + num_sub - 1;

							for (j=i, k=1; j < num_in; j++, k++, in++) {
								if (*in == *sub) break;
							}
						}
						if (j == num_in) {
							Warning ("Traveler %d Plans do Not Match", next_in);

						} else if (k >= num_sub) {

							//---- update the token count ----

							input_ptr->tokens -= k - num_sub;

							//---- replace the legs ----

							for (k=0; k < num_sub; k++, path_in++, path_sub++) {
								*path_in = *path_sub;
							}

							//---- remove the gap ----

							for (++j, ++in; j < num_in; j++, path_in++, in++) {
								*path_in = *in;
							}

						} else {

							//---- make room ----

							k = num_sub - k;
							num = input_ptr->tokens - 2;
							input_ptr->tokens += k;

							input_plans.Check_Size ((int) input_ptr->tokens);

							//---- update the path pointer ----

							input_ptr = input_plans.Plan ();
							path_in = input_ptr->data + 2;

							//---- enlarge the gap ----

							in = path_in + num - 1;
							sub = in + k;
							k = i + num_sub - k;

							for (j=num; j > k; j--, in--, sub--) {
								*sub = *in;
							}

							//---- add the new legs ----

							path_in += i;
			
							for (k=0; k < num_sub; k++, path_in++, path_sub++) {
								*path_in = *path_sub;
							}
						}
					}
				}
			}

			//---- get the next subarea plan ----

			if (subarea_plans.Read ()) {
				subarea_ptr = subarea_plans.Plan ();
				next_sub = subarea_ptr->key1;

				if (next_sub < last_sub) {
					Error ("The Subarea Plan File is Not Sorted by Traveler (%d < %d)", next_sub, last_sub); 
				}
				last_sub = next_sub;
			} else {
				next_sub = MAX_INTEGER;
			}
		}
	}
	End_Progress ();

	input_plans.Close ();
	subarea_plans.Close ();
	output_plans.Close ();
}
