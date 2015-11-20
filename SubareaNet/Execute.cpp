//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "SubareaNet.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void SubareaNet::Execute (void)
{

	//---- read the network ----

	Network_Service::Execute ();

	//---- save the node file ----

	Write_Node ();

	//---- save the shape file ----

	if (Network_File_Flag (NEW_SHAPE)) {
		Write_Shape ();
	}

	//---- save the timing plan file ----

	if (Network_File_Flag (NEW_TIMING_PLAN)) {
		Write_Timing ();
	}

	//---- save the signal coordinator file ----

	if (Network_File_Flag (NEW_SIGNAL_COORDINATOR)) {
		Write_Coordinator ();
	}

	//---- write the boundary records ----

	Write_Boundary ();

	//---- save the transit files ----

	if (transit_flag) {
		Write_Route ();
	}

	//---- summary statistics ----

	Break_Check (8);
	Print (1);
	Write (1, "Number of Subarea Node Records = %d", nnode);
	Write (1, "Number of Subarea Link Records = %d", nlink);
	Write (1, "Number of Subarea Pocket Lane Records = %d", npocket);
	Write (1, "Number of Subarea Lane Connectivity Records = %d", nconnect);
	Write (1, "Number of Subarea Activity Locations = %d", nlocation);
	Write (1, "Number of Subarea Parking Lots = %d", nparking);
	Write (1, "Number of Subarea Process Links = %d", naccess);

	Break_Check (11);
	if (Network_File_Flag (NEW_ZONE)) {
		Write (1, "Number of Subarea Zone Records = %d", nzone);
	}
	if (Network_File_Flag (NEW_SHAPE)) {
		Write (1, "Number of Subarea Shape Records = %d", nshape);
	}
	if (Network_File_Flag (NEW_LANE_USE)) {
		Write (1, "Number of Subarea Lane Use Records = %d", nlane_use);
	}
	if (Network_File_Flag (NEW_TURN_PROHIBITION)) {
		Write (1, "Number of Subarea Turn Prohibition Records = %d", nturn);
	}
	if (Network_File_Flag (NEW_TOLL)) {
		Write (1, "Number of Subarea Toll Records = %d", ntoll);
	}
	if (Network_File_Flag (NEW_UNSIGNALIZED_NODE)) {
		Write (1, "Number of Subarea Unsignalized Node Records = %d", nsign);
	}
	if (Network_File_Flag (NEW_SIGNALIZED_NODE)) {
		Write (1, "Number of Subarea Signalized Node Records = %d", nsignal);
	}
	if (Network_File_Flag (NEW_TIMING_PLAN)) {
		Write (1, "Number of Subarea Timing Plan Records = %d", ntiming);
	}
	if (Network_File_Flag (NEW_PHASING_PLAN)) {
		Write (1, "Number of Subarea Phasing Plan Records = %d", nphasing);
	}
	if (Network_File_Flag (NEW_DETECTOR)) {
		Write (1, "Number of Subarea Detector Records = %d", ndetector);
	}
	if (Network_File_Flag (NEW_SIGNAL_COORDINATOR)) {
		Write (1, "Number of Subarea Signal Coordinator Records = %d", ncoord);
	}
	if (transit_flag) {
		Write (1, "Number of Subarea Transit Stop Records = %d", nstop);
		Write (1, "Number of Subarea Transit Route Records = %d", nroute);
		Write (1, "Number of Subarea Transit Schedule Records = %d", nschedule);
		Write (1, "Number of Subarea Transit Driver Records = %d", ndriver);
	}

	Break_Check (7);
	Write (2, "Number of Boundary Links = %d", nboundary);
	Write (1, "Number of Short Links Skipped = %d", nshort);
	Write (1, "Number of New Activity Locations = %d", new_location);
	Write (1, "Number of New Parking Lots = %d", new_parking);
	Write (1, "Number of New Process Links = %d", new_access);
	if (transit_flag) {
		Write (1, "Number of New Transit Stops = %d", new_stop);
		Write (1, "Number of New Transit Routes = %d", new_route);
	}
	Show_Message (1);

	Exit_Stat (DONE);
}
