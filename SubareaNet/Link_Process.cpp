//*********************************************************
//	Link_Process.cpp - read the link file
//*********************************************************

#include "SubareaNet.hpp"

#include "Link_File.hpp"

//---------------------------------------------------------
//	Link_Processing
//---------------------------------------------------------

bool SubareaNet::Link_Processing (Db_File *fh)
{
	static Link_File *new_file;

	if (First_Record ()) {
		new_file = (Link_File *) Network_Db_Base (NEW_LINK);
	}

	if (Network_Service::Link_Processing (fh)) {
		int type;
		Node_Data *anode_ptr, *bnode_ptr;

		Link_Data *link_ptr = link_data.New_Record ();
		type = 0;

		anode_ptr = node_data.Get (link_ptr->Anode ());
		if (anode_ptr->Type () == 1) type = 1;

		bnode_ptr = node_data.Get (link_ptr->Bnode ());
		if (bnode_ptr->Type () == 1) type += 2;

		if (type > 0) {
			Link_File *file = (Link_File *) fh;

			//---- copy the fields to the subarea file ----

			new_file->Copy_Fields (file);

			if (!new_file->Write ()) {
				Error ("Writing %s", new_file->File_Type ());
			}
			nlink++;

			//---- update the node flags ----

			if (type == 1) {
				bnode_ptr->Type (2);
				nboundary++;
			} else if (type == 2) {
				anode_ptr->Type (2);
				nboundary++;
			}
		}
		if (transit_flag || type > 0) {
			link_ptr->Type (type);

			return (true);
		}
	}
	return (false);
}
