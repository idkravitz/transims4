//*********************************************************
//	Data_Setup.cpp - set the node and link data
//*********************************************************

#include "EnhanceNet.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Data_Setup
//---------------------------------------------------------

void EnhanceNet::Data_Setup (void)
{
	int dir;

	Node_List *node_ptr, *anode_ptr, *bnode_ptr;
	Link_Data *link_ptr;
	Dir_Connect *dir_ptr;

	//---- initialize the node data ----

	for (node_ptr = node_data.First (); node_ptr; node_ptr = node_data.Next ()) {
		node_ptr->Type (0);
		node_ptr->From_Dir (0);
		node_ptr->To_Dir (0);
	}

	Show_Message ("Setting the Node Connections - Record");
	Set_Progress ();

	//---- build the link list ----

	for (link_ptr = link_data.First (); link_ptr; link_ptr = link_data.Next ()) {
		Show_Progress ();

		//---- add to the link lists ----

		anode_ptr = node_data.Get (link_ptr->Anode ());
		bnode_ptr = node_data.Get (link_ptr->Bnode ());

		dir = link_ptr->AB_Dir ();

		if (dir > 0) {
			dir_ptr = dir_data [dir];

			dir_ptr->Set_Dir ();

			dir_ptr->Type (link_ptr->Type ());
			dir_ptr->Length (link_ptr->Length ());
			dir_ptr->Setback (link_ptr->Boffset ());

			dir_ptr->From_List (anode_ptr->From_Dir ());
			anode_ptr->From_Dir (dir);

			dir_ptr->To_List (bnode_ptr->To_Dir ());
			bnode_ptr->To_Dir (dir);
		}
		dir = link_ptr->BA_Dir ();

		if (dir > 0) {
			dir_ptr = dir_data [dir];

			dir_ptr->Set_Dir ();

			dir_ptr->Type (link_ptr->Type ());
			dir_ptr->Length (link_ptr->Length ());
			dir_ptr->Setback (link_ptr->Aoffset ());

			dir_ptr->To_List (anode_ptr->To_Dir ());
			anode_ptr->To_Dir (dir);

			dir_ptr->From_List (bnode_ptr->From_Dir ());
			bnode_ptr->From_Dir (dir);
		}
	}
	End_Progress ();
}
