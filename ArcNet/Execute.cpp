//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "ArcNet.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void ArcNet::Execute (void)
{

	//---- read the network ----

	Network_Service::Execute ();

	arcview_node.Close ();
	arcview_zone.Close ();
	arcview_link.Close ();
	arcview_pocket.Close ();
	arcview_lane_use.Close ();
	arcview_location.Close ();
	arcview_parking.Close ();
	arcview_access.Close ();
	arcview_connect.Close ();
	arcview_turn.Close ();
	arcview_toll.Close ();
	arcview_sign.Close ();
	arcview_signal.Close ();
	arcview_phasing.Close ();
	arcview_detector.Close ();
	arcview_stop.Close ();

	if (guideway_flag) {
		guideway_file.Close ();
	}
	if (timing_flag) {
		Write_Timing ();
	}
	if (coordinator_flag) {
		Write_Coordinator ();
	}

	if (subzone_flag) {
		Write_Subzone ();
	}
	if (route_flag) {
		Write_Route ();
	} else if (driver_flag) {
		Write_Driver ();
	} else if (route_node_flag) {
		Route_Nodes ();
	}
	Exit_Stat (DONE);
}

