//*********************************************************
//	Write_Schedule.cpp - calculate the new schedule
//*********************************************************

#include "Reschedule.hpp"

//---------------------------------------------------------
//	Write_Schedule
//---------------------------------------------------------

void Reschedule::Write_Schedule (void)
{
	int stop, stops, stop1, run, runs, link, links, link1, time, ttime, route, period, index, dwell;
	int from_offset, to_offset, in_link_dir, out_link_dir, schedule, time0, tod, tim, tim0, t;
	int on, off, on_time, off_time, count, num_new, shift;
	double factor, length;
	bool warning_flag, skip, flag;

    TTime_Data *ttime_ptr;
	Connect_Time *connect_ptr;
	Link_Data *link_ptr;
	Driver_Data *driver_ptr;
	Stop_Data *stop_ptr;
	Rider_Data *rider_ptr;
	Veh_Type_Data *veh_type_ptr;
	Route_Data *route_ptr;

	Schedule_File *new_schedule = (Schedule_File *) Network_Db_File (NEW_TRANSIT_SCHEDULE);

	Show_Message ("Writing %s -- Route", new_schedule->File_Type ());
	Set_Progress (100);
	count = num_new = shift = 0;

	veh_type_ptr = NULL;
	on_time = Round (3.0);
	off_time = Round (2.0);
	warning_flag = true;

	//---- read each route ----

	for (rider_ptr = rider_data.First_Key (); rider_ptr; rider_ptr = rider_data.Next_Key ()) {
		Show_Progress ();

		route = rider_ptr->Route ();

		new_schedule->Route (route);

		stops = rider_ptr->Stops ();
		runs = rider_ptr->Runs ();

		if (shift_flag || run_flag) {
			if (shift_flag) {
				factor = 0.5;
				time0 = Resolve (rider_ptr->Schedule (1, 1));

				if (time0 < max_shift * factor) {
					factor = (double) time0 / max_shift;
				}
				shift = (int) (max_shift * (random.Probability () - factor) + 0.5);

				if (shift < 0) {
					if (-shift > time0) {
						shift = -time0;
					}
				}
			} else {
				time0 = rider_ptr->Schedule (1, 1);
				shift = rider_ptr->Penalty (1, 1);

				if (shift > 0) {
					shift = Resolve (shift) - Resolve (time0);
				}
			}
			if (!header_flag && !delay_flag && !rider_flag) {

				//---- check the selection criteria ----

				flag = ((route_flag && !select_routes.In_Range (route)) || 
					!select_modes [rider_ptr->Mode ()] || shift == 0);

				//---- save each run and stop ----

				for (run=1; run <= runs; run++) {
					time0 = Resolve (rider_ptr->Schedule (run, 1));
					schedule = Resolve (rider_ptr->Schedule (run, stops));
					tod = (schedule + time0) / 2;

					skip = flag;
					if (!skip && period_flag) {
						skip = (!select_periods.In_Range (tod));
					}
					if (run_flag) {
						shift = rider_ptr->Penalty (run, 1);

						if (shift > 0) {
							shift = Resolve (shift) - time0;
						}
					}

					//---- store report data ----

					if (diff_flag && !skip) {
						time = schedule - time0;
						time_diff.Add_Trip (tod, time + shift, time);
					}

					for (stop=1; stop <= stops; stop++) {

						//---- write the schedule data ----

						new_schedule->Stop (rider_ptr->Stop (stop));
						time = Resolve (rider_ptr->Schedule (run, stop));

						if (!skip) {
							time += shift;
							if (time < 0) time = 0;
							num_new++;
							warning_flag = false;
						}
						new_schedule->Time (time);

						if (!new_schedule->Write ()) {
							Error ("Writing %s", new_schedule->File_Type ());
						}
						count++;
					}
				}
				continue;
			}
		}

		//---- apply the route header adjustment ----

		if (header_flag) {

			//---- check the selection criteria ----

			skip = ((route_flag && !select_routes.In_Range (route)) || 
				!select_modes [rider_ptr->Mode ()]);

			if (!skip) {
				route_ptr = route_data.Get (route);
				skip = (route_ptr == NULL);
			}

			//---- write the schedule data ----

			for (run=1; run <= runs; run++) {
				flag = false;
				if (skip) {
					factor = 1.0;
				} else {
					time0 = Resolve (rider_ptr->Schedule (run, 1));

					if (period_flag && !select_periods.In_Index (time0)) {
						factor = 1.0;
					} else {
						time = Resolve (rider_ptr->Schedule (run, stops)) - time0;

						if (time > 0) {
							period = time_periods.In_Index (time0);

							tim = route_ptr->TTime (period);

							if (tim == 0) {
								tim = route_ptr->Min_Time ();
							}
							if (tim == 0) {
								factor = 1.0;
							} else {
								factor = (double) tim / time;
								flag = true;
							}
						} else {
							factor = 1.0;
						}
					}
				}

				//---- apply the adjustment factor to each stop ----

				for (stop=1; stop <= stops; stop++) {
					schedule = time = Resolve (rider_ptr->Schedule (run, stop));

					if (flag) {
						time = time0 + (int) ((time - time0) * factor + 0.5) + shift;
						if (time < 0) time = 0;
						num_new++;

						if (warning_flag && time != schedule) {
							warning_flag = false;
						}

						//---- store report data ----

						if (diff_flag) {
							tod = (schedule + time0) / 2;

							time_diff.Add_Trip (tod, time - time0, schedule - time0);
						}
					}
					new_schedule->Stop (rider_ptr->Stop (stop));
					new_schedule->Time (time);

					if (!new_schedule->Write ()) {
						Error ("Writing %s", new_schedule->File_Type ());
					}
					count++;
				}
			}
			continue;
		}

		//---- check the selection criteria ----

		if ((route_flag && !select_routes.In_Range (route)) || 
			!select_modes [rider_ptr->Mode ()]) {

			for (run=1; run <= runs; run++) {
				for (stop=1; stop <= stops; stop++) {

					//---- write the schedule data ----

					new_schedule->Stop (rider_ptr->Stop (stop));
					new_schedule->Time (Resolve (rider_ptr->Schedule (run, stop)));

					if (!new_schedule->Write ()) {
						Error ("Writing %s", new_schedule->File_Type ());
					}
					count++;
				}
			}
			continue;
		}

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
			in_link_dir = 0;

			time0 = tim0 = time = rider_ptr->Schedule (run, stop);

			stop_ptr = stop_data.Get (stop1);

			if (stop_ptr == NULL) {
				Warning ("Route %d Stop %d was Not Found", route, stop1);
				continue;
			}

			//---- check the time selection ----

			if (period_flag) {
				period = Resolve ((rider_ptr->Schedule (run, stops) + time0) / 2);

				if (!select_periods.In_Range (period)) {
					for (stop=1; stop <= stops; stop++) {

						//---- write the schedule data ----

						new_schedule->Stop (rider_ptr->Stop (stop));
						new_schedule->Time (Resolve (rider_ptr->Schedule (run, stop)));

						if (!new_schedule->Write ()) {
							Error ("Writing %s", new_schedule->File_Type ());
						}
						count++;
					}
					continue;
				}
			}
			if (run_flag) {
				shift = rider_ptr->Penalty (run, 1);

				if (shift > 0) {
					shift = Resolve (shift) - Resolve (time0);
				}
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
					tim = time + ttime;
					if (tim < time) {
						Warning ("1Time Overflow Problem %d < %d %d", Resolve (tim), Resolve (time), ttime);
						tim = time + 1;
					}
					time = tim;
				}
				in_link_dir = out_link_dir;

				//---- get the link and travel time ----

				link_ptr = link_data.Get (driver_ptr->Link (link));

				if (link_ptr == NULL) {
					Warning ("Route %d Link %d was Not Found", route, driver_ptr->Link (link));
					continue;
				}
				length = link_ptr->Length ();

				if (driver_ptr->Dir (link) == 1) {
					ttime_ptr = ttime_data [link_ptr->BA_Dir ()];
				} else {
					ttime_ptr = ttime_data [link_ptr->AB_Dir ()];
				}
				if (ttime_ptr == NULL) {
					Warning ("Route %d Link %d was Not Found", route, driver_ptr->Link (link));
					continue;
				}
				tod = Resolve (time);
				period = ttime_data.Period (tod);
				index = time_periods.In_Index (tod);

				ttime = (int) (ttime_ptr->TTime (period) * time_factor [index] + 0.5);

				if (ttime < 0) {
					Warning ("Time Overflow Problem %d < 0", Resolve (ttime));
					ttime = 1;
				}

				while (stop_ptr->Link_Dir () == out_link_dir) {
					if (stop_ptr->Dir () == 1) {
						to_offset = link_ptr->Length () - stop_ptr->Offset ();
					} else {
						to_offset = stop_ptr->Offset ();
					}
					if (stop > 1) {

						//---- get the ridership time ----

						if (rider_flag && !delay_flag) {
							time = rider_ptr->Time (run, stop);
						} else {
							if (to_offset <= from_offset) {
								Warning ("Route %d Stops are Out of Order", route);
								break;
							}
							tim = time + (int) (ttime * ((to_offset - from_offset) / length) + 0.5);
							if (tim < time) {
								Warning ("2Time Overflow Problem %d < %d", Resolve (tim), Resolve (time));
								tim = time + 1;
							}
							time = tim;

							//---- add stop dwell time ----

							if (rider_flag) {
								on = rider_ptr->Board (run, stop);
								off = rider_ptr->Alight (run, stop);

								if (veh_type_ptr != NULL) {
									if (veh_type_ptr->Method ()) {
										dwell = (int) (MAX ((on * veh_type_ptr->Loading ()), (off * veh_type_ptr->Unloading ())) + 0.5);
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
									if (dwell < min_dwell [index]) {
										dwell = min_dwell [index];
									} else if (dwell > max_dwell [index]) {
										dwell = max_dwell [index];
									}
								}
							} else {
								dwell = min_dwell [index];
							}
							if (dwell < 0) {
								Warning ("Dwell Time %d is Out of Range (>0)", Resolve (dwell));
								dwell = 0;
							}
							tim = time + dwell;
							if (tim < time) {
								Warning ("3Time Overflow Problem %d < %d", Resolve (tim), Resolve (time));
								tim = time + 1;
							}
							time = tim;
						}
					}
					if (time < tim0) {
						Warning ("Route %d stop %d run %d time %d < %d", route, stop, run, Resolve (time), Resolve (tim0));
					}

					//---- write the schedule data ----

					new_schedule->Stop (stop_ptr->Stop ());
					t = Resolve (time) + shift;
					if (t < 0) t = 0;

					new_schedule->Time (t);
					tim0 = time;

					if (!new_schedule->Write ()) {
						Error ("Writing %s", new_schedule->File_Type ());
					}
					count++;
					num_new++;

					//---- store report data ----

					if (diff_flag || warning_flag) {
						schedule = rider_ptr->Schedule (run, stop);
						if (warning_flag) {
							if (time != schedule) {
								warning_flag = false;
							}
						}
						if (diff_flag) {
							tod = Resolve ((schedule + time0) / 2);

							time_diff.Add_Trip (tod, Resolve (time - time0), Resolve (schedule - time0));
						}
					}
					tod = Resolve (time);
					period = ttime_data.Period (tod);
					index = time_periods.In_Index (tod);

					ttime = (int) (ttime_ptr->TTime (period) * time_factor [index] + 0.5);
					if (ttime < 0) {
						Warning ("Time Overflow Problem %d < 0", Resolve (ttime));
						ttime = 1;
					}

					//---- get the next stop ----

					if (++stop > stops) break;

					from_offset = to_offset;

					stop_ptr = stop_data.Get (rider_ptr->Stop (stop));

					if (stop_ptr == NULL) {
						Warning ("Route %d Stop %d was Not Found", route, rider_ptr->Stop (stop));
						break;
					}					
				}

				//---- update the travel time ----

				ttime = (int) (ttime * (length - from_offset) / length + 0.5);
			}
		}
	}
	End_Progress ();

	Write (2, "Number of %s Records = %d", new_schedule->File_Type (), count);
	Write (1, "Number of Records Rescheduled = %d", num_new);

	if (warning_flag) {
		Write (1);
		Warning ("No Schedule Times were Adjusted");
	}
}
