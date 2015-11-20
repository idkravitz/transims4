//*********************************************************
//	Make_Node_Map.cpp - create a node map
//*********************************************************

#include "CoordMatch.hpp"

//---------------------------------------------------------
//	Make_Node_Map
//---------------------------------------------------------

void CoordMatch::Make_Node_Map (void)
{
	int num_null, node;

	Node_Data *node_ptr, *ref_ptr;
	Node_Map map_rec;

	//---- allocate space ----

	if (new_map_flag || replace_flag) {
		node = node_data.Num_Records ();
	} else {
		node = MIN (node_data.Num_Records (), ref_node_data.Num_Records ());
	}
	if (!node_map.Max_Records (node)) {
		Error ("Insufficient Memory for Node Map");
	}

	//---- match reference nodes to node file ----

	Show_Message ("Generating a Node Map -- Record");
	Set_Progress (100);

	//---- process the reference nodes ----

	num_null = 0;

	for (ref_ptr = ref_node_data.First (); ref_ptr; ref_ptr = ref_node_data.Next ()) {
		Show_Progress ();

		node = ref_ptr->Node ();

		node_ptr = node_data.Get (node);

		if (node_ptr == NULL) {
			if (num_null == 0) {
				Warning ("Reference Node %d was Not Found in the Node File", node);
			}
			num_null++;
			continue;
		}

		map_rec.node = node;
		map_rec.new_node = node;
		map_rec.x = node_ptr->X ();
		map_rec.y = node_ptr->Y ();
		map_rec.dx = ref_ptr->X () - node_ptr->X ();
		map_rec.dy = ref_ptr->Y () - node_ptr->Y ();

		ref_ptr->Z (node);						//---- node map ----

		if (!node_map.Add (&map_rec)) {
			Error ("Adding Node Map Record");
		}
	}
	End_Progress ();

	num_match = node_map.Num_Records ();

	Print (2, "Number of Reference Nodes Mapped = %d", num_match);

	if (num_null > 0) Print (1, "Number of Reference Nodes Skipped = %d", num_null);
}
