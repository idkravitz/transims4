//*********************************************************
//	VC_Ratio.cpp - Create a V/C Ratio Report
//*********************************************************

#include "PlanSum.hpp"

//---------------------------------------------------------
//	VC_Ratio
//---------------------------------------------------------

void PlanSum::VC_Ratio (double min_vc)
{
	int i, j, k, dir, from, to, volume, cap, tod_cap, lanes, tod, list;
	double vc;

	Link_Data *link_ptr;
	TTime_Data *ttime_ptr;
	Link_Use_Data *use_ptr;
	Range_Data *range_ptr;

	Show_Message ("Writing a V/C Ratio Report -- Record");
	Set_Progress (1000);

	//---- find V/C ratios greater than min_vc ----

	minimum_vc = min_vc;

	Header_Number (VC_RATIO);
	New_Page ();

	for (link_ptr = link_data.First_Key (); link_ptr != NULL; link_ptr = link_data.Next_Key ()) {

		//---- check the directional v/c ratios ----

		for (i=0; i < 2; i++) {

			dir = ((i) ? link_ptr->BA_Dir () : link_ptr->AB_Dir ());

			if (dir == 0) continue;

			ttime_ptr = ttime_data [dir];

			if (ttime_ptr->Periods () == 0) continue;

			cap = (int) (ttime_ptr->Capacity () * cap_factor + 0.5);
			if (cap <= 0) continue;

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

				volume = ttime_ptr->Data (j, layer);

				if (volume == 0) continue;

				//---- time-of-day capacity ----

				tod_cap = cap;
				range_ptr = time_periods [j];

				if (list > 0) {

					//---- get the time period ----

					tod = Round ((range_ptr->High () + range_ptr->Low () + 1) / 2);

					for (k = list; k; k = use_ptr->TOD_List ()) {
						use_ptr = link_use_data [k]; 

						if (use_ptr->Start () <= tod && tod < use_ptr->End ()) {
							tod_cap = (tod_cap * use_ptr->Lanes () + lanes / 2) / lanes;
							if (tod_cap == 0) tod_cap = cap / 2;
							break;
						}
					}
				}

				vc = (double) volume / tod_cap;

				if (vc > min_vc) {
					Show_Progress ();

					//---- print the data record ----

					Print (1, "%8d  %8d  %8d   %7d  %12.12s  %7d   %6.2lf",
						link_ptr->Link (), from, to, tod_cap,
						time_periods.Range_Format (j), volume, vc);
				}
			}
		}
	}
	End_Progress ();

	if (Progress_Count () > 0) {
		Print (2, "Number of Records in the Report = %d", Progress_Count ());
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	VC_Ratio_Header
//---------------------------------------------------------

void PlanSum::VC_Ratio_Header (void)
{
	Print (1, "All V/C Ratios Greater Than %.2lf", minimum_vc);
	Print (2, "    Link      From   To-Node  Capacity   Time-of-Day   Volume  V/C Ratio");
	Print (1);
}

/*********************************************|***********************************************

	All V/C Ratios Greater Than ff.ff

 	    Link      From   To-Node  Capacity   Time-of-Day   Volume  V/C Ratio

	dddddddd  dddddddd  dddddddd   ddddddd  dd:dd..dd:dd  ddddddd    ff.ff

	Number of Records in the Report = dddddd

**********************************************|***********************************************/ 
