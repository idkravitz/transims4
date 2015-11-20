//*********************************************************
//	Write_Node.cpp - Write the Node File
//*********************************************************

#include "TransimsNet.hpp"

#include "Node_File.hpp"
#include "Zone_File.hpp"

//---------------------------------------------------------
//	Write_Node
//---------------------------------------------------------

void TransimsNet::Write_Node (void)
{
	bool flag;
	Node_List *node_ptr;

	Zone_File *zone_file;

	Node_File *node_file = (Node_File *) Network_Db_Base (NEW_NODE);
	
	Show_Message ("Writing %s - Record", node_file->File_Type ());
	Set_Progress ();

	flag = (zout_flag && !zone_flag);

	if (flag) {
		zone_file = (Zone_File *) Network_Db_File (NEW_ZONE);
	}

	for (node_ptr = node_data.First_Key (); node_ptr; node_ptr = node_data.Next_Key ()) {
		Show_Progress ();

		//---- output the zone centroid ----

		if (flag && node_ptr->Node () <= high_zone) {
			zone_file->Zone (node_ptr->Node ());
			zone_file->X (UnRound (node_ptr->X ()));
			zone_file->Y (UnRound (node_ptr->Y ()));

			if (node_ptr->Node () < ext_zone) {
				zone_file->Area_Type (2);
				zone_file->Notes ("Zone Centroid");
			} else {
				zone_file->Area_Type (0);
				zone_file->Notes ("External Station");
			}
			if (!zone_file->Write ()) {
				Error ("Writing Zone Number %d", zone_file->Zone ());
			}
			nzout++;
		}

		if (delete_flag && delete_nodes.In_Range (node_ptr->Node ())) {
			node_ptr->Type (DELETE_NODE);
			xnode++;
		}
		if (node_ptr->Type () == DELETE_NODE) continue;

		//---- write the node record ----

		if (update_flag) {
			if (update_nodes.In_Range (node_ptr->Node ())) {
				nnode++;
			}
		} else {		
			nnode++;
		}
		node_file->Node (node_ptr->Node ());
		node_file->X (UnRound (node_ptr->X ()));
		node_file->Y (UnRound (node_ptr->Y ()));

		if (node_ptr->Node () <= high_zone) {
			if (node_ptr->Node () < ext_zone) {
				node_file->Notes ("Zone Centroid");
			} else {
				node_file->Notes ("External Station");
			}
		} else {
			node_file->Notes ("Network Node");
		}

		if (!node_file->Write ()) {
			Error ("Writing Node Number %d", node_ptr->Node ());
		}
	}
	End_Progress ();

	node_file->Close ();
}
