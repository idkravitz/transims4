//*********************************************************
//	Read_Run_Start.cpp - read the run start time file
//*********************************************************

#include "Reschedule.hpp"

//---------------------------------------------------------
//	Write_Schedule
//---------------------------------------------------------

void Reschedule::Read_Run_Start (void)
{
	int i, route, last_route, first_stop, stop, time, nrun, nstop, num_lines, num_first, num_match;

	Line_Data *line_ptr;

	Show_Message ("Reading %s -- Route", run_file.File_Type ());
	Set_Progress (10000);

	//---- read run start times ----

	Set_Progress ();
	last_route = first_stop = num_lines = num_first = num_match = 0;
	nrun = nstop = 1;

	while (run_file.Read ()) {
		Show_Progress ();

		route = run_file.Route ();
		if (route == 0) continue;

		//---- check the stop id ----

		stop = run_file.Stop ();

		//---- check for a new route ----

		if (route != last_route) {
			last_route = route;

			line_ptr = line_data.Get (route);
			if (line_ptr == NULL) {
				num_lines++;
				continue;
			}
			first_stop = line_ptr->Stop (1);

			if (stop != first_stop) {
				line_ptr = NULL;
				num_first++;
				continue;
			}
			nrun = nstop = i = 1;

		} else {
			if (line_ptr == NULL) continue;

			//---- find the stop id on the route ----

			for (i=nstop; i <= line_ptr->Stops (); i++) {
				if (line_ptr->Stop (i) == stop) break;
			}
			if (i > line_ptr->Stops ()) {
				if (stop == first_stop) {
					nrun++;
					nstop = i = 1;
				} else {
					nstop = 0;
				}
			}
		}
		nstop++;
		if (i > 1) continue;

		//---- add the time point ----

		time = Round (run_file.Time ());
		if (time <= 0) time = 1;

		line_ptr->Penalty (nrun, i, time);
		num_match++;
	}
	End_Progress ();

	run_file.Close ();

	Print (2, "Number of %s Records = %d", run_file.File_Type (), Progress_Count ());
	Print (1, "Number of Routes that were Not Found = %d", num_lines);
	Print (1, "Number of Routes with Different Stops = %d", num_first);
	Print (1, "Number of Runs with Schedule Checks = %d", num_match);
}
