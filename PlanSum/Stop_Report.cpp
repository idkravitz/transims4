//*********************************************************
//	Stop_Report.cpp - Report Transit Stops
//*********************************************************

#include "PlanSum.hpp"

//---------------------------------------------------------
//	Stop_Report
//---------------------------------------------------------

void PlanSum::Stop_Report (void)
{
	int num, total_board, total_alight;
	int walk_board, bus_board, rail_board, drive_board;
	int walk_alight, bus_alight, rail_alight, drive_alight;

	Transfer_Data *transfer_ptr;

	Show_Message ("Transit Stop Report -- Stop");
	Set_Progress (100);

	Header_Number (SUM_STOPS);

	num = transfer_data.Num_Records ();

	if (!Break_Check (num + 8)) {
		Print (1);
		Stop_Header ();
	}

	walk_board = bus_board = rail_board = drive_board = 0;
	walk_alight = bus_alight = rail_alight = drive_alight = 0;

	//---- process each transit stop ----

	for (transfer_ptr = transfer_data.First_Key (); transfer_ptr; transfer_ptr = transfer_data.Next_Key ()) {
		Show_Progress ();

		total_board = transfer_ptr->Walk_Board () + transfer_ptr->Bus_Board () + 
						transfer_ptr->Rail_Board () + transfer_ptr->Drive_Board ();

		total_alight = transfer_ptr->Walk_Alight () + transfer_ptr->Bus_Alight () + 
						transfer_ptr->Rail_Alight () + transfer_ptr->Drive_Alight ();

		if (total_board == 0 && total_alight == 0) continue;

		if (total_board > 0) {
			walk_board += transfer_ptr->Walk_Board ();
			bus_board += transfer_ptr->Bus_Board ();
			rail_board += transfer_ptr->Rail_Board ();
			drive_board += transfer_ptr->Drive_Board ();
		}
		if (total_alight > 0) {
			walk_alight += transfer_ptr->Walk_Alight ();
			bus_alight += transfer_ptr->Bus_Alight ();
			rail_alight += transfer_ptr->Rail_Alight ();
			drive_alight += transfer_ptr->Drive_Alight ();
		}

		Print (1, "%-10d %7d %7d %7d %7d %7d %7d %7d %7d %7d %7d", 
			transfer_ptr->Stop (),
			transfer_ptr->Walk_Board (), transfer_ptr->Bus_Board (),
			transfer_ptr->Rail_Board (), transfer_ptr->Drive_Board (),
			total_board,
			transfer_ptr->Walk_Alight (), transfer_ptr->Bus_Alight (),
			transfer_ptr->Rail_Alight (), transfer_ptr->Drive_Alight (),
			total_alight);
	}
	End_Progress ();

	total_board = walk_board + bus_board + rail_board + drive_board;
	total_alight = walk_alight + bus_alight + rail_alight + drive_alight;

	Print (2, "Total      %7d %7d %7d %7d %7d %7d %7d %7d %7d %7d", 
		walk_board, bus_board, rail_board, drive_board, total_board,
		walk_alight, bus_alight, rail_alight, drive_alight, total_alight);

	Header_Number (0);
}

//---------------------------------------------------------
//	Stop_Header
//---------------------------------------------------------

void PlanSum::Stop_Header (void)
{
	Print (1, "Transit Stop Summary");
	Print (2, "%11c----------- Boardings From ------------ ---------- Alightings To -------------", BLANK);
	Print (1, "Stop          Walk     Bus    Rail   Drive   Total    Walk     Bus    Rail   Drive   Total");
	Print (1);
}

/*********************************************|***********************************************

	Transit Stop Summary

               ----------- Boardings From ------------ ---------- Alightings To --------------
	Stop          Walk     Bus    Rail   Drive   Total    Walk     Bus    Rail   Drive   Total 
	
	dddddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd

	Total      ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd

**********************************************|***********************************************/ 
