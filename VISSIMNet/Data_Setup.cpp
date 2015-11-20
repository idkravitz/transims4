//*********************************************************
//	Data_Setup.cpp - set the node and link data
//*********************************************************

#include "VISSIMNet.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Data_Setup
//---------------------------------------------------------

void VISSIMNet::Data_Setup (void)
{
	int dir;

	Node_List *anode_ptr, *bnode_ptr;
	Link_Name *link_ptr;
	Dir_List *dir_ptr;

	//---- initialize the node data ----

	for (anode_ptr = node_data.First (); anode_ptr; anode_ptr = node_data.Next ()) {
		anode_ptr->Type (0);
		anode_ptr->From_Dir (0);
		anode_ptr->To_Dir (0);
	}

	//---- build the link list ----

	for (link_ptr = link_data.First (); link_ptr; link_ptr = link_data.Next ()) {

		//---- add to the link lists ----

		anode_ptr = node_data.Get (link_ptr->Anode ());
		bnode_ptr = node_data.Get (link_ptr->Bnode ());

		dir = link_ptr->AB_Dir ();

		if (dir > 0) {
			dir_ptr = dir_data [dir];

			dir_ptr->From_List (anode_ptr->From_Dir ());
			anode_ptr->From_Dir (dir);

			dir_ptr->To_List (bnode_ptr->To_Dir ());
			bnode_ptr->To_Dir (dir);
		}
		dir = link_ptr->BA_Dir ();

		if (dir > 0) {
			dir_ptr = dir_data [dir];

			dir_ptr->To_List (anode_ptr->To_Dir ());
			anode_ptr->To_Dir (dir);

			dir_ptr->From_List (bnode_ptr->From_Dir ());
			bnode_ptr->From_Dir (dir);
		}
	}
}
