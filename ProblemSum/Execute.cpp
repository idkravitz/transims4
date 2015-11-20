//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "ProblemSum.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void ProblemSum::Execute (void)
{
	int nfile;

	//---- read the network ----

	Demand_Service::Execute ();

	//---- read the problem file ----

	for (nfile=0; ; nfile++) {
		if (!problem_file.Open (nfile)) break;
			
		Read_Problem (nfile);
	}
	if (nfile == 0) {
		File_Error ("No Problem Files were Found", problem_file.Filename ());
	}
	if (nproblem == 0) {
		Warning ("No Problems were Selected");
		Exit_Stat (DONE);
	}
	Print (2, "Number of Problem Files Read = %d", nfile);
	Write (1, "Number of Problem Records Summarized = %d", nproblem);

	//---- write the problem time file ----

	if (problem_time_flag) {
		Problem_Time ();
	}

	//---- write the problem link file ----

	if (problem_link_flag) {
		Problem_Link ();
	}

	//---- print reports ----

	Show_Message (2, "Writing Summary Reports");

	for (int i=First_Report (); i != 0; i=Next_Report ()) {
		Print (1);

		switch (i) {
			case TYPE_REPORT:		//---- Problem Type Summary ----
				Type_Report ();
				break;
			case TIME_REPORT:		//---- Problem Time Summary ---
				Time_Report ();
				break;
			case DENSITY_REPORT:	//---- Problem Density Summary -----
				Density_Report (Report_Data ());
				break;
			default:
				break;
		}
	}
	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void ProblemSum::Page_Header (void)
{
	switch (Header_Number ()) {
		case TYPE_REPORT:		//---- Problem Type Summary ----
			Type_Header ();
			break;
		case TIME_REPORT:		//---- Problem Time Summary ---
			Time_Header ();
			break;
		case DENSITY_REPORT:	//---- Problem Density Summary -----
			Density_Header ();
			break;
		default:
			break;
	}
}
