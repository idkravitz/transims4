//*********************************************************
//	Top_100.cpp - Create a Top 100 Volume Report
//*********************************************************

#include "LinkSum.hpp"

//---------------------------------------------------------
//	Top_100_Report
//---------------------------------------------------------

void LinkSum::Top_100_Report (int type)
{
	int i, j, k, num, anode, bnode, min_vol, max_vol, max_tim, max_an, max_bn, vol;
	int tod_list, tod, lanes, max_lanes;

	Link_Data *link_ptr;
	Performance_Data *perf_ptr;
	Link_Use_Data *use_ptr;
	Range_Data *range_ptr;

	//---- Top 100 Data ----

	typedef struct {
		int link;
		int from;
		int to;
		int period;
		int volume;
	} Volume_Data;

	Volume_Data volume [100], *vol_ptr;

	Show_Message ("Creating the Top 100 Volume Report -- Record");
	Set_Progress (10000);

	memset (volume, '\0', 100 * sizeof (Volume_Data));

	//---- find Top 100 Volumes ----

	min_vol = 0;

	for (link_ptr = link_data.First (); link_ptr; link_ptr = link_data.Next ()) {

		if (select_flag && link_ptr->Use () == 0) continue;

		Show_Progress ();

		for (i=0; i < 2; i++) {
			if (i) {
				if (link_ptr->Use () == -1) continue;
				perf_ptr = performance_data [link_ptr->BA_Dir ()];
				anode = link_ptr->Bnode ();
				bnode = link_ptr->Anode ();
			} else {
				if (link_ptr->Use () == -2) continue;
				perf_ptr = performance_data [link_ptr->AB_Dir ()];
				anode = link_ptr->Anode ();
				bnode = link_ptr->Bnode ();
			}
			if (perf_ptr == NULL) continue;

			num = perf_ptr->Periods ();
			max_vol = max_tim = max_an = max_bn = 0;

			max_lanes = lanes = perf_ptr->Thru ();
			tod_list = perf_ptr->TOD_List ();

			//---- scan each time period ----
			
			for (j=1; j <= num; j++) {

				vol = perf_ptr->Volume (j);

				if (vol < min_volume) continue;

				if (type == TOP_PERIOD) {
					if (vol <= min_vol) continue;

					vol_ptr = volume;

					for (k=0; k < 100; k++, vol_ptr++) {
						if (vol > vol_ptr->volume) {
							if (vol_ptr->volume > 0 && k < 99) {
								memmove (vol_ptr+1, vol_ptr, (99-k) * sizeof (Volume_Data));
								min_vol = volume [99].volume;
							}
							vol_ptr->link = link_ptr->Link ();
							vol_ptr->from = anode;
							vol_ptr->to = bnode;
							vol_ptr->period = j;
							vol_ptr->volume = vol;
							break;
						}
					}
				} else {
					if (type == TOP_LANE_VOL) {
						if (tod_list > 0) {

							//---- get the time period ----

							range_ptr = time_periods [j];

							tod = Round ((range_ptr->High () + range_ptr->Low () + 1) / 2);
							lanes = max_lanes;

							for (k = tod_list; k; k = use_ptr->TOD_List ()) {
								use_ptr = link_use_data [k]; 

								if (use_ptr->Start () <= tod && tod < use_ptr->End ()) {
									lanes = use_ptr->Lanes ();
									break;
								}
							}
						}
						if (lanes > 1) vol /= lanes;
					}
					if (vol > max_vol) {
						max_vol = vol;
						max_tim = j;
						max_an = anode;
						max_bn = bnode;
					}
				}
			}

			//---- add to the list ----

			if (max_vol > min_vol) {
				vol_ptr = volume;

				for (k=0; k < 100; k++, vol_ptr++) {
					if (max_vol > vol_ptr->volume) {
						if (vol_ptr->volume > 0 && k < 99) {
							memmove (vol_ptr+1, vol_ptr, (99-k) * sizeof (Volume_Data));							
							min_vol = volume [99].volume;
						}
						vol_ptr->link = link_ptr->Link ();
						vol_ptr->from = max_an;
						vol_ptr->to = max_bn;
						vol_ptr->period = max_tim;
						vol_ptr->volume = max_vol;
						break;
					}
				}
			}
		}
	}
	End_Progress ();

	//---- print the report ----

	vol_ptr = volume;

	Header_Number (type);
	New_Page ();

	for (i=0; i < 100; i++, vol_ptr++) {
		if (vol_ptr->volume == 0) break;

		//---- print the data record ----

		Print (1, "%10d%10d%10d  %12.12s%10d", 
			vol_ptr->link, vol_ptr->from, vol_ptr->to, 
			time_periods.Range_Format (vol_ptr->period),
			vol_ptr->volume);
	}
	if (i) {
		Print (2, "Number of Records in the Report = %d", i);
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Top_100_Link_Header
//---------------------------------------------------------

void LinkSum::Top_100_Link_Header (void)
{
	Print (1, "Top 100 Link Volume Report");
	Print (2, "      Link      From   To-Node   Time-of-Day    Volume");
	Print (1);
}

//---------------------------------------------------------
//	Top_100_Lane_Header
//---------------------------------------------------------

void LinkSum::Top_100_Lane_Header (void)
{
	Print (1, "Top 100 Lane Volume Report");
	Print (2, "      Link      From   To-Node   Time-of-Day    Volume");
	Print (1);
}

//---------------------------------------------------------
//	Top_100_Time_Header
//---------------------------------------------------------

void LinkSum::Top_100_Time_Header (void)
{
	Print (1, "Top 100 Period Volume Report");
	Print (2, "      Link      From   To-Node   Time-of-Day    Volume");
	Print (1);
}

/*********************************************|***********************************************

	Top 100 Link Volume Report

	      Link      From   To-Node   Time-of-Day    Volume

	dddddddddd  dddddddd  dddddddd  dd:dd..dd:dd  dddddddd

	Number of Records in the Report = dddddd

**********************************************|***********************************************/ 
