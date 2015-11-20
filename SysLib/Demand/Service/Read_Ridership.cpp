//*********************************************************
//	Read_Ridership.cpp - read the Transit Ridership file
//*********************************************************

#include "Demand_Service.hpp"
#include "Ridership_File.hpp"
#include "Time_Step.hpp"

//---------------------------------------------------------
//	Read_Ridership
//---------------------------------------------------------

void Demand_Service::Read_Ridership (void)
{
	int stop, stops, stop1, stop2, run, runs, time, time1, time2, sched, sched1, sched2;

	Rider_Data *rider_ptr;

	Db_File *file = Demand_Db_File (RIDERSHIP);

	//---- check for required data ----

	if (Check_Data () || Renumber ()) { 
		if (!Network_File_Flag (TRANSIT_ROUTE) && Network_Option_Flag (TRANSIT_ROUTE)) {
			Error (file_error, "Transit Route", file->File_ID ());
		}
		if (!Network_File_Flag (TRANSIT_SCHEDULE) && Network_Option_Flag (TRANSIT_SCHEDULE)) {
			Error (file_error, "Transit Route", file->File_ID ());
		}
	}

	//---- store the ridership data ----

	Show_Message ("Reading %s -- Record", file->File_Type ());
	Set_Progress (10000);

	First_Record (true);

	while (file->Read ()) {
		Show_Progress ();

		Rider_Processing (file);

		First_Record (false);
	}
	End_Progress ();
	file->Close ();

	Print (2, "Number of Ridership Records = %d", Progress_Count ());

	if (Progress_Count () > 0) Demand_Data_True (RIDERSHIP);

	//---- interpolate the loaded time data ----

	for (rider_ptr = rider_data.First (); rider_ptr; rider_ptr = rider_data.Next ()) {

		stops = rider_ptr->Stops ();
		runs = rider_ptr->Runs ();

		for (run=1; run <= runs; run++) {
			stop1 = 0;

			for (stop=1; stop <= stops; stop++) {
				time = rider_ptr->Time (run, stop);

				if (time == 0) {
					if (stop == 1) {
						time = rider_ptr->Schedule (run, stop);
						rider_ptr->Time (run, stop, time);
						stop1 = 1;
						continue;
					}
					for (stop2 = stop + 1; stop2 <= stops; stop2++) {
						time = rider_ptr->Time (run, stop2);
						if (time > 0) break;
					}
					if (stop2 <= stops) {
						sched1 = rider_ptr->Schedule (run, stop1);
						sched2 = rider_ptr->Schedule (run, stop2) - sched1;
						if (sched2 == 0) sched2 = 1;

						time1 = rider_ptr->Time (run, stop1);
						time2 = rider_ptr->Time (run, stop2) - time1;

						for (; stop < stop2; stop++) {
							sched = rider_ptr->Schedule (run, stop) - sched1;

							time = time1 + (int) ((double) sched * time2 / sched2);
							rider_ptr->Time (run, stop, time);
						}
						stop1 = stop2;
					} else {
						time1 = rider_ptr->Schedule (run, stop1);
						time2 = rider_ptr->Time (run, stop1) - time1;

						for (; stop <= stops; stop++) {
							time1 += time2;
							time = rider_ptr->Schedule (run, stop);
							if (time <= 0) {
								time = time1;
							}
							rider_ptr->Time (run, stop, time + time2);
						}
					}
				} else {
					stop1 = stop;
				}
			}
		}
	}
}

//---------------------------------------------------------
//	Rider_Processing
//---------------------------------------------------------

bool Demand_Service::Rider_Processing (Db_File *fh)
{
	int i, route, run, stop, index, num, time, diff, best, schedule;
	bool stop_flag;

	static Time_Step trip_time (1, "SECONDS");

	Rider_Data *rider_ptr;

	Ridership_File *file = (Ridership_File *) fh;

	stop_flag = Network_Data_Flag (TRANSIT_STOP);

	//---- get the route id ----

	route = file->Route ();

	if (route == 0) return (false);

	rider_ptr = rider_data.Get (route);

	if (rider_ptr == NULL) {
		Error ("Ridership Route %d was Not Found in the Route File", route);
		return (false);
	}

	//---- get the run and stop numbers ----

	run = file->Run ();
	stop = file->Stop ();

	if (stop_flag) {
		index = stop_data.Get_Index (stop);
		if (index == 0) goto stop_error;

		if (Renumber ()) stop = index;
	}

	//---- find the stop index ----

	num = rider_ptr->Stops ();
	schedule = Round (trip_time.Step (file->Schedule ()));
	best = Round (3*60);	//---- three minutes ----
	index = 0;

	for (i=1; i <= num; i++) {
		if (rider_ptr->Stop (i) == stop) {
			diff = abs (rider_ptr->Schedule (run, i) - schedule);
			if (diff < best) {
				index = i;
				best = diff;
				if (diff == 0) break;
			} else if (index == 0) {
				index = i;
			}
		}
	}
	if (index == 0) {
		stop = file->Stop ();
		goto stop_error;
	} 
	stop = index;

	//---- get the time ----

	time = Round (trip_time.Step (file->Time ()));

	if (time > 0) {
		rider_ptr->Time (run, stop, time);
	}

	//--- get the ridership ----

	rider_ptr->Board (run, stop, file->Board ());
	rider_ptr->Alight (run, stop, file->Alight ());

	return (true);

stop_error:
	Error ("Ridership Stop %d on Route %d was Not Found", stop, route);
	return (false);
}
