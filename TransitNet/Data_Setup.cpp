//*********************************************************
//	Data_Setup.cpp - set the node and link data
//*********************************************************

#include "TransitNet.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Data_Setup
//---------------------------------------------------------

void TransitNet::Data_Setup (void)
{
	int i, j, dir, a_index, b_index, num, stop, stop_sort [40], stop_off [40];

	Node_Path_Data *node_ptr, *anode_ptr, *bnode_ptr;
	Dir_Path_Data *dir_ptr;
	Link_Data *link_ptr;
	Stop_Data *stop_ptr;

	//---- initialize the node data ----

	for (node_ptr = node_data.First (); node_ptr; node_ptr = node_data.Next ()) {
		node_ptr->From_Dir (0);
		node_ptr->To_Dir (0);
	}

	//---- process the stop data ----

	if (nstop > 0) {

		//---- flip the stop offsets ----

		for (stop_ptr = stop_data.First (); stop_ptr; stop_ptr = stop_data.Next ()) {
			if (stop_ptr->Dir () == 1) {
				link_ptr = link_data.Get (stop_ptr->Link ());
				stop_ptr->Offset (link_ptr->Length () - stop_ptr->Offset ());
			}
		}

		//---- sor the stop list ----

		for (dir_ptr = dir_data.First (); dir_ptr; dir_ptr = dir_data.Next ()) {
			i = 0;
			stop = dir_ptr->Stop_List ();

			while (stop && i < 40) {
				stop_ptr = stop_data [stop];
				stop_sort [i] = stop;
				stop_off [i++] = stop_ptr->Offset ();
				stop = stop_ptr->Traveler ();
			}
			if (i <= 1) continue;
			num = i - 1;

			for (i=0; i < num; i++) {
				for (j=i+1; j <= num; j++) {
					if (stop_off [i] > stop_off [j]) {
						stop = stop_sort [i];
						stop_sort [i] = stop_sort [j];
						stop_sort [j] = stop;

						stop = stop_off [i];
						stop_off [i] = stop_off [j];
						stop_off [j] = stop;
					}
				}
			}
			dir_ptr->Stop_List (stop_sort [0]);

			for (i=0; i <= num; i++) {
				stop = stop_sort [i];
				stop_ptr = stop_data [stop];
				
				if (i < num) {
					stop_ptr->Traveler (stop_sort [i+1]);
				} else {
					stop_ptr->Traveler (0);
				}
			}
		}
	}

	Show_Message ("Setting the Node Connections - Record");
	Set_Progress ();

	//---- build the link list ----

	for (link_ptr = link_data.First (); link_ptr; link_ptr = link_data.Next ()) {
		Show_Progress ();

		//---- add to the link lists ----

		anode_ptr = node_data.Get (link_ptr->Anode ());
		a_index = node_data.Record_Index ();

		bnode_ptr = node_data.Get (link_ptr->Bnode ());
		b_index = node_data.Record_Index ();

		dir = link_ptr->AB_Dir ();

		if (dir > 0) {
			dir_ptr = dir_data [dir];

			dir_ptr->Type (link_ptr->Type ());
			dir_ptr->Use (link_ptr->Use ());
			dir_ptr->Length (link_ptr->Length ());
			dir_ptr->Bnode (b_index);
			dir_ptr->Aoffset (link_ptr->Aoffset ());
			dir_ptr->Boffset (link_ptr->Boffset ());
			dir_ptr->Zone (0);

			dir_ptr->From_List (anode_ptr->From_Dir ());
			anode_ptr->From_Dir (dir);

			dir_ptr->To_List (bnode_ptr->To_Dir ());
			bnode_ptr->To_Dir (dir);
		}
		dir = link_ptr->BA_Dir ();

		if (dir > 0) {
			dir_ptr = dir_data [dir];

			dir_ptr->Type (link_ptr->Type ());
			dir_ptr->Use (link_ptr->Use ());
			dir_ptr->Length (link_ptr->Length ());
			dir_ptr->Bnode (a_index);
			dir_ptr->Aoffset (link_ptr->Boffset ());
			dir_ptr->Boffset (link_ptr->Aoffset ());
			dir_ptr->Zone (0);

			dir_ptr->To_List (anode_ptr->To_Dir ());
			anode_ptr->To_Dir (dir);

			dir_ptr->From_List (bnode_ptr->From_Dir ());
			bnode_ptr->From_Dir (dir);
		}
	}
	End_Progress ();
}
