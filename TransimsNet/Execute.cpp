//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void TransimsNet::Execute (void)
{
	//---- read the update and delete files ----

	if (update_link_flag || update_node_flag || delete_link_flag || delete_node_flag) {
		Read_Files ();
	}

	//---- read the network ----

	Network_Service::Execute ();

	//---- read link detail data ----

	if (detail_flag) {
		Read_Details ();
	}

	//---- set the node and link data ----

	Data_Setup ();

	//---- write the transims node file ----

	Write_Node ();

	//---- build connection lists ----

	Connection_List ();

	//---- write the transims link data ----

	Write_Link ();

	//---- create intersection records ----

	Intersection_Data ();

	//---- write the turn restrictions ----

	if (turn_flag && Network_File_Flag (NEW_TURN_PROHIBITION)) {
		Write_Turn ();
	}

	//---- write summary statistics ----

	Break_Check (6);
	Write (2, "Number of Input Node Records = %d", node_data.Num_Records ());
	Write (1, "Number of Input Link Records = %d", link_data.Num_Records ());
	Write (1, "Number of Input Zone Records = %d", zone_data.Num_Records ());

	Write (2, "Highest Zone Number = %d", high_zone);

	if (update_flag || delete_flag) {
		Break_Check (10);
		Write (1);
		if (xlink) Write (1, "Number of Deleted Link Records = %d", xlink);
		if (xnode) Write (1, "Number of Deleted Node Records = %d", xnode);
		if (xactivity) Write (1, "Number of Deleted Activity Location Records = %d", xactivity);
		if (xparking) Write (1, "Number of Deleted Parking Lot Records = %d", xparking);
		if (xprocess) Write (1, "Number of Deleted Process Link Records = %d", xprocess);
		if (xpocket) Write (1, "Number of Deleted Pocket Lane Records = %d", xpocket);
		if (xconnect) Write (1, "Number of Deleted Lane Connectivity Records = %d", xconnect);
		if (xsign) Write (1, "Number of Deleted Unsignalized Node Records = %d", xsign);
		if (xsignal) Write (1, "Number of Deleted Signalized Node Records = %d", xsignal);	
	}

	Break_Check (15);
	Write (2, "Number of New Node Records = %d", nnode);
	if (zout_flag) {
		Write (1, "Number of New Zone Records = %d", nzout);
	}
	Write (1, "Number of New Link Records = %d", nlink);

	if (shape_flag) {
		Write (1, "Number of New Link Shapes = %d", nshapes);
		Write (1, "Number of New Shape_Records = %d", nshape);
	}
	Write (1, "Number of New Activity Location Records = %d", nactivity);
	Write (1, "Number of New Parking Lot Records = %d", nparking);
	Write (1, "Number of New Process Link Records = %d", nprocess);
	Write (1, "Number of New Pocket Lane Records = %d", npocket);
	Write (1, "Number of New Lane Connectivity Records = %d", nconnect);
	if (lane_use_flag) Write (1, "Number of New Lane Use Records = %d", nlane_use);
	if (nturn > 0) Write (1, "Number of New Turn Prohibition Records = %d", nturn);
	Write (1, "Number of New Unsignalized Node Records = %d", nsign);
	Write (1, "Number of New Signalized Node Records = %d", nsignal);

	if (nexternal) {
		Write (2, "Number of External Connections = %d", nexternal);
	}
	if (nshort) {
		Print (2, "Number of Short Links Increased in Length = %d", nshort);
	}
	if (nlength) {
		Print (2, "Number of Coordinate-Based Length Adjustments = %d", nlength);
	}
	if (nsign > 0) {
		Break_Check (2);
		Print (1);
		if (nstop > 0) {
			Print (1, "Number of Stop Signs = %d", nstop);
		}
		if (nyield > 0) {
			Print (1, "Number of Yield Signs = %d", nyield);
		}
	}
	if (nsignal > 0) {
		Break_Check (5);
		Print (1);
		if (nfixed1 > 0) {
			Print (1, "Number of Fixed Timed Single Ring Signals = %d", nfixed1);
		}
		if (nfixed2 > 0) {
			Print (1, "Number of Fixed Timed Dual Ring Signals = %d", nfixed2);
		}
		if (nfixed3 > 0) {
			Print (1, "Number of Fixed Timed Triple Ring Signals = %d", nfixed3);
		}
		if (nactuated1 > 0) {
			Print (1, "Number of Demand Actuated Single Ring Signals = %d", nactuated1);
		}
		if (nactuated2 > 0) {
			Print (1, "Number of Demand Actuated Dual Ring Signals = %d", nactuated2);
		}
		if (nactuated3 > 0) {
			Print (1, "Number of Demand Actuated Triple Ring Signals = %d", nactuated3);
		}
	}
	Exit_Stat (DONE);
}
