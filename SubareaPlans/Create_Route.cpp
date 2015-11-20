//*********************************************************
//	Create_Route.cpp - create subarea transit route data
//*********************************************************

#include "SubareaPlans.hpp"

//---------------------------------------------------------
//	Create_Route
//---------------------------------------------------------

void SubareaPlans::Create_Route (void)
{
	int link, links, route, s, s0, s1, n;
	int i, j, k, stops, runs, num, stop, time, diff, dir, link_dir, offset, new_dist, old_dist;
	int first_stop, first_link, first_index, first_dir;
	int last_stop, last_link, last_index, last_dir, last_zone;
	double first_factor, last_factor;
	bool version3_flag, sub_flag, split_flag, save_flag;

	Line_Data *line_ptr, *new_line_ptr;
	Link_Data *link_ptr;
	Stop_Data *stop_ptr, *stop2_ptr;
	Driver_Data *driver_ptr, *new_driver_ptr;
	Dir_Data *dir_ptr;

	if (output_flag) {
		version3_flag = (line_file->Dbase_Format () == VERSION3);
	}

	line_ptr = line_data.Last_Key ();
	max_route = line_ptr->Route ();
	max_route = ((max_route + 1000) / 1000) * 1000;

	//---- process each subarea route ----
	
	Show_Message ("Creating Subarea Transit Route Data -- Record");
	Set_Progress (1000);

	for (line_ptr = line_data.First (); line_ptr; line_ptr = line_data.Next ()) {
		Show_Progress ();

		route = line_ptr->Route ();
		stops = line_ptr->Stops ();
		runs = line_ptr->Runs ();

		//---- scan the path for boundary links ----

		driver_ptr = driver_data [line_ptr->Driver ()];
		if (driver_ptr == NULL) continue;

		links = driver_ptr->Links ();
		sub_flag = split_flag = save_flag = false;

		s = s0 = 1;
		stop_ptr = stop_data.Get (line_ptr->Stop (s));
		first_index = 1;
		last_index = stops;

		for (i=1; i <= links && s <= stops; i++) {
			link_ptr = link_data.Get (driver_ptr->Link (i));
			link_dir = driver_ptr->Link_Dir (i);

			s1 = 0;
			while (s <= stops && stop_ptr->Link_Dir () == link_dir) {
				if (s1 == 0) s1 = s;
				if (++s > stops) break;
				stop_ptr = stop_data.Get (line_ptr->Stop (s));
			}

			//---- check for a subarea link ----

			if (!sub_flag && link_ptr->Type () > 0 && s > 1) {

				//---- enter the subarea ----

				sub_flag = true;
				first_link = i;
				if (s1 > 0) {
					first_index = s1;
				} else {
					first_index = s;
				}
				if (link_ptr->Type () < 3) {
					if (driver_ptr->Dir (i) == 0) {
						if (link_ptr->Type () == 1) {
							sub_flag = false;
						}
					} else if (link_ptr->Type () == 2) {
						sub_flag = false;
					}

					//---- exit the subarea ----

					if (!sub_flag) {
						last_link = i;
						last_index = s - 1;
						save_flag = true;
					}
				}
			} else if (sub_flag && (link_ptr->Type () < 3 || s > stops)) {

				//---- exit the subarea ----

				sub_flag = false;
				last_link = i;
				last_index = s - 1;
				save_flag = true;
			}
			if (!save_flag) continue;
			save_flag = false;

			//---- save the subarea route segment ----
			//---- mark the prior stops outside the subarea ----

			for (j=s0; j < first_index; j++) {
				line_ptr->New_Line (j, route);
				line_ptr->New_Stop (j, ((split_flag) ? -1 : 0));
			}

			//---- set the route number ----

			if (split_flag) {
				route = ++max_route;
				new_route++;
			} else {
				split_flag = true;
			}

			//---- create subarea driver records ----

			num = last_link - first_link + 1;
			new_driver_ptr = new_driver_data.New_Record (true);

			new_driver_ptr->Route (route);
			if (!new_driver_ptr->Links (num)) goto mem_error;

			for (n=1, j=first_link; j <= last_link; j++, n++) {
				new_driver_ptr->Link_Dir (n, driver_ptr->Link_Dir (j));
			}
			if (!new_driver_data.Add ()) goto mem_error;

			//---- output the driver records ----

			if (output_flag) {
				driver_file->Route (route);
				driver_file->Links (num);
				driver_file->Veh_Type (driver_ptr->Type ());
				driver_file->Sub_Type (driver_ptr->Sub_Type ());
				driver_file->Notes ((char *) driver_ptr->Notes ());

				if (!driver_file->Write (false)) {
					Error ("Writing %s", driver_file->File_Type ());
				}
				ndriver++;
	
				//---- save the link records ----

				for (j=first_link; j <= last_link; j++) {
					link = driver_ptr->Link (j);
					link_ptr = link_data.Get (link);

					dir = driver_ptr->Dir (j);

					if (driver_file->LinkDir_Type () == LINK_SIGN) {
						if (dir == 0) {
							driver_file->Link (link);
						} else {
							driver_file->Link (-link);
						}
					} else if (driver_file->LinkDir_Type () == LINK_DIR) {
						driver_file->Link (link);
						driver_file->Dir (dir);
					} else {
						driver_file->Link (link);
						if (dir == 0) {
							driver_file->Dir (link_ptr->Bnode ());
						} else {
							driver_file->Dir (link_ptr->Anode ());
						}
					}
					if (!driver_file->Write (true)) {
						Error ("Writing %s", driver_file->File_Type ());
					}
					ndriver++;
				}
			}

			//---- create the subarea route ----

			num = last_index - first_index + 1;
			if (first_index > 1) num++;
			if (last_index < stops) num++;
			if (num < 2) num = 2;

			new_line_ptr = new_line_data.New_Record (true);
			new_line_ptr->Route (route);
			if (!new_line_ptr->Stops (num)) goto mem_error;
			if (!new_line_ptr->Runs (runs)) goto mem_error;
			n = 1;

			//---- output the route records ----

			if (output_flag) {
				line_file->Route (route);
				line_file->Stops (num);
				line_file->Mode (Transit_Code ((Transit_Type) line_ptr->Mode ()));
				line_file->Name ((char *) line_ptr->Name ());
				line_file->Notes ((char *) line_ptr->Notes ());

				if (!line_file->Write (false)) {
					Error ("Writing %s", line_file->File_Type ());
				}
				nroute++;
			}

			first_factor = last_factor = 1.0;
			first_stop = last_stop = 0;

			//---- add a stop to the beginning of the line ----

			if (first_index > 1) {
				first_dir = driver_ptr->Link_Dir (first_link);

				dir_ptr = dir_data.Get (first_dir);

				first_stop = dir_ptr->Stop_List ();

				link_ptr = link_data.Get (dir_ptr->Link ());

				//---- add the first stop ----

				new_line_ptr->Stop (n++, first_stop);
				
				//---- save the first stop ----

				if (output_flag) {
					if (version3_flag) {
						line_file->Link (link_ptr->Link ());

						if (dir_ptr->Dir () == 0) {
							line_file->Node (link_ptr->Bnode ());
						} else {
							line_file->Node (link_ptr->Anode ());
						}
					}
					line_file->Stop (first_stop);
					line_file->Zone (line_ptr->Zone (first_index));
					line_file->Time_Flag (0);

					if (!line_file->Write (true)) {
						Error ("Writing %s", line_file->File_Type ());
					}
					nroute++;
				}

				//---- find the distance to the stop ahead ----

				stop2_ptr = stop_data.Get (first_stop);

				if (dir_ptr->Dir () == 0) {
					offset = stop2_ptr->Offset ();
				} else {
					offset = link_ptr->Length () - stop2_ptr->Offset ();
				}
				new_dist = -offset;

				stop = line_ptr->Stop (first_index);
				stop2_ptr = stop_data.Get (stop);
				link_dir = stop2_ptr->Link_Dir ();

				if (link_dir == first_dir) {
					if (dir_ptr->Dir () == 0) {
						new_dist += stop2_ptr->Offset ();
					} else {
						new_dist += link_ptr->Length () - stop2_ptr->Offset ();
					}
				} else {
					for (j=first_link; j <= links; j++) {
						dir = driver_ptr->Link_Dir (j);
						link_ptr = link_data.Get (driver_ptr->Link (j));

						if (dir != link_dir) {
							new_dist += link_ptr->Length ();
						} else {
							if (stop2_ptr->Dir () == 0) {
								new_dist += stop2_ptr->Offset ();
							} else {
								new_dist += link_ptr->Length () - stop2_ptr->Offset ();
							}
							break;
						}
					}
				}

				//---- find the distance to the stop behind ----

				stop2_ptr = stop_data.Get (first_stop);
				if (stop2_ptr->Dir () == 0) {
					old_dist = stop2_ptr->Offset ();
				} else {
					link_ptr = link_data.Get (stop2_ptr->Link ());
					old_dist = link_ptr->Length () - stop2_ptr->Offset ();
				}
				stop = line_ptr->Stop (first_index-1);
				stop2_ptr = stop_data.Get (stop);
				link_dir = stop2_ptr->Link_Dir ();

				if (link_dir == first_dir) {
					if (stop2_ptr->Dir () == 0) {
						old_dist -= stop2_ptr->Offset ();
					} else {
						old_dist -= link_ptr->Length () - stop2_ptr->Offset ();
					}
				} else {
					for (j=first_link-1; j > 0; j--) {
						dir = driver_ptr->Link_Dir (j);
						link_ptr = link_data.Get (driver_ptr->Link (j));
						old_dist += link_ptr->Length ();

						if (dir == link_dir) {
							if (stop2_ptr->Dir () == 0) {
								old_dist -= stop2_ptr->Offset ();
							} else {
								old_dist -= link_ptr->Length () - stop2_ptr->Offset ();
							}
							break;
						}
					}
				}
				old_dist += new_dist;
				if (old_dist > 0) {
					first_factor = (double) new_dist / old_dist;
				}
			}

			//---- save the stop records ----

			if (output_flag) {
				for (j=first_index; j <= last_index; j++) {
					stop = line_ptr->Stop (j);

					if (version3_flag) {
						stop_ptr = stop_data.Get (stop);

						link_ptr = link_data.Get (stop_ptr->Link ());

						line_file->Link (link_ptr->Link ());

						if (stop_ptr->Dir () == 0) {
							line_file->Node (link_ptr->Bnode ());
						} else {
							line_file->Node (link_ptr->Anode ());
						}
					}
					line_file->Stop (stop);
					line_file->Zone (line_ptr->Zone (j));
					line_file->Time_Flag (line_ptr->Time_Flag (j));

					if (!line_file->Write (true)) {
						Error ("Writing %s", line_file->File_Type ());
					}
					nroute++;
				}
			}

			//---- add the stop records ----

			for (j=first_index; j <= last_index; j++) {
				last_zone = line_ptr->Zone (j);

				line_ptr->New_Line (j, route);
				line_ptr->New_Stop (j, n);

				new_line_ptr->Stop (n++, line_ptr->Stop (j));
			}

			//---- add a stop to the end of the line ----

			if (last_index < stops) {
				last_dir = driver_ptr->Link_Dir (last_link);

				dir_ptr = dir_data.Get (last_dir);

				last_stop = dir_ptr->Stop_List ();

				link_ptr = link_data.Get (dir_ptr->Link ());

				//---- add the last stop ----

				new_line_ptr->Stop (n, last_stop);

				//---- save the last stop ----

				if (output_flag) {
					if (version3_flag) {
						line_file->Link (link_ptr->Link ());

						if (dir_ptr->Dir () == 0) {
							line_file->Node (link_ptr->Bnode ());
						} else {
							line_file->Node (link_ptr->Anode ());
						}
					}
					line_file->Stop (last_stop);
					line_file->Zone (last_zone);
					line_file->Time_Flag (0);

					if (!line_file->Write (true)) {
						Error ("Writing %s", line_file->File_Type ());
					}
					nroute++;
				}

				//---- find the distance to the stop ahead ----

				stop2_ptr = stop_data.Get (last_stop);

				if (dir_ptr->Dir () == 0) {
					offset = stop2_ptr->Offset ();
				} else {
					offset = link_ptr->Length () - stop2_ptr->Offset ();
				}
				old_dist = -offset;

				stop = line_ptr->Stop (last_index+1);
				stop2_ptr = stop_data.Get (stop);
				link_dir = stop2_ptr->Link_Dir ();

				if (link_dir == last_dir) {
					if (dir_ptr->Dir () == 0) {
						old_dist += stop2_ptr->Offset ();
					} else {
						old_dist += link_ptr->Length () - stop2_ptr->Offset ();
					}
				} else {
					for (j=last_link; j <= links; j++) {
						dir = driver_ptr->Link_Dir (j);
						link_ptr = link_data.Get (driver_ptr->Link (j));

						if (dir != link_dir) {
							old_dist += link_ptr->Length ();
						} else {
							if (stop2_ptr->Dir () == 0) {
								old_dist += stop2_ptr->Offset ();
							} else {
								old_dist += link_ptr->Length () - stop2_ptr->Offset ();
							}
							break;
						}
					}
				}

				//---- find the distance to the stop behind ----

				stop2_ptr = stop_data.Get (last_stop);
				if (stop2_ptr->Dir () == 0) {
					new_dist = stop2_ptr->Offset ();
				} else {
					link_ptr = link_data.Get (stop2_ptr->Link ());
					new_dist = link_ptr->Length () - stop2_ptr->Offset ();
				}
				stop = line_ptr->Stop (last_index);
				stop2_ptr = stop_data.Get (stop);
				link_dir = stop2_ptr->Link_Dir ();

				if (link_dir == last_dir) {
					if (stop2_ptr->Dir () == 0) {
						new_dist -= stop2_ptr->Offset ();
					} else {
						new_dist -= link_ptr->Length () - stop2_ptr->Offset ();
					}
				} else {
					for (j=last_link-1; j > 0; j--) {
						dir = driver_ptr->Link_Dir (j);
						link_ptr = link_data.Get (driver_ptr->Link (j));
						new_dist += link_ptr->Length ();

						if (dir == link_dir) {
							if (stop2_ptr->Dir () == 0) {
								new_dist -= stop2_ptr->Offset ();
							} else {
								new_dist -= link_ptr->Length () - stop2_ptr->Offset ();
							}
							break;
						}
					}
				}
				old_dist += new_dist;
				if (old_dist > 0) {
					last_factor = (double) new_dist / old_dist;
				}

			}

			//---- output the transit schedules ----

			if (output_flag) {
				schedule_file->Route (route);
			}
			for (j=1; j <= runs; j++) {
				n = 1;

				if (first_index > 1) {
					time = line_ptr->Schedule (j, first_index);
					diff = time - line_ptr->Schedule (j, first_index - 1);
					time -= (int) (diff * first_factor + 0.5);

					new_line_ptr->Schedule (j, n++, time);

					if (output_flag) {
						schedule_file->Stop (first_stop);
						schedule_file->Time (Resolve (time));

						if (!schedule_file->Write ()) {
							Error ("Writing %s", schedule_file->File_Type ());
						}
						nschedule++;
					}
				}
				for (k=first_index; k <= last_index; k++) {
					stop = line_ptr->Stop (k);
					time = line_ptr->Schedule (j, k);

					new_line_ptr->Schedule (j, n++, time);

					if (output_flag) {
						schedule_file->Stop (stop);
						schedule_file->Time (Resolve (time));

						if (!schedule_file->Write ()) {
							Error ("Writing %s", schedule_file->File_Type ());
						}
						nschedule++;
					}
				}
				if (last_index < stops) {
					time = line_ptr->Schedule (j, last_index);
					diff = line_ptr->Schedule (j, last_index + 1) - time;
					time += (int) (diff * last_factor + 0.5);

					new_line_ptr->Schedule (j, n, time);

					if (output_flag) {
						schedule_file->Stop (last_stop);
						schedule_file->Time (Resolve (time));

						if (!schedule_file->Write ()) {
							Error ("Writing %s", schedule_file->File_Type ());
						}
						nschedule++;
					}
				}
			}

			//---- save the new line ----

			if (!new_line_data.Add ()) goto mem_error;
			s0 = last_index + 1;
		}

		//---- mark the prior stops outside the subarea ----

		for (j=s0; j <= stops; j++) {
			line_ptr->New_Line (j, route);
			line_ptr->New_Stop (j, ((split_flag) ? -1 : 0));
		}
	}
	End_Progress ();
	return;

mem_error:
	Error ("Insufficient Memory for Subarea Transit Data");
}
