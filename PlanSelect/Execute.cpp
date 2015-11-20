//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "PlanSelect.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void PlanSelect::Execute (void)
{
	int nfile;

	//---- read the network ----

	Demand_Service::Execute ();

	//---- read the population file ----

	if (pop_flag) {
		Read_Population ();
	}

	//---- read the plan file ----

	for (nfile=0; ; nfile++) {
		if (!plan_file.Open (nfile)) break;

		//---- open the hhold list file ----

		if (nfile > 0) {
			if (hhrec_flag) {
				if (hhrec_file.Extend () > 0) {
					if (!hhrec_file.Open (nfile)) {
						Error ("Opening %s", hhrec_file.File_Type ());
					}
				}
				hhrec_data.Reset ();
			} else {
				if (hhlist_file.Extend () > 0) {
					if (!hhlist_file.Open (nfile)) {
						Error ("Opening %s", hhlist_file.File_Type ());
					}
				}
				hhold_list.Reset ();
			}
		}

		//---- process the plan files ----

		Read_Plan ();
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

	if (hhrec_flag) {
		hhlist_file.Close ();
		Write (2, "Number of Household Records Written = %d", num_hhold);
	} else {
		hhrec_file.Close ();
		Write (2, "Number of Household IDs Written = %d", num_hhold);
	}

	//---- subarea IO report ----

	if (subarea_flag && Report_Flag (SUBAREA_IO)) {
		Break_Check (9);
		Write (2, "Subarea Internal-External Trips");

		int total = num_ii + num_ie + num_ei + num_ee;

		if (total > 0) {
			Write (2, "Internal-Internal Trips = %d (%.2lf)", num_ii, (100.0 * num_ii / total));
			Write (1, "Internal-External Trips = %d (%.2lf)", num_ie, (100.0 * num_ie / total));
			Write (1, "External-Internal Trips = %d (%.2lf)", num_ei, (100.0 * num_ei / total));
			Write (1, "External-External Trips = %d (%.2lf)", num_ee, (100.0 * num_ee / total));
		}
		Write (2, "Total Subarea Trips = %d", total);
	}

	Exit_Stat (DONE);
}
