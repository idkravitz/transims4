//*********************************************************
//	Read_Node_Map.cpp - read the node map file
//*********************************************************

#include "CoordMatch.hpp"

//---------------------------------------------------------
//	Read_Node_Map
//---------------------------------------------------------

void CoordMatch::Read_Node_Map (void)
{
	int node_field, new_field, x_field, y_field, node, new_node;
	double x, y;

	Node_Map map_rec;
	Node_Data *node_ptr, *ref_ptr;

	//---- allocate space ----

	if (new_map_flag || replace_flag) {
		node = node_data.Num_Records ();
	} else {
		node = node_map_file.Num_Records ();
	}
	if (!node_map.Max_Records (node)) {
		Error ("Insufficient Memory for Node Map");
	}

	//---- read the node map file ----

	Show_Message ("Reading %s -- Record", node_map_file.File_Type ());
	Set_Progress (100);

	node_field = node_map_file.Required_Field ("NODE", "CURRENT", "IN", "IN_NODE", "OLD");

	if (ref_node_flag || replace_flag) {
		new_field = node_map_file.Required_Field ("NEW_NODE", "REFERENCE", "NEW", "OUT_NODE", "OUT");
	}
	if (!ref_node_flag) {
		x_field = node_map_file.Required_Field ("X_COORD", "EASTING", "X");
		y_field = node_map_file.Required_Field ("Y_COORD", "NORTHING", "Y");
	}

	//---- process each record ----

	while (node_map_file.Read ()) {
		Show_Progress ();

		node_map_file.Get_Field (node_field, &node);
		if (node == 0) continue;

		node_ptr = node_data.Get (node);
		if (node_ptr == NULL) {
			Warning ("Map Node %d was Not Found in the Node File", node);
			continue;
		}
		map_rec.node = node;
		map_rec.new_node = node;
		map_rec.x = node_ptr->X ();
		map_rec.y = node_ptr->Y ();

		if (ref_node_flag || replace_flag) {
			node_map_file.Get_Field (new_field, &new_node);
			map_rec.new_node = new_node;

			if (ref_node_flag) {
				ref_ptr = ref_node_data.Get (new_node);
				if (ref_ptr == NULL) {
					Warning ("Map Node %d was Not Found in the Reference Node File", new_node);
					continue;
				}
				map_rec.dx = ref_ptr->X () - map_rec.x;
				map_rec.dy = ref_ptr->Y () - map_rec.y;
				ref_ptr->Z (node);							//---- node map ----
			}
		}
		if (!ref_node_flag) {
			node_map_file.Get_Field (x_field, &x);
			node_map_file.Get_Field (y_field, &y);

			map_rec.dx = Round (x) - map_rec.x;
			map_rec.dy = Round (y) - map_rec.y;
		}
		if (!node_map.Add (&map_rec)) {
			Error ("Adding Node Map Record");
		}
	}
	End_Progress ();

	node_map_file.Close ();

	num_match = node_map.Num_Records ();

	Print (2, "Number of %s Records = %d", node_map_file.File_Type (), Progress_Count ());

	if (num_match != Progress_Count ()) {
		Print (1, "Number of Node Map Data Records = %d", num_match);
	}
}
