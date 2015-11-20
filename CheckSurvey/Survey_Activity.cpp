//*********************************************************
//	Survey_Activity.cpp - read the survey activity
//*********************************************************

#include "CheckSurvey.hpp"

//---------------------------------------------------------
//	Survey_Activity
//---------------------------------------------------------

void CheckSurvey::Survey_Activity (void)
{
	int hhold, person, time, last_hh, last_per, num;
	char *str_ptr;

	Household_Data *household_ptr, *hhold_ptr;
	Population_Data *population_ptr;
	Activity_Data *act_ptr, *last_ptr;

	last_hh = last_per = num = 0;
	last_ptr = NULL;
	hhold_ptr = NULL;

	//---- read the survey activity file ----

	Show_Message ("Reading %s -- Record", survey_act_file.File_Type ());
	Set_Progress (10000);
	
	while (survey_act_file.Read ()) {
		Show_Progress ();

		//---- check the household id ----

		hhold = survey_act_file.Household ();
		if (hhold <= 0) continue;

		if (hhlist_flag) {
			if (hhold_list.Get_Index (hhold) == 0) continue;
		}
		household_ptr = survey_hh_data.Get (hhold);
		if (household_ptr == NULL) {
			Error ("Survey Activity Household %d was Not Found", hhold);
		}

		//---- check the person id ----

		person = survey_act_file.Person ();
		
		population_ptr = survey_pop_data.Get (hhold, person);
		if (population_ptr == NULL) {
			Error ("Survey Activity Person %d-%d was Not Found", hhold, person);
		}

		//---- check for a new household ----

		if (hhold != last_hh) {
			if (last_hh > 0) {
				Assign_Vehicles (hhold_ptr);

				Locate_Activity ();

				Travel_Time (hhold_ptr->Location ());

				Write_Activity (hhold_ptr->Location ());
			}
			last_hh = hhold;
			last_per = person;
			hhold_act_data.Reset ();
			last_ptr = NULL;
			hhold_ptr = household_ptr;
			num = 0;
		} else if (person != last_per) {
			num = 0;
			last_per = person;
		}

		//---- store the activity data ----
		
		act_ptr = hhold_act_data.New_Record ();
		if (act_ptr == NULL) {
			Error ("Insufficient Memory for Household Activity Data");
		}
		act_ptr->Household (hhold);
		act_ptr->Person (person);
		act_ptr->Purpose (survey_act_file.Purpose ());
		act_ptr->Priority (survey_act_file.Priority ());

		time = activity_time.Step (survey_act_file.Start_Min ());
		time = (time + activity_time.Step (survey_act_file.Start_Max ())) / 2;

		act_ptr->Start_Time (time);

		time = activity_time.Step (survey_act_file.End_Min ());
		time = (time + activity_time.Step (survey_act_file.End_Max ())) / 2;

		act_ptr->End_Time (time);

		str_ptr = survey_act_file.Time_Min ();

		if (*str_ptr != '\0') {
			time = activity_time.Duration (str_ptr);
		} else {
			time = act_ptr->End_Time () - act_ptr->Start_Time ();
		}

		str_ptr = survey_act_file.Time_Max ();

		if (*str_ptr != '\0') {
			time = (time + activity_time.Duration (str_ptr)) / 2;
		} else {
			time = (time + act_ptr->End_Time () - act_ptr->Start_Time ()) / 2;
		}
		act_ptr->Duration (time);

		act_ptr->Mode (survey_act_file.Mode ());
		act_ptr->Vehicle (survey_act_file.Vehicle ());
		act_ptr->Location (survey_act_file.Location ());
		act_ptr->Passengers (survey_act_file.Passengers ());
		act_ptr->Constraint (survey_act_file.Constraint ());
		act_ptr->Problem (0);

		//---- collapse home activities ----

		if (num > 0 && act_ptr->Purpose () == 0) {
			if (last_ptr->Purpose () == 0) {
				last_ptr->End_Time (act_ptr->End_Time ());
				last_ptr->Duration (last_ptr->End_Time () - last_ptr->Start_Time ());
				continue;
			}
		}

		//---- keep the activity ----
		
		act_ptr->Activity (++num);

		if (act_ptr->Vehicle () == 1) {
			act_ptr->Vehicle (household_ptr->Vehicles ());

			if (act_ptr->Vehicle () == 0) {
				act_ptr->Vehicle (-1);
			}
		} else if (act_ptr->Vehicle () == 2) {
			if (act_ptr->Mode () >= MAGIC_MOVE) {
				act_ptr->Vehicle (0);
			} else {
				act_ptr->Vehicle (-2);
			}
		}
		if (!hhold_act_data.Add ()) {
			Error ("Adding Record to the Activity Data Array");
		}
		last_ptr = act_ptr;
		num_act_gen++;
	}

	//---- process the last household ----

	if (last_hh > 0) {
		Assign_Vehicles (hhold_ptr);

		Locate_Activity ();

		Travel_Time (hhold_ptr->Location ());

		Write_Activity (hhold_ptr->Location ());
	}
	End_Progress ();
	survey_act_file.Close ();

	Print (2, "Number of %s Records = %d", survey_act_file.File_Type (), Progress_Count ());
	if (hhlist_flag) {
		Print (1, "Number of Activity Records Processed = %d", num_act_gen);
	}

	if (new_act_flag) {
		Print (2, "Number of %s Records = %d", new_act_file.File_Type (), num_new);
	}
}
