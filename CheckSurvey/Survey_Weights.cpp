//*********************************************************
//	Survey_Weights.cpp - read the survey household weights
//*********************************************************

#include "CheckSurvey.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Survey_Weights
//---------------------------------------------------------

void CheckSurvey::Survey_Weights (void)
{
	int id, count;
	double weight;
	char *str_ptr;

	Household_Data *household_ptr;

	count = 0;

	//---- read the survey household weights ----

	Show_Message ("Reading %s -- Record", weight_file.File_Type ());
	Set_Progress (50000);
	
	while (weight_file.Read ()) {
		str_ptr = weight_file.Record ();

		//---- check the household id ----

		str_ptr = Get_Integer (str_ptr, &id);
		if (id <= 0) continue;

		Show_Progress ();

		household_ptr = survey_hh_data.Get (id);
		if (household_ptr == NULL) continue;

		Get_Double (str_ptr, &weight);

		household_ptr->Location ((int) (weight + random.Probability ()));
		count++;
	}
	End_Progress ();
	weight_file.Close ();

	Print (2, "Number of %s Records = %d", weight_file.File_Type (), Progress_Count ());

	if (count != Progress_Count ()) {
		Print (1, "Number of Household Weights Kept = %d", count);
	}
}
