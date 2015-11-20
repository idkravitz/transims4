//*********************************************************
//	Adjust_Nodes.cpp - output node adjustments
//*********************************************************

#include "CoordMatch.hpp"

#include "Node_File.hpp"

#include <math.h>

//---------------------------------------------------------
//	Adjust_Nodes
//---------------------------------------------------------

void CoordMatch::Adjust_Nodes (void)
{
	int node, x, y, node_field, new_field;
	char buffer [FIELD_BUFFER];

	Node_Map *map_ptr;
	Node_Data *node_ptr;

	Node_File *new_node_file;
	
	if (new_node_flag) {
		new_node_file = (Node_File *) Network_Db_Base (NEW_NODE);
	}
	if (new_map_flag) {
		node_field = new_map_file.Required_Field ("NODE");
		new_field = new_map_file.Required_Field ("NEW_NODE");
	}

	//---- adjust the node data ----

	Show_Message ("Adjusting the Node Data -- Record");
	Set_Progress (100);

	//---- process the reference nodes ----

	for (node_ptr = node_data.First (); node_ptr; node_ptr = node_data.Next ()) {
		Show_Progress ();

		node = node_ptr->Node ();

		map_ptr = (Node_Map *) node_map.Get (&node);

		if (map_ptr != NULL) {
			if (new_map_flag) {
				new_map_file.Put_Field (node_field, node);
				new_map_file.Put_Field (new_field, map_ptr->new_node);

				if (!new_map_file.Write ()) {
					Error ("Writing %s", new_map_file.File_Type ());
				}
				num_new_maps++;
			}
			if (replace_flag) {
				node = map_ptr->new_node;
			}
			x = map_ptr->x + map_ptr->dx;
			y = map_ptr->y + map_ptr->dy;
		} else {
			x = node_ptr->X ();
			y = node_ptr->Y ();
			Adjust_Coord (&x, &y);
		}
		if (new_node_flag) {
			new_node_file->Node (node);

			new_node_file->X (UnRound (x));
			new_node_file->Y (UnRound (y));
			new_node_file->Z (0.0);
			
			if (node != node_ptr->Node ()) {
				str_fmt (buffer, sizeof (buffer), "Originally Node %d", node_ptr->Node ());
				new_node_file->Notes (buffer);
			} else {
				new_node_file->Notes ("Adjusted XY");
			}

			if (!new_node_file->Write ()) {
				Error ("Writing %s", new_node_file->File_Type ());
			}
			num_new_nodes++;
		}
	}
	End_Progress ();	


}
