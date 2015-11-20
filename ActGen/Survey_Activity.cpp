//*********************************************************
//	Survey_Activity.cpp - read the survey activity
//*********************************************************

#include "ActGen.hpp"

//---------------------------------------------------------
//	Survey_Activity
//---------------------------------------------------------

void ActGen::Survey_Activity (void)
{
	int hhold, person, time;
	char *str_ptr;

	Survey_HHold *household_ptr;
	Population_Data *population_ptr;
	Activity_Data *activity_ptr;

	if (!survey_act_data.Max_Records (survey_act_file.Num_Records ())) goto mem_error;

	//---- read the survey activity file ----

	Show_Message ("Reading %s -- Record", survey_act_file.File_Type ());
	Set_Progress (50000);
	
	while (survey_act_file.Read ()) {
		Show_Progress ();

		//---- check the household id ----

		hhold = survey_act_file.Household ();
		if (hhold <= 0) continue;

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

		//---- store the activity data ----

		activity_ptr = survey_act_data.New_Record (true);
		if (activity_ptr == NULL) goto mem_error;

		activity_ptr->Household (hhold);
		activity_ptr->Person (person);
		activity_ptr->Activity (survey_act_file.Activity ());
		activity_ptr->Purpose (survey_act_file.Purpose ());
		activity_ptr->Priority (survey_act_file.Priority ());

		time = activity_time.Step (survey_act_file.Start_Min ());
		time = (time + activity_time.Step (survey_act_file.Start_Max ())) / 2;

		activity_ptr->Start_Time (time);

		time = activity_time.Step (survey_act_file.End_Min ());
		time = (time + activity_time.Step (survey_act_file.End_Max ())) / 2;

		activity_ptr->End_Time (time);

		str_ptr = survey_act_file.Time_Min ();

		if (*str_ptr != '\0') {
			time = activity_time.Duration (str_ptr);
		} else {
			time = activity_ptr->End_Time () - activity_ptr->Start_Time ();
		}

		str_ptr = survey_act_file.Time_Max ();

		if (*str_ptr != '\0') {
			time = (time + activity_time.Duration (str_ptr)) / 2;
		} else {
			time = (time + activity_ptr->End_Time () - activity_ptr->Start_Time ()) / 2;
		}
		activity_ptr->Duration (time);

		activity_ptr->Mode (survey_act_file.Mode ());
		activity_ptr->Vehicle (survey_act_file.Vehicle ());
		activity_ptr->Location (survey_act_file.Location ());
		activity_ptr->Passengers (survey_act_file.Passengers ());
		activity_ptr->Constraint (survey_act_file.Constraint ());

		if (!survey_act_data.Add ()) {
			Error ("Adding Record to the Activity Data Array");
		}
	}
	End_Progress ();
	survey_act_file.Close ();

	Print (2, "Number of %s Records = %d", survey_act_file.File_Type (), Progress_Count ());

	survey_act_data.Optimize ();
	return;

mem_error:
	Error ("Insufficient Memory for Survey Activity Data");
}
