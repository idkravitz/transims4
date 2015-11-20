//*********************************************************
//	Delete_Node.cpp - remove extra nodes and links
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Delete_Node
//---------------------------------------------------------

bool TransimsNet::Delete_Node (Node_List *node_ptr)
{
	int dir, to_dir, from_dir;
	bool to_flip, from_flip;

	Node_List *bnode_ptr;
	Link_Nodes *to_link, *from_link;
	Dir_Connect *dir_ptr, *to_ptr, *from_ptr;

	to_dir = node_ptr->To_Dir ();
	to_ptr = dir_data [to_dir];
	to_link = link_data.Get (to_ptr->Link ());
	if (to_link->Anode () == to_link->Bnode ()) return (false);

	for (from_dir = node_ptr->From_Dir (); from_dir; from_dir = from_ptr->From_List ()) {
		from_ptr = dir_data [from_dir];

		if (from_ptr->Link () != to_ptr->Link ()) break;
	}
	if (from_dir == 0) return (false);

	from_link = link_data.Get (from_ptr->Link ());
	if (from_link->Anode () == from_link->Bnode ()) return (false);

	from_link->Type (0);

	//---- adjust the link orientation ----

	if (to_link->Bnode () == node_ptr->Node ()) {
		to_flip = false;
	} else {
		to_flip = true;
		to_link->Anode (to_link->Bnode ());
		to_link->Bnode (node_ptr->Node ());
		dir = to_link->AB_Dir ();
		to_link->AB_Dir (to_link->BA_Dir ());
		to_link->BA_Dir (dir);
		to_link->Flip_Nodes ();
	}

	if (from_link->Anode () == node_ptr->Node ()) {
		from_flip = false;
	} else {
		from_flip = true;
		from_link->Bnode (from_link->Anode ());
		from_link->Anode (node_ptr->Node ());
		dir = from_link->AB_Dir ();
		from_link->AB_Dir (from_link->BA_Dir ());
		from_link->BA_Dir (dir);
		from_link->Flip_Nodes ();
	}

	//---- update the link attributes ----

	to_link->Length (to_link->Length () + from_link->Length ());
	to_link->Add_Node (to_link->Bnode ());
	to_link->Bnode (from_link->Bnode ());

	for (int node = from_link->First_Node (); node; node = from_link->Next_Node ()) {
		to_link->Add_Node (node);
	}

	to_ptr->Out_Bearing (from_ptr->Out_Bearing ());

	//---- update the node lists ----

	bnode_ptr = node_data.Get (to_link->Bnode ());

	dir = bnode_ptr->To_Dir ();

	if (dir == from_dir) {
		bnode_ptr->To_Dir (to_dir);
		to_ptr->To_List (from_ptr->To_List ());
	} else {
		dir_ptr = dir_data [dir];

		for (dir = dir_ptr->To_List (); dir; dir = dir_ptr->To_List ()) {
			if (dir == from_dir) {
				dir_ptr->To_List (to_dir);
				to_ptr->To_List (from_ptr->To_List ());
			}
			dir_ptr = dir_data [dir];
		}
	}

	//---- update the opposition direction ----

	if (to_link->BA_Dir () > 0 && from_link->BA_Dir () > 0) {

		from_dir = from_link->BA_Dir ();
		from_ptr = dir_data [from_dir];

		to_dir = to_link->BA_Dir ();
		to_ptr = dir_data [to_dir];

		to_ptr->In_Bearing (from_ptr->In_Bearing ());

		dir = bnode_ptr->From_Dir ();

		if (dir == from_dir) {
			bnode_ptr->From_Dir (to_dir);
			to_ptr->From_List (from_ptr->From_List ());
		} else {
			dir_ptr = dir_data [dir];
			for (dir = dir_ptr->From_List (); dir; dir = dir_ptr->From_List ()) {
				if (dir == from_dir) {
					dir_ptr->From_List (to_dir);
					to_ptr->From_List (from_ptr->From_List ());
				}
				dir_ptr = dir_data [dir];
			}
		}
	}

	//---- reestablish positioning ----

	node_ptr = node_data.Get (node_ptr->Node ());

	//---- adjust the link shapes ----

	if (!shape_flag) return (true);

	Shape_Data *shape_ptr;

	if (to_link->Shape () > 0) {
		shape_ptr = shape_data [to_link->Shape ()];
		
		if (to_flip) {
			shape_ptr->Flip_Points ();
		}
		if (!shape_ptr->Point (node_ptr->X (), node_ptr->Y ())) goto point_error;
	} else {
		shape_ptr = shape_data.New_Record (true);
		if (shape_ptr == NULL) goto point_error;

		shape_ptr->Link (to_link->Link ());
		if (!shape_ptr->Max_Points (1)) goto point_error;

		if (!shape_ptr->Point (node_ptr->X (), node_ptr->Y ())) goto point_error;

		if (!shape_data.Add ()) goto point_error;

		to_link->Shape (shape_data.Num_Records ());
	}

	//---- add the shape of the deleted link ----

	if (from_link->Shape () > 0) {
		Shape_Data *from_ptr;
		XYZ *pt_ptr;

		from_ptr = shape_data [from_link->Shape ()];

		if (from_flip) {
			from_ptr->Flip_Points ();
		}
		if (!shape_ptr->Max_Points (shape_ptr->Points () + from_ptr->Points ())) goto point_error;

		for (pt_ptr = from_ptr->First_Point (); pt_ptr; pt_ptr = from_ptr->Next_Point ()) {
			if (!shape_ptr->Point (pt_ptr)) goto point_error;
		}
		from_ptr->Clear ();
		from_link->Shape (0);
	}
	return (true);

point_error:
	Error ("Insufficient Memory for Shape Points");
	return (false);
}
