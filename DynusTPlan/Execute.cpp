//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "DynusTPlan.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void DynusTPlan::Execute (void)
{
	int nfile;

	//---- read the network ----

	Demand_Service::Execute ();

	Set_Zone ();

	//---- write a temporary header record ----

	fprintf (veh_file.File (), "0000000000 1 # of vehicles, max # of stops, start_time=%s\n", time_range.Format_Step (start_time));
	fprintf (veh_file.File (), "      #   usec   dsec   stime usrcls vehtype ioc #ONode #IntDe info ribf   comp    OZ\n");

	//---- read the plan file ----

	for (nfile=0; ; nfile++) {
		if (!plan_file.Open (nfile)) break;

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

	Write (2, "Number of Output Trips = %d", num_out);

	//---- update the header ----

	veh_file.Rewind ();

	fprintf (veh_file.File (), "%10d", num_out);

	veh_file.Close ();
	path_file.Close ();

	//---- end the program ----

	Exit_Stat (DONE);
}
