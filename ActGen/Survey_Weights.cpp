//*********************************************************
//	Survey_Weights.cpp - read the survey household weights
//*********************************************************

#include "ActGen.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Survey_Weights
//---------------------------------------------------------

void ActGen::Survey_Weights (void)
{
	int id;
	double weight;
	char *str_ptr;

	Weight_Data weight_rec;

	if (!weight_data.Max_Records (survey_hh_file.Num_Records ())) {
		Error ("Insufficient Memory for Survey Household Weights");
	}

	//---- read the survey household weights ----

	Show_Message ("Reading %s -- Record", weight_file.File_Type ());
	Set_Progress (50000);
	
	while (weight_file.Read ()) {
		str_ptr = weight_file.Record ();

		//---- check the household id ----

		str_ptr = Get_Integer (str_ptr, &id);

		if (id <= 0) continue;

		Show_Progress ();

		weight_rec.Household (id);

		Get_Double (str_ptr, &weight);

		weight_rec.Weight (weight);

		if (!weight_data.Add (&weight_rec)) {
			Error ("Adding Survey Household Weight");

		}
	}
	End_Progress ();
	weight_file.Close ();

	Print (2, "Number of %s Records = %d", weight_file.File_Type (), Progress_Count ());

	weight_data.Optimize ();
}
