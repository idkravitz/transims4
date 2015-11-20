//*********************************************************
//	Top_100_Link.cpp - Create a Top 100 Link Events
//*********************************************************

#include "EventSum.hpp"

//---------------------------------------------------------
//	Top_100_Link_Report
//---------------------------------------------------------

void EventSum::Top_100_Link_Report (void)
{
	int i, j, k, dir, vc, from, to, link, volume, tod_cap, lanes, tod, list;

	Link_Data *link_ptr;
	TTime_Data *ttime_ptr;
	Link_Use_Data *use_ptr;
	Range_Data *range_ptr;

	typedef struct {
		int vc;
		int link;
		int from;
		int to;
		int period;
		int volume;
		int capacity;
	} VC_Data;

	VC_Data vc_data [100], *vc_ptr;

	Show_Message ("Top 100 Link Event Report -- Link");
	Set_Progress (1000);

	memset (vc_data, '\0', 100 * sizeof (VC_Data));

	//---- find V/C ratios greater than min_vc ----

	for (link_ptr = link_data.First ();	link_ptr != NULL; link_ptr = link_data.Next ()) {
		Show_Progress ();

		link = link_ptr->Link ();

		//---- process each direction ----

        for (i=0; i < 2; i++) {
			dir = ((i) ? link_ptr->BA_Dir () : link_ptr->AB_Dir ());

			if (dir == 0) continue;

			ttime_ptr = ttime_data [dir];

			if (ttime_ptr->Periods () == 0) continue;

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

				volume = ttime_ptr->Data (j, 1);

				if (volume == 0) continue;

				//---- time-of-day capacity ----

				tod_cap = 0;

				if (list > 0) {

					//---- get the time period ----

					range_ptr = time_periods [j];
					tod = Round ((range_ptr->High () + range_ptr->Low () + 1) / 2);

					for (k = list; k; k = use_ptr->TOD_List ()) {
						use_ptr = link_use_data [k]; 

						if (use_ptr->Start () <= tod && tod < use_ptr->End ()) {
							tod_cap = (tod_cap * use_ptr->Lanes () + lanes / 2) / lanes;
							if (tod_cap == 0) tod_cap = 0;
							break;
						}
					}
				}

				vc = (volume * 100 + tod_cap / 2) / tod_cap;

				vc_ptr = vc_data;

				for (k=0; k < 100; k++, vc_ptr++) {
					if (vc > vc_ptr->vc) {
						if (vc_ptr->vc > 0 && k < 99) {
							memmove (vc_ptr+1, vc_ptr, (99-k) * sizeof (VC_Data));
						}
						vc_ptr->vc = vc;
						vc_ptr->link = link;
						vc_ptr->from = from;
						vc_ptr->to = to;
						vc_ptr->period = j;
						vc_ptr->volume = volume;
						vc_ptr->capacity = tod_cap;
						break;
					}
				}
			}
		}
	}
	End_Progress ();

	//---- print the report ----

	vc_ptr = vc_data;

	Header_Number (TOP_100_LINK);

	if (!Break_Check (100 + 7)) {
		Print (1);
		Top_100_Link_Header ();
	}		

	for (k=0; k < 100; k++, vc_ptr++) {
		if (vc_ptr->vc == 0) break;

		range_ptr = time_periods [vc_ptr->period];

		//---- print the data record ----

		Print (1, "%8d  %8d  %8d   %7d  %12.12s  %7d   %6.2lf", 
			vc_ptr->link, vc_ptr->from, vc_ptr->to, vc_ptr->capacity, 
			time_periods.Range_Format (vc_ptr->period), 
			vc_ptr->volume, ((double) vc_ptr->vc / 100.0));
	}
	if (k) {
		Print (2, "Number of Records in the Report = %d", k);
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Top_100_Link_Header
//---------------------------------------------------------

void EventSum::Top_100_Link_Header (void)
{
	Print (1, "Top 100 Link Event Report");
	Print (2, "    Link      From   To-Node  Capacity   Time-of-Day   Volume  V/C Ratio");
	Print (1);
}

/*********************************************|***********************************************

	Top 100 Link Event Report

	    Link      From   To-Node  Capacity   Time-of-Day   Volume  V/C Ratio

	dddddddd  dddddddd  dddddddd   ddddddd  dd:dd..dd:dd  ddddddd    ff.ff

	Number of Records in the Report = dddddd

**********************************************|***********************************************/ 
