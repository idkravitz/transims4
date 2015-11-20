//*********************************************************
//	Read_Turn.cpp - Read the Turn Movement Files
//*********************************************************

#include "Validate.hpp"

//---------------------------------------------------------
//	Read_Turn
//---------------------------------------------------------

void Validate::Read_Turn (Movement_File *file, bool type)
{
	int i, link, node, dir, num, start, end;

	Connect_Time *connect_ptr, connect_rec;
	Link_Data *link_ptr;
    
	//---- read the volume file ----

	Show_Message ("Reading %s -- Record", file->File_Type ());
	Set_Progress ();

	num = time_periods.Num_Ranges ();

	//---- initialize the counts ----

	if (!type) {
		for (connect_ptr = connect_time.First (); connect_ptr; connect_ptr = connect_time.Next ()) {
			if (!connect_ptr->Periods (num)) {
				Error ("Insufficient Memory for Turn Time Periods");
			}
			for (i=1; i <= num; i++) {
				connect_ptr->Volume (i, 0);
				connect_ptr->Count (i, 0);
			}
		}
	}

	//---- read the count data ----

	while (file->Read ()) {
		Show_Progress ();

		node = file->Node ();
		if (node == 0) continue;

		//---- identify the in link ----

		link = file->In_Link ();
		link_ptr = link_data.Get (link);

		if (link_ptr == NULL) {
			Error ("Turn Movement Link %d was Not Found in the Link File", link);
		}

		if (link_ptr->Anode () == node) {
			dir = 1;
		} else if (link_ptr->Bnode () == node) {
			dir = 0;
		} else {
			Error ("Turn Movement Link %d is Not Connected to Node %d", link, node);
		}
		connect_rec.In_Link (link);
		connect_rec.In_Dir (dir);

		//---- identify the out link ----

		link = file->Out_Link ();
		link_ptr = link_data.Get (link);

		if (link_ptr == NULL) {
			Error ("Turn Movement Link %d was Not Found in the Link File", link);
		}

		if (link_ptr->Anode () == node) {
			dir = 0;
		} else if (link_ptr->Bnode () == node) {
			dir = 1;
		} else {
			Error ("Turn Movement Link %d is Not Connected to Node %d", link, node);
		}
		connect_rec.Out_Link (link);
		connect_rec.Out_Dir (dir);

		//---- get the connection record ----

		connect_ptr = connect_time.Get (&connect_rec);
		if (connect_ptr == NULL) {
			Error ("Turn Movement %d to %d is Not in the Lane Connectivity File", 
				connect_rec.In_Link (), connect_rec.Out_Link ());
		}

		//---- save the volume data ----

		start = time_periods.Step (file->Start ());
		end = time_periods.Step (file->End ());
		i = time_periods.In_Index ((start + end + 1) / 2);

		if (type) {
			connect_ptr->Add_Count (i, file->Volume ());
		} else {
			connect_ptr->Add_Volume (i, file->Volume ());
		}
	}
	End_Progress ();
	Print (2, "Number of %s Records = %d", file->File_Type (), Progress_Count ());

	file->Close ();
}

