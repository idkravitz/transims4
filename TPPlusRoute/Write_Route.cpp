//*********************************************************
//	Write_Route.cpp - write the route header and nodes
//*********************************************************

#include "TPPlusRoute.hpp"

//---------------------------------------------------------
//	Write_Route
//---------------------------------------------------------

void TPPlusRoute::Write_Route (void)
{
	int i, route, mode, headway, ttim, ttime, nodes, previous, current, node, offset;
	char buffer [FIELD_BUFFER];
	bool flag, previous_flag;

	Route_Data *route_ptr;
	Node_Data *node_ptr;
	AB_Data *ab_ptr;

	//---- read the route file ----

	Show_Message ("Writing Route Header and Nodes Files -- Record");
	Set_Progress (100);

	for (route_ptr = route_data.First_Key (); route_ptr; route_ptr = route_data.Next_Key ()) {
		Show_Progress ();

		route = route_ptr->Route ();
		mode = route_ptr->Mode ();
		nodes = route_ptr->Nodes ();

		str_cpy (buffer, sizeof (buffer), Transit_Code ((Transit_Type) mode));
		
		header_file->Route (route);
		header_file->Mode (buffer);
		header_file->Name ((char *) (route_ptr->Name ()));

		//---- calculate the travel time ----
		
		if (!time_flag) {
			ttime = 0;

			for (i=1; i <= num_periods; i++) {
				ttim = route_ptr->TTime (i);
				if (ttim > 0 && (ttim < ttime || ttime == 0)) {
					ttime = ttim;
				}
			}
			header_file->Min_Time ((ttime + 50) / 60);
		}

		//---- write period headways and travel times ----

		for (i=1; i <= num_periods; i++) {
			headway = (route_ptr->Headway (i) + 30) / 60;
			ttime = (route_ptr->TTime (i) + 50) / 60;
			offset = route_ptr->Offset (i);

			if (offset > 0) {
				offset = (offset + 30) / 60;
			}
			header_file->Headway (i, headway);
			header_file->Offset (i, offset);
			header_file->TTime (i, ttime);
		}
		header_file->Notes ((char *) route_ptr->Notes ());

		if (!header_file->Write ()) {
			Error ("Writing Router Header File");
		}
		nroute++;

		//---- write the node data ----

		nodes_file->Route (route);

		previous = 0;
		previous_flag = true;

		for (i=1; i <= nodes; i++) {
			node = route_ptr->Node (i);
			if (node == 0) continue;

			if (node_flag || link_flag) {
				current = node;

				if (current < 0) {
					current = -current;
				}

				//---- check the node number ----

				flag = true;

				if (node_flag) {
					node_ptr = node_data.Get (current);

					if (node_ptr == NULL) {
						Warning ("Route %d Node %d was Not Found", route, current);
						flag = false;
						continue;
					}
				}

				//---- check for a link ----

				if (link_flag) {
					if (previous > 0 && previous_flag && flag) {
						ab_ptr = ab_key.Get (previous, current);

						if (ab_ptr == NULL) {
							Warning ("Route %d Link %d-%d was Not Found", route, previous, current);
							continue;
						}
					}
					previous = current;
					previous_flag = flag;
				}
			}
			nodes_file->Node (node);

			if (!nodes_file->Write ()) {
				Error ("Writing Route Nodes File");
			}
			nnodes++;
		}
	}
	End_Progress ();

	header_file->Close ();
	nodes_file->Close ();
}
