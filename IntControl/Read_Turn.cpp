//*********************************************************
//	Read_Turn.cpp - Read the Turn Movement File
//*********************************************************

#include "IntControl.hpp"

//---------------------------------------------------------
//	Read_Turn
//---------------------------------------------------------

void IntControl::Read_Turn (void)
{
	int i, link, node, dir, num, start, end, nperiods;

	Connect_Time *connect_ptr, connect_rec;
	Link_Data *link_ptr;
    
	//---- read the turn movement file ----

	Show_Message ("Reading %s -- Record", turn_file.File_Type ());
	Set_Progress ();

	if (time_flag) {
		num = time_range.Num_Ranges ();
	} else {
		num = 1;
	}
	nperiods = num;

	//---- read the turn movement data ----

	while (turn_file.Read ()) {
		Show_Progress ();

		node = turn_file.Node ();
		if (node == 0) continue;

		//---- identify the in link ----

		link = turn_file.In_Link ();
		link_ptr = link_data.Get (link);

		if (link_ptr == NULL) {
			Warning ("Turn Movement Link %d was Not Found in the Link File", link);
			continue;
		}

		if (link_ptr->Anode () == node) {
			dir = 1;
		} else if (link_ptr->Bnode () == node) {
			dir = 0;
		} else {
			Warning ("Turn Movement Link %d is Not Connected to Node %d", link, node);
			continue;
		}
		connect_rec.In_Link (link);
		connect_rec.In_Dir (dir);

		//---- identify the out link ----

		link = turn_file.Out_Link ();
		link_ptr = link_data.Get (link);

		if (link_ptr == NULL) {
			Warning ("Turn Movement Link %d was Not Found in the Link File", link);
			continue;
		}

		if (link_ptr->Anode () == node) {
			dir = 0;
		} else if (link_ptr->Bnode () == node) {
			dir = 1;
		} else {
			Warning ("Turn Movement Link %d is Not Connected to Node %d", link, node);
			continue;
		}
		connect_rec.Out_Link (link);
		connect_rec.Out_Dir (dir);

		//---- get the connection record ----

		connect_ptr = connect_time.Get (&connect_rec);

		if (connect_ptr == NULL) {
			Warning ("Turn Movement %d to %d is Not in the Lane Connectivity File", 
				connect_rec.In_Link (), connect_rec.Out_Link ());
			continue;
		}
		start = time_range.Step (turn_file.Start ());
		end = time_range.Step (turn_file.End ());

		if (connect_ptr->Periods () == 0) {
			if ((regen_flag || update_flag) && connect_time.Period_Size () == 0) {
				connect_time.Period_Size (end - start);
				nperiods = connect_time.Periods ();
			}
			if (!connect_ptr->Periods (nperiods)) {
				Error ("Insufficient Memory for Turn Time Periods");
			}
			for (i=1; i <= nperiods; i++) {
				connect_ptr->Volume (i, 0);
			}
		}

		//---- save the volume data ----

		if (regen_flag || update_flag) {
			i = connect_time.Period (end - 1);
		} else if (time_flag) {
			i = time_range.Period ((start + end + 1) / 2);
		} else {
			i = 1;
		}
		connect_ptr->Add_Volume (i, turn_file.Volume ());
	}
	End_Progress ();
	Print (2, "Number of %s Records = %d", turn_file.File_Type (), Progress_Count ());

	turn_file.Close ();
}

