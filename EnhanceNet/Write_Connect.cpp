//*********************************************************
//	Write_Connect.cpp - write the lane connectivity data
//*********************************************************

#include "EnhanceNet.hpp"

#include "Connect_File.hpp"

//---------------------------------------------------------
//	Write_Connect
//---------------------------------------------------------

void EnhanceNet::Write_Connect (void)
{
	int node, link;

	Link_Data *link_ptr;
	Connect_File *new_file;
	Connect_Data *connect_ptr, connect_rec;

	new_file = (Connect_File *) Network_Db_File (NEW_LANE_CONNECTIVITY);

	Show_Message ("Writing %s -- Record", new_file->File_Type ());
	Set_Progress (1000);

	connect_db.Rewind ();

	while (connect_db.Read_Record ()) {
		Show_Progress ();

		new_file->Copy_Fields (&connect_db);

		node = new_file->Node ();

		//---- input link dir ----

		link = new_file->In_Link ();

		connect_rec.In_Link (link);	

		link_ptr = link_data.Get (link);

		if (link_ptr->Anode () == node) {
			connect_rec.In_Dir (1);
		} else {
			connect_rec.In_Dir (0);
		}

		//---- output link dir ----

		link = new_file->Out_Link ();

		connect_rec.Out_Link (link);	

		link_ptr = link_data.Get (link);
	
		if (link_ptr->Bnode () == node) {
			connect_rec.Out_Dir (1);
		} else {
			connect_rec.Out_Dir (0);
		}

		//---- retrieve the connection record ----

		connect_ptr = connect_data.Get (&connect_rec);

		if (connect_ptr == NULL) {
			Error ("Retrieving Connection Data %d-%d %d-%d", 
				connect_rec.In_Link (), connect_rec.In_Dir (), 
				connect_rec.Out_Link (), connect_rec.Out_Dir ());
		}

		//---- update the connection type ----
		
		new_file->Type (Connect_Code ((Connect_Type) connect_ptr->Type ()));

		if (!new_file->Write ()) {
			Error ("Writing %s", new_file->File_Type ());
		}
	}
	End_Progress ();

	new_file->Close ();
}
