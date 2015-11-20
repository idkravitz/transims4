//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "VehGen.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void VehGen::Execute (void)
{
	//---- read the files ----

	Demand_Service::Execute ();

	//---- end the program ----

	Write (2, "Number of Households = %d", num_hhold);
	Write (1, "Number of Vehicles = %d", num_vehicle);

	if (num_problem) {
		Write (2, "Number of Vehicles that could not be Located = %d", num_problem);
	}
	Exit_Stat (DONE);
}
