//*********************************************************
//	Write_Connect.cpp - convert the lane connectivity data
//*********************************************************

#include "DynusTNet.hpp"

//---------------------------------------------------------
//	Write_Connections
//---------------------------------------------------------

void DynusTNet::Write_Connections (void)
{
	int nrec, node, left, thru, right, other1, other2, uturn;

	Link_Name *link_ptr;
	Dir_List *dir_ptr;
	AB_Data *ab_ptr;
	Connect_Data *connect_ptr;

	FILE *file = move_file.File ();

	Show_Message ("Writing %s -- Record", move_file.File_Type ());
	Set_Progress ();

	nrec = 0;

	//---- write the link records ----

	for (ab_ptr = ab_key.First_Key (); ab_ptr; ab_ptr = ab_key.Next_Key ()) {
		Show_Progress ();

		dir_ptr = dir_data [ab_ptr->dir];

		connect_ptr = connect_data.Get_GE (dir_ptr->Link_Dir (), 0);

		left = thru = right = other1 = other2 = uturn = 0;

		for (; connect_ptr; connect_ptr = connect_data.Next_Key ()) {
			if (connect_ptr->In_Link_Dir () != dir_ptr->Link_Dir ()) break;

			link_ptr = link_data.Get (connect_ptr->Out_Link ());

			if (connect_ptr->Out_Dir () == 0) {
				node = link_ptr->Bnode ();
			} else {
				node = link_ptr->Anode ();
			}
			if (left == 0 && connect_ptr->Type () == LEFT) {
				left = node;
			} else if (right == 0 && connect_ptr->Type () == RIGHT) {
				right = node;
			} else if (thru == 0 && connect_ptr->Type () == THRU) {
				thru = node;
			} else if (uturn == 0 && connect_ptr->Type () == UTURN) {
				uturn = 1;
			} else if (other1 == 0) {
				other1 = node;
			} else if (other2 == 0) {
				other2 = node;
			}
		}
		fprintf (file, "%7d%7d%7d%7d%7d%7d%7d%7d\n", 
			ab_ptr->anode, ab_ptr->bnode, left, thru, right, other1, other2, uturn);
		nrec++;
	}
	End_Progress ();

	move_file.Close ();

	Print (2, "Number of %s Records = %d", move_file.File_Type(), nrec);
}
