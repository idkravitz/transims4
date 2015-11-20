//*********************************************************
//	Fix_Plan.cpp - Fix Plan Legs
//*********************************************************

#include "PlanPrep.hpp"

//---------------------------------------------------------
//	Fix_Plan
//---------------------------------------------------------

Plan_Data * PlanPrep::Fix_Plan (Plan_Data *plan_ptr)
{
	if (plan_ptr == NULL) return (plan_ptr);
	if (plan_ptr->mode != AUTO_MODE || plan_ptr->driver_flag != 1) return (plan_ptr);

	int i, j, k, num, first, *path_ptr, *path, *p, *old, *fix;

	Fix_List *fix_ptr;

	num = plan_ptr->tokens - 2 - plan_ptr->data [1];
	path_ptr = plan_ptr->data + 2;

	for (fix_ptr = (Fix_List *) fix_list.First (); fix_ptr; fix_ptr = (Fix_List *) fix_list.Next ()) {
		path = path_ptr;
		first = fix_ptr->old_list [0];

		for (i=0; i < num; i++, path++) {

			//---- scan for the first leg ----

			if (*path == first) {
				p = path;
				old = fix_ptr->old_list;

				//---- compare the leg lists ----

				for (j=i, k=0; j < num && k < fix_ptr->num_old; j++, k++, p++, old++) {
					if (*p != *old) break;
				}

				//---- found a match ----

				if (k == fix_ptr->num_old) {
					fix_ptr->num_in++;

					if (random.Probability () <= fix_ptr->share) {
						fix_ptr->num_out++;

						//---- replace the legs ----

						if (fix_ptr->num_old >= fix_ptr->num_new) {
							p = path;
							fix = fix_ptr->new_list;

							for (j=i, k=0; j < num && k < fix_ptr->num_new; j++, k++, p++, fix++) {
								*p = *fix;
							}
							k = fix_ptr->num_old - fix_ptr->num_new;

							//---- remove the gap ----

							if (k > 0) {
								fix = p + k;

								for (j += k; j < num; j++, p++, fix++) {
									*p = *fix;
								}
							}

							//---- update the token count ----

							plan_ptr->tokens -= k;

						} else {

							//---- make room ----

							k = fix_ptr->num_new - fix_ptr->num_old;
							num = plan_ptr->tokens - 2;
							plan_ptr->tokens += k;

							input_plans.Check_Size ((int) plan_ptr->tokens);

							//---- update the path pointer ----

							plan_ptr = input_plans.Plan ();
							path_ptr = plan_ptr->data + 2;

							//---- enlarge the gap ----

							p = path_ptr + num - 1;
							fix = p + k;
							k = i + fix_ptr->num_old;

							for (j=num; j > k; j--, p--, fix--) {
								*fix = *p;
							}

							//---- add the new legs ----

							p = path_ptr + i;
							fix = fix_ptr->new_list;

							for (k=0; k < fix_ptr->num_new; k++, p++, fix++) {
								*p = *fix;
							}
						}
					}
					break;
				}
			}
		}
	}
	return (plan_ptr);
}
