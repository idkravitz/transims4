//*********************************************************
//	Survey_Population.cpp - read the survey population
//*********************************************************

#include "ActivityPattern.hpp"

//---------------------------------------------------------
//	Survey_Population
//---------------------------------------------------------

void ActivityPattern::Survey_Population (void)
{
	int hhold;

	Survey_HHold *household_ptr;
	Survey_Person *population_ptr;

	if (!survey_pop_data.Max_Records (survey_pop_file.Num_Records ())) goto mem_error;

	//---- read the survey population file ----

	Show_Message ("Reading %s -- Record", survey_pop_file.File_Type ());
	Set_Progress (50000);
	
	while (survey_pop_file.Read ()) {
		Show_Progress ();

		//---- check the household id ----

		hhold = survey_pop_file.Household ();
		if (hhold <= 0) continue;

		household_ptr = survey_hh_data.Get (hhold);
		if (household_ptr == NULL) {
			Error ("Survey Person Household %d was Not Found", hhold);
		}

		population_ptr = survey_pop_data.New_Record (true);
		if (population_ptr == NULL) goto mem_error;

		population_ptr->Household (hhold);
		population_ptr->Person (survey_pop_file.Person ());
		population_ptr->Age (survey_pop_file.Age ());
		population_ptr->Gender (survey_pop_file.Gender ());
		population_ptr->Work (survey_pop_file.Work ());
		population_ptr->Drive (survey_pop_file.Drive ());
		population_ptr->Count (0);

		if (!survey_pop_data.Add ()) {
			Error ("Adding Record to the Population Data Array");
		}
	}
	End_Progress ();
	survey_pop_file.Close ();

	Print (2, "Number of %s Records = %d", survey_pop_file.File_Type (), Progress_Count ());

	survey_pop_data.Optimize ();
	return;

mem_error:
	Error ("Insufficient Memory for Survey Population Data");
}
