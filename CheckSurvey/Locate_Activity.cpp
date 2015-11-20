//*********************************************************
//	Locate_Activity.cpp - set tour anchor
//*********************************************************

#include "CheckSurvey.hpp"

//---------------------------------------------------------
//	Locate_Activity
//---------------------------------------------------------

void CheckSurvey::Locate_Activity (void)
{
	int i, j, k, nact, hhold, person, start, end, anchor, arrive, leave, purpose, duration, order;
	bool home_flag;

	Activity_Data *act_ptr, *loc_ptr, *anchor_ptr, *des_ptr;

	//---- identify household tours ----

	home_flag = true;
	hhold = person = start = end = 0;
	nact = hhold_act_data.Num_Records ();

	for (i=1; i <= nact; i++) {

		act_ptr = hhold_act_data [i];

		if (person != act_ptr->Person () || act_ptr->Purpose () == 0 || i == nact) {

			//---- process the tour ----

			if (!home_flag && person > 0) {
				end = i;

				//---- find the anchor activity ----

				anchor = leave = duration = 0;

				for (j = start + 1; j < end; j++) {
					loc_ptr = hhold_act_data [j];

					purpose = loc_ptr->Purpose ();
					if (purpose < 1) continue;

					order = 0;
					for (k = anchor_list.First (); k > 0; k = anchor_list.Next ()) {
						if (k == purpose) {
							order = anchor_list.Record_Index ();
							break;
						}
					}
					if (order > 0) {
						loc_ptr->Priority (ANCHOR);

						if (anchor == 0) {
							anchor = leave = j;
							duration = loc_ptr->Duration ();
							anchor_ptr = loc_ptr;
						} else {
							if (anchor_ptr->Purpose () == loc_ptr->Purpose ()) {
								if (anchor_ptr->Location () == loc_ptr->Location ()) {
									duration += loc_ptr->Duration ();
									leave = j;
								} else if (loc_ptr->Duration () > duration) {
									anchor = leave = j;
									duration = loc_ptr->Duration ();
									anchor_ptr = loc_ptr;
								}
							} else if (loc_ptr->Duration () > duration) {
								anchor = leave = j;
								duration = loc_ptr->Duration ();
								anchor_ptr = loc_ptr;
							}
						}
					} else if (anchor == 0) {
						if (loc_ptr->Duration () > duration) {
							leave = j;
							duration = loc_ptr->Duration ();
							anchor_ptr = loc_ptr;
						}
					}
				}

				//---- locate the anchor activity -----

				if (anchor > 0 || leave > 0) {
					if (anchor == 0) {
						arrive = leave;
					} else {
						arrive = anchor;
					}
					des_ptr = hhold_act_data [arrive];

					des_ptr->Priority (PRIMARY_ANCHOR);
				}
			}

			//---- reset the data ----

			home_flag = true;
			hhold = act_ptr->Household ();
			person = act_ptr->Person ();
			start = i;

		} else {
			home_flag = false;
		}
	}
}
