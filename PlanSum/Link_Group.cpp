//*********************************************************
//	Link_Group.cpp - Create a Link Group Report
//*********************************************************

#include "PlanSum.hpp"

#include "Data_Array.hpp"

//---------------------------------------------------------
//	Link_Group
//---------------------------------------------------------

void PlanSum::Link_Group (double min_vc)
{
	int i, j, k, num, dir, lnk, link, volume, cap, tod_cap, lanes, tod, list;
	double vc;
    char *label;
	bool flag;

	Link_Data *link_ptr;
	TTime_Data *ttime_ptr;
	Link_Use_Data *use_ptr;
	Range_Data *range_ptr;
	Integer_List *group;

	typedef struct {
		int links;
		int capacity;
		int volume;
	} Group_Data;

	Group_Data *group_ptr;
	Data_Array group_data (sizeof (Group_Data));

	group_data.Num_Records (num_inc);

	Show_Message ("Writing the Link Group Report -- Record");
	Set_Progress (1000);

	minimum_vc = min_vc;

	//---- find V/C ratios for each link group ----

	num = link_equiv.Num_Groups ();

	Header_Number (LINK_GROUP);

	if (!Break_Check (num + 7)) {
		Print (1);
		Link_Group_Header ();
	}
	num = link_equiv.Max_Group ();

	for (i=1; i <= num; i++) {

		group = link_equiv.Group_List (i);
		if (group == NULL) continue;

		label = link_equiv.Group_Label (i);

		flag = false;
		group_data.Zero_Fill ();

		//---- process each link in the link group ----

		for (link = group->First (); link != 0; link = group->Next ()) {

			lnk = abs (link);
			link_ptr = link_data.Get (lnk);

			if (link_ptr == NULL) continue;

			if (link < 0) {
				dir = link_ptr->BA_Dir ();
			} else {
				dir = link_ptr->AB_Dir ();
			}
			if (dir == 0) continue;
			
			//---- get the directional data ----

			ttime_ptr = ttime_data [dir];

			if (ttime_ptr->Periods () == 0) continue;

			cap = (int) (ttime_ptr->Capacity () * cap_factor + 0.5);
			if (cap <= 0) continue;

			lanes = ttime_ptr->Thru ();
			if (lanes < 1) lanes = 1;

			list = ttime_ptr->TOD_List ();

			//---- process each time period ----
			
			for (j=1; j <= num_inc; j++) {

				volume = ttime_ptr->Data (j, layer);

				//---- time-of-day capacity ----

				tod_cap = cap;

				if (list > 0) {

					//---- get the time period ----

					range_ptr = time_periods [j];
					tod = Round ((range_ptr->High () + range_ptr->Low () + 1) / 2);

					for (k = list; k; k = use_ptr->TOD_List ()) {
						use_ptr = link_use_data [k]; 

						if (use_ptr->Start () <= tod && tod < use_ptr->End ()) {
							tod_cap = (tod_cap * use_ptr->Lanes () + lanes / 2) / lanes;
							break;
						}
					}
				}
				if (tod_cap > 0) {
					group_ptr = (Group_Data *) group_data [j];

					flag = true;
					group_ptr->links++;
					group_ptr->volume += volume;
					group_ptr->capacity += tod_cap;
				}
			}
		}

		if (!flag) continue;

		//---- print the link group data ----

		flag = true;

		for (j=1; j <= num_inc; j++) {
			group_ptr = (Group_Data *) group_data [j];

			if (group_ptr->capacity <= 0) continue;

			vc = (double) group_ptr->volume / group_ptr->capacity;

			if (vc > min_vc) {
				Show_Progress ();

				//---- print the data record ----

				if (flag) {
					flag = false;
					Print (1, "%s", label);
				}
				Print (1, "%20c%5d   %7d  %12.12s  %7d   %6.2lf",
					BLANK, group_ptr->links, group_ptr->capacity, 
					time_periods.Range_Format (j), 
					group_ptr->volume, vc);
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
//	Link_Group_Header
//---------------------------------------------------------

void PlanSum::Link_Group_Header (void)
{
	Print (1, "Link Group V/C Ratios Greater Than %.2lf", minimum_vc);
	Print (2, "Link Group          Links  Capacity   Time-of-Day   Volume  V/C Ratio");
	Print (1);
}

/*********************************************|***********************************************

	Link Group V/C Ratios Greater Than ff.ff

	Link Group          Links  Capacity   Time-of-Day   Volume  V/C Ratio

	ssssssssssssssssssssssssssssssssssssssss	
	                    ddddd   ddddddd  dd:dd..dd:dd  ddddddd    ff.ff  
	                            ddddddd  dd:dd..dd:dd  ddddddd    ff.ff

	Number of Records in the Report = dddddd

**********************************************|***********************************************/ 
