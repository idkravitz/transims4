//*********************************************************
//	Update_Plan.cpp - Update the Plan Time and Cost
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Update_Plan
//---------------------------------------------------------

bool Router::Update_Plan (void)
{
	int i, trip, start, tod_a, tod_b, penalty, pen_imp, wait, mode, variance, cost, cost_a, length, list;
	int traveler, board, alight, tim, value_waiting, imp, imp_b, wait_imp, dir, lnk, veh_id, use, cum, time;
	int *path, num_path, num, anode, last_dir, in_link_dir, tod, period, offset, offset_factor;
	int board_id, alight_id, node_id, acc, org, stop, stop_pen, stat;
	bool flag, ab_flag, hov_flag, hov_lanes;
	Use_Type type;

	Line_Data *line_ptr;
    TTime_Data *ttime_ptr;
	AB_Data *ab_ptr;
	Link_Data *link_ptr = 0;
	Parking_Data *parking_ptr = 0;
	Connect_Time *connect_ptr;
	Fare_Data fare_rec, *fare_ptr;
	TOD_Turn_Data *turn_ptr = 0;
	Vehicle_Data *veh_ptr;
	Link_Use_Data *use_ptr;
	Toll_Data *toll_ptr;
	Access_Data *access_ptr;
	List_Data *acc_ptr;
	Stop_Data *stop_ptr;

	//---- check the traveler ----

	traveler = old_plan_file.Traveler ();
	trip = old_plan_file.Trip ();
	start = old_plan_file.Time ();
	mode = old_plan_file.Mode ();
	period = stop_pen = stat = 0;

	if (traveler == last_traveler && trip == last_trip) {
		if (start != last_time) nlegs++;
		start = last_time;
	} else {
		if (attribute_flag) {
			Write_Attribute ();

			attribute_data.traveler = traveler;
			attribute_data.trip = trip;
		}
		last_traveler = traveler;
		last_trip = trip;
		last_xfer = last_mode = 0;
	}

	//---- update the times ----

	last_time = start + old_plan_file.Duration ();

	old_plan_file.Time (start);
	old_plan_file.Stop_Time (last_time);

	//---- remove the wait time from the walk leg ----

	if (transit_net && !wait_leg_flag && mode == WALK_MODE && 
		old_plan_file.Start_Type () == LOCATION_ID && old_plan_file.End_Type () == STOP_ID) {

		last_acc = 30;
		org = location_data.Get_Index (old_plan_file.Start_ID ());
		stop = stop_data.Get_Index (old_plan_file.End_ID ());

		for (acc = transit_access [org]->From_List (); acc; acc = acc_ptr->From_List ()) {
			acc_ptr = access_list [acc];
			access_ptr = access_data [acc];

			if (access_ptr->To_ID () == stop) {
				last_acc = Resolve (access_ptr->Time ());
				break;
			}
		}
		temp_plan.Plan (old_plan_file.Plan ());

		last_wait = Round (temp_plan.Duration () - last_acc);
		last_time = start + last_acc;
		return (false);
	}

	//---- gather walk time ----

	if (attribute_flag && mode == WALK_MODE) {
		attribute_data.impedance += Round (old_plan_file.GCF ());
		attribute_data.walk += Round (old_plan_file.Duration ());
		attribute_data.cost += old_plan_file.Cost ();
	}

	//---- update the transit leg ----

	if (mode == TRANSIT_MODE) {
		if (!transit_net) return (true);

		line_ptr = line_data.Get (old_plan_file.Route ());
		if (line_ptr == NULL) {
			if (warn_flag) {
				Warning ("Traveler %d Route %d was Not Found", traveler, old_plan_file.Route ());
			}
			stat = Set_Problem (PATH_PROBLEM);
			goto transit_return;
		}

		//---- find the stop index ----

		board_id = old_plan_file.Start_ID ();

		board = stop_data.Get_Index (board_id);
		if (board == 0) {
			if (warn_flag) {
				Warning ("Traveler %d Route %d Stop %d was Not Found", traveler, old_plan_file.Route (), board_id);
			}
			stat = Set_Problem (PATH_PROBLEM);
			goto transit_return;
		}
		board_id = board;
		stop_pen = 0;

		if (attribute_flag) {
			stop_ptr = stop_data [board];
			if (stop_ptr != NULL) {
				if (stop_ptr->Type () == STOP) {
					stop_pen = stop_imped;
				} else {
					stop_pen = station_imped;
				}
			}
		}
		alight_id = old_plan_file.End_ID ();

		alight = stop_data.Get_Index (alight_id);
		if (alight == 0) {
			if (warn_flag) {
				Warning ("Traveler %d Route %d Stop %d was Not Found", traveler, old_plan_file.Route (), alight_id);
			}
			stat = Set_Problem (PATH_PROBLEM);
			goto transit_return;
		}
		flag = false;

		for (i=1; i <= line_ptr->Stops (); i++) {
			if (flag) {
				if (line_ptr->Stop (i) == alight) {
					alight = i;
					break;
				}

				//---- check for loop routes ----

				if (line_ptr->Stop (i) == board_id) {
					board = i;
				}
			} else {
				if (line_ptr->Stop (i) == board) {
					board = i;
					flag = true;
				}
			}
		}
		if (i > line_ptr->Stops ()) {
			if (warn_flag) {
				Warning ("Traveler %d Route %d from Stop %d to %d was Not Found", 
					traveler, line_ptr->Route (), old_plan_file.Start_ID (), old_plan_file.End_ID ());
			}
			stat = Set_Problem (PATH_PROBLEM);
			goto transit_return;
		}

		//---- find the best run ----

		start = Round (start);
		tod_b = penalty = 0;
		wait = -1;

		for (i=1; i <= line_ptr->Runs (); i++) {
			tod_b = line_ptr->Schedule (i, board);
			penalty = line_ptr->Penalty (i, board);

			wait = tod_b - start;
			if (wait >= min_wait) break;
		}

		//---- check the waiting time ----

		if (wait < min_wait) {
			if (wait > 0) {
				i = line_ptr->Runs ();
			} else {
				if (warn_flag) {
					Warning ("No Runs Start after %d at Stop %d on Route %d for Traveler %d", 
						Resolve (start), old_plan_file.Start_ID (), line_ptr->Route (), traveler);
				}
				stat = Set_Problem (WAIT_PROBLEM);
				goto transit_return;
			}
		}
		if (wait_flag && wait > max_wait) {
			if (warn_flag) {
				Warning ("Maximum Wait Time Exceeded for Traveler %d Route %d", 
					traveler, line_ptr->Route ());
			}
			stat = Set_Problem (WAIT_PROBLEM);
			//goto transit_return;
		}
		tod_a = line_ptr->Schedule (i, alight);

		value_waiting = (++last_xfer > 1) ? value_xfer : value_wait;

		//---- add wait to the walk leg ----

		if (!wait_leg_flag) {
			start += wait;
			last_time = Resolve (start);
			old_plan_file.Time (last_time);

			if (wait != last_wait) {
				temp_plan.Stop_Time (last_time);
				temp_plan.Duration (last_time - temp_plan.Time ());

				imp_b = (wait * value_waiting) >> value_roll;
				imp = (last_wait * value_waiting) >> value_roll;
				imp = Round (temp_plan.GCF ()) - imp + imp_b;
				if (imp < 1) imp = 1;
				temp_plan.GCF (Resolve (imp));
			} else {
				nlegs--;
			}
			if (attribute_flag) {
				attribute_data.impedance += Round (temp_plan.GCF ());
				attribute_data.walk += Round (temp_plan.Duration ());
				attribute_data.cost += temp_plan.Cost ();

				if (last_xfer > 1) {
					attribute_data.xfer_wait += wait;
				} else {
					attribute_data.first_wait += wait;
				}
			}
			wait = 0;
		}

		imp_b = ((wait * value_waiting + penalty + value_round) >> value_roll);

		imp = ((tod_a - tod_b) * value_time + value_round) >> value_roll;

		//---- get the fare ----

		cost_a = 0;

		if (fare_flag) {
			fare_rec.from_mode = (char) last_mode;
			fare_rec.type = (char) traveler_class;
			fare_rec.from_zone = (short) line_ptr->Zone (board);
			fare_rec.to_mode = (char) line_ptr->Mode ();
			fare_rec.period = (char) fare_data.Period (Resolve (tod_b));
			fare_rec.to_zone = (short) line_ptr->Zone (alight);

			fare_ptr = fare_data.Get (&fare_rec);

			if (fare_ptr != NULL) {
				cost_a = fare_ptr->fare;

				imp += ((cost_a * value_cost + value_round) >> value_roll);
			}
		}
	
		//---- apply bias ----

		if (line_ptr->Mode () > EXPRESS_BUS) {
			if (rail_bias_flag) {
				imp = (imp * rail_bias + rail_constant + 50) / 100;
				if (imp < 1) imp = 1;
			}
		} else if (bus_bias_flag) {
			imp = (imp * bus_bias + bus_constant + 50) / 100;
			if (imp < 1) imp = 1;
		}
		
		//---- randomize impedance ----

		wait_imp = value_waiting;

		if (random_flag) {
			variance = (imp * random_imped + 50) / 100;
			if (variance > 0) {
				tim = (int) (imp + variance * (random.Probability () - 0.5) + 0.5);
				wait_imp = (wait_imp * tim + (imp >> 1)) / imp;
				imp  = tim;
			}
		}
		if (imp < 1) imp = 1;
		//wait_imp = ((wait * wait_imp + value_round) >> value_roll);
		//imp += imp_b + wait_imp;

		imp += imp_b;

		//---- update the leg data ----

		last_time = Resolve (tod_a);
		last_mode = line_ptr->Mode ();

		old_plan_file.Stop_Time (last_time);
		old_plan_file.Duration (Resolve (tod_a - start));
		old_plan_file.Cost (cost_a);
		old_plan_file.GCF (Resolve (imp));
		nlegs_transit++;

		if (attribute_flag) {
			attribute_data.impedance += imp;
			attribute_data.transit += Round (old_plan_file.Duration ());
			attribute_data.cost += old_plan_file.Cost ();

			if (last_xfer > 1) {
				attribute_data.xfer_wait += wait;
				attribute_data.penalty += stop_pen + xfer_imped;
			} else {
				attribute_data.first_wait += wait;
				attribute_data.penalty += stop_pen;
			}
			attribute_data.num_xfer = last_xfer;
		}
		goto transit_return;
	}

	//---- update the drive legs ----

	if ((mode == AUTO_MODE || mode == CARPOOL_MODE) && old_plan_file.Driver_Flag ()) {
		veh_id = old_plan_file.Vehicle ();

		if (ignore_veh_flag) {
			veh_ptr = &default_vehicle;
			if (veh_id <= 0) veh_id = 1;
		} else {
			if (veh_id <= 0 || (veh_ptr = vehicle_data.Get (veh_id)) == NULL) {
				if (warn_flag) {
					Warning ("Traveler %d Vehicle %d was Not Found", traveler, veh_id);
				}
				stat = Set_Problem (VEHICLE_PROBLEM);
				goto path_problem;
			}
		}
		type = (Use_Type) veh_ptr->Type ();
		if (type == CAR) type = SOV;
		hov_flag = (type == HOV2 || type == HOV3 || type == HOV4);

		path = old_plan_file.Path (&num_path);

		if (node_flag) {
			num = num_path++;
		} else {
			num = num_path - 1;
		}
		if (num == 0) num = 1;

		last_dir = anode = in_link_dir = 0;
		tod = Round (start);
		cost_a = cum = 0;

		for (i=1; i <= num_path; i++) {

			//---- get the time period ----

			if (tod_flag) {
				period = ttime_data.Period (tod);
			}

			//---- origin parking lot ----

			offset = 0;

			if (i == 1) {		
				parking_ptr = parking_data.Get (old_plan_file.Start_ID ());
				if (parking_ptr == NULL) {
					if (warn_flag) {
						Warning ("Traveler %d Parking Lot %d was Not Found", traveler, old_plan_file.Start_ID ());
					}
					stat = Set_Problem (PATH_PROBLEM);
					goto path_problem;
				}
				offset = parking_ptr->Offset ();
			}
			if (i == num_path) {
				parking_ptr = parking_data.Get (old_plan_file.End_ID ());
				if (parking_ptr == NULL) {
					if (warn_flag) {
						Warning ("Traveler %d Parking Lot %d was Not Found", traveler, old_plan_file.End_ID ());
					}
					stat = Set_Problem (PATH_PROBLEM);
					goto path_problem;
				}
				offset = parking_ptr->Offset () - offset;
			}
			
			//---- identify the link direction ----

			if (node_flag) {		//---- node-based paths ----

				if (i == 1) {		//---- origin parking lot ----
					link_ptr = link_data [parking_ptr->Link ()];
					if (link_ptr == NULL) {
						if (warn_flag) {
							Warning ("Traveler %d Parking Link %d was Not Found", traveler, parking_ptr->Link ());
						}
						stat = Set_Problem (PATH_PROBLEM);
						goto path_problem;
					}
					node_id = node_data.Get_Index (*path);

					if (i == num_path) {
						if (offset >= 0) {
							dir = link_ptr->AB_Dir ();
						} else {
							dir = link_ptr->BA_Dir ();
							offset = -offset;
						}
					} else if (link_ptr->Bnode () == node_id) {
						dir = link_ptr->AB_Dir ();
						offset = link_ptr->Length () - offset;
					} else if (link_ptr->Anode () == node_id) {
						dir = link_ptr->BA_Dir ();
					} else {
						if (warn_flag) {
							Warning ("Traveler %d Node %d is not on Origin Link %d", 
								traveler, *path, link_ptr->Link ());
						}
						stat = Set_Problem (PATH_PROBLEM);
						goto path_problem;
					}
					if (dir == 0) {
						if (warn_flag) {
							Warning ("Traveler %d Link %d is Not Available", traveler, link_ptr->Link ());
						}
						stat = Set_Problem (PATH_PROBLEM);
						goto path_problem;
					}
				} else if (i == num_path) {		//---- destination parking lot ----
					link_ptr = link_data [parking_ptr->Link ()];
					if (link_ptr == NULL) {
						if (warn_flag) {
							Warning ("Traveler %d Parking Link %d was Not Found", traveler, parking_ptr->Link ());
						}
						stat = Set_Problem (PATH_PROBLEM);
						goto path_problem;
					}
					node_id = node_data.Get_Index (anode);

					if (link_ptr->Anode () == node_id) {
						dir = link_ptr->AB_Dir ();
					} else if (link_ptr->Bnode () == node_id) {
						dir = link_ptr->BA_Dir ();
						offset = link_ptr->Length () - offset;
					} else {
						if (warn_flag) {
							Warning ("Traveler %d Node %d is not on Destination Link %d", 
								old_plan_file.Traveler (), anode, link_ptr->Link ());
						}
						stat = Set_Problem (PATH_PROBLEM);
						goto path_problem;
					}
					if (dir == 0) {
						if (warn_flag) {
							Warning ("Traveler %d Link %d is Not Available", traveler, link_ptr->Link ());
						}
						stat = Set_Problem (PATH_PROBLEM);
						goto path_problem;
					}
				} else {
					ab_ptr = ab_key.Get (anode, *path);

					if (ab_ptr == NULL) {
						if (warn_flag) {
							Warning ("Traveler %d No Link between %d and %d", traveler, anode, *path);
						}
						stat = Set_Problem (PATH_PROBLEM);
						goto path_problem;
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
					if (warn_flag) {
						Warning ("Traveler %d Link %d is Not Found", traveler, lnk);
					}
					stat = Set_Problem (PATH_PROBLEM);
					goto path_problem;
				}
				offset = abs (offset);
				if (ab_flag) {
					dir = link_ptr->AB_Dir ();
					if (i == 1 && i != num_path) {
						offset = link_ptr->Length () - offset;
					}
				} else {
					dir = link_ptr->BA_Dir ();
					if (i != 1 && i == num_path) {
						offset = link_ptr->Length () - offset;
					}
				}
				if (dir == 0) {
					if (warn_flag) {
						Warning ("Traveler %d is Not Permitted on Link %d in %s Direction", traveler, lnk, ((ab_flag) ? "AB" : "BA"));
					}
					stat = Set_Problem (PATH_PROBLEM);
					goto path_problem;
				}
			}

			//---- get the data records ----
			
			penalty = pen_imp = 0;
			ttime_ptr = ttime_data [dir];
			connect_ptr = connect_time.Get (in_link_dir, ttime_ptr->Link_Dir ());
			in_link_dir = ttime_ptr->Link_Dir ();

			//---- link length ----

			if (offset > 0) {
				length = offset;
			} else {
				if (node_flag) {
					link_ptr = link_data [ttime_ptr->Link ()];
				}
				length = link_ptr->Length ();
			}

			//---- check for time of day prohibitions ----

			if (connect_ptr != NULL) {
				for (list = connect_ptr->TOD_List (); list; list = turn_ptr->TOD_List ()) {
					turn_ptr = tod_turn [list]; 

					if (turn_ptr->Start () <= tod && tod < turn_ptr->End ()) {
						if (turn_ptr->Use () == 0 || Use_Permission (turn_ptr->Use (), type)) break;
					}
				}
				if (list) {
					if (turn_ptr->Penalty () == 0) {
						if (warn_flag) {
							lnk = link_data [connect_ptr->In_Link ()]->Link ();
							dir = link_data [connect_ptr->Out_Link ()]->Link ();
							Warning ("Traveler %d has a Turn Prohibition from Link %d to %d", traveler, lnk, dir);
						}
						stat = Set_Problem (PATH_PROBLEM);
						//goto path_problem;
					}
					penalty = turn_ptr->Penalty ();
				}
			}

			//---- check for time of day restrictions ----

			use = link_ptr->Use ();
			hov_lanes = false;

			for (list = ttime_ptr->TOD_List (); list; list = use_ptr->TOD_List ()) {
				use_ptr = link_use_data [list]; 

				if (use_ptr->Start () <= tod && tod < use_ptr->End ()) {
					use = use_ptr->Use ();
					hov_lanes = (hov_flag && use_ptr->HOV_Lanes () > 0 && 
								use_ptr->HOV_Lanes () < use_ptr->Lanes ());
					break;
				}
			}

			//---- check the vehicle type ----

			if (!Use_Permission (use, type)) {
				if (warn_flag) {
					Warning ("Traveler %d has a Use Restriction on Link %d", traveler, link_ptr->Link ());
				}
				stat = Set_Problem (USE_PROBLEM);
				//goto path_problem;
			}

			//---- get direction data ----

			if (tod_flag && !hov_lanes) {
				time = ttime_ptr->TTime (period);
			} else {
				time = ttime_ptr->Time0 ();
			}

			//---- update the connection penalty -----

			if (tod_flag && connect_ptr != NULL) {
				penalty += connect_ptr->TTime (period);
			}
			if (penalty > 0) {
				pen_imp = (penalty * value_time + value_round) >> value_roll;
			} else if (penalty < 0) {
				pen_imp = -((-penalty * value_time + value_round) >> value_roll);
			}

			//---- add turn impedance penalty ----

			if (turn_flag && connect_ptr != NULL) {
				if (connect_ptr->Type () == LEFT) {
					imp = left_imped;
				} else if (connect_ptr->Type () == RIGHT) {
					imp = right_imped;
				} else if (connect_ptr->Type () == UTURN) {
					imp = uturn_imped;
				} else {
					imp = 0;
				}
				if (pen_imp <= 0) {
					pen_imp += imp;
				} else if (pen_imp < imp) {
					pen_imp = imp;
				}
			}

			//---- get cost data ----

			cost = 0;

			if (toll_flag) {
				for (list = ttime_ptr->Cost_List (); list; list = toll_ptr->TOD_List ()) {
					toll_ptr = toll_data [list]; 

					if (toll_ptr->Start () <= tod && tod < toll_ptr->End ()) {
						if (Use_Permission (toll_ptr->Use (), type)) {
							cost = toll_ptr->Toll ();
							break;
						}
					}
				}
			}
			cost_a += cost;

			//---- calculate the impedance ----

			imp_b = (link_ptr->Length () * value_distance + cost * value_cost + 
					time * value_time + value_round) >> value_roll;
			if (imp_b < 0) imp_b = 1;

			imp = imp_b + pen_imp;
			if (imp < 1) {
				imp = 1;
			}
			if (random_flag) {
				variance = (imp * random_imped + 50) / 100;
				if (variance > 0) {
					variance = (int) (imp + variance * (random.Probability () - 0.5) + 0.5);

					if (pen_imp == 0) {
						imp_b = variance;
					} else {
						imp_b = (imp_b * variance + (imp >> 1)) / imp;
						pen_imp = variance - imp_b;
					}
				}
			}

			//---- apply the offset factor ----

			if (offset > 0) {
				offset_factor = ((offset << offset_roll) + (link_ptr->Length () >> 1)) / link_ptr->Length ();

				time = (time * offset_factor + offset_round) >> offset_roll;
				if (time < 0) time = 1;

				imp = ((imp_b * offset_factor + offset_round) >> offset_roll) + pen_imp;
			} else {
				imp = imp_b + pen_imp;

			}

			//---- check the time and impedance ----

			time += penalty;
			if (time < 0 && penalty < 0) time = 1;

			tod += time;

			if (imp < 1) imp = 1;
			cum += imp;

			//---- process the link ----

			last_dir = dir;

			if (node_flag) {
				if (i < num_path) {
					anode = *path++;
				}
			} else {
				path++;
			}
		}

		//---- update the leg data ----

		last_time = Resolve (tod);

		old_plan_file.Stop_Time (last_time);
		old_plan_file.Duration (last_time - start);
		old_plan_file.Cost (cost_a);
		old_plan_file.GCF (Resolve (cum));
		nlegs_auto++;

		if (attribute_flag) {
			attribute_data.impedance += cum;
			attribute_data.drive += Round (old_plan_file.Duration ());
			attribute_data.cost += old_plan_file.Cost ();
		}
	}
	goto path_problem;

transit_return:
	if (!wait_leg_flag) {
		if (!plan_file.Write (temp_plan.Plan ())) {
			Error ("Writing Plan File");
		}
	}
path_problem:
	if (stat > 0 && problem_flag) {
		problem_file.Time (trip_time.Format_Step (old_plan_file.Time ()));
		problem_file.Link (0);
		problem_file.Household (old_plan_file.Household ());
		problem_file.Person (old_plan_file.Person ());
		problem_file.Trip (old_plan_file.Trip ());
		problem_file.Mode (old_plan_file.Mode ());
		problem_file.Problem (stat);
		problem_file.Start (trip_time.Format_Step (old_plan_file.Time ()));
		problem_file.Origin (old_plan_file.Start_ID ());
		problem_file.Arrive (trip_time.Format_Step (old_plan_file.Stop_Time ()));
		problem_file.Destination (old_plan_file.End_ID ());

		problem_file.Write ();
	}
	return (true);
}
