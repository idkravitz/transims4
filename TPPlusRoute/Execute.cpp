//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "TPPlusRoute.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void TPPlusRoute::Execute (void)
{
	int old_routes, old_nodes;

	//---- read the network ----

	Network_Service::Execute ();

	//---- check the input route numbers ----

	if (input_flag) {
		Route_Data *route_ptr = route_data.Last_Key ();

		if (route_ptr != NULL) {
			if (route_id <= route_ptr->Route ()) {
				route_id = ((route_ptr->Route () / 100) + 1) * 100;
			}
		}
		old_routes = route_data.Num_Records ();
		old_nodes = route_data.Num_Nodes ();
	} else {
		old_routes = old_nodes = 0;
	}

	//---- read the tpplus node map file ----

	if (map_flag) {
		Read_Node_Map ();
	}

	//---- read the tpplus line files ----

	Read_Line ();

	//---- write the route header and nodes ----

	Write_Route ();

	//---- print the results ----

	Break_Check (4);
	Write (2, "Number of Route Header Records = %d", nroute);
	if (old_routes > 0) Write (0, " (%d new)", (nroute - old_routes));

	Write (1, "Number of Route Nodes Records = %d", nnodes);
	if (old_nodes > 0) Write (0, " (%d new)", (nnodes - old_nodes));

	if (num_match > 0) Write (1, "Number of Matched Routes = %d", num_match);

	Exit_Stat (DONE);
}

