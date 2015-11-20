//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "ArcTraveler.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void ArcTraveler::Execute (void)
{

	//---- read the network ----

	Demand_Service::Execute ();

	if (trip_flag) {
		Read_Trip ();
	} else if (activity_flag) {
		Read_Activity ();
	} else {
		Read_Traveler ();
	}
	Exit_Stat (DONE);
}

