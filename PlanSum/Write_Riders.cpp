//*********************************************************
//	Write_Riders.cpp - Create the Transit Ridership File
//*********************************************************

#include "PlanSum.hpp"

#include "Ridership_File.hpp"

//---------------------------------------------------------
//	Write_Riders
//---------------------------------------------------------

void PlanSum::Write_Riders (void)
{
	int stop, stops, stop1, run, runs, link, links, link1, time, ttime, route, period, dwell;
	int from_offset, to_offset, in_link_dir, out_link_dir, schedule;
	int nout, on, off, on_time, off_time, load;
	double factor;
	bool veh_type_flag;

    TTime_Data *ttime_ptr;
	Connect_Time *connect_ptr;
	Link_Data *link_ptr;
	Driver_Data *driver_ptr;
	Stop_Data *stop_ptr;
	Rider_Data *rider_ptr;
	Veh_Type_Data *veh_type_ptr;

	Time_Step trip_time (1, "SECONDS");

	Ridership_File *rider_file = (Ridership_File *) Demand_Db_Base (NEW_RIDERSHIP);

	veh_type_flag = Demand_File_Flag (VEHICLE_TYPE);
	veh_type_ptr = NULL;
	on_time = Round (3.0);
	off_time = Round (2.0);
	ttime = period = 0;

	Show_Message ("Writing %s -- Route", rider_file->File_Type ());
	Set_Progress (100);
	nout = 0;

	//---- write data for each line ----

	for (rider_ptr = rider_data.First_Key (); rider_ptr; rider_ptr = rider_data.Next_Key ()) {
		Show_Progress ();

		route = rider_ptr->Route ();
				
		rider_file->Mode (Transit_Code ((Transit_Type) rider_ptr->Mode ()));
		rider_file->Route (route);

		stops = rider_ptr->Stops ();
		runs = rider_ptr->Runs ();

		//---- get the driver data ----

		driver_ptr = driver_data [rider_ptr->Driver ()];

		if (driver_ptr == NULL) {
			Warning ("Route %d was Not Found in the Transit Driver File", route);
			continue;
		}
		links = driver_ptr->Links ();

		//---- get the vehicle type data ----

		if (veh_type_flag) {
			veh_type_ptr = veh_type_data.Get (driver_ptr->Type (), driver_ptr->Sub_Type ());
		}

		//---- find the first link ----

		stop_ptr = stop_data.Get (rider_ptr->Stop (1));

		if (stop_ptr == NULL) {
			Warning ("Route %d Stop %d was Not Found", route, rider_ptr->Stop (1));
			continue;
		}
		stop1 = stop_ptr->Stop ();

		for (link1=1; link1 <= links; link1++) {
			if (stop_ptr->Link_Dir () == driver_ptr->Link_Dir (link1)) break;
		}
		if (link1 > links) {
			Warning ("Route %d Stops and Driver Links are Incompatible", route);
			continue;
		}

		//---- process each stop on each run ----

		for (run=1; run <= runs; run++) {

			stop = 1;
			load = in_link_dir = 0;

			schedule = time = rider_ptr->Schedule (run, stop);

			stop_ptr = stop_data.Get (stop1);

			if (stop_ptr == NULL) {
				Warning ("Route %d Stop %d was Not Found", route, stop1);
				continue;
			}

			//---- accumulate travel time from the network ----

			for (link=link1; link <= links && stop_ptr; link++) {

				out_link_dir = driver_ptr->Link_Dir (link);
				from_offset = 0;

				if (in_link_dir > 0) {

					//---- add turn delay ----

					if (turn_delay) {
						connect_ptr = connect_time.Get (in_link_dir, out_link_dir);

						if (connect_ptr != NULL && connect_ptr->Periods () > 0) {
							ttime += connect_ptr->TTime (period);
							if (ttime < 1) {
								ttime = 1;
							}
						}
					}
					time += ttime;
				}
				in_link_dir = out_link_dir;

				//---- get the link and travel time ----

				link_ptr = link_data.Get (driver_ptr->Link (link));

				if (link_ptr == NULL) {
					Warning ("Route %d Link %d was Not Found", route, driver_ptr->Link (link));
					continue;
				}
				if (driver_ptr->Dir (link) == 1) {
					ttime_ptr = ttime_data [link_ptr->BA_Dir ()];
				} else {
					ttime_ptr = ttime_data [link_ptr->AB_Dir ()];
				}
				if (ttime_ptr == NULL) {
					Warning ("Route %d Link %d was Not Found", route, driver_ptr->Link (link));
					continue;
				}
				period = ttime_data.Period (Resolve (time));

				ttime = ttime_ptr->TTime (period);
	
				while (stop_ptr->Link_Dir () == out_link_dir) {

					on = rider_ptr->Board (run, stop);
					off = rider_ptr->Alight (run, stop);

					load += on - off;

					//---- calculate the stop arrival time ----

					if (stop_ptr->Dir () == 1) {
						to_offset = link_ptr->Length () - stop_ptr->Offset ();
					} else {
						to_offset = stop_ptr->Offset ();
					}

					if (stop > 1) {
						if (to_offset < from_offset) {
							Warning ("Route %d Stop %d is Out of Order", route, stop_ptr->Stop ());
							break;
						} else if (to_offset == from_offset) {
							Warning ("Route %d Stop %d duplicated the previous Stop", route, stop_ptr->Stop ());
							break;
						}
						time += ttime * (to_offset - from_offset) / link_ptr->Length ();

						//---- add stop dwell time ----

						if (on > 0 || off > 0) {
							if (veh_type_ptr != NULL) {
								if (veh_type_ptr->Method ()) {
									dwell = MAX ((int) (on * veh_type_ptr->Loading () + 0.5), (int) (off * veh_type_ptr->Unloading () + 0.5));
								} else {
									dwell = (int) (on * veh_type_ptr->Loading () + off * veh_type_ptr->Unloading () + 0.5);
								}
								if (dwell < veh_type_ptr->Min_Dwell ()) {
									dwell = veh_type_ptr->Min_Dwell ();
								} else if (dwell > veh_type_ptr->Max_Dwell ()) {
									dwell = veh_type_ptr->Max_Dwell ();
								}
							} else {
								dwell = on * on_time + off * off_time;
							}
							time += dwell;
						}
					}

					//---- output the record ----

					if (stops_flag || on > 0 || off > 0) {
						rider_file->Run (run);
						rider_file->Stop (stop_ptr->Stop ());
						rider_file->Schedule (trip_time.Format_Time (Resolve (schedule)));
						rider_file->Time (trip_time.Format_Time (Resolve (time)));
						rider_file->Board (on);
						rider_file->Alight (off);
						rider_file->Load (load);
						if (veh_type_ptr != NULL && veh_type_ptr->Capacity () > 0) {
							factor = (double) load / veh_type_ptr->Capacity ();
							rider_file->Factor (factor);
						} else {
							rider_file->Factor (0.0);
						}
						rider_file->Write ();
						nout++;
					}

					//---- update travel time ----

					period = ttime_data.Period (Resolve (time));

					ttime = ttime_ptr->TTime (period);

					//---- get the next stop ----

					from_offset = to_offset;

					if (++stop > stops) {
						stop_ptr = NULL;
						break;
					}
					stop_ptr = stop_data.Get (rider_ptr->Stop (stop));

					if (stop_ptr == NULL) {
						Warning ("Route %d Stop %d was Not Found", route, rider_ptr->Stop (stop));
						break;
					}
					schedule = rider_ptr->Schedule (run, stop);
				}

				//---- travel time to the end of the link ----

				ttime *= (link_ptr->Length () - from_offset) / link_ptr->Length ();
			}
		}
	}
	End_Progress ();

	rider_file->Close ();

	Print (2, "Number of %s Records = %d", rider_file->File_Type (), nout);
}
