//*********************************************************
//	Line_Sum.cpp - Create a Line Summary Report
//*********************************************************

#include "LineSum.hpp"

//---------------------------------------------------------
//	Line_Sum_Report
//---------------------------------------------------------

void LineSum::Line_Sum_Report (int number)
{
	int stop, stops, run, runs, period, per, num, time;
	int on, off, ride, total, max_ride, *total_rider, *max_load;
	char buffer [FIELD_BUFFER];
	bool last_stop_flag, stop_flag, line_flag;

	Rider_Data *rider_ptr;
	Offset_Data *offset_ptr;

	Show_Message ("Creating a Line Summary Report -- Record");
	Set_Progress (100);

	//---- initialize the selection sets ----

	set_ptr = select_set [number];
	if (set_ptr == NULL) return;

	Header_Number (LINE_SUM);

	//---- initialize the report data ----

	if (!Break_Check (rider_data.Num_Records () + 6)) {
		Print (1);
		Line_Sum_Header ();
	}
	num = set_ptr->Num_Periods () + 1;

	total_rider = new int [num];
	max_load = new int [num];

	if (total_rider == NULL || max_load == NULL) {
		Error ("Insufficient Space for %d Time Periods", num);
	}
	memset (total_rider, '\0', num * sizeof (int));
	memset (max_load, '\0', num * sizeof (int));

	//---- process each route ----

	for (rider_ptr = rider_data.First_Key (); rider_ptr; rider_ptr = rider_data.Next_Key ()) {
		Show_Progress ();

		//---- check the selection criteria ----

		if (!set_ptr->Select_Modes (rider_ptr->Mode ())) continue;
		if (!set_ptr->Select_Routes (rider_ptr->Route ())) continue;

		stops = rider_ptr->Stops ();
		runs = rider_ptr->Runs ();

		//---- check for stops in the subarea ----

		if (offset_flag && set_ptr->Subarea_Flag ()) {
			line_flag = false;

			for (stop=1; stop <= stops; stop++) {
				offset_ptr = stop_offset.Get (rider_ptr->Stop (stop));
				if (offset_ptr == NULL) continue;

				if (set_ptr->In_Subarea (offset_ptr->X (), offset_ptr->Y ())) {
					line_flag = true;
					break;
				}
			}
			if (!line_flag) continue;
		}

		//---- print the line label ----

		str_fmt (buffer, sizeof (buffer), "%s %d", Transit_Code ((Transit_Type) rider_ptr->Mode ()), rider_ptr->Route ());

		Print (1, "%16.16s", buffer);

		//---- process each time period ----

		for (per=1; per < num; per++) {

			total = max_ride = 0;

			for (run=1; run <= runs; run++) {
				switch (set_ptr->Time_Method ()) {
					case RUN_START:
						time = rider_ptr->Time (run, 1);
						break;
					case RUN_END:
						time = rider_ptr->Time (run, stops);
						break;
					case RUN_MID:
						time = (rider_ptr->Time (run, 1) + rider_ptr->Time (run, stops)) / 2;
						break;
					case SCHED_START:
						time = rider_ptr->Schedule (run, 1);
						break;
					case SCHED_END:
						time = rider_ptr->Schedule (run, stops);
						break;
					case SCHED_MID:
						time = (rider_ptr->Schedule (run, 1) + rider_ptr->Schedule (run, stops)) / 2;
						break;
				}
				period = set_ptr->Time_Period (Resolve (time));

				if (period < per) continue;
				if (period > per) break;

				//---- sum the line ridership ----

				ride = 0;
				last_stop_flag = stop_flag = true;

				for (stop=1; stop <= stops; stop++) {
					on = rider_ptr->Board (run, stop);
					off = rider_ptr->Alight (run, stop);

					if (offset_flag && set_ptr->Subarea_Flag ()) {
						offset_ptr = stop_offset.Get (rider_ptr->Stop (stop));
						if (offset_ptr == NULL) continue;

						stop_flag = set_ptr->In_Subarea (offset_ptr->X (), offset_ptr->Y ());

						if (stop_flag && !last_stop_flag) {
							total += ride;
						}
						last_stop_flag = stop_flag;
					}
					ride += on - off;

					if (stop_flag) {
						total += on;
						if (ride > max_ride) max_ride = ride;
					}
				}
			}
			Print (0, "  %7d %7d", total, max_ride);

			total_rider [per] += total;
			if (max_ride > max_load [per]) max_load [per] = max_ride;
		}

		//---- add the row totals ----

		if (num > 2) {
			total = max_ride = 0;

			for (run=1; run <= runs; run++) {
				switch (set_ptr->Time_Method ()) {
					case RUN_START:
						time = rider_ptr->Time (run, 1);
						break;
					case RUN_END:
						time = rider_ptr->Time (run, stops);
						break;
					case RUN_MID:
						time = (rider_ptr->Time (run, 1) + rider_ptr->Time (run, stops)) / 2;
						break;
					case SCHED_START:
						time = rider_ptr->Schedule (run, 1);
						break;
					case SCHED_END:
						time = rider_ptr->Schedule (run, stops);
						break;
					case SCHED_MID:
						time = (rider_ptr->Schedule (run, 1) + rider_ptr->Schedule (run, stops)) / 2;
						break;
				}
				period = set_ptr->Time_Period (Resolve (time));
				if (period == 0) continue;

				//---- sum the line ridership ----

				ride = 0;
				last_stop_flag = stop_flag = true;

				for (stop=1; stop <= stops; stop++) {
					on = rider_ptr->Board (run, stop);
					off = rider_ptr->Alight (run, stop);

					if (offset_flag && set_ptr->Subarea_Flag ()) {
						offset_ptr = stop_offset.Get (rider_ptr->Stop (stop));
						if (offset_ptr == NULL) continue;

						stop_flag = set_ptr->In_Subarea (offset_ptr->X (), offset_ptr->Y ());

						if (stop_flag && !last_stop_flag) {
							total += ride;
						}
						last_stop_flag = stop_flag;
					}
					ride += on - off;

					if (stop_flag) {
						total += off;
						if (ride > max_ride) max_ride = ride;
					}
				}
			}
			Print (0, "  %7d %7d", total, max_ride);

			total_rider [0] += total;
			if (max_ride > max_load [0]) max_load [0] = max_ride;
		}
	}
	End_Progress ();

	//---- print the period totals ----

	Print (2, "           Total");

	for (per=1; per <= num; per++) {
		if (per == num) {
			if (num > 2) {
				per = 0;
			} else {
				break;
			}
		}
		Print (0, " %8d %7d", total_rider [per], max_load [per]);
		if (per == 0) break;
	}
	delete [] total_rider;
	delete [] max_load;

	Header_Number (0);
}

