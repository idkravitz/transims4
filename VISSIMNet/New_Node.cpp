//*********************************************************
//	New_Node.cpp - write the new node records
//*********************************************************

#include "VISSIMNet.hpp"

#include "Node_File.hpp"
#include "Utility.hpp"

//---------------------------------------------------------
//	New_Node
//---------------------------------------------------------

void VISSIMNet::New_Node (void)
{
	int i, node;
	double factor;

	XYZ_Point *pt_ptr;

	Link_Splits *split_ptr;
	Link_Name *link_ptr;
	Node_List *node_ptr, node_rec;
	Node_File *node_file;

	//---- copy and add nodes ----

	for (node_ptr = node_data.First_Key (); node_ptr; node_ptr = node_data.Next_Key ()) {
		new_node_data.Add (node_ptr);
	}
	node_ptr = (Node_List *) node_data.Last_Key ();
	node = ((node_ptr->Node () + 1) / 100 + 1) * 100;

	memset (&node_rec, '\0', sizeof (node_rec));

	for (split_ptr = (Link_Splits *) link_splits.First_Key (); split_ptr; split_ptr = (Link_Splits *) link_splits.Next_Key ()) {
		if (split_ptr->num_splits == 0) continue;

		link_ptr = link_data [split_ptr->record];
		
		for (i=0; i < split_ptr->num_splits; i++) {
			split_ptr->node [i] = node;

			node_rec.Node (node++);

			factor = UnRound (split_ptr->offset [i]);

			Link_Shape (link_ptr, 0, &points, factor, 0.0, 0.0);

			pt_ptr = points [1];

			node_rec.X (Round (pt_ptr->x));
			node_rec.Y (Round (pt_ptr->y));

			if (!new_node_data.Add (&node_rec)) {
				Error ("Adding Node Record");
			}

			//---- parking node in ab direction ----

			if (split_ptr->park_ab [i] < 0) {
				split_ptr->park_ab [i] = node;

				node_rec.Node (node++);

				Link_Shape (link_ptr, 0, &points, factor, 0.0, PARKING_OFFSET);

				pt_ptr = points [1];

				node_rec.X (Round (pt_ptr->x));
				node_rec.Y (Round (pt_ptr->y));

				if (!new_node_data.Add (&node_rec)) {
					Error ("Adding Node Record");
				}
			}

			//---- parking node in ba direction ----

			if (split_ptr->park_ba [i] < 0) {
				split_ptr->park_ba [i] = node;

				node_rec.Node (node++);

				Link_Shape (link_ptr, 1, &points, factor, 0.0, PARKING_OFFSET);

				pt_ptr = points [1];

				node_rec.X (Round (pt_ptr->x));
				node_rec.Y (Round (pt_ptr->y));

				if (!new_node_data.Add (&node_rec)) {
					Error ("Adding Node Record");
				}
			}
		}
	}

	//---- write the new node file ----

	if (!Network_File_Flag (NEW_NODE)) return;

	node_file = (Node_File *) Network_Db_Base (NEW_NODE);

	for (node_ptr = new_node_data.First (); node_ptr; node_ptr = new_node_data.Next ()) {
		node_file->Node (node_ptr->Node ());
		node_file->X (UnRound (node_ptr->X ()));
		node_file->Y (UnRound (node_ptr->Y ()));
		node_file->Z (0.0);

		if (!node_file->Write ()) {
			Error ("Writing %s", node_file->File_Type ());
		}
	}
	node_file->Close ();

	Print (2, "Number of New Node Records = %d", new_node_data.Num_Records ());
}
