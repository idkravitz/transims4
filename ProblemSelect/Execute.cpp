//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "ProblemSelect.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void ProblemSelect::Execute (void)
{
	int nfile, num;

	//---- read the network ----

	Demand_Service::Execute ();

	//---- read the problem and hhold list file ----

	for (nfile=num=0; ; nfile++) {
		if (!problem_file.Open (nfile)) {
			if (hh_flag) {
				if (!hhold_file.Open (nfile)) break;
			} else {
				break;
			}
		} else {
			Read_Problem ();
			num = nfile + 1;

			if (hh_flag) {
				if (!hhold_file.Open (nfile)) {
					File_Error (hhold_file.File_Type (), hhold_file.Filename ());
				}
			}
		}
		if (!new_hhold_file.Open (nfile)) {
			File_Error (new_hhold_file.File_Type (), new_hhold_file.Filename ());
		}
		Read_Household ();
	}
	if (num == 0) {
		File_Error ("No Problem Files were Found", problem_file.Filename ());
	}

	//---- write summary statistics ----

	Break_Check (8);

	Write (2, "Number of Problem Files = %d", num);
	Write (1, "Number of Problem Records = %d", nproblem);
	Write (1, "Number of Problems Selected = %d", num_select);

	Write (2, "Number of Household Files = %d", nfile);
	Write (1, "Number of Input Households = %d", nhhold);
	Write (1, "Number of Output Households = %d", nselect);

	Exit_Stat (DONE);
}
