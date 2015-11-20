//*********************************************************
//	Group_Sum.cpp - Create a Line Group Summary Report
//*********************************************************

#include "LineSum.hpp"

//---------------------------------------------------------
//	Line_Group_Report
//---------------------------------------------------------

void LineSum::Line_Group_Report (void)
{
	int i, stop, stops, run, runs, per, num, nper, time, route;
	int on, off, ride, total, max_ride, *total_rider, *max_load;

	Rider_Data *rider_ptr;
	Integer_List *group;

	Show_Message ("Creating a Line Group Summary Report -- Record");
	Set_Progress (100);

	//---- initialize the selection sets ----

	set_ptr = select_set.First ();
	if (set_ptr == NULL) return;

	Header_Number (GROUP_SUM);

	num = line_equiv.Num_Groups ();

	if (!Break_Check (num + 8)) {
		Print (1);
		Line_Group_Header ();
	}
	num = line_equiv.Max_Group ();
	
	nper = set_ptr->Num_Periods ();

	total_rider = new int [nper];
	max_load = new int [nper];

	if (total_rider == NULL || max_load == NULL) {
		Error ("Insufficient Space for %d Time Periods", nper);
	}

	//---- process each line gorup ----

	for (i=1; i <= num; i++) {

		group = line_equiv.Group_List (i);
		if (group == NULL) continue;

		memset (total_rider, '\0', nper * sizeof (int));
		memset (max_load, '\0', nper * sizeof (int));

		//---- process each route ----

		for (route = group->First (); route; route = group->Next ()) {
			Show_Progress ();

			rider_ptr = rider_data.Get (route);
			if (rider_ptr == NULL) continue;

			stops = rider_ptr->Stops ();
			runs = rider_ptr->Runs ();

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
				per = set_ptr->Time_Period (Resolve (time));
				if (per == 0) continue;
				per--;

				//---- sum the line ridership ----
			
				total = max_ride = ride = 0;

				for (stop=1; stop <= stops; stop++) {
					on = rider_ptr->Board (run, stop);
					off = rider_ptr->Alight (run, stop);
					ride += on - off;
					total += on;
					if (ride > max_ride) max_ride = ride;
				}
				total_rider [per] += total;
				if (max_ride > max_load [per]) max_load [per] = max_ride;
			}
		}

		//---- print the line label ----

		Print (1, "%-24.24s", line_equiv.Group_Label (i));

		total = max_ride = 0;

		for (per=0; per < nper; per++) {
			Print (0, "  %7d %7d", total_rider [per], max_load [per]);

			total += total_rider [per];
			if (max_ride < max_load [per]) max_ride = max_load [per];
		}
		if (nper > 1) {
			Print (0, "  %7d %7d", total, max_ride);
		}
	}
	End_Progress ();

	delete [] total_rider;
	delete [] max_load;

	Header_Number (0);
}

//---------------------------------------------------------
//	Line_Group_Header
//---------------------------------------------------------

void LineSum::Line_Group_Header (void)
{
	int i, num;
	
	num = set_ptr->Num_Periods ();

	Print (1, "Line Group Summary Report");
	if (set_ptr->Select_Label () != NULL) Print (0, " (%s)", set_ptr->Select_Label ());

	Print (2, "%24c", BLANK);

	for (i=1; i <= num; i++) {
		Print (0, "  --%12.12s-", set_ptr->Time_Format (i));
	}
	if (num > 1) {
		Print (0, "  ---- Total ----");
		num++;
	}
	Print (1, "Line Group              ");

	for (i=1; i <= num; i++) {
		Print (0, "   Riders MaxLoad");
	}
	Print (1);
}

/*********************************************|***********************************************

	Line Group Summary Report (%s)

	                          --dd:dd..dd:dd-  --dd:dd..dd:dd-  ---- Total ----
	Line Group                 Riders MaxLoad   Riders MaxLoad   Riders MaxLoad

	ssssssss24ssssssssssssss  ddddddd ddddddd  ddddddd ddddddd  ddddddd ddddddd


**********************************************|***********************************************/ 
