//*********************************************************
//	PUMS_Population.cpp - read the PUMS population
//*********************************************************

#include "PopSyn.hpp"

//---------------------------------------------------------
//	PUMS_Population
//---------------------------------------------------------

void PopSyn::PUMS_Population (void)
{
	int hhold, person, db_hhold_field, db_person_field, last_hhold, num_person;
	bool warning_flag = false;

	Household_Data *household_ptr;
	Population_Data *population_ptr;

	db_hhold_field = population_db.Field_Number ("HHOLD");
	db_person_field = population_db.Field_Number ("PERSON");

	if (!population_db.Max_Records (pums_pop_file.Num_Records ())) goto mem_error;
	if (!population_data.Max_Records (pums_pop_file.Num_Records ())) goto mem_error;

	//---- read the pums population file ----

	Show_Message ("Reading %s -- Record", pums_pop_file.File_Type ());
	Set_Progress (50000);

	last_hhold = num_person = 0;
	
	while (pums_pop_file.Read ()) {
		Show_Progress ();

		//---- check the household id ----

		pums_pop_file.Get_Field (pop_hhold_field, &hhold);
		if (hhold <= 0) continue;

		household_ptr = household_data.Get (hhold);
		if (household_ptr == NULL) continue;

		if (hhold != last_hhold) {
			last_hhold = hhold;
			num_person = 1;
		} else {
			num_person++;
		}

		//---- save the population database ----

		population_db.Copy_Fields (&pums_pop_file);

		population_db.Put_Field (db_hhold_field, hhold);

		if (pop_person_field > 0) {
			pums_pop_file.Get_Field (pop_person_field, &person);
			if (person == 0) {
				person = num_person;
			}
		} else {
			person = num_person;
		}

		//---- check for a person number ----

		if (person >= Traveler_Scale ()) {
			if (!warning_flag) {
				Warning ("Person %d is Out of Range (1..%d)", person, Traveler_Scale ());
				warning_flag = true;
			}
			person = person % Traveler_Scale ();
		}
		population_db.Put_Field (db_person_field, person);

		if (!population_db.Add_Record ()) {
			Error ("Writing %s Database", population_db.File_Type ());
		}

		//---- save the population data ----

		population_ptr = population_data.New_Record (true);
		if (population_ptr == NULL) goto mem_error;

		population_ptr->Household (hhold);
		population_ptr->Person (person);

		if (!population_data.Add ()) {
			Error ("Adding Record to the Population Data Array");
		}
	}
	End_Progress ();
	pums_pop_file.Close ();

	Print (2, "Number of %s Records = %d", pums_pop_file.File_Type (), Progress_Count ());
	Print (1, "Number of %s Records = %d", population_db.File_Type (), population_db.Num_Records ());

	population_data.Optimize ();
	return;

mem_error:
	Error ("Insufficient Memory for PUMS Population Data");
}
