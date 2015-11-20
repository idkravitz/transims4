//*********************************************************
//	Type_Report.cpp - survey household type report
//*********************************************************

#include "CheckSurvey.hpp"

//---------------------------------------------------------
//	Type_Report
//---------------------------------------------------------

void CheckSurvey::Type_Report (void)
{
	int i, num_types, *hholds, *weights, total_hh, total_wt;

	Household_Data *household_ptr;

	//---- gather data ----

	num_types = total_hh = total_wt = 0;

	for (household_ptr = survey_hh_data.First (); household_ptr; household_ptr = survey_hh_data.Next ()) {
		if (household_ptr->Type () > num_types) num_types = household_ptr->Type ();
	}
	if (num_types++ == 0) return;

	hholds = new int [num_types];
	weights = new int [num_types];

	memset (hholds, '\0', num_types * sizeof (int));
	memset (weights, '\0', num_types * sizeof (int));

	for (household_ptr = survey_hh_data.First (); household_ptr; household_ptr = survey_hh_data.Next ()) {
		i = household_ptr->Type ();

		hholds [i] += 1;
		weights [i] += household_ptr->Location ();
	}

	//---- print the report ----

	Header_Number (SURVEY_TYPE);

	if (!Break_Check (num_types + 6)) {
		Print (1);
		Page_Header ();
	}

	for (i=0; i < num_types; i++) {
		if (hholds [i] > 0) {
			Print (1, "%4d  %8d  %13d       %10.3lf", i, hholds [i], weights [i], (double) weights [i] / hholds [i]);
			total_hh += hholds [i];
			total_wt += weights [i];
		}
	}
	Print (2, "Total %8d  %13d       %10.3lf", total_hh, total_wt, (double) total_wt / total_hh);

	Header_Number (0);

	delete [] hholds;
	delete [] weights;
}
