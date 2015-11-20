//*********************************************************
//	Survey_Population.cpp - read the survey population
//*********************************************************

#include "CheckSurvey.hpp"

//---------------------------------------------------------
//	Survey_Population
//---------------------------------------------------------

void CheckSurvey::Survey_Population (void)
{
	int hhold;

	Household_Data *household_ptr;
	Population_Data *population_ptr;

	if (!survey_pop_data.Max_Records (survey_pop_file.Num_Records ())) goto mem_error;

	//---- read the survey population file ----

	Show_Message ("Reading %s -- Record", survey_pop_file.File_Type ());
	Set_Progress (50000);
	
	while (survey_pop_file.Read ()) {
		Show_Progress ();

		//---- check the household id ----

		hhold = survey_pop_file.Household ();
		if (hhold <= 0) continue;

		if (hhlist_flag) {
			if (hhold_list.Get_Index (hhold) == 0) continue;
		}
		household_ptr = survey_hh_data.Get (hhold);
		if (household_ptr == NULL) {
			Error ("Survey Person Household %d was Not Found", hhold);
		}

		//---- store the population record ----

		population_ptr = survey_pop_data.New_Record (true);
		if (population_ptr == NULL) goto mem_error;

		population_ptr->Household (hhold);
		population_ptr->Person (survey_pop_file.Person ());
		population_ptr->Age (survey_pop_file.Age ());
		population_ptr->Gender (survey_pop_file.Gender ());
		population_ptr->Work (survey_pop_file.Work ());
		population_ptr->Drive (survey_pop_file.Drive ());

		if (!survey_pop_data.Add ()) {
			Error ("Adding Record to the Population Data Array");
		}
	}
	End_Progress ();
	survey_pop_file.Close ();

	Print (2, "Number of %s Records = %d", survey_pop_file.File_Type (), Progress_Count ());
	if (hhlist_flag) {
		Print (1, "Number of Population Records Kept = %d", survey_pop_data.Num_Records ());
	}
	survey_pop_data.Optimize ();
	return;

mem_error:
	Error ("Insufficient Memory for Survey Population Data");
}
