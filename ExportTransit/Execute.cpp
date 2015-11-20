//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "ExportTransit.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void ExportTransit::Execute (void)
{

	//---- read the network ----

	Network_Service::Execute ();

	//---- write the transit routes ----

	Write_Route ();

	//---- print the results ----

	Break_Check (3);
	Write (2, "Number of Route Header Records = %d", nroute);
	Write (1, "Number of Route Nodes Records = %d", nnodes);

	Exit_Stat (DONE);
}


