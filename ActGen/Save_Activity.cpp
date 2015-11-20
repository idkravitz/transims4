//*********************************************************
//	Save_Activity.cpp - save the person activity
//*********************************************************

#include "ActGen.hpp"

//---------------------------------------------------------
//	Save_Activity
//---------------------------------------------------------

void ActGen::Save_Activity (HHold_Data *hh_ptr, Population_Data *pop_ptr, int hhold, int person)
{
	int num = 1;

	Activity_Data *new_ptr, *last_ptr, *act_ptr, act_rec;

	//---- build the record index ----

	act_rec.Household (hhold);
	act_rec.Person (person);
	act_rec.Activity (1);
	last_ptr = 0;

	//---- retrieve each activity ----

	for (act_ptr = survey_act_data.Get_GE (&act_rec); act_ptr; act_ptr = survey_act_data.Next_Key ()) {

		if (act_ptr->Household () != hhold || act_ptr->Person () != person) break;

		//---- collapse home activities ----

		if (num > 1 && act_ptr->Purpose () == 0) {
			if (last_ptr->Purpose () == 0) {
				last_ptr->End_Time (MAX ((act_ptr->End_Time () + time_variance), 0));
				last_ptr->Duration (last_ptr->End_Time () - last_ptr->Start_Time ());
				continue;
			}
		}

		//---- save a new activity ----

		new_ptr = hhold_act_data.New_Record ();

		memcpy (new_ptr, act_ptr, hhold_act_data.Data_Size ());

		new_ptr->Household (hh_ptr->Household ());
		new_ptr->Person (pop_ptr->Person ());
		new_ptr->Activity (num++);

		if (time_variance != 0) {
			new_ptr->Start_Time (MIN (MAX ((new_ptr->Start_Time () + time_variance), 0), MIDNIGHT - 1));
			new_ptr->End_Time (MAX ((new_ptr->End_Time () + time_variance), 0));
			if (new_ptr->End_Time () <= new_ptr->Start_Time ()) {
				new_ptr->End_Time (new_ptr->Start_Time () + 1);
			}
			new_ptr->Duration (new_ptr->End_Time () - new_ptr->Start_Time ());
		}
		if (act_ptr->Purpose () == 0) {
			new_ptr->Location (hh_ptr->Location ());
		} else {
			new_ptr->Location (-act_ptr->Location ());
		}

		if (act_ptr->Vehicle () == 1) {
			new_ptr->Vehicle (hh_ptr->Vehicle_List ());

			if (new_ptr->Vehicle () == 0) {
				new_ptr->Vehicle (-1);
			}
		} else if (act_ptr->Vehicle () == 2) {
			if (act_ptr->Mode () >= MAGIC_MOVE) {
				new_ptr->Vehicle (0);
			} else {
				new_ptr->Vehicle (-2);
			}
		}
		new_ptr->Priority (0);
		new_ptr->Constraint (0);

		if (!hhold_act_data.Add ()) {
			Error ("Saving Activity Data");
		}
		last_ptr = new_ptr;
	}
}
