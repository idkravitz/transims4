//*********************************************************
//	Stop_Group.cpp - Create a Stop Group Summary Report
//*********************************************************

#include "LineSum.hpp"

//---------------------------------------------------------
//	Stop_Group_Report
//---------------------------------------------------------

void LineSum::Stop_Group_Report (void)
{
	int on_bus, on_rail, off_bus, off_rail;
	int tot_on_bus, tot_on_rail, tot_off_bus, tot_off_rail;

	int i, j, s, stop, stops, run, runs, period, per, nper, num, time, on, off;

	Rider_Data *rider_ptr;
	Integer_List *group;

	Show_Message ("Creating Transit Stop Group Summary -- Record");
	Set_Progress (100);

	//---- initialize the selection sets ----

	set_ptr = select_set.First ();
	if (set_ptr == NULL) return;

	Header_Number (STOP_GROUP);

	num = stop_equiv.Num_Groups ();

	if (!Break_Check (num + 8)) {
		Print (1);
		Stop_Group_Header ();
	}
	num = stop_equiv.Max_Group ();

	nper = set_ptr->Num_Periods ();

	tot_on_bus = tot_on_rail = tot_off_bus = tot_off_rail = 0;

	//---- process each stop group ----

	for (i=1; i <= num; i++) {

		group = stop_equiv.Group_List (i);
		if (group == NULL) continue;

		on_bus = on_rail = off_bus = off_rail = 0;

		//---- process each time period ----

		for (per=1; per <= nper; per++) {
			Show_Progress ();

			//---- process each route ----

			for (rider_ptr = rider_data.First_Key (); rider_ptr; rider_ptr = rider_data.Next_Key ()) {

				//---- check the selection criteria ----

				if (!set_ptr->Select_Modes (rider_ptr->Mode ())) continue;
				if (!set_ptr->Select_Routes (rider_ptr->Route ())) continue;

				stops = rider_ptr->Stops ();
				runs = rider_ptr->Runs ();

				//---- check the stop ----

				for (j=1; j <= stops; j++) {

					stop = rider_ptr->Stop (j);

					//---- check for stops in the stop group ----

					for (s = group->First (); s != 0; s = group->Next ()) {
						if (s == stop) break;
					}
					if (s == 0) continue;

					//---- process each run ----

					for (run=1; run <= runs; run++) {
						switch (set_ptr->Time_Method ()) {
							case RUN_START:
							case RUN_END:
							case RUN_MID:
								time = rider_ptr->Time (run, j);
								break;
							case SCHED_START:
							case SCHED_END:
							case SCHED_MID:
								time = rider_ptr->Schedule (run, j);
								break;
						}
						period = set_ptr->Time_Period (Resolve (time));

						if (period < per) continue;
						if (period > per) break;

						on = rider_ptr->Board (run, j);
						off = rider_ptr->Alight (run, j);

						if (rider_ptr->Mode () > EXPRESS_BUS) {
							on_rail += on;
							off_rail += off;
						} else {
							on_bus += on;
							off_bus += off;
						}
					}
				}
			}
		}

		//---- print the time period data ---

		Print (1, "%-25.25s%7d%7d%7d %7d%7d%7d %7d%7d%7d", 
			stop_equiv.Group_Label (i), on_bus, on_rail, (on_bus + on_rail),
			off_bus, off_rail, (off_bus + off_rail), (on_bus + off_bus),
			(on_rail + off_rail), (on_bus + off_bus + on_rail + off_rail));

		tot_on_bus += on_bus;
		tot_on_rail += on_rail;
		tot_off_bus += off_bus;
		tot_off_rail += off_rail;
	}
	End_Progress ();

	//---- print the total ---

	Print (2, "Total                    %7d%7d%7d %7d%7d%7d %7d%7d%7d", 
		tot_on_bus, tot_on_rail, (tot_on_bus + tot_on_rail),
		tot_off_bus, tot_off_rail, (tot_off_bus + tot_off_rail), 
		(tot_on_bus + tot_off_bus), (tot_on_rail + tot_off_rail), 
		(tot_on_bus + tot_off_bus + tot_on_rail + tot_off_rail));

	Header_Number (0);
}

//---------------------------------------------------------
//	Stop_Group_Header
//---------------------------------------------------------

void LineSum::Stop_Group_Header (void)
{
	Print (1, "Transit Stop Group Summary Report");
	if (set_ptr->Select_Label () != NULL) Print (0, " (%s)", set_ptr->Select_Label ());

	Print (2, "%26c----- Boardings ----  ---- Alightings ----  ------ Total -------", BLANK);
	Print (1, "Stop Group                   Bus   Rail  Total     Bus   Rail  Total     Bus   Rail  Total");
	Print (1);
}

/*********************************************|***********************************************

	Transit Stop Group Summary Report (sssssssssss)

	                          ----- Boardings ----  ---- Alightings ----  ------ Total -------
	Stop Group                   Bus   Rail  Total     Bus   Rail  Total     Bus   Rail  Total

	sssssssssssssssssssssssss dddddd dddddd dddddd  dddddd dddddd dddddd  dddddd dddddd dddddd

	Total                     dddddd dddddd dddddd  dddddd dddddd dddddd  dddddd dddddd dddddd

**********************************************|***********************************************/ 
