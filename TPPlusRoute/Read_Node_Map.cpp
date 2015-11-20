//*********************************************************
//	Read_Node_Map.cpp - read the node map file
//*********************************************************

#include "TPPlusRoute.hpp"

//---------------------------------------------------------
//	Read_Node_Map
//---------------------------------------------------------

void TPPlusRoute::Read_Node_Map (void)
{
	int node_field, new_field, node, new_node;

	Node_Map map_rec;

	//---- read the node map file ----

	Show_Message ("Reading %s -- Record", node_map_file.File_Type ());
	Set_Progress (100);
		
	node_field = node_map_file.Required_Field ("NODE", "TPPLUS", "OLD", "OLD_NODE", "IN");
	new_field = node_map_file.Required_Field ("NEW_NODE", "TRANSIMS", "NEW", "NEWNODE", "OUT");

	//---- process each record ----

	while (node_map_file.Read ()) {
		Show_Progress ();

		node_map_file.Get_Field (node_field, &node);
		node_map_file.Get_Field (new_field, &new_node);

		if (node != 0) {
			map_rec.node = node;
			map_rec.new_node = new_node;

			if (!node_map.Add (&map_rec)) {
				Error ("Adding Node Map Record");
			}
		}
	}
	End_Progress ();

	node_map_file.Close ();

	Print (1, "Number of %s = %d", node_map_file.File_Type (), node_map.Num_Records ());
}
