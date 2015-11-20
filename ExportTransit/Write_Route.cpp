//*********************************************************
//	Write_Route.cpp - Write Router Header and Node Files
//*********************************************************

#include "ExportTransit.hpp"

#include "Range_Data.hpp"

//---------------------------------------------------------
//	Write_Route
//---------------------------------------------------------

void ExportTransit::Write_Route (void)
{
	int i, j, node, link, dir, link_dir, route, mode, start, previous, num, headway, offset, high, low;
	int min_ttime, max_ttime, ttime, stops, runs, dwell, speed, time, num1, period;
	char buffer [FIELD_BUFFER];
	double factor;
	bool route_flag;

	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
	Line_Data *line_ptr;
	Stop_Data *stop_ptr;
	Driver_Data *driver_ptr;
	Range_Data *range_ptr;

	dwell = 5;
	time = 10;
	speed = 15;

	//---- read the route file ----

	Show_Message ("Writing Route Header and Nodes Files -- Record");
	Set_Progress (100);

	route_flag = (select_routes.Num_Ranges () > 0);

	for (line_ptr = line_data.First_Key (); line_ptr; line_ptr = line_data.Next_Key ()) {
		Show_Progress ();
		route = line_ptr->Route ();
		mode = line_ptr->Mode ();
		stops = line_ptr->Stops ();
		runs = line_ptr->Runs ();

		if (!select_modes [mode]) continue;
		if (route_flag && !select_routes.In_Range (route)) continue;

		str_cpy (buffer, sizeof (buffer), Transit_Code ((Transit_Type) mode));
		
		header_file->Route (route);
		header_file->Mode (buffer);
		header_file->Name ((char *) (line_ptr->Name ()));

		//---- calculate the travel time ----
		
		if (!time_flag) {
			min_ttime = MAX_INTEGER;
			max_ttime = 0;

			for (i=1; i <= runs; i++) {
				ttime = line_ptr->Schedule (i, stops);
				ttime -= line_ptr->Schedule (i, 1);

				if (ttime < min_ttime) min_ttime = ttime;
				if (ttime > max_ttime) max_ttime = ttime;
			}

			//---- average the lower times ----

			min_ttime = (max_ttime + min_ttime + 1) / 2;
			max_ttime = 0;

			for (i=1, j=0; i <= runs; i++) {
				ttime = line_ptr->Schedule (i, stops);
				ttime -= line_ptr->Schedule (i, 1);

				if (ttime <= min_ttime) {
					j++;
					max_ttime += ttime;
				}
			}
			if (j > 1) {
				max_ttime = (max_ttime + (j + 1) / 2) / j;
			}
			ttime = (Resolve (max_ttime) + 50) / 60;

			header_file->Min_Time (ttime);
		}

		//---- calculate the period headway and offset ----

		for (i=1; i <= num_periods; i++) {
			range_ptr = time_period [i];
			low = Round (range_ptr->Low ());
			high = Round (range_ptr->High ());

			headway = offset = ttime = num = previous = 0;

			for (j=1; j <= runs; j++) {
				start = line_ptr->Schedule (j, 1);

				if (start > high) break;
				if (start < low) continue;

				ttime += (line_ptr->Schedule (j, stops) - start);

				if (previous > 0) {
					headway += start - previous;
				}
				if (num == 0) {
					offset = start - low;
				}
				previous = start;
				num++;
			}
			if (num) {
				ttime = (ttime + (num + 1) / 2) / num;

				if (--num > 0) {
					headway = (headway + num / 2) / num;
				} else if (start > high) {
					if (start - previous < high - low) {
						headway = start - previous;
					} else {
						headway = high - low;
					}
				} else {
					headway = high - low;
				}

				//---- convert to minutes ----

				offset = (Resolve (offset) + 30) / 60;
				headway = (Resolve (headway) + 30) / 60;
				ttime = (Resolve (ttime) + 50) / 60;

				if (headway < 1) headway = 1;

				period = (range_ptr->High () - range_ptr->Low () + 31) / 60;
				if (offset >= period) {
					offset = period - 1;
				}
			}
			header_file->Headway (i, headway);
			header_file->Offset (i, offset);
			header_file->TTime (i, ttime);
		}
		header_file->Notes ((char *) line_ptr->Notes ());

		if (!header_file->Write ()) {
			Error ("Writing Router Header File");
		}
		nroute++;

		//---- write the node data ----

		nodes_file->Route (route);

		driver_ptr = driver_data [line_ptr->Driver ()];

		previous = 0;

		//---- estimate the travel time factors ----

		if (dwell_flag || ttime_flag || speed_flag) {
			min_ttime = MAX_INTEGER;
			j = 0;

			for (i=1; i <= runs; i++) {
				ttime = line_ptr->Schedule (i, stops);
				ttime -= line_ptr->Schedule (i, 1);

				if (ttime < min_ttime) {
					min_ttime = ttime;
					j = i;
				}
			}

			//---- store the leg times for the minimum run in run 1 ----

			for (i=1; i < stops; i++) {
				ttime = line_ptr->Schedule (j, i+1);
				ttime -= line_ptr->Schedule (j, i);

				line_ptr->Schedule (1, i, ttime);
			}

			//---- calculate the link travel time between stops ----

			j = 1;
			stop_ptr = stop_data.Get (line_ptr->Stop (j));
			if (stop_ptr == NULL) continue;
			ttime = 0;

			for (i=1; i <= driver_ptr->Links (); i++) {
				link = driver_ptr->Link (i);
				dir = driver_ptr->Dir (i);
				link_dir = driver_ptr->Link_Dir (i);

				link_ptr = link_data.Get (link);
				if (link_ptr == NULL) continue;

				if (dir == 0) {
					dir_ptr = dir_data [link_ptr->AB_Dir ()];
				} else {
					dir_ptr = dir_data [link_ptr->BA_Dir ()];
				}
				ttime += dir_ptr->Time0 ();
		
				//---- check for stops ----

				while (stop_ptr->Link_Dir () == link_dir) {
					if (dir == 0) {
						factor = ((double) (link_ptr->Length () - stop_ptr->Offset ())) / link_ptr->Length ();
					} else {
						factor = (double) stop_ptr->Offset () / link_ptr->Length ();
					}
					ttime -= time = (int) (dir_ptr->Time0 () * factor + 0.5);

					//---- calculate the time ratio ----

					if (j > 1) {
						if (ttime > 0) {
							ttime = (line_ptr->Schedule (1, j-1) * 10000 + (ttime + 1) / 2) / ttime;
						} else {
							ttime = 10000;
						}
						line_ptr->Schedule (1, j-1, ttime);
					}

					//---- find the next stop -----

					if (++j > stops) break;

					ttime = time;

					stop_ptr = stop_data.Get (line_ptr->Stop (j));
					if (stop_ptr == NULL) break;
				}
			}
		}

		//---- locate the stops to the flag nodes ----

		j = 1;
		stop_ptr = stop_data.Get (line_ptr->Stop (j));
		if (stop_ptr == NULL) continue;
		speed = time = 0;
		dwell = min_dwell;

		for (i=1; i <= driver_ptr->Links (); i++) {
			link = driver_ptr->Link (i);
			dir = driver_ptr->Dir (i);
			link_dir = driver_ptr->Link_Dir (i);

			link_ptr = link_data.Get (link);
			if (link_ptr == NULL) continue;

			if (dir == 0) {
				dir_ptr = dir_data [link_ptr->AB_Dir ()];
			} else {
				dir_ptr = dir_data [link_ptr->BA_Dir ()];
			}
			ttime = dir_ptr->Time0 ();

			//---- get the end nodes ----

			if (j == 1) {
				if (dir == 0) {
					previous = link_ptr->Anode ();
				} else {
					previous = link_ptr->Bnode ();
				}
			}
			if (dir == 0) {
				node = -link_ptr->Bnode ();
			} else {
				node = -link_ptr->Anode ();
			}

			//---- check for stops ----

			num = num1 = 0;

			while (stop_ptr->Link_Dir () == link_dir) {

				if (dir == 0) {
					if (stop_ptr->Offset () < link_ptr->Length () / 2) {
						if (previous < 0) previous = -previous;
						num1++;
					} else {
						if (node < 0) node = -node;
					}
				} else {
					if (stop_ptr->Offset () > link_ptr->Length () / 2) {
						if (previous < 0) previous = -previous;
						num1++;
					} else {
						if (node < 0) node = -node;

					}
				}
				num++;

				//---- find the next stop -----

				if (++j > stops) break;

				stop_ptr = stop_data.Get (line_ptr->Stop (j));
				if (stop_ptr == NULL) break;
			}

			//---- save the previous node ----

			if (j > 1) {
				nodes_file->Node (previous);

				if (dwell_flag) nodes_file->Dwell (((previous < 0) ? 0 : Resolve (dwell)));
				if (ttime_flag) nodes_file->Time (Resolve (time));
				if (speed_flag) nodes_file->Speed (UnRound (speed));

				if (!nodes_file->Write ()) {
					Error ("Writing Route Nodes File");
				}
				nnodes++;

				if (dwell_flag || ttime_flag || speed_flag) {

					//---- get the time adjustment factor ----

					if (num > 1 && num > num1) {

						//---- average the factors for multiple stops on the link ----

						factor = 0.0;

						for (num1 = j - num; num1 < j - 1; num1++) {
							factor += line_ptr->Schedule (1, num1);
						}
						factor /= ((num - 1) * 10000.0);
					} else {
						factor = line_ptr->Schedule (1, j - 1) / 10000.0;
					}

					//---- apply the time factor ----

					time = (int) (ttime * factor + 0.5);

					if (num > 0) {
						dwell = (time - ttime + num / 2) / num;
						if (dwell < min_dwell) {
							dwell = min_dwell;
							time = ttime;
						} else {
							if (dwell > max_dwell) {
								dwell = max_dwell;
							}
							time -= dwell * num;
						}
					} else {
						dwell = 0;
					}
					if (time == 0) {
						time = ttime;
					}
					speed = (Round (link_ptr->Length ()) + time / 2) / time;
				}
			}

			//---- save the last stop ----

			if (j > stops) {
				if (node < 0) node = -node;
				if (dwell == 0) dwell = min_dwell;

				nodes_file->Node (node);
				if (dwell_flag) nodes_file->Dwell (Resolve (dwell));
				if (ttime_flag) nodes_file->Time (Resolve (time));
				if (speed_flag) nodes_file->Speed (UnRound (speed));
				
				if (!nodes_file->Write ()) {
					Error ("Writing Route Nodes File");
				}
				nnodes++;
				break;
			}
			previous = node;
		}
	}
	End_Progress ();

	header_file->Close ();
	nodes_file->Close ();
}
