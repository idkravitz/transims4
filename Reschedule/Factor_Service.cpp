//*********************************************************
//	Factor_Service.cpp - modify the number of runs
//*********************************************************

#include "Reschedule.hpp"

//---------------------------------------------------------
//	Factor_Service
//---------------------------------------------------------

void Reschedule::Factor_Service (void)
{
	int stop, stops, run, runs, route, tod, count;
	int num_in, num_out, shift, period, num_period;
	int duration [25], start [25], time0 [25], increment [25], num_runs [25], num_new [25], num [25];
	bool skip, flag, change;

	Line_Data *line_ptr;
	Range_Data *range_ptr;

	Schedule_File *new_schedule = (Schedule_File *) Network_Db_File (NEW_TRANSIT_SCHEDULE);

	Show_Message ("Writing %s -- Route", new_schedule->File_Type ());
	Set_Progress (100);
	count = num_in = num_out = 0;

	num_period = select_periods.Num_Ranges () + 1;

	if (period_flag) {
		for (range_ptr = select_periods.First (); range_ptr; range_ptr = select_periods.Next ()) {
			period = select_periods.Record_Index ();
			time0 [period] = range_ptr->Low ();
			duration [period] = range_ptr->High () - range_ptr->Low () + 1;
		}
	} else {
		duration [1] = MIDNIGHT;
		time0 [1] = 0;
	}

	//---- read each route ----

	for (line_ptr = line_data.First_Key (); line_ptr; line_ptr = line_data.Next_Key ()) {
		Show_Progress ();

		route = line_ptr->Route ();
		new_schedule->Route (route);

		stops = line_ptr->Stops ();
		runs = line_ptr->Runs ();

		//---- check the selection criteria ----

		change = false;
		flag = ((!route_flag || select_routes.In_Range (route)) && 
			select_modes [line_ptr->Mode ()]);

		memset (num_runs, '\0', sizeof (num_runs));
		memset (num_new, '\0', sizeof (num_new));
		memset (num, '\0', sizeof (num));
		memcpy (start, time0, sizeof (start));

		//---- count the runs in the period ----

		if (flag) {
			for (run=1; run <= runs; run++) {
				if (method == RUN_START) {
					tod = Resolve (line_ptr->Schedule (run, 1));
				} else if (method == RUN_END) {
					tod = Resolve (line_ptr->Schedule (run, stops));
				} else {
					tod = Resolve ((line_ptr->Schedule (run, stops) + line_ptr->Schedule (run, 1)) / 2);
				}
				if (period_flag) {
					period = select_periods.Period (tod);
				} else {
					period = 1;
				}
				if (period > 0) {
					num_runs [period]++;
				}
			}
			for (period=1; period <= num_period; period++) {
				if (num_runs [period] > 0) {
					num_new [period] = (int) (num_runs [period] * service_factor + random.Probability ());
					if (num_new [period] < 1) num_new [period] = 1;

					if (num_new [period] != num_runs [period]) {
						increment [period] = (duration [period] + num_new [period] / 2) / num_new [period];
						start [period] += increment [period] / 2;
						change = true;
					}
				}
			}
		}

		//---- save each run and stop ----

		for (run=1; run <= runs; run++) {
			if (method == RUN_START) {
				tod = Resolve (line_ptr->Schedule (run, 1));
			} else if (method == RUN_END) {
				tod = Resolve (line_ptr->Schedule (run, stops));
			} else {
				tod = Resolve ((line_ptr->Schedule (run, stops) + line_ptr->Schedule (run, 1)) / 2);
			}
			shift = period = 0;
			num_in++;

			skip = !flag && !change;
			if (!skip) {
				if (period_flag) {
					period = select_periods.Period (tod);
				} else {
					period = 1;
				}
				if (period > 0 && num_runs [period] != num_new [period]) {
					if (num [period] >= num_new [period]) continue;
					shift = start [period] - tod;
					start [period] += increment [period];
					num [period]++;
				} else {
					skip = true;
				}
			}

			for (stop=1; stop <= stops; stop++) {
				new_schedule->Stop (line_ptr->Stop (stop));
				new_schedule->Time (Resolve (line_ptr->Schedule (run, stop)) + shift);

				if (!new_schedule->Write ()) {
					Error ("Writing %s", new_schedule->File_Type ());
				}
				count++;
			}
			num_out++;

			if (!skip && num [period] == num_runs [period] && num_new [period] > num_runs [period]) {
				for (; num [period] < num_new [period]; num [period]++) {
					shift += increment [period];

					for (stop=1; stop <= stops; stop++) {
						new_schedule->Stop (line_ptr->Stop (stop));
						new_schedule->Time (Resolve (line_ptr->Schedule (run, stop)) + shift);

						if (!new_schedule->Write ()) {
							Error ("Writing %s", new_schedule->File_Type ());
						}
						count++;
					}
					num_out++;
				}
			}
		}
	}
	End_Progress ();

	Write (2, "Number of %s Records = %d", new_schedule->File_Type (), count);
	Write (1, "Number of Input Transit Runs = %d", num_in);
	Write (1, "Number of Output Transit Runs = %d", num_out);
}
