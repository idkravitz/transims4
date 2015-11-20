//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "ArcProblem.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void ArcProblem::Execute (void)
{

	//---- read the network ----

	Demand_Service::Execute ();

	Read_Problem ();

	Exit_Stat (DONE);
}

