//*********************************************************
//	Node_Path.cpp - build a path between two nodes
//*********************************************************

#include "TransitNet.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Node_Path
//---------------------------------------------------------

void TransitNet::Node_Path (int node1, int node2, Use_Type use)
{
	int anode, bnode, dir, best_cum, cum_a, cum_b, last_dir;

	Node_Path_Data *node_ptr, *first_ptr, *last_ptr, path_root;
	Dir_Path_Data *dir_ptr;
	Path_Node path_rec;

	node1 = node_data.Get_Index (node1);
	node2 = node_data.Get_Index (node2);

	path_node.Reset ();

	for (node_ptr = node_data.First (); node_ptr; node_ptr = node_data.Next ()) {
		node_ptr->Distance (0);
		node_ptr->Next_List (0);
		node_ptr->From_Node (0);
		node_ptr->Link_Dir (0);
	}
	best_cum = MAX_INTEGER;

	first_ptr = &path_root;
	first_ptr->Next_List (node1);
	last_ptr = node_data [node1];
	last_dir = 0;

	for (;;) {
		anode = first_ptr->Next_List ();
		if (anode <= 0) break;

		first_ptr->Next_List (-1);

		first_ptr = node_data [anode];

		//---- check the cumulative impedance ----

		cum_a = first_ptr->Distance ();

		if (cum_a >= best_cum) continue;

		//---- identify the approach link ----

		last_dir = 0;
		if (first_ptr->Link_Dir () > 0) {
			dir_ptr = dir_data [first_ptr->Link_Dir ()];

			if (dir_ptr != NULL) {
				last_dir = dir_ptr->Link_Dir ();
			}
		}

		//---- process each link leaving the node ----

		for (dir = first_ptr->From_Dir (); dir; dir = dir_ptr->From_List ()) {

			dir_ptr = dir_data [dir];
			if (!Use_Permission (dir_ptr->Use (), use)) continue;

			bnode = dir_ptr->Bnode ();
			if (bnode == node1) continue;

			//---- check the lane connectivity ----

			if (last_dir > 0 && connect_flag) {
				if (connect_data.Get (last_dir, dir_ptr->Link_Dir ()) == NULL) continue;
			}

			//---- check the cumulative distance ----

			cum_b = cum_a + dir_ptr->Length ();
			if (cum_b > best_cum) continue;

			node_ptr = node_data [bnode];
			if (cum_b >= node_ptr->Distance () && node_ptr->Distance () > 0) continue;

			if (bnode == node2) {
				best_cum = cum_b;
			}

			//---- add to the tree ----

			if (node_ptr->Next_List () == -1) {
				node_ptr->Next_List (first_ptr->Next_List ());
				first_ptr->Next_List (bnode);
			} else if (node_ptr->Next_List () == 0 && last_ptr != node_ptr) {
				last_ptr->Next_List (bnode);
				last_ptr = node_ptr;
			}
			node_ptr->Distance (cum_b);
			node_ptr->From_Node (anode);
			node_ptr->Link_Dir (dir);
		}
	}

	//---- trace the path ----

	anode = node2;
	while (anode != node1 && anode > 0) {
		node_ptr = node_data [anode];
		anode = node_ptr->From_Node ();
		if (anode == 0) break;

		path_rec.Node (node_ptr->Node ());
		path_rec.Dir (node_ptr->Link_Dir ());

		if (!path_node.Add (&path_rec)) {
			Error ("Adding Path Node");
		}
	}
}
