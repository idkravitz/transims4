//*********************************************************
//	Group_Perf.cpp - Create a Line Group Performance Report
//*********************************************************

#include "LineSum.hpp"

//---------------------------------------------------------
//	Group_Performance_Report
//---------------------------------------------------------

void LineSum::Group_Performance_Report (void)
{
	int i, stop, stops, run, runs, nper, per, num, time, on, off, ride, distance, route;

	Rider_Data *rider_ptr;
	Integer_List *group;

	typedef struct {
		double veh_miles;
		double veh_hours;
		double pass_miles;
		double pass_hours;
		int board;
		int alight;
		int vehicles;
		int passengers;
	} Period_Data;

	Period_Data *period_data, *period_ptr;

	Show_Message ("Creating a Line Group Performance Report -- Record");
	Set_Progress (100);

	//---- initialize the selection sets ----

	set_ptr = select_set.First ();
	if (set_ptr == NULL) return;

	Header_Number (GROUP_PERF);

	//---- initialize the report data ----

	num = line_equiv.Max_Group ();

	nper = set_ptr->Num_Periods () + 1;

	period_data = new Period_Data [nper];

	if (period_data == NULL) {
		Error ("Insufficient Space for %d Time Periods", nper);
	}

	//---- process each line gorup ----

	for (i=1; i <= num; i++) {

		group = line_equiv.Group_List (i);
		if (group == NULL) continue;

		group_num = i;

		if (!Break_Check (nper + 6)) {
			Print (1);
			Group_Performance_Header ();
		}
		memset (period_data, '\0', nper * sizeof (Period_Data));

		//---- process each route ----

		for (route = group->First (); route; route = group->Next ()) {
			Show_Progress ();

			rider_ptr = rider_data.Get (route);
			if (rider_ptr == NULL) continue;

			stops = rider_ptr->Stops ();
			runs = rider_ptr->Runs ();

			//---- process each run ----

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
				period_ptr = period_data + per;

				//---- sum the line ridership ----

				ride = 0;

				for (stop=1; stop <= stops; stop++) {
					on = rider_ptr->Board (run, stop);
					off = rider_ptr->Alight (run, stop);
					time = rider_ptr->Time (run, stop);
					distance = rider_ptr->Distance (stop);
					ride += on - off;

					period_ptr->passengers += on;
					period_ptr->board += on;
					period_ptr->alight += off;

					if (stop < stops) {
						time = rider_ptr->Time (run, stop+1) - time;
						distance = rider_ptr->Distance (stop+1) - distance;

						period_ptr->pass_hours += ride * time;
						period_ptr->veh_hours += time;

						period_ptr->pass_miles += ride * distance;
						period_ptr->veh_miles += distance;
					}
				}
				period_ptr->vehicles++;
			}
		}

		//---- print the line group data ----

		for (per=1; per < nper; per++) {

			period_ptr = period_data + per;

			period_ptr->veh_miles /= Round (MILETOMETER);
			period_ptr->pass_miles /= Round (MILETOMETER);
			period_ptr->veh_hours /= 3600.0;
			period_ptr->pass_hours /= 3600.0;

			Print (1, "%-12.12s%10.1lf%10.1lf%8d%8d%10.1lf%10.1lf", 
				set_ptr->Time_Format (per), period_ptr->veh_miles, period_ptr->veh_hours, 
				period_ptr->board, period_ptr->alight, period_ptr->pass_miles, period_ptr->pass_hours);

			//---- add to the total ----

			period_data->veh_miles += period_ptr->veh_miles;
			period_data->veh_hours += period_ptr->veh_hours;
			period_data->pass_miles += period_ptr->pass_miles;
			period_data->pass_hours += period_ptr->pass_hours;
			period_data->board += period_ptr->board;
			period_data->alight += period_ptr->alight;
			period_data->vehicles += period_ptr->vehicles;
			period_data->passengers += period_ptr->passengers;

			if (period_ptr->veh_miles <= 0.0) period_ptr->veh_miles = 1.0;
			if (period_ptr->veh_hours <= 0.0) period_ptr->veh_hours = 1.0;
			if (period_ptr->vehicles <= 0) period_ptr->vehicles = 1;

			Print (0, " %7.1lf%7.1lf%7.1lf", period_ptr->passengers / period_ptr->veh_miles, 
				period_ptr->passengers / period_ptr->veh_hours, 
				(double) period_ptr->passengers / period_ptr->vehicles);
		}

		if (nper > 2) {
			Print (2, "Total       %10.1lf%10.1lf%8d%8d%10.1lf%10.1lf", period_data->veh_miles, 
				period_data->veh_hours, period_data->board, period_data->alight, 
				period_data->pass_miles, period_data->pass_hours);

			if (period_data->veh_miles <= 0.0) period_data->veh_miles = 1.0;
			if (period_data->veh_hours <= 0.0) period_data->veh_hours = 1.0;
			if (period_data->vehicles <= 0) period_data->vehicles = 1;

			Print (0, " %7.1lf%7.1lf%7.1lf", period_data->passengers / period_data->veh_miles, 
				period_data->passengers / period_data->veh_hours, 
				(double) period_data->passengers / period_data->vehicles);
		}

	}
	End_Progress ();

	Header_Number (0);
}

//---------------------------------------------------------
//	Group_Performance_Header
//---------------------------------------------------------

void LineSum::Group_Performance_Header (void)
{
	Print (1, "Line Group Performance Report (%s)", line_equiv.Group_Label (group_num));
	Print (2, "%13c----- Vehicle -----  ------- Transit Passengers -------  ---Passengers per---", BLANK);
	Print (1, "Time Period      Miles     Hours   Board  Alight     Miles     Hours    Mile   Hour    Veh");
	Print (1);
}

/*********************************************|***********************************************

	Line Group Performance Report (Group dd sssssssssssssssssss)

	             ----- Vehicle -----  ------- Transit Passengers -------  ---Passengers per---
	Time Period      Miles     Hours   Board  Alight     Miles     Hours    Mile   Hour    Veh

	dd:dd..dd:dd fffffff.f fffffff.f ddddddd ddddddd fffffff.f fffffff.f  ffff.f ffff.f ffff.f

	Total        fffffff.f fffffff.f ddddddd ddddddd fffffff.f fffffff.f  ffff.f ffff.f ffff.f 

**********************************************|***********************************************/ 
