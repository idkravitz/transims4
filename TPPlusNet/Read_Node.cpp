//*********************************************************
//	Read_Node.cpp - convert the node data
//*********************************************************

#include "TPPlusNet.hpp"

#include "Node_File.hpp"

//---------------------------------------------------------
//	Read_Node
//---------------------------------------------------------

void TPPlusNet::Read_Node (void)
{
	int nnode, node, node_field, x_field, y_field, z_field;
	double x, y, z;

	Db_Header *file;
	XYZ_Point *pt_ptr;
	Node_File *new_node;
	Node_Data node_rec;

	if (node_shape_flag) {
		file = &node_shape_file;

		x_field = y_field = z_field = 0;
	} else {
		file = &node_file;

		x_field = file->Required_Field ("X", "X_COORD", "EASTING");
		y_field = file->Required_Field ("Y", "Y_COORD", "NORTHING");
		z_field = file->Optional_Field ("Z", "Z_COORD", "ELEVATION");
	}
	//---- initialize the node array ----

	new_node = (Node_File *) Network_Db_Base (NEW_NODE);

	node_field = file->Required_Field ("N", "NODE", "ID");
	nnode = 0;

	//---- read each node record----

	Show_Message ("Reading %s -- Record", file->File_Type ());
	Set_Progress (1000);

	while (((node_shape_flag) ? node_shape_file.Read_Record () : node_file.Read_Record ())) {
		Show_Progress ();

		//---- read the node data ----

		file->Get_Field (node_field, &node);
		if (node == 0 || node < ext_zone) continue;

		if (node_shape_flag) {
			pt_ptr = node_shape_file.points.First ();

			x = pt_ptr->x;
			y = pt_ptr->y;
			z = pt_ptr->z;
		} else {
			file->Get_Field (x_field, &x);
			file->Get_Field (y_field, &y);
			file->Get_Field (z_field, &z);

			if (!projection.Convert (&x, &y)) {
				Error ("Converting Coordinate %lf, %lf", x, y);
			}
		}

		//---- save to index class -----

		node_rec.Node (node);
		node_rec.X (Round (x));
		node_rec.Y (Round (y));
		node_rec.Z (Round (z));

		if (!node_data.Add (&node_rec)) {
			Error ("Adding Node Data Record");
		}

		//---- save to the new node file ----

		new_node->Node (node);
		new_node->X (x);
		new_node->Y (y);
		new_node->Z (z);

		if (!new_node->Write ()) {
			Error ("Writing New Node Record");
		}
		nnode++;
	}
	End_Progress ();

	node_file.Close ();
	new_node->Close ();

	Print (2, "Number of %s Records = %d", node_file.File_Type (), Progress_Count ());
	Print (1, "Number of %s Records = %d", new_node->File_Type (), nnode);

}
