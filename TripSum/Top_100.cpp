//*********************************************************
//	Top_100.cpp - Create a Top 100 Link/Lane/VC Report
//*********************************************************

#include "TripSum.hpp"

//---------------------------------------------------------
//	Top_100_Report
//---------------------------------------------------------

void TripSum::Top_100_Report (int report_num)
{
	int i, j, k, dir, from, to, link, volume, cap, tod_cap, lanes, tod_lanes, tod, list, criteria, period;

	Link_Data *link_ptr;
	TTime_Data *ttime_ptr;
	Link_Use_Data *use_ptr;
	Range_Data *range_ptr;

	typedef struct {
		int criteria;
		int link;
		int from;
		int to;
		int period;
		int volume;
		int capacity;
	} Top_Data;

	Top_Data top_data [100], *top_ptr;

	Show_Message ("Creating a Top 100 Report -- Link");
	Set_Progress (1000);

	memset (top_data, '\0', 100 * sizeof (Top_Data));

	//---- find the top 100 records ----

	for (link_ptr = link_data.First ();	link_ptr; link_ptr = link_data.Next ()) {
		Show_Progress ();

		link = link_ptr->Link ();

		//---- process each direction ----

        for (i=0; i < 2; i++) {
			dir = ((i) ? link_ptr->BA_Dir () : link_ptr->AB_Dir ());

			if (dir == 0) continue;

			ttime_ptr = ttime_data [dir];

			if (ttime_ptr->Periods () == 0 || ttime_ptr->Capacity () <= 0) continue;

			lanes = ttime_ptr->Thru ();
			if (lanes < 1) lanes = 1;

			list = ttime_ptr->TOD_List ();

			//---- set the direction index ----

			if (i) {
				from = link_ptr->Bnode ();
				to = link_ptr->Anode ();
			} else {
				from = link_ptr->Anode ();
				to = link_ptr->Bnode ();
			}

			//---- process each time period ----

			for (j=1; j <= num_inc; j++) {

				volume = ttime_ptr->Volume (j);

				if (volume == 0) continue;

				//---- get the time period ----

				range_ptr = time_periods [j];
				tod = Round ((range_ptr->High () + range_ptr->Low () + 1) / 2);
				period = range_ptr->High () - range_ptr->Low () + 1;

				//---- time period capacity ----

				cap = (ttime_ptr->Capacity () * period + 1800) / 3600;
				if (cap <= 0) continue;

				tod_cap = cap;
				tod_lanes = lanes;

				if (list > 0) {
					for (k = list; k; k = use_ptr->TOD_List ()) {
						use_ptr = link_use_data [k]; 

						if (use_ptr->Start () <= tod && tod < use_ptr->End ()) {
							tod_lanes = use_ptr->Lanes ();
							tod_cap = (tod_cap * tod_lanes + lanes / 2) / lanes;
							if (tod_cap == 0) tod_cap = cap / 2;
							break;
						}
					}
				}

				//---- set the Top 100 criteria ----

				switch (report_num) {
					case TOP_100_LINKS:
						criteria = volume;
						break;
					case TOP_100_LANES:
						if (tod_lanes > 0) {
							criteria = (volume + tod_lanes / 2) / tod_lanes;
						} else {
							criteria = 0;
						}
						break;
					case TOP_100_VC_RATIOS:
						criteria = (volume * 100 + tod_cap / 2) / tod_cap;
						break;
				}
				top_ptr = top_data;

				for (k=0; k < 100; k++, top_ptr++) {
					if (criteria > top_ptr->criteria) {
						if (top_ptr->criteria > 0 && k < 99) {
							memmove (top_ptr+1, top_ptr, (99-k) * sizeof (Top_Data));
						}
						top_ptr->criteria = criteria;
						top_ptr->link = link;
						top_ptr->from = from;
						top_ptr->to = to;
						top_ptr->period = j;
						top_ptr->volume = volume;
						top_ptr->capacity = tod_cap;
						break;
					}
				}
			}
		}
	}
	End_Progress ();

	//---- print the report ----

	top_ptr = top_data;

	Header_Number (report_num);

	if (!Break_Check (100 + 7)) {
		Print (1);
		Top_100_Header (report_num);
	}		

	for (k=0; k < 100; k++, top_ptr++) {
		if (top_ptr->criteria == 0) break;

		range_ptr = time_periods [top_ptr->period];

		//---- print the data record ----

		Print (1, "%8d  %8d  %8d   %7d  %12.12s  %7d    %6.2lf", 
			top_ptr->link, top_ptr->from, top_ptr->to, top_ptr->capacity, 
			time_periods.Range_Format (top_ptr->period), 
			top_ptr->volume, (double) top_ptr->volume / top_ptr->capacity);
	}
	if (k) {
		Print (2, "Number of Records in the Report = %d", k);
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Top_100_Header
//---------------------------------------------------------

void TripSum::Top_100_Header (int report_num)
{
	switch (report_num) {
		case TOP_100_LINKS:
			Print (1, "Top 100 Link Trip Ends");
			break;
		case TOP_100_LANES:
			Print (1, "Top 100 Lane Trip Ends");
			break;
		case TOP_100_VC_RATIOS:
			Print (1, "Top 100 Trip/Capacity Ratios");
			break;
	}
	Print (2, "    Link      From   To-Node  Capacity   Time-of-Day  TripEnds  T/C Ratio");
	Print (1);
}

/*********************************************|***********************************************

	Top 100 Link Trip End Report
	Top 100 Lane Trip End Report
	Top 100 V/C Ratio Report

	    Link      From   To-Node  Capacity   Time-of-Day  TripEnds  T/C Ratio

	dddddddd  dddddddd  dddddddd   ddddddd  dd:dd..dd:dd  ddddddd     ff.ff

	Number of Records in the Report = dddddd

**********************************************|***********************************************/ 
