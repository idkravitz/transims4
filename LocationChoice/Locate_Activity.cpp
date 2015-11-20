//*********************************************************
//	Locate_Activity.cpp - select activity locations
//*********************************************************

#include "LocationChoice.hpp"

//---------------------------------------------------------
//	Locate_Activity
//---------------------------------------------------------

void LocationChoice::Locate_Activity (void)
{
	int i, j, k, nact, hhold, person, start, end, anchor, arrive, leave, purpose, duration, group;
	bool home_flag;

	Activity_Data *act_ptr, *loc_ptr, *anchor_ptr, *des_ptr;
	Purpose_Group *group_ptr;

	//---- initialize the household data ----

	if (choice_flag && hhold_flag) {
		household_db.Read_Record (household_data.Record_Index ());
	}

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

				if (dump_flag) {
					Print (2, "**** Household %d, Person %d, Tour %d-%d ****", hhold, person, start, end);
				}

				//---- find the anchor activity ----

				anchor = leave = duration = 0;

				for (j = start + 1; j < end; j++) {
					loc_ptr = hhold_act_data [j];

					purpose = loc_ptr->Purpose ();
					if (purpose < 1) continue;

					if (purpose > max_purpose) {
						if (purpose > MAX_PURPOSE) purpose = MAX_PURPOSE - 1;
						max_purpose = purpose;
						purpose_map [purpose] = -1;
						continue;
					}
					group = purpose_map [purpose];
					if (group < 1) {
						if (group == 0) purpose_map [purpose] = -1;
						continue;
					}
					group_ptr = purpose_group [group];
					if (group_ptr == NULL) continue;

					loc_ptr->Constraint (group_ptr->Constraint ());

					if (group_ptr->Anchor ()) {
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

					Set_Location (start, arrive, leave, end);
				}

				//---- process each intermediate stop ----

				for (j=start; j <= end; j++) {
					loc_ptr = hhold_act_data [j];

					if (loc_ptr->Location () > 0) {
						start = j;
					} else {
						arrive = leave = j;

						for (k=j+1; k <= end; k++) {
							loc_ptr = hhold_act_data [k];

							if (loc_ptr->Location () > 0) break;
						}
						if (k > end) {
							anchor = start;
						} else {
							anchor = k;
						}
						Set_Location (start, arrive, leave, anchor);

						if (loc_ptr->Location () > 0) start = leave;
					}
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