//---------------------------------------------------------
//	Line_Sum_Header
//---------------------------------------------------------

void LineSum::Line_Sum_Header (void)
{
	int i, num;
	
	num = set_ptr->Num_Periods ();

	Print (1, "Line Summary Report");
	if (set_ptr->Number () > 0) Print (0, " #%d", set_ptr->Number ());
	if (set_ptr->Select_Label () != NULL) Print (0, " (%s)", set_ptr->Select_Label ());

	Print (2, "%16c", BLANK);

	for (i=1; i <= num; i++) {
		Print (0, "  --%12.12s-", set_ptr->Time_Format (i));
	}
	if (num > 1) {
		Print (0, "  ---- Total ----");
		num++;
	}
	Print (1, "      Mode:Route");

	for (i=1; i <= num; i++) {
		Print (0, "   Riders MaxLoad");
	}
	Print (1);
}

/*********************************************|***********************************************

	Line Summary Report (%s)

	                  --dd:dd..dd:dd-  --dd:dd..dd:dd-  ---- Total ----
	      Mode:Route   Riders MaxLoad   Riders MaxLoad   Riders MaxLoad

	ssssssss16ssssss  ddddddd ddddddd  ddddddd ddddddd  ddddddd ddddddd

	           Total dddddddd ddddddd dddddddd ddddddd dddddddd ddddddd

**********************************************|***********************************************/ 
