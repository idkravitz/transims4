//*********************************************************
//	Read_Survey.cpp - read the survey files into memory
//*********************************************************

#include "ActivityPattern.hpp"

//---------------------------------------------------------
//	Read_Survey
//---------------------------------------------------------

void ActivityPattern::Read_Survey (void)
{
	//---- read the survey household weights ----

	if (weight_flag) {
		Survey_Weights ();
	}

	//---- read the survey household data ----

	Survey_Households ();

	//---- survey type summary report ----

	if (Report_Flag (SURVEY_TYPE)) {
		Header_Number (SURVEY_TYPE);

		if (!Break_Check (type_data.Num_Records () + 4)) {
			Print (1);
			Page_Header ();
		}
		Type_Data *type_ptr;

		for (type_ptr = type_data.First_Key (); type_ptr; type_ptr = type_data.Next_Key ()) {
			Print (1, "%4d  %8d      %15.6lf  %14.6lf", 
				type_ptr->Type (), type_ptr->Num_Survey (), type_ptr->Cum_Weight (),
				type_ptr->Cum_Weight () / ((type_ptr->Num_Survey () > 0) ? type_ptr->Num_Survey () : 1));
		}
		Header_Number (0);
	}

	//---- read the survey popoulation data ----

	Survey_Population ();

	//---- read the survey activity data ----

	Survey_Activity ();
}
