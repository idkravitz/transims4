//*********************************************************
//	Survey_Households.cpp - read the survey households
//*********************************************************

#include "ActivityPattern.hpp"

//---------------------------------------------------------
//	Survey_Households
//---------------------------------------------------------

void ActivityPattern::Survey_Households (void)
{
	int id, type;
	double weight;

	Type_Data *type_ptr;
	Weight_Data *weight_ptr;
	Survey_HHold *household_ptr;

	if (!survey_hh_data.Max_Records (survey_hh_file.Num_Records ())) goto mem_error;

	//---- read the survey household file ----

	Show_Message ("Reading %s -- Record", survey_hh_file.File_Type ());
	Set_Progress (50000);
	
	while (survey_hh_file.Read ()) {
		Show_Progress ();

		//---- check the household id ----

		id = survey_hh_file.Household ();
		if (id <= 0) continue;

		household_ptr = survey_hh_data.New_Record (true);
		if (household_ptr == NULL) goto mem_error;

		household_ptr->Household (id);
		household_ptr->Persons (survey_hh_file.Persons ());
		household_ptr->Workers (survey_hh_file.Workers ());
		household_ptr->Vehicles (survey_hh_file.Vehicles ());
		household_ptr->Location (survey_hh_file.Location ());

		//---- household weight ----

		weight = 1.0;

		if (weight_flag) {
			weight_ptr = weight_data.Get (id);

			if (weight_ptr != NULL) {
				weight = weight_ptr->Weight ();
			}
		}
		household_ptr->Weight (weight);

		//---- get the household type ----

		if (survey_type_flag) {
			type = survey_type.Execute ();
		} else {
			type = survey_hh_file.Type ();
		}
		if (type <= 0) {
			Error ("Survey Household Type %d is Out of Range (>0)", type);
		}

		//---- add to the type list ----

		type_ptr = type_data.Get (type);

		if (type_ptr == NULL) {
			type_ptr = type_data.New_Record (true);

			type_ptr->Type (type);

			if (!type_data.Add ()) {
				Error ("Adding Type Data to the List");
			}
		}

		//---- save the type link list and household data ----

		household_ptr->Type (type_ptr->Household ());

		if (!survey_hh_data.Add ()) {
			Error ("Adding Record to the Household Data Array");

		}

		//---- update the type data ----

		type_ptr->Household (survey_hh_data.Num_Records ());
		type_ptr->Add_Survey ();
		type_ptr->Add_Weight (weight);
	}
	End_Progress ();
	survey_hh_file.Close ();

	Print (2, "Number of %s Records = %d", survey_hh_file.File_Type (), Progress_Count ());
	Print (1, "Number of Survey Household Types = %d", type_data.Num_Records ());

	//---- check the weights ----

	for (type_ptr = type_data.First (); type_ptr; type_ptr = type_data.Next ()) {
		if (type_ptr->Cum_Weight () <= 0.0) {
			Error ("Household Type %d Cumulative Weight = %.6lf", 
				type_ptr->Type (), type_ptr->Cum_Weight ());
		}
	}
	survey_hh_data.Optimize ();
	return;

mem_error:
	Error ("Insufficient Memory for Survey Household Data");
}
