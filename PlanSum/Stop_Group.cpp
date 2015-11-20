//*********************************************************
//	Stop_Group.cpp - Report Transit Stop Groups
//*********************************************************

#include "PlanSum.hpp"

//---------------------------------------------------------
//	Stop_Group_Report
//---------------------------------------------------------

void PlanSum::Stop_Group_Report (void)
{
	int i, num, stop, total_board, total_alight;
	int walk_board, bus_board, rail_board, drive_board;
	int walk_alight, bus_alight, rail_alight, drive_alight;
	int tot_walk_on, tot_bus_on, tot_rail_on, tot_drive_on;
	int tot_walk_off, tot_bus_off, tot_rail_off, tot_drive_off;
    char buffer [STRING_BUFFER];

	Integer_List *group;
	Transfer_Data *transfer_ptr;

	Show_Message ("Transit Stop Group Report -- Group");
	Set_Progress (1);

	Header_Number (STOP_GROUP);

	num = stop_equiv.Num_Groups ();

	if (!Break_Check (num + 8)) {
		Print (1);
		Stop_Group_Header ();
	}
	num = stop_equiv.Max_Group ();

	tot_walk_on = tot_bus_on = tot_rail_on = tot_drive_on = 0;
	tot_walk_off = tot_bus_off = tot_rail_off = tot_drive_off = 0;

	for (i=1; i <= num; i++) {
		Show_Progress ();

		group = stop_equiv.Group_List (i);
		if (group == NULL) continue;

		walk_board = bus_board = rail_board = drive_board = 0;
		walk_alight = bus_alight = rail_alight = drive_alight = 0;

		str_fmt (buffer, sizeof (buffer), "%d %s", i, stop_equiv.Group_Label (i));

		//---- process each stop in the stop group ----

		for (stop = group->First (); stop != 0; stop = group->Next ()) {

			transfer_ptr = transfer_data.Get (stop);
			if (transfer_ptr == NULL) continue;

			walk_board += transfer_ptr->Walk_Board ();
			bus_board += transfer_ptr->Bus_Board ();
			rail_board += transfer_ptr->Rail_Board ();
			drive_board += transfer_ptr->Drive_Board ();

			walk_alight += transfer_ptr->Walk_Alight ();
			bus_alight += transfer_ptr->Bus_Alight ();
			rail_alight += transfer_ptr->Rail_Alight ();
			drive_alight += transfer_ptr->Drive_Alight ();
		}
		total_board = walk_board + bus_board + rail_board + drive_board;
		total_alight = walk_alight + bus_alight + rail_alight + drive_alight;

		tot_walk_on += walk_board;
		tot_bus_on += bus_board;
		tot_rail_on += rail_board;
		tot_drive_on += drive_board;

		tot_walk_off += walk_alight;
		tot_bus_off += bus_alight;
		tot_rail_off += rail_alight;
		tot_drive_off += drive_alight;

		Print (1, "%-18.18s%7d%7d%7d%7d %7d%7d%7d%7d%7d %7d", 
			buffer,	walk_board, bus_board, rail_board, drive_board, total_board,
			walk_alight, bus_alight, rail_alight, drive_alight, total_alight);
	}
	End_Progress ();

	total_board = tot_walk_on + tot_bus_on + tot_rail_on + tot_drive_on;
	total_alight = tot_walk_off + tot_bus_off + tot_rail_off + tot_drive_off;

	Print (2, "%-18.18s%7d%7d%7d%7d %7d%7d%7d%7d%7d %7d", 
		"Total", tot_walk_on, tot_bus_on, tot_rail_on, tot_drive_on, total_board,
		tot_walk_off, tot_bus_off, tot_rail_off, tot_drive_off, total_alight);

	Header_Number (0);
}

//---------------------------------------------------------
//	Stop_Group_Header
//---------------------------------------------------------

void PlanSum::Stop_Group_Header (void)
{
	Print (1, "Transit Stop Group Summary");
	Print (2, "%19c--------- Boardings From ---------- -------- Alightings To ------------", BLANK);
	Print (1, "Stop Group           Walk    Bus   Rail  Drive   Total   Walk    Bus   Rail  Drive   Total");
	Print (1);
}

/*********************************************|***********************************************

	Transit Stop Group Summary

	                   --------- Boardings From ---------- -------- Alightings To ------------
	Stop Group           Walk    Bus   Rail  Drive   Total   Walk    Bus   Rail  Drive   Total 
	
	ssssssssssssssssss dddddd dddddd dddddd dddddd ddddddd dddddd dddddd dddddd dddddd ddddddd

	Total              dddddd dddddd dddddd dddddd ddddddd dddddd dddddd dddddd dddddd ddddddd

**********************************************|***********************************************/ 
