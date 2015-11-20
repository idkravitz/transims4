//*********************************************************
//	Read_Route_Nodes.cpp - read the route nodes file
//*********************************************************

#include "Network_Service.hpp"

#include "Route_Nodes_File.hpp"

//---------------------------------------------------------
//	Read_Route_Nodes
//---------------------------------------------------------

void Network_Service::Read_Route_Nodes (void)
{
	int route, node, nnode, last_route, num;
	bool node_flag;

	Route_Data *route_ptr = NULL;

	Route_Nodes_File *file = (Route_Nodes_File *) Network_Db_File (ROUTE_NODES);

	if (!Network_Data_Flag (ROUTE_HEADER)) {
		Error ("Route Header file is required for Route Nodes file processing");
	}
	node_flag = Network_Data_Flag (NODE);

	if (Check_Data () || Renumber ()) { 
		if (!Network_File_Flag (NODE) && Network_Option_Flag (NODE)) {
			Error (file_error, "Node", file->File_ID ());
		}
	}
	Show_Message ("Reading %s -- Record", file->File_Type ());

	//---- scan for number of nodes per route ----

	last_route = nnode = num = 0;

	while (file->Read ()) {
		route = file->Route ();
		if (route == 0) continue;
			
		//---- check the node data ----

		node = abs (file->Node ());

		if (Check_Data ()) {
			if (node_data.Get_Index (node) == 0) {
				Warning ("Node %d on Route %d was Not Found in the Node File", node, route);
				continue;
			}
		}
		if (route != last_route) {
			if (last_route > 0) {
				if (!route_ptr->Nodes (nnode)) goto mem_error;
			}
			last_route = route;
			nnode = 1;

			route_ptr = route_data.Get (route);

			if (route_ptr == NULL) {
				Check_Message ("Route %d was Not Found in the Route Header File", route);
				continue;
			}
		} else {
			nnode++;
		}
	}
	if (last_route > 0) {
		if (!route_ptr->Nodes (nnode)) goto mem_error;
	}
	file->Rewind ();

	//---- store the route nodes ----

	Set_Progress ();
	last_route = nnode = 0;

	while (file->Read ()) {
		Show_Progress ();

		route = file->Route ();
		if (route == 0) continue;

		//---- check the node ----

		node = file->Node ();

		if (Renumber ()) {
			if (node < 0) {
				node = -(node_data.Get_Index (-node));
			} else {
				node = node_data.Get_Index (node);
			}
			if (node == 0) continue;
		} else if (Check_Data ()) {
			if (node_data.Get_Index (abs (node)) == 0) continue;
		}

		//---- check for a new route ----

		if (route != last_route) {
			last_route = route;

			route_ptr = route_data.Get (route);
			nnode = 1;

		} else {
			nnode++;
		}
		route_ptr->Node (nnode, node);
		route_ptr->Time (nnode, file->Time ());
		route_ptr->Dwell (nnode, file->Dwell ());
		route_ptr->Speed (nnode, Round (file->Speed ()));
		num++;
	}
	End_Progress ();

	file->Close ();

	Print (2, "Number of %s Records = %d", file->File_Type (), Progress_Count ());

	route_data.Num_Nodes (num);

	if (num > 0) Network_Data_True (ROUTE_NODES);
	return;

mem_error:
	Error ("Insufficient Memory for Route Nodes Data");
}
