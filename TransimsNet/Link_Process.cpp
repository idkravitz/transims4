//*********************************************************
//	Link_Process.cpp - Read the Link File
//*********************************************************

#include "TransimsNet.hpp"

#include "Link_File.hpp"

//---------------------------------------------------------
//	Link_Processing
//---------------------------------------------------------

bool TransimsNet::Link_Processing (Db_File *fh)
{
	bool keep_flag;
		
	Link_Nodes *link_ptr = NULL;
	Link_File *file;

	if (Network_Service::Link_Processing (fh)) {
		file = (Link_File *) fh;	

		link_ptr = link_data.New_Record ();

		//---- remove internal zone connectors ----

		if (ext_zone > 0) {
			if (link_ptr->Anode () < ext_zone || link_ptr->Bnode () < ext_zone) {
				if (!connect_flag && !replicate_flag) return (false);
				link_ptr->Type (LOCAL);

				Dir_Data *dir_ptr = dir_data.New_Record ();

				if (dir_ptr->Thru () > 2) dir_ptr->Thru (2);

				if (dir_ptr->Dir () == 0) {
					dir_ptr = (Dir_Data *) (((char *) dir_ptr) + dir_data.Data_Size ());

					if (dir_ptr->Thru () > 2) dir_ptr->Thru (2);
				}
			}
		}

		//---- store street names ----

		if (name_flag) {
			link_ptr->Name (file->Street ());
		}
		if (file->Notes_Flag ()) {
			char buffer [FIELD_BUFFER];
			file->Get_Field (file->Notes_Field (), buffer);
			if (buffer [0] != '\0') {
				link_ptr->Notes (buffer);
			}
		}
		link_ptr->Set_Nodes ();

		if (!update_flag && !delete_flag) return (true);

		//---- check the update criteria ----

		keep_flag = true;

		if (update_flag && update_links.In_Range (link_ptr->Link ())) {
			keep_flag = false;
			if (delete_flag && (delete_nodes.In_Range (link_ptr->Anode ()) ||
				delete_nodes.In_Range (link_ptr->Bnode ()))) goto node_error;
		}
		if (delete_flag && delete_links.In_Range (link_ptr->Link ())) {
			keep_flag = false;
			xlink++;
		}
		if (keep_flag) {
			Link_File *new_file = (Link_File *) Network_Db_Base (NEW_LINK);

			new_file->Copy_Fields (file);

			if (!new_file->Write ()) {
				Error ("Writing %s", new_file->File_Type ());
			}
			if (delete_flag && (delete_nodes.In_Range (link_ptr->Anode ()) ||
				delete_nodes.In_Range (link_ptr->Bnode ()))) goto node_error;
		} else {

			//---- check for node updates ----
			
			if (!update_nodes.In_Range (link_ptr->Anode ()) && 
				!delete_nodes.In_Range (link_ptr->Anode ())) {
				if (!repair_nodes.In_Range (link_ptr->Anode ())) {
					repair_nodes.Add_Range (link_ptr->Anode (), link_ptr->Anode ());
				}
			}
			if (!update_nodes.In_Range (link_ptr->Bnode ()) &&
				!delete_nodes.In_Range (link_ptr->Bnode ())) {
				if (!repair_nodes.In_Range (link_ptr->Bnode ())) {
					repair_nodes.Add_Range (link_ptr->Bnode (), link_ptr->Bnode ());
				}
			}
		}
		return (true);
	}
	return (false);

node_error:
	Error ("A Node was Deleted for Link %d (%d-%d)", 
		link_ptr->Link (), link_ptr->Anode (), link_ptr->Bnode ());
	return (false);
}
