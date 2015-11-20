//*********************************************************
//	Perf_Report.cpp - Create a Transit Performance Report
//*********************************************************

#include "LineSum.hpp"

//---------------------------------------------------------
//	Performance_Report
//---------------------------------------------------------

void LineSum::Performance_Report (int number)
{
	double veh_miles, veh_hours, pass_miles, pass_hours;
	int board, alight, vehicles, passengers;

	double tot_veh_miles, tot_veh_hours, tot_pass_miles, tot_pass_hours;
	int tot_board, tot_alight, tot_vehicles, tot_passengers;

	int stop, stops, run, runs, period, per, num, time, on, off, ride, /*percent,*/ distance;
	bool last_stop_flag, stop_flag;

	Rider_Data *rider_ptr;
	Offset_Data *offset_ptr;
	//Route_Legs *legs_ptr;

	Show_Message ("Creating a Transit Performance Report -- Record");
	Set_Progress (100);

	//---- initialize the selection sets ----

	set_ptr = select_set [number];
	if (set_ptr == NULL) return;

	Header_Number (PERF_REPORT);

	//---- initialize the report data ----

	num = set_ptr->Num_Periods ();

	if (!Break_Check (num + 6)) {
		Print (1);
		Performance_Header ();
	}

	tot_veh_miles = tot_veh_hours = tot_pass_miles = tot_pass_hours = 0.0;
	tot_board = tot_alight = tot_vehicles = tot_passengers = 0;

	//---- process each time period ----

	for (per=1; per <= num; per++) {

		veh_miles = veh_hours = pass_miles = pass_hours = 0.0;
		board = alight = vehicles = passengers = 0;

		//---- process each route ----

		for (rider_ptr = rider_data.First_Key (); rider_ptr; rider_ptr = rider_data.Next_Key ()) {
			Show_Progress ();

			//---- check the selection criteria ----

			if (!set_ptr->Select_Modes (rider_ptr->Mode ())) continue;
			if (!set_ptr->Select_Routes (rider_ptr->Route ())) continue;

			stops = rider_ptr->Stops ();
			runs = rider_ptr->Runs ();

			//---- check for routes in the subarea ----

			//legs_ptr = set_ptr->Route_Legs (rider_data.Record_Index ());
			//if (legs_ptr == NULL) continue;

			//if (!legs_ptr->Subarea_Flag ()) continue;

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
				period = set_ptr->Time_Period (Resolve (time));

				if (period < per) continue;
				if (period > per) break;

				//---- sum the line ridership ----

				ride = 0;
				last_stop_flag = stop_flag = true;

				for (stop=1; stop <= stops; stop++) {
					on = rider_ptr->Board (run, stop);
					off = rider_ptr->Alight (run, stop);
					time = rider_ptr->Time (run, stop);
					distance = rider_ptr->Distance (stop);

					//percent = legs_ptr->Percent (stop);
					//distance = legs_ptr->Distance (stop);

					if (offset_flag && set_ptr->Subarea_Flag ()) {
						offset_ptr = stop_offset.Get (rider_ptr->Stop (stop));
						if (offset_ptr == NULL) continue;

						stop_flag = set_ptr->In_Subarea (offset_ptr->X (), offset_ptr->Y ());

						if (stop_flag && !last_stop_flag) {
							passengers += ride;
						}
						last_stop_flag = stop_flag;
					}
					ride += on - off;

					if (stop_flag) {
						passengers += on;
						board += on;
						alight += off;

						if (stop < stops) {
							time = rider_ptr->Time (run, stop+1) - time;
							distance = rider_ptr->Distance (stop+1) - distance;

							pass_hours += ride * time;
							veh_hours += time;

							pass_miles += ride * distance;
							veh_miles += distance;
						}
					}
				}
				vehicles++;
			}
		}
		veh_miles /= Round (MILETOMETER);
		pass_miles /= Round (MILETOMETER);
		veh_hours /= 3600.0;
		pass_hours /= 3600.0;

		Print (1, "%-12.12s%10.1lf%10.1lf%8d%8d%10.1lf%10.1lf", 
			set_ptr->Time_Format (per), veh_miles, veh_hours, 
			board, alight, pass_miles, pass_hours);

		tot_veh_miles += veh_miles;
		tot_veh_hours += veh_hours;
		tot_pass_miles += pass_miles;
		tot_pass_hours += pass_hours;
		tot_board += board;
		tot_alight += alight;
		tot_vehicles += vehicles;
		tot_passengers += passengers;

		if (veh_miles <= 0.0) veh_miles = 1.0;
		if (veh_hours <= 0.0) veh_hours = 1.0;
		if (vehicles <= 0) vehicles = 1;

		Print (0, " %7.1lf%7.1lf%7.1lf", passengers / veh_miles, 
			passengers / veh_hours, (double) passengers / vehicles);
	}
	End_Progress ();

	if (num > 1) {
		Print (2, "Total       %10.1lf%10.1lf%8d%8d%10.1lf%10.1lf", tot_veh_miles, tot_veh_hours, 
			tot_board, tot_alight, tot_pass_miles, tot_pass_hours);

		if (tot_veh_miles <= 0.0) tot_veh_miles = 1.0;
		if (tot_veh_hours <= 0.0) tot_veh_hours = 1.0;
		if (tot_vehicles <= 0) tot_vehicles = 1;

		Print (0, " %7.1lf%7.1lf%7.1lf", tot_passengers / tot_veh_miles, 
			tot_passengers / tot_veh_hours, (double) tot_passengers / tot_vehicles);
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Performance_Header
//---------------------------------------------------------

void LineSum::Performance_Header (void)
{
	Print (1, "Transit Performance Report");
	if (set_ptr->Number () > 0) Print (0, " #%d", set_ptr->Number ());
	if (set_ptr->Select_Label () != NULL) Print (0, " (%s)", set_ptr->Select_Label ());

	Print (2, "%13c----- Vehicle -----  ------- Transit Passengers -------  ---Passengers per---", BLANK);
	Print (1, "Time Period      Miles     Hours   Board  Alight     Miles     Hours    Mile   Hour    Veh");
	Print (1);
}

/*********************************************|***********************************************

	Transit Performance Report (%s)

	             ----- Vehicle -----  ------- Transit Passengers -------  ---Passengers per---
	Time Period      Miles     Hours   Board  Alight     Miles     Hours    Mile   Hour    Veh

	dd:dd..dd:dd fffffff.f fffffff.f ddddddd ddddddd fffffff.f fffffff.f  ffff.f ffff.f ffff.f

	Total        fffffff.f fffffff.f ddddddd ddddddd fffffff.f fffffff.f  ffff.f ffff.f ffff.f 

**********************************************|***********************************************/ 
