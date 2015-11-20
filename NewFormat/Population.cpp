//*********************************************************
//	Population.cpp - convert the population file
//*********************************************************

#include "NewFormat.hpp"

#include "Household_File.hpp"
#include "Population_File.hpp"

//---------------------------------------------------------
//	Population 
//---------------------------------------------------------

void NewFormat::Population (void)
{
	int i, nhhold, npop, hhold, nperson, person, person_field;

	Household_File *household_file;
	Population_File *population_file;
	Person_Number person_rec;

	household_file = (Household_File *) Demand_Db_Base (NEW_HOUSEHOLD);
	population_file = (Population_File *) Demand_Db_Base (NEW_POPULATION);

	//---- construct the field list ----

	Pop_Header ();

	person_field = pop_file.Field_Number ("PERSON");

	if (person_field == 0) {
		Error ("PERSON Field was Not Found");
	}

	//---- read each record ----

	nhhold = npop = 0;
	hhold = nperson = 0;

	Show_Message ("Reading %s -- Record", pop_file.File_Type ());
	Set_Progress (10000);

	while (pop_file.Read (false)) {
		Show_Progress ();

		pop_file.Get_Field ("HHOLD", &hhold);
		pop_file.Get_Field ("PERSONS", &nperson);

		//---- copy the household fields ----

		if (!household_file->Copy_Fields (&pop_file)) {
			Error ("Copying Household %d Fields", hhold);
		}
		if (!household_file->Write ()) {
			Error ("Writing Household Record");
		}
		nhhold++;

		//---- process each population record ----

		for (i=1; i <= nperson; i++) {
			if (!pop_file.Read (true)) {
				Error ("Number of Nested Records for Household %d", hhold);
			}
			Show_Progress ();

			//---- copy the population fields ----

			if (!population_file->Copy_Fields (&pop_file)) {
				Error ("Copying Household %d Population Fields", hhold); 
			}

			//---- check the person number ----

			pop_file.Get_Field (person_field, &person);

			if (activity_flag) {
				person_rec.person_id = person;
				person_rec.person_num = i;

				if (!person_number.Add (&person_rec)) {
					Error ("Adding Person ID Record");
				}
				population_file->Person (i);
			} else if (person > nperson) {
				population_file->Person (i);
			}

			if (!population_file->Write ()) {
				Error ("Writing Population Record");
			}
			npop++;
		}
	}
	End_Progress ();
	pop_file.Close ();

	Print (2, "Number of %s Records = %d", pop_file.File_Type (), Progress_Count ());

	household_file->Close ();
	population_file->Close ();

	Print (1);
	Write (1, "Number of %s Records = %d", household_file->File_Type (), nhhold);
	Write (1, "Number of %s Records = %d", population_file->File_Type (), npop);
	Show_Message (1);
}
