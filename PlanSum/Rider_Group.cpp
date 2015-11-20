//*********************************************************
//	Rider_Group.cpp - Create a Transit Link Group Report
//*********************************************************

#include "PlanSum.hpp"

#include "Data_Array.hpp"

//---------------------------------------------------------
//	Rider_Group
//---------------------------------------------------------

void PlanSum::Rider_Group (void)
{
	int i, j, k, max_j, max_k, num, dir, length, link, riders, run, stop, previous_period, period;
    char *label;
	bool flag;

	Link_Data *link_ptr;
	Integer_List *group;
	Driver_Data *driver_ptr;
	Stop_Data *stop_ptr;
	Rider_Data *rider_ptr;

	typedef struct {
		int lines;
		int runs;
		int riders;
	} Group_Data;

	Group_Data *group_ptr;
	Data_Array group_data (sizeof (Group_Data));

	group_data.Num_Records (num_inc);

	Show_Message ("Writing Transit Link Groups -- Record");
	Set_Progress (1000);

	//---- find each link group ----

	num = link_equiv.Num_Groups ();

	Header_Number (RIDER_GROUP);

	if (!Break_Check (num + 7)) {
		Print (1);
		Rider_Header ();
	}
	num = link_equiv.Max_Group ();

	for (i=1; i <= num; i++) {

		group = link_equiv.Group_List (i);
		if (group == NULL) continue;

		label = link_equiv.Group_Label (i);

		flag = false;
		group_data.Zero_Fill ();

		//---- write data for each line ----

		for (rider_ptr = rider_data.First_Key (); rider_ptr; rider_ptr = rider_data.Next_Key ()) {
			Show_Progress ();

			driver_ptr = driver_data [rider_ptr->Driver ()];

			//---- process each link in the link group ----

			for (link = group->First (); link != 0; link = group->Next ()) {
				if (link < 0) {
					link = -link;
					dir = 1;
				} else {
					dir = 0;
				}

				//---- search for the link on the route ----

				for (j=1; j <= driver_ptr->Links (); j++) {
					if (link == driver_ptr->Link (j) && dir == driver_ptr->Dir (j)) break;
				}
				if (j > driver_ptr->Links ()) continue;

				max_j = j;

				//---- get the link length ----

				link_ptr = link_data.Get (link);

				if (link_ptr == NULL) {
					Write (1, "Route %d Link %d was Not Found", rider_ptr->Route (), link);
					continue;
				}
				length = link_ptr->Length () / 2;

				//---- find the last stop before the center of the link ----

				j = 1;
				max_k = 0;

				for (k=1; k <= rider_ptr->Stops (); k++) {
					stop = rider_ptr->Stop (k);

					stop_ptr = stop_data.Get (stop);

					if (stop_ptr == NULL) {
						Write (1, "Route %d Stop %d was Not Found", rider_ptr->Route (), stop);
						continue;
					}

					for (; j <= max_j; j++) {
						if (stop_ptr->Link_Dir () == driver_ptr->Link_Dir (j)) break;
					}
					if (j < max_j) continue;
					if (j == max_j) {
						if (dir == 1) {
							if (stop_ptr->Offset () > length) continue;
						} else {
							if (stop_ptr->Offset () < length) continue;
						}
					}
					max_k = k - 1;
					break;
				}
				if (max_k == 0) continue;

				//---- add the rider for each run ----

				previous_period = 0;

				for (run=1; run <= rider_ptr->Runs (); run++) {

					//---- find the time period ----

					period = time_periods.Period (Resolve (rider_ptr->Schedule (run, max_k)));
					if (period == 0) continue;

					group_ptr = (Group_Data *) group_data [period];

					//---- calculate the riders ----

					for (k=1, riders=0; k <= max_k; k++) {
						riders += rider_ptr->Board (run, k) - rider_ptr->Alight (run, k);
					}

					//---- add the riders to the summary ----

					if (period != previous_period) {
						previous_period = period;
						group_ptr->lines++;
					}
					group_ptr->runs++;
					group_ptr->riders += riders;
					flag = true;
				}
			}
		}
		if (!flag) continue;

		//---- print the link group data ----

		k = 0;

		for (j=1; j <= num_inc; j++) {
			group_ptr = (Group_Data *) group_data [j];
			if (group_ptr->lines == 0) continue;

			//---- print the data record ----

			if (k == 0) {
				Print (1, "%3d %-40.40s", i, label);
			} else {
				Print (1, "%44c", BLANK);
			}
			k++;
			Print (0, "%5d %6d  %12.12s  %7d",
				group_ptr->lines, group_ptr->runs,  
				time_periods.Range_Format (j), group_ptr->riders);
		}
		if (k > 1) Print (1);
	}
	End_Progress ();

	Header_Number (0);
}

//---------------------------------------------------------
//	Rider_Header
//---------------------------------------------------------

void PlanSum::Rider_Header (void)
{
	Print (1, "Transit Link Group Summary");
	Print (2, "Link Group%35cLines  Runs   Time-of-Day   Riders", BLANK);
	Print (1);
}

/*********************************************|***********************************************

	Transit Link Group Summary

	Link Group                                   Lines  Runs  Time-of-Day    Riders

	ddd ssssssssssssssssssssssssssssssssssssssss dddd  ddddd  dd:dd..dd:dd  ddddddd  
	                                             dddd  ddddd  dd:dd..dd:dd  ddddddd

**********************************************|***********************************************/ 
