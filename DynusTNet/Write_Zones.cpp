//*********************************************************
//	Write_Zones.cpp - convert the origin/destination zones
//*********************************************************

#include "DynusTNet.hpp"

//---------------------------------------------------------
//	Write_Zones
//---------------------------------------------------------

void DynusTNet::Write_Zones (void)
{
	int zone, nnode, nout, dir, anode, bnode;

	Node_List *node_ptr;
	Link_Name *link_ptr;
	Dir_List *dir_ptr;

	//---- origin zones ----

	Show_Message ("Writing %s -- Record", org_file.File_Type ());
	Set_Progress ();

	nout = 0;

	for (zone=1; zone <= nzone; zone++) {
		Show_Progress ();
		nnode = 0;

		for (node_ptr = node_data.First (); node_ptr; node_ptr = node_data.Next ()) {
			if (node_ptr->Type () == zone) {

				for (dir = node_ptr->From_Dir (); dir > 0; dir = dir_ptr->From_List ()) {
					dir_ptr = dir_data [dir];
					nnode++;
				}
			}
		}
		fprintf (org_file.File (), "%5d%5d%5d\n", zone, nnode, 0);
		nout++;

		if (nnode > 0) {
			for (node_ptr = node_data.First_Key (); node_ptr; node_ptr = node_data.Next_Key ()) {
				if (node_ptr->Type () == zone) {

					for (dir = node_ptr->From_Dir (); dir > 0; dir = dir_ptr->From_List ()) {
						dir_ptr = dir_data [dir];

						link_ptr = link_data.Get (dir_ptr->Link ());

						if (dir_ptr->Dir () == 0) {
							anode = link_ptr->Anode ();
							bnode = link_ptr->Bnode ();
						} else {
							anode = link_ptr->Bnode ();
							bnode = link_ptr->Anode ();
						}
						fprintf (org_file.File (), "%7d%7d%7.3lf\n", anode, bnode, 0.0);
						nout++;
					}
				}
			}
		}
	}
	End_Progress ();

	org_file.Close ();

	Print (2, "Number of %s Records = %d", org_file.File_Type(), nout);

	//---- destination zones ----

	Show_Message ("Writing %s -- Record", des_file.File_Type ());
	Set_Progress ();

	nout = 0;

	for (zone=1; zone <= nzone; zone++) {
		Show_Progress ();
		nnode = 0;

		for (node_ptr = node_data.First (); node_ptr; node_ptr = node_data.Next ()) {
			if (node_ptr->Type () == zone) nnode++;
		}
		fprintf (des_file.File (), "%5d%5d", zone, nnode);

		if (nnode > 0) {
			for (node_ptr = node_data.First_Key (); node_ptr; node_ptr = node_data.Next_Key ()) {
				if (node_ptr->Type () == zone) {
					fprintf (des_file.File (), "%7d", node_ptr->Node ());
				}
			}
		}
		fprintf (des_file.File (), "\n");
		nout++;
	}
	End_Progress ();

	des_file.Close ();

	Print (2, "Number of %s Records = %d", des_file.File_Type(), nout);
}
