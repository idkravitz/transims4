//*********************************************************
//	Read_Delay.cpp - Read the Link Delay File
//*********************************************************

#include "IntControl.hpp"

//---------------------------------------------------------
//	Read_Delay
//---------------------------------------------------------

void IntControl::Read_Delay (void)
{
	int i, j, link, dir, num, start, end, nconnect, volume, nperiods;
	bool skip;

	Connect_Time *connect_ptr, connect_rec;
	Link_Data *link_ptr;
    
	//---- read the turn movement file ----

	Show_Message ("Reading %s -- Record", delay_file.File_Type ());
	Set_Progress ();

	if (time_flag) {
		num = time_range.Num_Ranges ();
	} else {
		num = 1;
	}
	nperiods = num;

	//---- read the turn movement data ----

	while (delay_file.Read (false)) {
		Show_Progress ();

		//---- get the link record ----

		link = delay_file.Link ();
		dir = delay_file.Dir ();
		skip = false;

		if (delay_file.LinkDir_Type () == LINK_SIGN) {
			if (link < 0) {
				link = -link;
				dir = 1;
			}
		}
		link_ptr = link_data.Get (link);

		if (link_ptr == NULL) {
			skip = true;
		} else {
			if (delay_file.LinkDir_Type () == LINK_NODE) {

				//---- from node!!! ----

				if (dir == link_ptr->Anode ()) {
					dir = 0;
				} else if (dir == link_ptr->Bnode ()) {
					dir = 1;
				} else {
					skip = true;
				}
			}
			if (!skip) {
				connect_rec.In_Link (link);
				connect_rec.In_Dir (dir);
			}
		}
		nconnect = delay_file.Num_Connect ();

		for (i=1; i <= nconnect; i++) {
			if (!delay_file.Read (true)) {
				Error ("Number of Nested Records for Link %d", link);
			}
			Show_Progress ();
			
			if (skip) continue;

			volume = delay_file.Out_Turn ();

			if (volume == 0) continue;

			link = delay_file.Out_Link ();
			dir = delay_file.Out_Dir ();

			if (delay_file.LinkDir_Type () == LINK_SIGN) {
				if (link < 0) {
					link = -link;
					dir = 1;
				}
			}
			link_ptr = link_data.Get (link);
			if (link_ptr == NULL) continue;

			if (delay_file.LinkDir_Type () == LINK_NODE) {

				//---- from node!!!! ----

				if (dir == link_ptr->Anode ()) {
					dir = 0;
				} else if (dir == link_ptr->Bnode ()) {
					dir = 1;
				} else {
					continue;
				}
			}
			connect_rec.Out_Link (link);
			connect_rec.Out_Dir (dir);

			//---- get the connection record ----

			connect_ptr = connect_time.Get (&connect_rec);
			if (connect_ptr == NULL) continue;

			start = time_range.Step (delay_file.Start_Time ());
			end = time_range.Step (delay_file.End_Time ());

			if (connect_ptr->Periods () == 0) {
				if ((regen_flag || update_flag) && connect_time.Period_Size () == 0) {
					connect_time.Period_Size (end - start);
					nperiods = connect_time.Periods ();
				}
				if (!connect_ptr->Periods (nperiods)) {
					Error ("Insufficient Memory for Turn Time Periods");
				}
				for (j=1; j <= nperiods; j++) {
					connect_ptr->Volume (j, 0);
				}
			}

			//---- save the volume data ----

			if (regen_flag || update_flag) {
				j = connect_time.Period (end - 1);
			} else if (time_flag) {
				j = time_range.Period ((start + end + 1) / 2);
			} else {
				j = 1;
			}
			connect_ptr->Add_Volume (j, volume);
		}
	}
	End_Progress ();
	Print (2, "Number of %s Records = %d", delay_file.File_Type (), Progress_Count ());

	delay_file.Close ();
}

