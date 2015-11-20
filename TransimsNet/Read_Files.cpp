//*********************************************************
//	Read_Files.cpp - read the update and delete files
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Read_Files
//---------------------------------------------------------

void TransimsNet::Read_Files (void)
{

	//---- read the update link file ----

	if (update_link_flag) {
		Show_Message ("Reading %s - Record", update_link_file.File_Type ());
		Set_Progress ();

		while (update_link_file.Read ()) {
			Show_Progress ();

			if (!update_links.Add_Ranges (update_link_file.Record ())) {
				Error ("Adding Update Link Ranges");
			}
		}
		End_Progress ();
	}

	//---- read the update node file ----

	if (update_node_flag) {
		Show_Message ("Reading %s - Record", update_node_file.File_Type ());
		Set_Progress ();

		while (update_node_file.Read ()) {
			Show_Progress ();

			if (!update_nodes.Add_Ranges (update_node_file.Record ())) {
				Error ("Adding Update Node Ranges");
			}
		}
		End_Progress ();
	}

	//---- read the delete link file ----

	if (delete_link_flag) {
		Show_Message ("Reading %s - Record", delete_link_file.File_Type ());
		Set_Progress ();

		while (delete_link_file.Read ()) {
			Show_Progress ();

			if (!delete_links.Add_Ranges (delete_link_file.Record ())) {
				Error ("Adding Delete Link Ranges");
			}
		}
		End_Progress ();
	}

	//---- read the delete node file ----

	if (delete_node_flag) {
		Show_Message ("Reading %s - Record", delete_node_file.File_Type ());
		Set_Progress ();

		while (delete_node_file.Read ()) {
			Show_Progress ();

			if (!delete_nodes.Add_Ranges (delete_node_file.Record ())) {
				Error ("Adding Delee Node Ranges");
			}
		}
		End_Progress ();
	}
}
