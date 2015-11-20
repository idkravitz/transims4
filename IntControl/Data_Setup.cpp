//*********************************************************
//	Data_Setup.cpp - set the node and link data
//*********************************************************

#include "IntControl.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Data_Setup
//---------------------------------------------------------

void IntControl::Data_Setup (void)
{
	int dir;

	Node_List *node_ptr, *anode_ptr, *bnode_ptr;
	Link_Data *link_ptr;
	Dir_Connect *dir_ptr;
	Sign_Data *sign_ptr;
	Signal_Data *signal_ptr;

	//---- initialize the node data ----

	for (node_ptr = node_data.First (); node_ptr; node_ptr = node_data.Next ()) {
		node_ptr->Type (0);
		node_ptr->From_Dir (0);
		node_ptr->To_Dir (0);

		//---- flag new/existing signals ----

		signal_ptr = signal_data.Get (node_ptr->Node (), 0);

		if (signal_ptr != NULL) {
			node_ptr->Type (1);
		}

		//---- flag new signals ----

		if (input_signal_flag && !update_flag) {
			signal_ptr = input_signal.Get_GE (node_ptr->Node (), 0);

			if (signal_ptr != NULL && signal_ptr->Node () == node_ptr->Node ()) {
				if (node_ptr->Type () == 1) {
					if (!delete_flag || delete_node.Get_Index (node_ptr->Node ()) == 0) {
						Warning ("A Signal at Node %d Already Exists", node_ptr->Node ());
					}
				} else {
					node_ptr->Type (1);
				}
			}
		}
	}

	Show_Message ("Setting the Node Connections - Record");
	Set_Progress ();

	//---- build the link list ----

	for (link_ptr = link_data.First (); link_ptr; link_ptr = link_data.Next ()) {
		Show_Progress ();

		//---- ignore walk links ----

		if (link_ptr->Type () == WALKWAY) continue;

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

			//---- check for sign-signal conflicts ----

			sign_ptr = sign_data.Get (dir_ptr->Link_Dir ());

			if (input_sign_flag) {
				if (sign_ptr == NULL) {
					sign_ptr = input_sign.Get (dir_ptr->Link_Dir ());
				} else {
					if (input_sign.Get (dir_ptr->Link_Dir ()) != NULL && method != UPDATE_SPLITS) {
						Warning ("A Sign on Link %d at Node %d Already Exists", 
							link_ptr->Link (), bnode_ptr->Node ());
					}
				}
			}
			if (sign_ptr != NULL && bnode_ptr->Type ()) {
				Warning ("Sign-Signal Conflict at Node %d Link %d", 
					bnode_ptr->Node (), dir_ptr->Link ());
			}
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

			//---- check for sign-signal conflicts ----

			sign_ptr = sign_data.Get (dir_ptr->Link_Dir ());

			if (input_sign_flag) {
				if (sign_ptr == NULL) {
					sign_ptr = input_sign.Get (dir_ptr->Link_Dir ());
				} else {
					if (input_sign.Get (dir_ptr->Link_Dir ()) != NULL && method != UPDATE_SPLITS) {
						Warning ("A Sign on Link %d at Node %d Already Exists", 
							link_ptr->Link (), anode_ptr->Node ());
					}
				}
			}
			if (sign_ptr != NULL && anode_ptr->Type ()) {
				Warning ("Sign-Signal Conflict at Node %d Link %d", 
					anode_ptr->Node (), dir_ptr->Link ());
			}
		}
	}
	End_Progress ();
}
