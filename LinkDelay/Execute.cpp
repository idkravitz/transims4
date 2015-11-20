//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "LinkDelay.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void LinkDelay::Execute (void)
{
	//---- read the link file ----

	Demand_Service::Execute ();

	if (link_flag) {
		Read_Previous_Link ();
	}

	//---- combine previous records ----

	if (previous_flag) {
		Combine_Delay ();
	}
	Output_Delay ();

	Exit_Stat (DONE);
}
