//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "EnhanceNet.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void EnhanceNet::Execute (void)
{
	//---- read the network files ----

	Network_Service::Execute ();

	//---- prepare the data ----

	if (connect_flag) {
		Data_Setup ();

		Connection_List ();

		Write_Connect ();
	}

	//---- processing results ----
 
	//Break_Check (8);

	//Write (2, "Number of Unsignalized Node Records = %d", nsign);
	//Write (1, "Number of Signalized Node Records = %d", nsignal);
	//Write (1, "Number of Timing Plan Records = %d", ntiming - base_timing);
	//Write (1, "Number of Phasing Plan Records = %d", nphasing);
	//Write (1, "Number of Detector Records = %d", ndetector - base_detector);
	//Write (1, "Number of Signal Coordinators = %d", ncoord - base_coord);
	//Write (1);

	//---- end the program ----

	Exit_Stat (DONE);
}
