//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "SmoothPlans.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void SmoothPlans::Execute (void)
{
	int i, j, nlink, nfile;

	//---- read the network ----

	Demand_Service::Execute ();

	//---- read the link list ----

	if (link_flag) {
		Read_List ();
	}

	//---- read the plan file ----

	for (nfile=0; ; nfile++) {
		if (!plan_file.Open (nfile)) break;

		//---- process the plan files ----

		Read_Plans ();
	}
	if (nfile == 0) {
		File_Error ("No Plan Files were Found", plan_file.Filename ());
	}

	//---- write summary statistics ----

	Break_Check (8);

	Write (2, "Number of Plan Files = %d", nfile);
	Write (1, "Number of Input Plans = %d", plan_file.Num_Plans ());
	Write (1, "Number of Input Records = %d", plan_file.Num_Records ());
	Write (1, "Number of Input Travelers = %d", plan_file.Num_Travelers ());
	Write (1, "Number of Input Trips = %d", plan_file.Num_Trips ());

	Print (2, "Number of Plans using Count Links = %d", travel_data.Num_Records ());

	//---- smooth each link ----

	if (link_flag) {
		nlink = link_list.Num_Records ();
		Show_Message (1);
	} else {
		Show_Message (2, "Smooth Start Times -- Iteration");
		Set_Progress (1);
	}

	for (i=1; i <= niter; i++) {
		if (link_flag) {
			Show_Message (1, "Smooth Link Trips -- Iteration %d -- Link", i);
			Set_Progress (1);

			for (j=1; j <= nlink; j++) {
				Show_Progress ();
				Smooth_Link (i, j);
			}
		} else {
			Show_Progress ();
			Smooth_Link (i, 1);
		}
	}

	//---- write the summary file ----

	if (summary_flag) {
		Write_Results ();
	}

	//---- output the results ----

	if (output_flag) {

		if (change_flag) {
			period_index.Zero_Data ();
		}

		//---- copy plan records ----

		Show_Message (1);

		for (nfile=0; ; nfile++) {
			if (!plan_file.Open (nfile)) break;

			if (!new_plan_file.Open (nfile)) {
				Error ("Creating %s", new_plan_file.File_Type ());
			}
			Copy_Plans ();
		}

		//---- write summary statistics ----

		Break_Check (10);

		Write (2, "Number of New Plan Files = %d", nfile);
		Write (1, "Number of Output Plans = %d", new_plan_file.Num_Plans ());
		Write (1, "Number of Output Records = %d", new_plan_file.Num_Records ());
		Write (1, "Number of Output Travelers = %d", new_plan_file.Num_Travelers ());
		Write (1, "Number of Output Trips = %d", new_plan_file.Num_Trips ());

		if (new_plan_file.Num_Trips () > 0) {
			Write (2, "Number of Trips with New Times = %d (%.1lf)", new_time,
				(new_time * 100.0 / new_plan_file.Num_Trips ()));
		}

		//---- print the change report ----

		if (change_flag) {
			Change_Report ();
		}
	}
	Exit_Stat (DONE);
}
