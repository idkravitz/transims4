//*********************************************************
//	Top_Ratios.cpp - Create a Top 100 Ratios Report
//*********************************************************

#include "LinkSum.hpp"

#pragma pack (push, 4)

//---------------------------------------------------------
//	Top_100_Ratios
//---------------------------------------------------------

void LinkSum::Top_100_Ratios (int type)
{
	int i, j, k, num, anode, bnode, min_ratio, ratio, tod_list, tod, lanes;
	int load, base, cap, capacity, tim, vol, time0, len;

	Link_Data *link_ptr;
	Performance_Data *perf_ptr;
	Link_Use_Data *use_ptr;
	Range_Data *range_ptr;

	//---- Top 100 Ratio Report Data ----

	typedef struct {
		int link;
		int from;
		int to;
		int period;
		int base;
		int load;
		int ratio;
	} Ratio_Data;

	Ratio_Data ratios [100], *ptr;

	Show_Message ("Creating the Top 100 Ratios Report -- Record");
	Set_Progress (10000);

	memset (ratios, '\0', 100 * sizeof (Ratio_Data));

	//---- find Top 100 Ratios ----

	min_ratio = 0;

	for (link_ptr = link_data.First (); link_ptr; link_ptr = link_data.Next ()) {
		Show_Progress ();

		if (select_flag && link_ptr->Use () == 0) continue;

		len = link_ptr->Length ();

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

			time0 = perf_ptr->Time0 ();
			if (time0 == 0) continue;

			capacity = cap = (int) (perf_ptr->Capacity () * cap_factor + 0.5);

			tod_list = perf_ptr->TOD_List ();

			lanes = perf_ptr->Thru ();
			if (lanes < 1) lanes = 1;

			num = perf_ptr->Periods ();

			//---- scan each time period ----
			
			for (j=1; j <= num; j++) {

				vol = perf_ptr->Volume (j);

				if (vol < min_volume) continue;

				switch (type) {
					case TOP_SPEED:
						tim = Round (perf_ptr->TTime (j));
						if (tim == 0) continue;
						base = (len * 10 + time0 / 2) / time0;
						load = (len * 10 + tim / 2) / tim;
						ratio = ((base - load) * 1000 + base / 2) / base;
						break;
					case TOP_TIME_RATIO:
						base = Resolve (time0 * 10);
						load = Resolve (Round (perf_ptr->TTime (j)) * 10);
						ratio = (load * 1000 + base / 2) / base;
						break;
					case TOP_VC_RATIO:
						if (tod_list > 0) {

							//---- get the time period ----

							range_ptr = time_periods [j];

							tod = Round ((range_ptr->High () + range_ptr->Low () + 1) / 2);
							cap = capacity;

							for (k = tod_list; k; k = use_ptr->TOD_List ()) {
								use_ptr = link_use_data [k]; 

								if (use_ptr->Start () <= tod && tod < use_ptr->End ()) {
									cap = capacity * use_ptr->Lanes () / lanes;
									break;
								}
							}
						}
						base = cap;
						if (base == 0) continue;
						load = vol;
						ratio = (load * 1000 + base / 2) / base;
						break;
					case TOP_TIME_CHANGE:
						if (perf_ptr->Volume2 (j) < min_volume) continue;
						base = Resolve (Round (perf_ptr->TTime2 (j)) * 10);
						if (base == 0) continue;
						load= Resolve (Round (perf_ptr->TTime (j)) * 10);
						ratio = ((load - base) * 1000 + base / 2) / base;
						break;
					case TOP_VOL_CHANGE:
						base = perf_ptr->Volume2 (j);
						if (base < min_volume) continue;
						load = vol;
						ratio = ((load - base) * 1000 + base / 2) / base;
						break;
				} 
				
				if (ratio > min_ratio) {
					ptr = ratios;

					for (k=0; k < 100; k++, ptr++) {
						if (ratio > ptr->ratio) {
							if (ptr->ratio > 0 && k < 99) {
								memmove (ptr+1, ptr, (99-k) * sizeof (Ratio_Data));							
								min_ratio = ratios [99].ratio;
							}
							ptr->link = link_ptr->Link ();
							ptr->from = anode;
							ptr->to = bnode;
							ptr->period = j;
							ptr->base = base;
							ptr->load = load;
							ptr->ratio = ratio;
							break;
						}
					}
				}
			}
		}
	}
	End_Progress ();

	//---- print the report ----

	Header_Number (type);
	New_Page ();

	ptr = ratios;

	for (i=0; i < 100; i++, ptr++) {
		if (ptr->ratio == 0) break;

		//---- print the data record ----

		Print (1, "%10ld%10ld%10ld  %12.12s   ",
			ptr->link, ptr->from, ptr->to, 
			time_periods.Range_Format (ptr->period));

		if (type == TOP_VC_RATIO) {
			Print (0, "%7d  %7d   %5.2lf", ptr->base, ptr->load, ptr->ratio / 1000.0);
		} else if (type == TOP_VOL_CHANGE) {
			Print (0, "%7d  %7d   %5.1lf", ptr->base, ptr->load, ptr->ratio / 10.0);
		} else {
			Print (0, "%7.1lf  %7.1lf   %5.1lf", ptr->base / 10.0, ptr->load / 10.0, ptr->ratio / 10.0);
		}
	}
	if (i) {
		Print (2, "Number of Records in the Report = %d", i);
	}
	Header_Number (0);
}

#pragma pack (pop)

//---------------------------------------------------------
//	Top_100_Speed_Header
//---------------------------------------------------------

void LinkSum::Top_100_Speed_Header (void)
{
	Print (1, "Top 100 Speed Reductions Report");
	Print (2, "      Link      From   To-Node   Time-of-Day      Base   Loaded  Percent");
	Print (1);
}

//---------------------------------------------------------
//	Top_100_Travel_Time_Header
//---------------------------------------------------------

void LinkSum::Top_100_Travel_Time_Header (void)
{
	Print (1, "Top 100 Travel Time Ratios");
	Print (2, "      Link      From   To-Node   Time-of-Day      Base   Loaded  Percent");
	Print (1);
}

//---------------------------------------------------------
//	Top_100_VC_Header
//---------------------------------------------------------

void LinkSum::Top_100_VC_Header (void)
{
	Print (1, "Top 100 Volume Capacity Ratios");
	Print (2, "      Link      From   To-Node   Time-of-Day  Capacity   Volume   Ratio");
	Print (1);
}

//---------------------------------------------------------
//	Top_100_Time_Change_Header
//---------------------------------------------------------

void LinkSum::Top_100_Time_Change_Header (void)
{
	Print (1, "Top 100 Travel Time Changes");
	Print (2, "      Link      From   To-Node   Time-of-Day  Previous  Current  Percent");
	Print (1);
}

//---------------------------------------------------------
//	Top_100_Volume_Change_Header
//---------------------------------------------------------

void LinkSum::Top_100_Volume_Change_Header (void)
{
	Print (1, "Top 100 Volume Changes");
	Print (2, "      Link      From   To-Node   Time-of-Day  Previous  Current  Percent");
	Print (1);
}

/*********************************************|***********************************************

	Top 100 Speed Reductions Report

	      Link      From   To-Node   Time-of-Day      Base   Loaded  Percent

	dddddddddd  dddddddd  dddddddd  dd:dd..dd:dd   ddddddd  ddddddd   ddddd

	Number of Records in the Report = dddddd

**********************************************|***********************************************/ 
