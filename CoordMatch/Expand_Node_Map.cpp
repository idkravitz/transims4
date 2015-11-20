//*********************************************************
//	Expand_Node_Map.cpp - add reference nodes to the map
//*********************************************************

#include "CoordMatch.hpp"

#include <math.h>

//---------------------------------------------------------
//	Expand_Node_Map
//---------------------------------------------------------

void CoordMatch::Expand_Node_Map (void)
{
	int node, x, y, num_new;
	double distance, min_distance, dx, dy;

	Node_Data *node_ptr, *ref_ptr, *best_ptr;
	Node_Map map_rec, *map_ptr;

	//---- match reference nodes to node coordinates ----

	Show_Message ("Expanding the Node Map -- Record");
	Set_Progress (100);

	num_new = 0;

	//---- process the node data ----

	for (node_ptr = node_data.First (); node_ptr; node_ptr = node_data.Next ()) {
		Show_Progress ();

		node = node_ptr->Node ();

		map_ptr = (Node_Map *) node_map.Get (&node);

		if (map_ptr != NULL) continue;

		map_rec.node = node;
		map_rec.x = x = node_ptr->X ();
		map_rec.y = y = node_ptr->Y ();

		//---- adjust the node coordinates ----

		Adjust_Coord (&x, &y);

		//---- search for a reference node at the new coordinates ----

		best_ptr = NULL;
		min_distance = max_distance + 0.01;

		for (ref_ptr = ref_node_data.First (); ref_ptr; ref_ptr = ref_node_data.Next ()) {

			if (ref_ptr->Z () != 0) continue;		//---- previously mapped node ----

			dx = (double) ref_ptr->X () - x;
			dy = (double) ref_ptr->Y () - y;

			distance = sqrt (dx * dx + dy * dy);

			if (distance < min_distance) {
				best_ptr = ref_ptr;
				if (distance == 0.0) break;
				min_distance = distance;
			}
		}

		//---- assign the best reference node ----

		if (best_ptr != NULL) {
			map_rec.new_node = best_ptr->Node ();

			map_rec.dx = best_ptr->X () - map_rec.x;
			map_rec.dy = best_ptr->Y () - map_rec.y;

			best_ptr->Z (node);		//---- node map ----

			if (!node_map.Add (&map_rec)) {
				Error ("Adding Node Map Record");
			}
			num_new++;
		}
	}
	End_Progress ();

	Print (2, "Number of Newly Mapped Nodes = %d", num_new);
}
