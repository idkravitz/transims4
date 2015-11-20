//*********************************************************
//	Merge_Riders.cpp - Merge the Ridership Data
//*********************************************************

#include "RiderPrep.hpp"

//---------------------------------------------------------
//	Merge_Riders
//---------------------------------------------------------

void RiderPrep::Merge_Riders (void)
{
	int i, route, run, stop, index, num, time, diff, best, schedule, last_route;



	Rider_Data *rider_ptr;

	//---- merge subarea ridership data ----

	Show_Message ("Reading %s -- Record", subarea_file.File_Type ());
	Set_Progress (10000);

	last_route = 0;

	while (subarea_file.Read ()) {
		Show_Progress ();

		//---- get the route id ----

		route = subarea_file.Route ();
		if (route == 0) continue;

		rider_ptr = rider_data.Get (route);

		if (rider_ptr == NULL) {
			rider_ptr = rider_data.Get (last_route);
			if (rider_ptr == NULL) continue;
		}
		last_route = rider_ptr->Route ();

		//---- get the run and stop numbers ----

		run = subarea_file.Run ();
		stop = subarea_file.Stop ();

		//---- find the stop index ----

		num = rider_ptr->Stops ();
		schedule = Round (trip_time.Step (subarea_file.Schedule ()));
		best = Round (3*60);	//---- three minutes ----
		index = 0;

		for (i=1; i <= num; i++) {
			if (rider_ptr->Stop (i) == stop) {
				diff = abs (rider_ptr->Schedule (run, i) - schedule);
				if (diff < best) {
					index = i;
					best = diff;
					if (diff == 0) break;
				}
			}
		}
		if (index == 0) continue;
		stop = index;

		//---- get the time ----

		time = Round (trip_time.Step (subarea_file.Time ()));

		if (time > 0) {
			diff = time - schedule;
			if (diff == 0) diff = 1;
			rider_ptr->Penalty (run, stop, diff);
		}

		////--- get the ridership ----

		//rider_ptr->Board (run, stop, subarea_file.Board ());
		//rider_ptr->Alight (run, stop, subarea_file.Alight ());
	}
	End_Progress ();

	subarea_file.Close ();
} 
