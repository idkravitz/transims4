//*********************************************************
//	Read_Weights.cpp - read the group period weights
//*********************************************************

#include "Progression.hpp"

//---------------------------------------------------------
//	Read_Weights
//---------------------------------------------------------

void Progression::Read_Weights (void)
{
	int group;
	double weight;
	char *record, *str_ptr, buffer [FIELD_BUFFER];

	Group_Weight *wt_ptr;

	Show_Message ("Reading %s -- Record", weight_file.File_Type ());
	Set_Progress (1000);

	while (weight_file.Read ()) {
		Show_Progress ();

		record = weight_file.Record ();

		str_ptr = Get_Integer (record, &group);
		if (group == 0) continue;

		wt_ptr = new Group_Weight (group);

		while (str_ptr != NULL) {
			str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));
			if (buffer [0] == '\0') break;

			weight = atof (buffer);

			if (weight < 0.01 || weight > 1000.0) {
				Error ("Group %d Period Weight %.2lf is Out of Range (0.01..1000.0)", group, weight);
			}
			wt_ptr->Add_Weight (weight);
		}

		//---- check the data ----

		group = wt_ptr->Num_Periods ();
		if (group > 0) {
			if (group > max_period) max_period = group;

			if (!weight_data.Add (wt_ptr)) {
				Error ("Adding Group Weights");
			}
		}
	}
	End_Progress ();
	weight_file.Close ();

	Print (2, "Number of Group Period Weight Records = %d", Progress_Count ());

	if (Report_Flag (WEIGHT_REPORT)) {
		double *wt;

		//---- print the report ----

		Header_Number (WEIGHT_REPORT);

		if (!Break_Check (weight_data.Num_Records () + 6)) {
			Print (1);
			Weight_Header ();
		}

		for (wt_ptr = weight_data.First_Key (); wt_ptr; wt_ptr = weight_data.Next_Key ()) {
			Print (1, "%5d", wt_ptr->Group ());

			for (wt = wt_ptr->First_Weight (); wt; wt = wt_ptr->Next_Weight ()) {
				Print (0, "  %8.2lf", *wt);
			}
		}
		Header_Number (0);
	}
}

//---------------------------------------------------------
//	Weight_Header
//---------------------------------------------------------

void Progression::Weight_Header (void)
{
	int i;

	Print (1, "Group Period Weight Report");
	Print (2, "Group");

	for (i=1; i <= max_period; i++) {
		Print (0, "  Period%2d", i);
	}
	Print (1);
}
	 
/*********************************************|***********************************************

	Group Period Weight Report

	Group  Period 1  Period 2  Period 3  Period 4  Period 5  Period 6  Period 7  Period 8

	ddddd  fffff.ff  fffff.ff  fffff.ff  fffff.ff  fffff.ff  fffff.ff  fffff.ff  fffff.ff

**********************************************|***********************************************/ 
