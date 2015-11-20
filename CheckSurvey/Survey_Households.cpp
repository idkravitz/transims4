//*********************************************************
//	Survey_Households.cpp - read the survey households
//*********************************************************

#include "CheckSurvey.hpp"

//---------------------------------------------------------
//	Survey_Households
//---------------------------------------------------------

void CheckSurvey::Survey_Households (void)
{
	int id, type;

	Household_Data *household_ptr;

	if (!survey_hh_data.Max_Records (survey_hh_file.Num_Records ())) goto mem_error;

	//---- read the survey household file ----

	Show_Message ("Reading %s -- Record", survey_hh_file.File_Type ());
	Set_Progress (50000);
	
	while (survey_hh_file.Read ()) {
		Show_Progress ();

		//---- check the household id ----

		id = survey_hh_file.Household ();
		if (id <= 0) continue;

		if (hhlist_flag) {
			if (hhold_list.Get_Index (id) == 0) continue;
		}

		//---- get the household type ----

		if (survey_type_flag) {
			type = survey_type.Execute ();
		} else {
			type = survey_hh_file.Type ();
		}
		if (type <= 0) {
			Warning ("Survey Household %d Type %d is Out of Range (>0)", id, type);
		}

		//---- store the household record ----

		household_ptr = survey_hh_data.New_Record (true);
		if (household_ptr == NULL) goto mem_error;

		household_ptr->Household (id);
		household_ptr->Persons (survey_hh_file.Persons ());
		household_ptr->Workers (survey_hh_file.Workers ());
		household_ptr->Vehicles (survey_hh_file.Vehicles ());
		household_ptr->Location (1);	//---- initialize household weight ----
		household_ptr->Type (type);

		if (!survey_hh_data.Add ()) {
			Error ("Adding Record to the Household Data Array");
		}

		nhhold++;
		nperson += household_ptr->Persons ();
		nwork += household_ptr->Workers ();
	}
	End_Progress ();
	survey_hh_file.Close ();

	Print (2, "Number of %s Records = %d", survey_hh_file.File_Type (), Progress_Count ());
	if (hhlist_flag) {
		Print (1, "Number of Household Records Kept = %d", survey_hh_data.Num_Records ());
	}
	survey_hh_data.Optimize ();
	return;

mem_error:
	Error ("Insufficient Memory for Survey Household Data");
}
