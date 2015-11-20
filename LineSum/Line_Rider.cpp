//*********************************************************
//	Line_Rider.cpp - Create a Line Ridership Report
//*********************************************************

#include "LineSum.hpp"

//---------------------------------------------------------
//	Line_Rider_Report
//---------------------------------------------------------

void LineSum::Line_Rider_Report (int number)
{
	int stop, stops, run, runs, period, per, num, time;
	int on, off, tot_on, tot_off, tot_ride, stop_id;
	bool first, last_stop_flag, stop_flag, line_flag;

	Rider_Data *rider_ptr;
	Offset_Data *offset_ptr;

	typedef struct {
		int ride;
		int tot_on;
		int tot_off;
		int tot_ride;
		int max_on;
		int max_off;
		int max_ride;
	} Period_Data;

	Period_Data *period_data, *period_ptr;

	Show_Message ("Creating a Line Ridership Report -- Record");
	Set_Progress (100);

	//---- initialize the selection sets ----

	set_ptr = select_set [number];
	if (set_ptr == NULL) return;

	Header_Number (LINE_RIDER);
	first = true;

	num = set_ptr->Num_Periods () + 1;
	period_data = new Period_Data [num];

	if (period_data == NULL) {
		Error ("Insufficient Space for %d Time Periods", num);
	}

	//---- process each route ----

	for (rider_ptr = rider_data.First_Key (); rider_ptr; rider_ptr = rider_data.Next_Key ()) {
		Show_Progress ();

		if (!set_ptr->Select_Modes (rider_ptr->Mode ())) continue;
		if (!set_ptr->Select_Routes (rider_ptr->Route ())) continue;

		stops = rider_ptr->Stops ();
		runs = rider_ptr->Runs ();

		memset (period_data, '\0', num * sizeof (Period_Data));
		line_flag = false;
		last_stop_flag = stop_flag = true;

		for (stop=1; stop <= stops; stop++) {
			stop_id = rider_ptr->Stop (stop);

			if (offset_flag && set_ptr->Subarea_Flag ()) {
				offset_ptr = stop_offset.Get (stop_id);
				if (offset_ptr == NULL) continue;

				stop_flag = set_ptr->In_Subarea (offset_ptr->X (), offset_ptr->Y ());
			}
			if (stop_flag) {
				if (!line_flag) {
					if (first) {
						New_Page ();
						first = false;
					} else {
						Break_Check (MIN ((stops - stop + 8), 15));
					}
					Print (1, "Line: %s %d  %s", Transit_Code ((Transit_Type) rider_ptr->Mode ()), 
						rider_ptr->Route (), rider_ptr->Name ());
					Print (1);
					line_flag = true;
				}
				Print (1, "%8d", stop_id);
			}
			tot_on = tot_off = tot_ride = 0;

			for (per=1; per < num; per++) {

				on = off = 0;
				period_ptr = period_data + per;

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

					on += rider_ptr->Board (run, stop);
					off += rider_ptr->Alight (run, stop);
				}
				if (stop_flag && !last_stop_flag) {
					period_ptr->tot_ride += period_ptr->ride;
				}
				period_ptr->ride += on - off;

				if (stop_flag) {
					Print (0, " %7d%7d%7d", on, off, period_ptr->ride);
				
					tot_on += on;
					tot_off += off;
					tot_ride += period_ptr->ride;

					period_ptr->tot_on += on;
					period_ptr->tot_off += off;
					period_ptr->tot_ride += on;
					if (on > period_ptr->max_on) period_ptr->max_on = on;
					if (off > period_ptr->max_off) period_ptr->max_off = off;
					if (period_ptr->ride > period_ptr->max_ride) period_ptr->max_ride = period_ptr->ride;
				}
			}
			if (stop_flag && num > 2) {
				Print (0, " %7d%7d%7d", tot_on, tot_off, tot_ride);

				period_ptr = period_data;

				period_ptr->tot_on += tot_on;
				period_ptr->tot_off += tot_off;
				period_ptr->tot_ride += tot_ride;
				if (tot_on > period_ptr->max_on) period_ptr->max_on = tot_on;
				if (tot_off > period_ptr->max_off) period_ptr->max_off = tot_off;
				if (tot_ride > period_ptr->max_ride) period_ptr->max_ride = tot_ride;
			}
			last_stop_flag = stop_flag;
		}
		if (line_flag) {
			Print (2, "   Total");

			for (per=1; per <= num; per++) {
				if (per == num) {
					if (num > 2) {
						per = 0;
					} else {
						break;
					}
				}
				period_ptr = period_data + per;

				Print (0, " %7d%7d%7d", period_ptr->tot_on, period_ptr->tot_off, period_ptr->tot_ride);
				if (per == 0) break;
			}

			Print (1, " Maximum");

			for (per=1; per <= num; per++) {
				if (per == num) {
					if (num > 2) {
						per = 0;
					} else {
						break;
					}
				}
				period_ptr = period_data + per;

				Print (0, " %7d%7d%7d", period_ptr->max_on, period_ptr->max_off, period_ptr->max_ride);
				if (per == 0) break;
			}
			Print (1);
		}
	}
	End_Progress ();

	Header_Number (0);

	if (period_data != NULL) {
		delete [] period_data;
	}
}

//---------------------------------------------------------
//	Line_Rider_Header
//---------------------------------------------------------

void LineSum::Line_Rider_Header (void)
{
	int i, num;
	
	num = set_ptr->Num_Periods ();

	Print (1, "Line Ridership Report");
	if (set_ptr->Number () > 0) Print (0, " #%d", set_ptr->Number ());
	if (set_ptr->Select_Label () != NULL) Print (0, " (%s)", set_ptr->Select_Label ());

	Print (2, "        ");

	for (i=1; i <= num; i++) {
		Print (0, "  ----%12.12s----", set_ptr->Time_Format (i));
	}
	if (num > 1) {
		Print (0, "  ------- Total ------");
		num++;
	}
	Print (1, "    Stop");

	for (i=1; i <= num; i++) {
		Print (0, "      On    Off   Ride");
	}
	Print (1);
}

/*********************************************|***********************************************

	Line Ridership Report

	          ----dd:dd..dd:dd----  ----dd:dd..dd:dd----  ------- Total ------
	    Stop      On    Off   Ride      On    Off   Ride      On    Off   Ride

	dddddddd  dddddd dddddd dddddd  dddddd dddddd dddddd  dddddd dddddd dddddd
	dddddddd  dddddd dddddd dddddd  dddddd dddddd dddddd  dddddd dddddd dddddd

	   Total  dddddd dddddd dddddd  dddddd dddddd dddddd  dddddd dddddd dddddd
	 Maximum  dddddd dddddd dddddd  dddddd dddddd dddddd  dddddd dddddd dddddd

**********************************************|***********************************************/ 
