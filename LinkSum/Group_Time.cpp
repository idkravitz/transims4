//*********************************************************
//	Group_Time.cpp - Create a Link Group Travel Time Report
//*********************************************************

#include "LinkSum.hpp"

//---------------------------------------------------------
//	Group_Time_Report
//---------------------------------------------------------

void LinkSum::Group_Time_Report (void)
{
	int i, j, num, lnk, dir, link, period, hour, minute, second, time, time0, ttime;
	int *ptr, in_link, in_link_dir, out_link_dir, base_time;
    char *label, *undefined = "undefined";
	bool time_flag, connect_flag;

	Link_Data *link_ptr;
	Performance_Data *perf_ptr;
	Connect_Time *connect_ptr;
	Range_Data *range_ptr;
	Integer_List *group;
	
	Data_Array group_data (sizeof (int));

	Show_Message ("Creating the Travel Time Report -- Record");
	Set_Progress (10000);

	//---- allocate work space ----

	if (!group_data.Num_Records (num_inc)) {
		Error ("Insufficient Memory for Link Group Report");
	}
	connect_flag = (connect_time.Num_Records () > 0);
	connect_ptr = NULL;

	//---- calculate group travel time ----

	num = link_equiv.Num_Groups ();

	Header_Number (TRAVEL_TIME);

	if (!Break_Check (num * num_inc + 7)) {
		Print (1);
		Group_Time_Header ();
	}
	num = link_equiv.Max_Group ();

	for (i=1; i <= num; i++) {

		group = link_equiv.Group_List (i);
		if (group == NULL) continue;

		label = link_equiv.Group_Label (i);

		in_link_dir = in_link = base_time = 0;
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
			
			time_flag = (perf_ptr->Periods () != 0);
			time0 = perf_ptr->Time0 ();
			time = 0;
			base_time += time0;

			//---- get the turn delay data ----

			if (connect_flag) {
				out_link_dir = perf_ptr->Link_Dir ();

				if (in_link_dir > 0) {
					connect_ptr = connect_time.Get (in_link_dir, out_link_dir);

					if (connect_ptr == NULL) {
						Warning ("A Connection between %d and %d was Not Found", in_link, link);
					} else if (connect_ptr->Periods () == 0) {
						connect_ptr = NULL;
					}
				} else {
					connect_ptr = NULL;
				}
				in_link_dir = out_link_dir;
				in_link = link;
			}

			//---- process each time period ----

			for (j=1; j <= num_inc; j++) {
				ptr = (int *) group_data [j];
				range_ptr = time_periods [j];

				second = range_ptr->Low () + Resolve (*ptr);
				period = performance_data.Period (second + 1);

				if (connect_ptr != NULL) {
					ttime = connect_ptr->TTime (period);
					*ptr += ttime;

					period = performance_data.Period (second + Resolve (ttime) + 1);
				}
				if (time_flag) {
					time = Round (perf_ptr->TTime (period));
				}
				*ptr += ((time > 0) ? time : time0);
			}
		}

		//---- print the link group data ----

		for (j=1; j <= num_inc; j++) {
			Show_Progress ();

			//---- print the start time ----

			range_ptr = time_periods [j];
			time = range_ptr->Low ();

			hour = time / 3600;
			second = hour * 3600;
			minute = (time - second) / 60;
			second = time - (minute * 60) - second; 

			Print (1, "%4d  %-40.40s  %2d:%02d:%02d", i, label, hour, minute, second);

			//---- print the base time ----

			time = Resolve (base_time);

			hour = time / 3600;
			second = hour * 3600;
			minute = (time - second) / 60;
			second = time - (minute * 60) - second; 

			Print (0, "    %2d:%02d:%02d", hour, minute, second);

			//---- print the travel time ----

			ptr = (int *) group_data [j];
			time = Resolve (*ptr);

			hour = time / 3600;
			second = hour * 3600;
			minute = (time - second) / 60;
			second = time - (minute * 60) - second; 

			Print (0, "    %2d:%02d:%02d", hour, minute, second);
		}
		Print (1);
	}
	End_Progress ();

	if (Progress_Count ()) {
		Print (2, "Number of Records in the Report = %d", Progress_Count ());
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Group_Time_Header
//---------------------------------------------------------

void LinkSum::Group_Time_Header (void)
{
	Print (1, "Link Group Travel Time Report");
	Print (2, "Group Description                              Start Time  Base Time  Travel Time");
	Print (1);
}

/*********************************************|***********************************************

	Link Group Travel Time Report

	Group Description                              Start Time  Base Time  Travel Time

	dddd  ssssssssssssssssssssssssss40ssssssssssss  dd:dd:dd    dd:dd:dd    dd:dd:dd

	Number of Records in the Report = dddddd

**********************************************|***********************************************/ 
