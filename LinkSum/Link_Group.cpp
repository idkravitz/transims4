//*********************************************************
//	Link_Group.cpp - Create a Link Group Report
//*********************************************************

#include "LinkSum.hpp"

//---------------------------------------------------------
//	Link_Group
//---------------------------------------------------------

void LinkSum::Link_Group (double min_volume)
{
	int i, j, num, lnk, dir, link;
	int *ptr, group_num;
    char *label, *undefined = "undefined";

	Link_Data *link_ptr;
	Performance_Data *perf_ptr;
	Integer_List *group;

	Data_Array group_data (sizeof (int));

	Show_Message ("Creating the Link Group Report -- Record");
	Set_Progress (10000);

	header_value = (int) (min_volume + 0.5);

	//---- allocate work space ----

	if (!group_data.Num_Records (num_inc)) {
		Error ("Insufficient Memory for Link Group Report");
	}

	//---- find events for each link group ----

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

		group_num = 0;
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

			perf_ptr = performance_data [dir];

			if (perf_ptr->Periods () == 0) continue;

			group_num++;

			//---- process each time period ----
			
			for (j=1; j <= num_inc; j++) {
				ptr = (int *) group_data [j];
				*ptr += perf_ptr->Volume (j);
			}

		}
		if (group_num == 0) continue;

		//---- print the link group data ----

		for (j=1; j <= num_inc; j++) {
			ptr = (int *) group_data [j];

			if (*ptr < header_value) continue;

			Show_Progress ();

			//---- print the data record ----

			Print (1, "%4d  %-40.40s  %5d  %12.12s%9d",
				i, label, group_num, time_periods.Range_Format (j), *ptr);
		}
	}
	End_Progress ();

	if (Progress_Count ()) {
		Print (2, "Number of Records in the Report = %d", Progress_Count ());
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Link_Group_Header
//---------------------------------------------------------

void LinkSum::Link_Group_Header (void)
{
	Print (1, "Link Group Volume Greater Than %.0lf", header_value);
	Print (2, "Group Description                               Links   Time-of-Day   Volume");
	Print (1);
}

/*********************************************|***********************************************

	Link Group Volume Greater Than dddddddd

	Group Description                               Links   Time-of-Day   Volume

	dddd  ssssssssssssssssssssssssss40ssssssssssss  ddddd  dd:dd..dd:dd  ddddddd

	Number of Records in the Report = dddddd

**********************************************|***********************************************/ 
