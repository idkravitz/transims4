//*********************************************************
//	Read_Plans.cpp - Calculate Link Volume
//*********************************************************

#include "PlanSum.hpp"

//---------------------------------------------------------
//	Read_Plans
//---------------------------------------------------------

void PlanSum::Read_Plans (void)
{
	int i, j, k, period, in_period, start, duration, num_path, *path, anode, lnk, dir, num, tod, end;
	int org, des, des_type, traveler, board, alight, tim, last_dir, in_link_dir, out_link_dir;
	int last_traveler, last_trip, offset, from_offset, to_offset, time1, time2, skim_period;
	int bus_board, rail_board, trip, stop, hhold, person, count, trip_turns, transfers;
	bool link_flag, mode_flag [MAX_MODE], skim_select, trip_select, flag, skip;
	bool bus_flag, rail_flag, drive_flag, transfer_flag, period_flag, ab_flag, tran_time_flag;
	double trip_length, travel_time, transit_length, transit_time, ttime, sum_time, schedule;

    TTime_Data *ttime_ptr = NULL;
	Location_Data *loc_ptr = NULL;
	AB_Data *ab_ptr = NULL;
	Link_Data *link_ptr = NULL;
	Parking_Data *parking_ptr = NULL;
	Rider_Data *rider_ptr = NULL;
	Movement_Data move_rec, *move_ptr = NULL;
	Connect_Time *connect_ptr = NULL;
	Transfer_Data *transfer_ptr = NULL;
	Skim_Data skim_rec, *skim_ptr = NULL;
	Time_Skim_Data time_rec, *time_ptr = NULL;
	Time_Len_Skim_Data tlen_rec, *tlen_ptr = NULL;
	Matrix_Data trip_rec, *trip_ptr = NULL;
	Trip_Sum_Data *trip_sum_ptr = NULL;
	Driver_Data *driver_ptr = NULL;
	Stop_Data *stop_ptr = NULL;
	Survey_Person *person_ptr = NULL;

	if (plan_file.Extend ()) {
		Show_Message ("Reading Plan File %s -- Plan", plan_file.Extension ());
	} else {
		Show_Message ("Reading Plan File -- Plan");
	}
	Set_Progress (10000);

	trip_length = travel_time = transit_length = transit_time = ttime = sum_time = schedule = 0.0;
	traveler = trip = stop = bus_board = rail_board = trip_turns = time1 = time2 = org = des = des_type = 0;
	in_period = start = end = last_traveler = last_trip = tod = skim_period = 0;
	bus_flag = rail_flag = drive_flag = skim_select = trip_select = false;

	link_flag = (delay_out || volume_flag || Report_Flag (TOP_100) || 
				Report_Flag (VC_RATIO) || Report_Flag (LINK_GROUP));

	tran_time_flag = (update_flag && Network_File_Flag (TRANSIT_DRIVER));

	transfer_flag = (Report_Flag (SUM_STOPS) || Report_Flag (STOP_GROUP));
	period_flag = skip = false;

	//---- read each plan ----

	for (;;) {
		flag = plan_file.Read ();
		if (flag) Show_Progress ();

		traveler = plan_file.Traveler ();
		trip = plan_file.Trip ();

		//---- skip activities ----

		if (traveler != last_traveler || trip != last_trip ||
			plan_file.Mode () == ACTIVITY_MODE || !flag) {

			//---- save the previous trip ----

			if (last_traveler > 0 && !skip) {

				//---- set the relication count ----

				if (count_flag) {
					person_ptr = person_count.Get (hhold, person);

					if (person_ptr != NULL) {
						count = person_ptr->Count ();
					} else {
						count = 1;
					}
				} else {
					count = 1;
				}

				//---- sum the travel statistics ----

				if (travel_flag && trip_length > 0) {
					trip_sum_total.Add_Trip (trip_length, travel_time, trip_turns, count);	

					if (num_inc > 1) {
						period = time_periods.In_Index ((time1 + time2 + 1) / 2);

						if (period > 0) {
							trip_sum_ptr = trip_sum_data.Get (period);

							if (trip_sum_ptr != NULL) {
								trip_sum_ptr->Add_Trip (trip_length, travel_time, trip_turns, count);
							}
						}
					}
				}

				//---- sum the passenger statistics ----

				if (passenger_flag && transit_length > 0) {
					transfers = bus_board + rail_board - 1;

					transit_length = UnRound ((int) (transit_length + 0.5));
					transit_time = UnRound ((int) (transit_time + 0.5));

					pass_sum_total.Add_Trip (transit_length, transit_time, transfers, count);

					if (num_inc > 1) {
						period = time_periods.In_Index ((time1 + time2 + 1) / 2);

						if (period > 0) {
							trip_sum_ptr = pass_sum_data.Get (period);

							if (trip_sum_ptr != NULL) {
								trip_sum_ptr->Add_Trip (transit_length, transit_time, transfers, count);
							}
						}
					}
				} else if (length_flag) {
					transit_length = UnRound ((int) (transit_length + 0.5));
				}

				//---- find the start time increment ----

				if (time_flag) {
					period = time_periods.In_Index (time1);

					if (period > 0) {
						(*(start_time.Record (period))) += count;
					}
				}

				//---- find the end time increment ----

				if (time_flag) {
					period = time_periods.In_Index (time2);

					if (period > 0) {
						(*(end_time.Record (period))) += count;
					}
				}

				//---- get the mid trip time ----

				period = time_periods.In_Index ((time1 + time2 + 1) / 2);

				if (skim_point == 0) {
					skim_period = time_periods.In_Index (time1);
				} else if (skim_point == 1) {
					skim_period = time_periods.In_Index (time2);
				} else {
					skim_period = period;
				}

				//---- increment the mid time counter ----

				if (period > 0 || skim_period > 0) {

					if (period > 0 && time_flag) {
						(*(mid_time.Record (period))) += count;
					}

					//---- get the destination zone ----
					
					skim_select = trip_select = false;

					if (skim_flag || trip_flag) {
						if (des_type == PARKING_ID) {
							des = park_location [parking_data.Get_Index (des)];
						} else if (des_type == STOP_ID) {
							des = stop_location [stop_data.Get_Index (des)];
						}
						loc_ptr = location_data.Get (des);

						if (loc_ptr == NULL || loc_ptr->Zone () == 0) {
							if (mode_flag [DRIVE_ALONE]) {
								Warning ("Traveler %d Zone Data was Not Found for Location %d", last_traveler, des);
							}
							des = 0;
						} else {
							des = loc_ptr->Zone ();

							//---- check the mode selection ----

							for (i=2; i < MAX_MODE; i++) {
								if (mode_flag [i]) {
									mode_flag [ALL_WALK] = false;
									break;
								}
							}
							for (i=1; i < MAX_MODE; i++) {
								if (mode_flag [i]) {
									if (skim_mode [i]) skim_select = true;
									if (trip_mode [i]) trip_select = true;
								}
							}
						}
					}

					//---- save skim data ----

					if (skim_flag && skim_select && skim_period > 0) {
						if (zone_flag) {
							i = zone_equiv.Zone_Group (org);
							j = zone_equiv.Zone_Group (des);
						} else {
							i = org;
							j = des;
						}
						if (i > 0 && j > 0) {
							skim_rec.ID (skim_data.Key (i, j, skim_period));

							if (skim_rec.ID () == 0) {
								Error ("Zone Skim Index %d-%d-%d is Out of Range", i, j, skim_period);
							}
							if (length_flag) {
								skim_rec.Length ((int) (count * (trip_length + transit_length) + 0.5));
							}
							if (count > 1) {
								skim_rec.Walk (count * skim_rec.Walk ());
								skim_rec.Wait (count * skim_rec.Wait ());
								skim_rec.Drive (count * skim_rec.Drive ());
								skim_rec.Transit (count * skim_rec.Transit ());
								skim_rec.Other (count * skim_rec.Other ());
								skim_rec.Cost (count * skim_rec.Cost ());
							}
							if (time_skim_flag) {
								if (length_flag) {
									tlen_rec.id = skim_rec.ID ();
									tlen_rec.count = count;
									tlen_rec.time = skim_rec.Walk () + skim_rec.Wait () + 
										skim_rec.Drive () + skim_rec.Transit () + skim_rec.Other ();
									tlen_rec.length = skim_rec.Length ();

									tlen_ptr = time_len_skim_data.Get (&tlen_rec);

									if (tlen_ptr == NULL) {
										if (!time_len_skim_data.Add (&tlen_rec)) {
											Error ("Adding Zone Skim Record");
										}
									} else {
										tlen_ptr->count += count;
										tlen_ptr->time += tlen_rec.time;
										tlen_ptr->length += tlen_rec.length;
									}
								} else {
									time_rec.id = skim_rec.ID ();
									time_rec.count = count;
									time_rec.time = skim_rec.Walk () + skim_rec.Wait () + 
										skim_rec.Drive () + skim_rec.Transit () + skim_rec.Other ();

									time_ptr = time_skim_data.Get (&time_rec);

									if (time_ptr == NULL) {
										if (!time_skim_data.Add (&time_rec)) {
											Error ("Adding Zone Skim Record");
										}
									} else {
										time_ptr->count += count;
										time_ptr->time += time_rec.time;
									}
								}
							} else {
								skim_ptr = skim_data.Get (&skim_rec);

								if (skim_ptr == NULL) {
									if (!skim_data.Add (&skim_rec)) {
										Error ("Adding Zone Skim Record");
									}
								} else {
									skim_ptr->Add_Skim (&skim_rec);
								}
							}
						}
					}

					//---- trip table data -----

					if (trip_flag && trip_select && period > 0) {
						if (zone_flag) {
							i = zone_equiv.Zone_Group (org);
							j = zone_equiv.Zone_Group (des);
						} else {
							i = org;
							j = des;
						}
						if (i > 0 && j > 0) {
							trip_rec.ID (trip_table.Key (i, j, period));
							trip_rec.Data (count);

							if (trip_rec.ID () == 0) {
								Error ("Trip Table Index %d-%d-%d is Out of Range", i, j, period);
							}
							trip_ptr = trip_table.Get (&trip_rec);

							if (trip_ptr == NULL) {
								if (!trip_table.Add (&trip_rec)) {
									Error ("Adding Trip Table Record");
								}
							} else {
								trip_ptr->Add_Data (count);
							}
						}
					}

					//----- set the boarding count ----

					if (period > 0 && (bus_board > 0 || rail_board > 0)) {
						if (bus_board >= MAX_BOARD) {
							bus_board = MAX_BOARD - 1;
						}
						if (rail_board >= MAX_BOARD) {
							rail_board = MAX_BOARD - 1;
						}
						k = bus_board * MAX_BOARD + rail_board;
						if (xfer_flag) {
							boardings [0] [k]++;
						}
						if (xfer_detail) {
							if (drive_flag) {
								num = (period - 1) * 2 + 2;
							} else {
								num = (period - 1) * 2 + 1;	
							}
							boardings [num] [k]++;
						}
					}
				}

				//---- finish the trip transfer summary ----

				if (transfer_flag && period_flag) {
					if (stop > 0) {
						transfer_ptr = transfer_data.Get (stop);

						if (transfer_ptr != NULL) {
							if (drive_flag) {
								transfer_ptr->Add_Drive_Alight ();
							} else {
								transfer_ptr->Add_Walk_Alight ();
							}
						}
					}
				}
			}
			if (!flag) break;

			if (plan_file.Mode () == ACTIVITY_MODE) {
				if (update_flag) {
					if (end < plan_file.Stop_Time ()) {
						end = plan_file.Stop_Time ();
					}
				}
				last_traveler = 0;
				continue;
			}
		}
		if (traveler == last_traveler && skip) continue;

		//---- process a trip leg ----

		start = plan_file.Time ();
		duration = plan_file.Duration ();
		time2 = start + duration;
		des_type = plan_file.End_Type ();
		des = plan_file.End_ID ();

		if (traveler != last_traveler) {
			last_traveler = traveler;
			last_trip = 0;

			if (hhlist_flag || count_flag) {
				hhold = plan_file.Household ();
				person = plan_file.Person ();

				if (hhlist_flag) {
					skip = (hhold_list.Get (&hhold) == NULL);
					if (skip) continue;
				}
			}
		}

		//---- initialize a new trip ----

		if (trip != last_trip) {
			last_trip = trip;

			time1 = start;			
			period_flag = (time_periods.In_Index (start) != 0);
			bus_flag = rail_flag = drive_flag = false;
			bus_board = rail_board = stop = 0;
			trip_length = travel_time = transit_length = transit_time = 0.0;
			trip_turns = transfers = 0;

			if (skim_flag || trip_flag) {
				if (plan_file.Start_Type () == LOCATION_ID) {
					org = plan_file.Start_ID ();
				} else if (plan_file.Start_Type () == PARKING_ID) {
					org = park_location [parking_data.Get_Index (plan_file.Start_ID ())];
				} else if (plan_file.Start_Type () == STOP_ID) {
					org = stop_location [stop_data.Get_Index (plan_file.Start_ID ())];
				}
				loc_ptr = location_data.Get (org);

				if (loc_ptr == NULL || loc_ptr->Zone () == 0) {
					Warning ("Traveler %d Zone Data was Not Found for Location %d", traveler, org);
					continue;
				}
                org = loc_ptr->Zone ();

				skim_rec.New_Skim ();

				memset (mode_flag, '\0', sizeof (mode_flag));
			}
		} else if (update_flag) {
			if (end > start) {
				start = end;
				end = time2 = start + duration;
			}
		}

		//---- save the transit leg ----

		if ((transit_flag || tran_time_flag) && plan_file.Mode () == TRANSIT_MODE) {
			rider_ptr = rider_data.Get (plan_file.Route ());

			if (rider_ptr == NULL) {
				Warning ("Traveler %d Route %d is not in the Route File", traveler, plan_file.Route ());
				continue;
			}

			//---- sum transfer modes ----

			if (transfer_flag && period_flag) {
				board = plan_file.Start_ID ();

				transfer_ptr = transfer_data.Get (board);

				if (transfer_ptr != NULL) {
					if (bus_flag) {
						transfer_ptr->Add_Bus_Board ();
					} else if (rail_flag) {
						transfer_ptr->Add_Rail_Board ();
					} else if (drive_flag) {
						transfer_ptr->Add_Drive_Board ();
					} else {
						transfer_ptr->Add_Walk_Board ();
					}
				}
				bus_flag = rail_flag = drive_flag = false;

				if (rider_ptr->Mode () == LOCAL_BUS || rider_ptr->Mode () == EXPRESS_BUS) {
					bus_flag = true;
				} else {
					rail_flag = true;
				}

				//---- process previous leg ----

				if (stop > 0) {
					transfer_ptr = transfer_data.Get (stop);

					if (transfer_ptr != NULL) {

						if (bus_flag) {
							transfer_ptr->Add_Bus_Alight ();
						} else {
							transfer_ptr->Add_Rail_Alight ();
						}
					}
				}
				stop = plan_file.End_ID ();
			}

			//---- bus or rail ----

			if (rider_ptr->Mode () == LOCAL_BUS || rider_ptr->Mode () == EXPRESS_BUS) {
				bus_board++;
			} else {
				rail_board++;
			}

			//---- find the stop index ----

			board = plan_file.Start_ID ();
			alight = plan_file.End_ID ();
			flag = false;

			for (i=1; i <= rider_ptr->Stops (); i++) {
				if (flag) {
					if (rider_ptr->Stop (i) == alight) {
						alight = i;
						break;
					}

					//---- check for loop routes ----

					if (rider_ptr->Stop (i) == plan_file.Start_ID ()) {
						board = i;
					}
				} else {
					if (rider_ptr->Stop (i) == board) {
						board = i;
						flag = true;
					}
				}
			}
			if (i > rider_ptr->Stops ()) {
				Warning ("Traveler %d Route %d from Stop %d to %d was Not Found", 
					traveler, rider_ptr->Route (), plan_file.Start_ID (), plan_file.End_ID ());
				continue;
			}

			//---- find the best run ----

			tim = Round (start);

			for (i=1; i <= rider_ptr->Runs (); i++) {
				if (tim <= rider_ptr->Schedule (i, board)) break;
			}

			if (i > rider_ptr->Runs ()) {
				Warning ("No Runs Start after %d at Stop %d on Route %d for Traveler %d", 
					start, plan_file.Start_ID (), rider_ptr->Route (), traveler);
				continue;
			} else {
				tod = Resolve (rider_ptr->Schedule (i, board));
				period = time_periods.In_Index (tod);

				if (period != 0) {
					//---- add the rider to the run ----

					rider_ptr->Board (i, board, 1);
					rider_ptr->Alight (i, alight, 1);
				}

				//---- get the route length and travel time ----

				if (passenger_flag || length_flag || update_flag) {
					schedule = rider_ptr->Schedule (i, alight) - rider_ptr->Schedule (i, board);

					//---- assume 10 seconds per stop ----

					sum_time = Round (10.0) * (alight - board);

					driver_ptr = driver_data [rider_ptr->Driver ()];

					//---- find the boarding link ----

					j = 1;
					stop_ptr = stop_data.Get (rider_ptr->Stop (j));

					for (i=1; i <= driver_ptr->Links (); i++) {
						while (stop_ptr && stop_ptr->Link_Dir () == driver_ptr->Link_Dir (i)) {
							if (j == board) break;
							stop_ptr = stop_data.Get (rider_ptr->Stop (++j));
						}
						if (j == board) break;
					}
					if (i > driver_ptr->Links ()) {
						Warning ("Traveler %d Route %d Stops and Driver Links are Incompatible", traveler, rider_ptr->Route ());
						continue;
					}

					//---- get the link and travel time ----

					link_ptr = link_data.Get (stop_ptr->Link ());

					if (link_ptr == NULL) {
						Warning ("Traveler %d Stop %d Link %d was Not Found", traveler, stop_ptr->Stop (), stop_ptr->Link ());
						continue;
					}
					if (stop_ptr->Dir () == 1) {
						from_offset = link_ptr->Length () - stop_ptr->Offset ();
					} else {
						from_offset = stop_ptr->Offset ();
					}

					//---- get the alight stop ---

					stop_ptr = stop_data.Get (rider_ptr->Stop (alight));

					if (stop_ptr == NULL) {
						Warning ("Traveler %d Route %d Stop %d was Not Found", traveler, rider_ptr->Route (), rider_ptr->Stop (alight));
						continue;
					}
					in_link_dir = 0;

					//---- sum the length and time to the alight stop ----

					for (; i <= driver_ptr->Links (); i++) {
						if (delay_in) {
							out_link_dir = driver_ptr->Link_Dir (i);

							if (in_link_dir > 0) {
								connect_ptr = connect_time.Get (in_link_dir, out_link_dir);

								if (connect_ptr != NULL && connect_ptr->Periods () > 0) {
									offset = connect_ptr->TTime (in_period);
									if (ttime + offset < 0) {
										offset = -((int) ttime);
									}
									tod = Resolve (Round (tod) + offset);
									sum_time += offset;
								}
							}
							in_link_dir = out_link_dir;
						}

						//---- get the link and travel time ----

						link_ptr = link_data.Get (driver_ptr->Link (i));

						if (link_ptr == NULL) {
							Warning ("Traveler %d Route %d Link %d was Not Found", traveler, rider_ptr->Route (), driver_ptr->Link (i));
							continue;
						}
						if (driver_ptr->Dir (i) == 1) {
							ttime_ptr = ttime_data [link_ptr->BA_Dir ()];
						} else {
							ttime_ptr = ttime_data [link_ptr->AB_Dir ()];
						}
						if (ttime_ptr == NULL) {
							Warning ("Traveler %d Route %d Link %d was Not Found", traveler, rider_ptr->Route (), driver_ptr->Link (i));
							continue;
						}
						in_period = ttime_data.Period (tod);

						ttime = ttime_ptr->TTime (in_period);

						if (stop_ptr->Link_Dir () == driver_ptr->Link_Dir (i)) {
							if (stop_ptr->Dir () == 1) {
								to_offset = link_ptr->Length () - stop_ptr->Offset ();
							} else {
								to_offset = stop_ptr->Offset ();
							}
						} else {
							to_offset = link_ptr->Length ();
						}
						offset = to_offset - from_offset;
						ttime *= offset / link_ptr->Length ();

						transit_length += offset;
						sum_time += ttime;

						if (to_offset < link_ptr->Length ()) break;

						from_offset = 0;
						tod = Resolve ((int) (Round (tod) + ttime));
					}
					if (i > driver_ptr->Links ()) {
						Warning ("Traveler %d Route %d Stops and Driver Links are Incompatible", traveler, rider_ptr->Route ());
						continue;
					}
					if (sum_time > schedule) {
						transit_time += sum_time;
						duration = Resolve ((int) (sum_time + 0.5));
					} else {
						transit_time += schedule;
						duration = Resolve ((int) (schedule + 0.5));
					}
				}
			}
		} else {
			if (plan_file.Mode () == AUTO_MODE) {
				drive_flag = true;
			}
		}

		//---- set the mode flag ----

		if (skim_flag || trip_flag) {
			switch (plan_file.Mode ()) {
				case AUTO_MODE:
					if (mode_flag [TRANSIT]) {
						mode_flag [PNR_IN] = true;
						mode_flag [TRANSIT] = false;
					} else {
						mode_flag [DRIVE_ALONE] = true;
					}
					break;
				case TRANSIT_MODE:
					if (mode_flag [DRIVE_ALONE]) {
						mode_flag [PNR_OUT] = true;
						mode_flag [DRIVE_ALONE] = false;
					} else if (!mode_flag [PNR_OUT]) {
						mode_flag [TRANSIT] = true;
					}
					break;
				case WALK_MODE:
					mode_flag [ALL_WALK] = true;
					break;
				case BIKE_MODE:
					mode_flag [BICYCLE] = true;
					break;
				case OTHER_MODE:
					mode_flag [SCHOOL_BUS] = true;
					break;
				case MAGIC_MODE:
					mode_flag [MAGIC_MOVE] = true;
					break;
				default:
					break;
			}
		}

		//---- save the leg inforamtion ----

		if (skim_flag) {
			switch (plan_file.Mode ()) {
				case AUTO_MODE:
					skim_rec.Add_Drive (duration);
					break;
				case TRANSIT_MODE:
					skim_rec.Add_Transit (duration);
					break;
				case WALK_MODE:
					if (plan_file.End_Type () == STOP_ID) {
						skim_rec.Add_Wait (duration);
					} else {
						skim_rec.Add_Walk (duration);
					}
					break;
				case BIKE_MODE:
					skim_rec.Add_Other (duration);
					break;
				case OTHER_MODE:
					skim_rec.Add_Other (duration);
					break;
				case MAGIC_MODE:
					skim_rec.Add_Other (duration);
					break;
				default:
					skim_rec.Add_Other (duration);
					break;
			}
			skim_rec.Add_Cost (plan_file.Cost ());
			skim_rec.Count (1);
		}

		//---- locate each link on the path ----

		if ((turn_flag || link_flag || travel_flag || length_flag || update_flag) && plan_file.Driver_Flag ()) {

			path = plan_file.Path (&num_path);

			if (type_flag) {
				num = num_path++;
			} else {
				num = num_path - 1;
			}
			if (num == 0) num = 1;

			last_dir = anode = in_link_dir = 0;

			if (delay_in) {
				tod = plan_file.Time ();
			} else {
				travel_time += duration;
			}

			for (i=1; i <= num_path; i++) {

				//---- check the time of day ----

				if (delay_in) {
					in_period = ttime_data.Period (tod);
					period = time_periods.In_Index (tod);
				} else {
					if (i == 1) {
						tod = start + duration / (2 * num);
					} else if (i == num_path) {
						tod = time2 - duration / (2 * num);
					} else {
						tod = start + (i - 1) * duration / num;
					}
					period = time_periods.In_Index (tod);
				}

				//---- origin parking lot ----

				offset = 0;

				if (i == 1) {		
					parking_ptr = parking_data.Get (plan_file.Start_ID ());
					if (parking_ptr == NULL) {
						Warning ("Traveler %d Parking Lot %d was Not Found", traveler, plan_file.Start_ID ());
						break;
					}
					offset = parking_ptr->Offset ();
				}
				if (i == num_path) {
					parking_ptr = parking_data.Get (plan_file.End_ID ());
					if (parking_ptr == NULL) {
						Warning ("Traveler %d Parking Lot %d was Not Found", traveler, plan_file.End_ID ());
						break;
					}
					offset = parking_ptr->Offset () - offset;
				}
				
				//---- identify the link direction ----

				if (type_flag) {		//---- node-based paths ----

					if (i == 1) {		//---- origin parking lot ----
						link_ptr = link_data.Get (parking_ptr->Link ());
						if (link_ptr == NULL) {
							Warning ("Traveler %d Parking Link %d was Not Found", traveler, parking_ptr->Link ());
							break;
						}
						if (i == num_path) {
							if (offset >= 0) {
								dir = link_ptr->AB_Dir ();
							} else {
								dir = link_ptr->BA_Dir ();
								offset = -offset;
							}
						} else if (link_ptr->Bnode () == *path) {
							dir = link_ptr->AB_Dir ();
							offset = link_ptr->Length () - offset;
							if (offset < 1) offset = 1;
						} else if (link_ptr->Anode () == *path) {
							dir = link_ptr->BA_Dir ();
						} else {
							Warning ("Traveler %d Node %d is not on Origin Link %d", 
								traveler, *path, link_ptr->Link ());
							break;
						}
						if (dir == 0) {
							Warning ("Traveler %d Link %d is Not Available", traveler, link_ptr->Link ());
							break;
						}
					} else if (i == num_path) {		//---- destination parking lot ----
						link_ptr = link_data.Get (parking_ptr->Link ());
						if (link_ptr == NULL) {
							Warning ("Traveler %d Parking Link %d was Not Found", traveler, parking_ptr->Link ());
							break;
						}
						if (link_ptr->Anode () == anode) {
							dir = link_ptr->AB_Dir ();
						} else if (link_ptr->Bnode () == anode) {
							dir = link_ptr->BA_Dir ();
							offset = link_ptr->Length () - offset;
							if (offset < 1) offset = 1;
						} else {
							Warning ("Traveler %d Node %d is not on Destination Link %d", 
								plan_file.Traveler (), anode, link_ptr->Link ());
							break;
						}
						if (dir == 0) {
							Warning ("Traveler %d Link %d is Not Available", traveler, link_ptr->Link ());
							break;
						}
					} else {
						ab_ptr = ab_key.Get (anode, *path);

						if (ab_ptr == NULL) {
							Warning ("Traveler %d No Link between %d and %d", traveler, anode, *path);
							break;
						}
						dir = ab_ptr->dir;
					}

				} else {		//---- link-based paths ----

					if (*path < 0) {
						ab_flag = false;
						lnk = -(*path);
					} else {
						ab_flag = true;
						lnk = *path;
					}
					link_ptr = link_data.Get (lnk);

					if (link_ptr == NULL) {
						Warning ("Traveler %d Link %d is Not Found", traveler, lnk);
						break;
					}
					offset = abs (offset);
					if (ab_flag) {
						dir = link_ptr->AB_Dir ();
						anode = link_ptr->Anode ();
						if (i == 1 && i != num_path) {
							offset = link_ptr->Length () - offset;
							if (offset <= 1) offset = 1;
						}
					} else {
						dir = link_ptr->BA_Dir ();
						anode = link_ptr->Bnode ();
						if (i != 1 && i == num_path) {
							offset = link_ptr->Length () - offset;
							if (offset <= 1) offset = 1;
						}
					}
					if (dir == 0) {
						Warning ("Traveler %d is Not Permitted on Link %d in %s Direction", traveler, lnk, ((ab_flag) ? "AB" : "BA"));
						break;
					}
				}

				//---- get the data records ----

				if (delay_in || link_flag || travel_flag || length_flag || update_flag) {
					ttime_ptr = ttime_data [dir];

					if (turn_delay || travel_flag || update_flag) {
						connect_ptr = connect_time.Get (in_link_dir, ttime_ptr->Link_Dir ());
						in_link_dir = ttime_ptr->Link_Dir ();

						if (travel_flag && connect_ptr != NULL) {
							if (connect_ptr->Type () == LEFT || connect_ptr->Type () == RIGHT ||
								connect_ptr->Type () == UTURN) {
								trip_turns++;
							}
						}
					}

					//---- sum the trip length ----

					if (travel_flag || length_flag) {
						if (offset > 0) {
							trip_length += UnRound (offset);
						} else {
							if (type_flag) {
								link_ptr = link_data.Get (ttime_ptr->Link ());
							}
							trip_length += UnRound (link_ptr->Length ());
						}
					}

					//---- get the travel time ----

					if (delay_in) {
						if (offset > 0) {
							tim = ttime_ptr->TTime (in_period) * offset / link_ptr->Length ();
						} else {
							tim = ttime_ptr->TTime (in_period);
						}
						if (turn_delay && connect_ptr != NULL && connect_ptr->Periods () > 0) {
							tim += connect_ptr->TTime (in_period);
							if (tim < 1) tim = 1;
						}
						tod = Resolve (Round (tod) + tim);

						if (travel_flag || update_flag) {
							travel_time += UnRound (tim);
						}
					}

					//---- sum the volume ----

					if (link_flag && period > 0) {
						if (ttime_ptr->Periods () == 0) {
							if (!ttime_ptr->Periods (num_inc, ttime_data.Layers ())) goto mem_error;
						}
						ttime_ptr->Add_Data (period, layer, 1);

						if (turn_delay && connect_ptr != NULL) {
							if (connect_ptr->Periods () == 0) {
								if (!connect_ptr->Periods (num_inc, connect_time.Layers ())) goto mem_error;
							}
							connect_ptr->Add_Data (period, layer, 1);
						}
					}
				}

				//---- process the link ----

				move_rec.node = anode;
				move_rec.in_link = last_dir;
				move_rec.out_link = dir;
				last_dir = dir;

				if (type_flag) {
					if (i < num_path) {
						anode = *path++;
					}
				} else {
					path++;
				}
				if (turn_flag) {
					if (period == 0) continue;
				}
				if (turn_flag && move_rec.in_link > 0) {
					if (node_range != NULL) {
						if (!node_range->In_Range (move_rec.node)) continue;
					}
					if (period < 1 || period > num_inc) {
						Error ("Time Period %d is Out of Range (1..%d)", period, num_inc);
					}
					move_ptr = movement_data [period]->Get (&move_rec);

					if (move_ptr == NULL) {
						move_rec.volume = 1;
						if (!movement_data [period]->Add (&move_rec)) {
							Error ("Adding Turn Movement Record");
						}
					} else {
						move_ptr->volume++;
					}
				}
			}
			duration = (int) (travel_time + 0.5);
		}

		//---- update the plan travel time and arrival time -----

		if (update_flag) {
			int time = plan_file.Duration ();
			int cost = plan_file.Cost ();
			int gcf = plan_file.GCF ();

			time += cost;
			if (time == 0) time = 1;

			gcf = (int) ((double) gcf * (duration + cost) / time + 0.5);

			end = start + duration;

			plan_file.Time (start);
			plan_file.Duration (duration);
			plan_file.Stop_Time (end);
			plan_file.GCF (gcf);

			if (!new_plan_file.Write (plan_file.Plan ())) {
				Error ("Writing %s", new_plan_file.File_Type ());
			}
		}
	}
	End_Progress ();

	plan_file.Close ();

	if (update_flag) {
		new_plan_file.Close ();
	}
	return;

mem_error:
	Error ("Insufficient Memory for Link Delay Data");
}
