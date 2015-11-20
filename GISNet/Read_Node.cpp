//*********************************************************
//	Read_Node.cpp - convert the node data
//*********************************************************

#include "GISNet.hpp"

//---------------------------------------------------------
//	Read_Node
//---------------------------------------------------------

void GISNet::Read_Node (void)
{
	int node, node_field, z_field;
	XYZ_Point *pt_ptr;

	Node_Data node_rec;

	//---- initialize the node array ----

	node_field = gis_node.Required_Field ("NODE", "ID", "NODE_ID", "NODEID", "N");
	z_field = gis_node.Optional_Field ("Z_COORD", "ELEVATION", "Z");

	//---- read each node record----

	Show_Message ("Reading %s -- Record", gis_node.File_Type ());
	Set_Progress (1000);

	while (gis_node.Read_Record ()) {
		Show_Progress ();

		//---- read the node data ----

		gis_node.Get_Field (node_field, &node);
		if (node == 0) continue;

		if (node >= first_node) first_node = node + 1;
		
		pt_ptr = gis_node.points.First ();

		//---- save to index class -----

		node_rec.Node (node);
		node_rec.X (Round (pt_ptr->x));
		node_rec.Y (Round (pt_ptr->y));
		node_rec.Z (Round (pt_ptr->z));

		if (!node_data.Add (&node_rec)) {
			Error ("Adding Node Data Record");
		}
	}
	End_Progress ();

	gis_node.Close ();

	Print (2, "Number of %s Records = %d", gis_node.File_Type (), Progress_Count ());
}
