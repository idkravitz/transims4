//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "CoordMatch.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void CoordMatch::Execute (void)
{
	//---- read the network ----

	Network_Service::Execute ();

	//---- read the reference node file ----

	if (ref_node_flag) {
		Read_Reference ();
	}

	//---- read the node map file ----

	if (node_map_flag) {
		Read_Node_Map ();
	} else {
		Make_Node_Map ();
	}

	//---- match other reference nodes ----

	if (new_map_flag || replace_flag) {
		Expand_Node_Map ();
	}

	//---- convert the nodes ----

	Adjust_Nodes ();

	//---- print the results ----

	Break_Check (4);
	Write (2, "Number of New Node Records = %d", num_new_nodes);
	Write (1, "Number of New Node Map Records = %d", num_new_maps);

	Exit_Stat (DONE);
}

