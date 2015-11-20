//*********************************************************
//	Write_Network.cpp - convert the link and node data
//*********************************************************

#include "DynusTNet.hpp"

//---------------------------------------------------------
//	Write_Network
//---------------------------------------------------------

void DynusTNet::Write_Network (void)
{
	int nrec, nname, type, saturation, service;

	Node_List *node_ptr;
	Link_Name *link_ptr;
	Dir_List *dir_ptr;
	AB_Data *ab_ptr;

	FILE *file = net_file.File ();

	Show_Message ("Writing %s -- Record", net_file.File_Type ());
	Set_Progress ();

	//---- write the header line ----

	nname = 0;
	nrec = 1;
	fprintf (file, "%d\t%d\t%d\t2\t0\n", nzone, node_data.Num_Records (), dir_data.Num_Records ());

	//---- write the node records ----

	for (node_ptr = node_data.First_Key (); node_ptr; node_ptr = node_data.Next_Key ()) {
		Show_Progress ();
		fprintf (file, "%d\t%d\n", node_ptr->Node (), node_ptr->Type ());
		nrec++;
	}

	//---- write the link records ----

	for (ab_ptr = ab_key.First_Key (); ab_ptr; ab_ptr = ab_key.Next_Key ()) {
		Show_Progress ();

		dir_ptr = dir_data [ab_ptr->dir];
		link_ptr = link_data.Get (dir_ptr->Link ());

		type = link_ptr->Type ();
		saturation = sat_flow [type];
		service = ser_flow [type];

		if (type == FREEWAY) {
			type = 1;
		} else if (type == RAMP) {
			type = 3;
		} else if (type == EXPRESSWAY) {
			type = 7;
		} else {
			type = 5;
		}
		fprintf (file, "%7d%7d%5d%5d%7d%3d%7d%4d%4d%6d%6d%3d%4d\n", 
			ab_ptr->anode, ab_ptr->bnode, dir_ptr->Left (), dir_ptr->Right (),
			(int) (UnRound (link_ptr->Length ()) * METERTOFEET + 0.5),
			dir_ptr->Thru (), ((type == 5) ? 2 : 1), 1, 
			(int) (UnRound (dir_ptr->Speed ()) * MPSTOMPH + 0.5),
			saturation, service, type, 0);
		nrec++;

		if (name_flag && strlen (link_ptr->Name ()) > 0) {
			fprintf (name_file.File (), "%d\t%d\t%s\n", ab_ptr->anode, ab_ptr->bnode, link_ptr->Name ());
			nname++;
		}
	}
	End_Progress ();

	net_file.Close ();
	name_file.Close ();

	Print (2, "Number of %s Records = %d", net_file.File_Type(), nrec);
	
	Print (2, "Number of %s Records = %d", name_file.File_Type (), nname);
}
