//*********************************************************
//	Link_Group.cpp - Create a Transit Link Group Report
//*********************************************************

#include "LineSum.hpp"

//---------------------------------------------------------
//	Link_Group_Report
//---------------------------------------------------------

void LineSum::Link_Group_Report (void)
{
	int i, j, j1, stop, stops, run, runs, per, num, nper, time, link, dir, link_dir, lnk, ttime;
	int enter, exit, time1, stop1, ride, link_ride;
	double factor;
	bool first;

	Driver_Data *driver_ptr;
	Rider_Data *rider_ptr;
	Stop_Data *stop_ptr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Integer_List *group;

	typedef struct {
		bool first;
		int lines;
		int runs;
		int length;
		int ttime;
		int riders;
	} Period_Data;

	Period_Data *period_data;

	Show_Message ("Creating a Transit Link Group Summary -- Record");
	Set_Progress (100);

	//---- initialize the selection sets ----

	set_ptr = select_set.First ();
	if (set_ptr == NULL) return;

	Header_Number (LINK_GROUP);

	nper = set_ptr->Num_Periods ();
	num = link_equiv.Num_Groups ();

	period_data = new Period_Data [nper+1];
	if (period_data == NULL) {
		Error ("Insuffiecent Memory for %d Summary Period", nper);
	}
	if (!Break_Check (num * nper + 8)) {
		Print (1);
		Link_Group_Header ();
	}
	num = link_equiv.Max_Group ();

	//---- process each link gorup ----

	for (i=1; i <= num; i++) {

		group = link_equiv.Group_List (i);
		if (group == NULL) continue;

		memset (period_data, '\0', (nper + 1) * sizeof (Period_Data));

		//---- process each route ----

		for (rider_ptr = rider_data.First (); rider_ptr; rider_ptr = rider_data.Next ()) {
			Show_Progress ();

			//---- check the selection criteria ----

			if (!set_ptr->Select_Modes (rider_ptr->Mode ())) continue;
			if (!set_ptr->Select_Routes (rider_ptr->Route ())) continue;

			//---- scan for links in the link group ----

			driver_ptr = driver_data [rider_ptr->Driver ()];
			first = false;

			for (j=1; j <= driver_ptr->Links (); j++) {
				link = driver_ptr->Link (j);
				dir = driver_ptr->Dir (j);

				for (lnk = group->First (); lnk; lnk = group->Next ()) {
					if (lnk < 0) {
						if (dir != 1 || link != abs (lnk)) continue;
					} else {
						if (dir != 0 || link != lnk) continue;
					}
					first = true;
					break;
				}
				if (first) break;
			}
			if (!first) continue;

			//---- retrieve the first stop ----

			stops = rider_ptr->Stops ();
			runs = rider_ptr->Runs ();

			stop1 = rider_ptr->Stop (1);
			stop_ptr = stop_data.Get (stop1);
			if (stop_ptr == NULL) continue;

			//---- find the link and travel time to the first stop ----

			time1 = j1 = 0;

			for (j=1; j <= driver_ptr->Links (); j++) {
				link_dir = driver_ptr->Link_Dir (j);

				if (stop_ptr->Link_Dir () == link_dir) {
					j1 = j;
					link = driver_ptr->Link (j);
					dir = driver_ptr->Dir (j);

					link_ptr = link_data.Get (link);
					if (link_ptr == NULL) continue;

					if (dir == 0) {
						dir_ptr = dir_data [link_ptr->AB_Dir ()];
						factor = (double) stop_ptr->Offset () / link_ptr->Length ();
					} else {
						dir_ptr = dir_data [link_ptr->BA_Dir ()];
						factor = ((double) (link_ptr->Length () - stop_ptr->Offset ())) / link_ptr->Length ();
					}
					time1 = (int) (dir_ptr->Time0 () * factor + 0.5);
					break;
				}
			}

			//---- reset the line flags ----

			for (per=1; per <= nper; per++) {
				period_data [per].first = true;
			}

			//---- estimate the travel time to each link for each run ----

			for (run=1; run <= runs; run++) {

				//---- initialize the time entering first link ----

				exit = rider_ptr->Schedule (run, 1) - time1;

				//---- set the entering and exiting time for each link ----

				stop = 1;
				stop_ptr = stop_data.Get (stop1);
				ride = link_ride = 0;

				for (j=j1; j <= driver_ptr->Links (); j++) {
					link = driver_ptr->Link (j);
					dir = driver_ptr->Dir (j);
					link_dir = driver_ptr->Link_Dir (j);

					link_ptr = link_data.Get (link);
					if (link_ptr == NULL) continue;

					if (dir == 0) {
						dir_ptr = dir_data [link_ptr->AB_Dir ()];
					} else {
						dir_ptr = dir_data [link_ptr->BA_Dir ()];
					}
					ttime = dir_ptr->Time0 ();
					
					enter = exit;
					exit += ttime;
					link_ride = ride;
			
					//---- check for stops ----

					while (stop_ptr->Link_Dir () == link_dir) {
						if (dir == 0) {
							factor = ((double) (link_ptr->Length () - stop_ptr->Offset ())) / link_ptr->Length ();
						} else {
							factor = (double) stop_ptr->Offset () / link_ptr->Length ();
						}
						time = (int) (ttime * factor + 0.5);
						exit = rider_ptr->Schedule (run, stop) + time;
						ride += rider_ptr->Board (run, stop) - rider_ptr->Alight (run, stop);
						if (factor < 0.80) link_ride = ride;

						if (++stop > stops) break;

						stop_ptr = stop_data.Get (rider_ptr->Stop (stop));
						if (stop_ptr == NULL) break;
					}

					//---- check the time period ----

					switch (set_ptr->Time_Method ()) {
						case RUN_START:
						case SCHED_START:
							time = enter;
							break;
						case RUN_END:
						case SCHED_END:
							time = exit;
							break;
						case RUN_MID:
						case SCHED_MID:
							time = (enter + exit) / 2;
							break;
					}
					per = set_ptr->Time_Period (Resolve (time));
					if (per == 0) continue;

					//---- check for links on the link group ----

					for (lnk = group->First (); lnk; lnk = group->Next ()) {
						if (lnk < 0) {
							if (dir != 1 || link != abs (lnk)) continue;
						} else {
							if (dir != 0 || link != lnk) continue;
						}
						if (period_data [per].first) {
							period_data [per].lines++;
							period_data [per].first = false;
						}
						period_data [per].runs++;
						period_data [per].length += link_ptr->Length ();
						period_data [per].ttime += exit - enter;
						period_data [per].riders += link_ride;
					}
				}
			}
		}

		//---- print the link group label ----

		Print (1, "%3d  %-36.36s %4d", i, link_equiv.Group_Label (i), group->Num_Records ());
		first = true;

		for (per=1; per <= nper; per++) {
			if (period_data [per].lines > 0) {
				if (first) {
					first = false;
				} else {
					Print (1, "%46c", BLANK);
				}
				Print (0, "  %12.12s  %4d  %5d  %6.2lf %7d", set_ptr->Time_Format (per), 
					period_data [per].lines, period_data [per].runs,
					((double) period_data [per].length / period_data [per].ttime),
					period_data [per].riders);
			}
		}
	}
	End_Progress ();

	Header_Number (0);

	delete [] period_data;
}

//---------------------------------------------------------
//	Link_Group_Header
//---------------------------------------------------------

void LineSum::Link_Group_Header (void)
{

	Print (1, "Transit Link Group Summary");
	Print (2, "Link Group%32cLinks  Time-of-Day  Lines  Runs   Speed  Riders", BLANK);
	Print (1);
}

/*********************************************|***********************************************

	Transit Link Group Summary

	Link Group                                Links  Time-of-Day  Lines  Runs   Speed  Riders

	ddd  ssssssssssssssssssss36ssssssssssssss dddd  00:00..00:00  dddd  ddddd  fff.ff ddddddd

**********************************************|***********************************************/ 
