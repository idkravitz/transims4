//*********************************************************
//	Write_Capacity.cpp - Create a Capacity Constraint File
//*********************************************************

#include "LineSum.hpp"

//---------------------------------------------------------
//	Write_Capacity
//---------------------------------------------------------

void LineSum::Write_Capacity (void)
{
	int per_runs, cap_runs, max_run, run_max, run_tot, cap_tot, max_load;
	int stop, stops, run, runs, period, last_per, time, on, off, ride;
	double avg_fac, cap_fac, max_fac;
	bool check_flag, stop_flag;

	Rider_Data *rider_ptr;
	Driver_Data *driver_ptr;
	Offset_Data *offset_ptr;
	Veh_Type_Data *veh_type_ptr;

	Show_Message ("Writing the Capacity Constraint File -- Record");
	Set_Progress (100);

	//---- initialize the selection sets ----

	set_ptr = select_set [0];
	if (set_ptr == NULL) return;

	//---- process each route ----

	for (rider_ptr = rider_data.First_Key (); rider_ptr; rider_ptr = rider_data.Next_Key ()) {
		Show_Progress ();

		//---- check the selection criteria ----

		if (!set_ptr->Select_Modes (rider_ptr->Mode ())) continue;
		if (!set_ptr->Select_Routes (rider_ptr->Route ())) continue;

		driver_ptr = driver_data.Get (rider_ptr->Route ());
		if (driver_ptr == NULL) continue;

		veh_type_ptr = veh_type_data.Get (driver_ptr->Type (), driver_ptr->Sub_Type ());
		if (veh_type_ptr == NULL) continue;

		if (veh_type_ptr->Capacity () <= 0) continue;

		stops = rider_ptr->Stops ();
		runs = rider_ptr->Runs ();
		last_per = cap_runs = 0;

		//---- process each run ----

		for (run=1; ; run++) {
			if (run <= runs) {
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
			} else {
				period = 0;
			}
			if (period != last_per) {
				if (last_per > 0 && cap_runs > 0) {
					if (per_runs > 0) {
						avg_fac = (double) run_tot / per_runs;
						run_tot = (int) (avg_fac + 0.5);
						avg_fac /= veh_type_ptr->Capacity ();
					} else {
						run_tot = 0;
						avg_fac = 0;
					}
					if (cap_runs > 0) {
						cap_fac = (double) cap_tot / cap_runs;
						cap_tot = (int) (cap_fac + 0.5);
						cap_fac /= veh_type_ptr->Capacity ();
					} else {
						cap_tot = 0;
						cap_fac = 0;
					}
					if (max_run > 0) {
						max_fac = (double) max_load / veh_type_ptr->Capacity ();
					} else {
						max_fac = 0.0;
					}
					cap_file.Put_Field (1, Transit_Code ((Transit_Type) rider_ptr->Mode ()));
					cap_file.Put_Field (2, rider_ptr->Route ());
					cap_file.Put_Field (3, (char *) set_ptr->Time_Format (last_per));
					cap_file.Put_Field (4, per_runs);
					cap_file.Put_Field (5, run_tot);
					cap_file.Put_Field (6, avg_fac);
					cap_file.Put_Field (7, cap_runs);
					cap_file.Put_Field (8, cap_tot);
					cap_file.Put_Field (9, cap_fac);
					cap_file.Put_Field (10, max_run);
					cap_file.Put_Field (11, max_load);
					cap_file.Put_Field (12, max_fac);

					if (!cap_file.Write ()) {
						Error ("Writing %s", cap_file.File_Type ());
					}
				}
				last_per = period;
				per_runs = cap_runs = max_run = run_tot = cap_tot = max_load = 0;
			}
			if (run > runs) break;
			if (last_per == 0) continue;

			//---- sum the line ridership ----

			stop_flag = true;
			check_flag = false;
			run_max = ride = 0;

			for (stop=1; stop <= stops; stop++) {
				on = rider_ptr->Board (run, stop);
				off = rider_ptr->Alight (run, stop);
				ride += on - off;

				if (offset_flag && set_ptr->Subarea_Flag ()) {
					offset_ptr = stop_offset.Get (rider_ptr->Stop (stop));
					if (offset_ptr == NULL) continue;

					stop_flag = set_ptr->In_Subarea (offset_ptr->X (), offset_ptr->Y ());
				}
				if (stop_flag) {
					check_flag = true;
					if (ride > run_max) run_max = ride;
				}
			}
			if (check_flag) {
				per_runs++;
				run_tot += run_max;
				if (run_max > veh_type_ptr->Capacity ()) {
					if (run_max > max_load) {
						max_load = run_max;
						max_run = run;
					}
					cap_runs++;
					cap_tot += run_max;
				}
			}
		}
	}
	End_Progress ();
}
