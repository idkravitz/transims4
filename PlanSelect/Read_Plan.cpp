//*********************************************************
//	Read_Plan.cpp - Read Each Plan File
//*********************************************************

#include "PlanSelect.hpp"

#include "In_Polygon.hpp"
#include "Utility.hpp"

//---------------------------------------------------------
//	Read_Plan
//---------------------------------------------------------

void PlanSelect::Read_Plan (void)
{
	int hh, hhold, num_hh, current_hhold, person, num_sel, cap, offset, dir, list, tod_cap, lanes;
	int id, id0, n1, n2, num, time, num_path, *path, skim, ttime, diff, ratio, min_vc, in_link_dir;
	int num_in, num_out, vehicle, board, alight, traveler, trip, current_person, current_trip;
	const char *rec_type;
	bool select, first, traveler_flag, location_flag, path_flag, time_flag, park_lot_flag, stop_flag;
	bool org_flag, des_flag, mid_flag, ie_flag, ncoord_flag, nin_flag, nout_flag, route_flag;
	bool link_flag, on_route, select_vc, select_ratio;
	double share, cap_factor;
	Box *coord_ptr, *in_ptr, *out_ptr;

	Person_Data *person_ptr;
	AB_Data *ab_ptr;
	TTime_Data *ttime_ptr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Parking_Data *parking_ptr;
	Link_Use_Data *use_ptr;
	Connect_Time *connect_ptr;
	Node_Data *node_ptr;
	Vehicle_Data *vehicle_ptr;
	Driver_Data *driver_ptr;
	Stop_Data *stop_ptr;
	HHRec_Index hh_rec, *hh_ptr;

	ie_flag = Report_Flag (SUBAREA_IO);

	if (coord_flag && coord_box.Num_Records () == 1) {
		ncoord_flag = false;
		coord_ptr = (Box *) coord_box.First ();
	}  else {
		ncoord_flag = true;
	}
	if (in_flag && od_box.Num_Records () == 1) {
		nin_flag = false;
		in_ptr = (Box *) od_box.First ();
	}  else {
		nin_flag = true;
	}
	if (out_flag && exclude_box.Num_Records () == 1) {
		nout_flag = false;
		out_ptr = (Box *) exclude_box.First ();
	}  else {
		nout_flag = true;
	}

	//---- set the filename and extension ----

	if (plan_file.Extend ()) {
		Show_Message ("Reading Plan File %s -- Plan", plan_file.Extension ());
	} else {
		Show_Message ("Reading Plan File -- Plan");
	}
	Set_Progress (10000);

	//---- read each plan record ----

	current_hhold = current_person = current_trip = num_hh = num_sel = 0;
	traveler_flag = (travelers.Num_Ranges () > 0);
	time_flag = (times.Num_Ranges () > 0);
	location_flag = (locations.Num_Ranges () > 0);
	park_lot_flag = (parking_lots.Num_Ranges () > 0);
	stop_flag = (transit_stops.Num_Ranges () > 0);
	route_flag = (transit_routes.Num_Ranges () > 0);
	path_flag = (nodes.Num_Records () > 0);
	link_flag = (links.Num_Records () > 0);

	if (vc_flag) {
		num = ttime_data.Period_Size ();
		cap_factor = num / 3600.0;

		min_vc = (int) (vc_ratio * 1000.0 + 0.5);
	}

	while (plan_file.Read ()) {
		Show_Progress ();

		//---- get the traveler id ----

		traveler = plan_file.Traveler ();
		trip = plan_file.Trip ();

		//---- convert the traveler id to household id ----

		if (pop_flag) {
			person_ptr = (Person_Data *) person_data.Get (&traveler);

			if (person_ptr == NULL) {
				Error ("Traveler %d was Not Found in the Population File", traveler);
			}
			hhold = person_ptr->hhold;
			person = 1;
		} else {
			hhold = traveler / Traveler_Scale ();
			person = traveler % Traveler_Scale ();
		}
		if (hhold <= 0) {
			Error ("Household ID %d is Out of Range", hhold);
		}
		if (current_hhold != hhold) {
			current_hhold = hhold;
			current_person = person;
			current_trip = trip;
			num_hh++;
		} else if (hhrec_flag) {
			if (current_person != person) {
				current_person = person;
				current_trip = trip;
				num_hh++;
			} else if (current_trip != trip) {
				current_trip = trip;
				num_hh++;
			}
		}

		//---- check the mode ----

		if (!mode_flag [plan_file.Mode ()]) continue;

		//---- check for travel activities ----

		if (plan_file.Start_Type () == LOCATION_ID && 
			plan_file.End_Type () == LOCATION_ID) continue;

		//---- check the traveler selection criteria ----

		if (traveler_flag) {
			if (!travelers.In_Range (traveler)) continue;
		}

		//---- check the vehicle type selection criteria ----

		if (vehicle_flag) {
			vehicle = plan_file.Vehicle ();
			if (vehicle <= 0) continue;
				
			vehicle_ptr = vehicle_data.Get (vehicle);
			if (vehicle_ptr == NULL) continue;

			if (!vehicle_types.In_Range (vehicle_ptr->Type ())) continue;
		}

		//---- check the time period criteria ----

		if (time_flag) {

			time = plan_file.Time ();
			
			//---- apply the time scaling factor ----

			if (!times.In_Range (time)) continue;
		}

		//---- check the activity location criteria ----

		if (location_flag) {
			num = 0;
			if (plan_file.Start_Type () == LOCATION_ID) {
				if (locations.In_Range (plan_file.Start_ID ())) {
					num++;
				}
			}
			if (plan_file.End_Type () == LOCATION_ID) {
				if (locations.In_Range (plan_file.End_ID ())) {
					num++;
				}
			}
			if (!num) continue;
		}

		//---- check the parking lot criteria ----

		if (park_lot_flag) {
			num = 0;
			if (plan_file.Start_Type () == PARKING_ID) {
				if (parking_lots.In_Range (plan_file.Start_ID ())) {
					num++;
				}
			}
			if (plan_file.End_Type () == PARKING_ID) {
				if (parking_lots.In_Range (plan_file.End_ID ())) {
					num++;
				}
			}
			if (!num) continue;
		}

		//---- check the transit stop criteria ----

		if (stop_flag) {
			num = 0;
			if (plan_file.Start_Type () == STOP_ID) {
				if (transit_stops.In_Range (plan_file.Start_ID ())) {
					num++;
				}
			}
			if (plan_file.End_Type () == STOP_ID) {
				if (transit_stops.In_Range (plan_file.End_ID ())) {
					num++;
				}
			}
			if (!num) continue;
		}

		//---- check the transit route criteria ----

		if (route_flag) {
			if (plan_file.Start_Type () != STOP_ID || plan_file.End_Type () != STOP_ID) continue;
			if (!transit_routes.In_Range (plan_file.Route ())) continue;
		}

		//---- od coordinate exclusion ----

		if (in_flag || out_flag) {
			path = plan_file.Path (&num_path);

			if (num_path > 0) {
				num_in = num_out = 0;

				for (n1=0; n1 < 2; n1++) {
					id = (n1 == 0) ? *path : path [num_path-1];

					//---- link list path ----

					if (!type_flag) {
						if (id < 0) {
							dir = 1;
							id = -id;
						} else {
							dir = 0;
						}
						link_ptr = link_data.Get (id);
						if (link_ptr == NULL) continue;

						if (dir == 0) {
							id = link_ptr->Bnode ();
						} else {
							id = link_ptr->Anode ();
						}
					}

					//---- check the node coordinate ----

					node_ptr = node_data.Get (id);
					if (node_ptr == NULL) continue;

					if (in_flag) {
						if (nin_flag) {
							for (in_ptr = (Box *) od_box.First (); in_ptr; in_ptr = (Box *) od_box.Next ()) {
								if (node_ptr->X () >= in_ptr->x1 && node_ptr->X () <= in_ptr->x2 &&
									node_ptr->Y () >= in_ptr->y1 && node_ptr->Y () <= in_ptr->y2) num_in++;
							}
						} else {
							if (node_ptr->X () >= in_ptr->x1 && node_ptr->X () <= in_ptr->x2 &&
								node_ptr->Y () >= in_ptr->y1 && node_ptr->Y () <= in_ptr->y2) num_in++;
						}
					}
					if (out_flag) {
						if (nout_flag) {
							for (out_ptr = (Box *) exclude_box.First (); out_ptr; out_ptr = (Box *) exclude_box.Next ()) {
								if (node_ptr->X () < out_ptr->x1 || node_ptr->X () > out_ptr->x2 ||
									node_ptr->Y () < out_ptr->y1 || node_ptr->Y () > out_ptr->y2) num_out++;
							}
						} else {
							if (node_ptr->X () < out_ptr->x1 || node_ptr->X () > out_ptr->x2 ||
								node_ptr->Y () < out_ptr->y1 || node_ptr->Y () > out_ptr->y2) num_out++;
						}
					}
				}
				if (in_flag && num_in < 1) continue;
				if (out_flag && num_out < 2) continue;
			}
		}

		//---- coordinate selection ----

		if (coord_flag) {
			path = plan_file.Path (&num_path);	

			while (num_path-- > 0) {
				id = *path++;

				//---- link list path ----

				if (!type_flag) {
					if (id < 0) {
						dir = 1;
						id = -id;
					} else {
						dir = 0;
					}
					link_ptr = link_data.Get (id);
					if (link_ptr == NULL) continue;

					if (dir == 0) {
						id = link_ptr->Bnode ();
					} else {
						id = link_ptr->Anode ();
					}
				}

				//---- check the node coordinate ----

				node_ptr = node_data.Get (id);
				if (node_ptr == NULL) continue;

				if (ncoord_flag) {
					for (coord_ptr = (Box *) coord_box.First (); coord_ptr; coord_ptr = (Box *) coord_box.Next ()) {
						if (node_ptr->X () >= coord_ptr->x1 && node_ptr->X () <= coord_ptr->x2 &&
							node_ptr->Y () >= coord_ptr->y1 && node_ptr->Y () <= coord_ptr->y2) break;
					}
					if (coord_ptr != NULL) break;
				} else {
					if (node_ptr->X () >= coord_ptr->x1 && node_ptr->X () <= coord_ptr->x2 &&
						node_ptr->Y () >= coord_ptr->y1 && node_ptr->Y () <= coord_ptr->y2) break;
				}
			}
			if (num_path < 0) continue;
		}

		//---- check the subarea ----

		if (subarea_flag) {
			path = plan_file.Path (&num_path);	
			org_flag = des_flag = mid_flag = select = false;
			first = true;

			while (num_path-- > 0) {
				id = *path++;

				//---- link list path ----

				if (!type_flag) {
					if (id < 0) {
						dir = 1;
						id = -id;
					} else {
						dir = 0;
					}
					link_ptr = link_data.Get (id);
					if (link_ptr == NULL) continue;

					if (dir == 0) {
						id = link_ptr->Bnode ();
					} else {
						id = link_ptr->Anode ();
					}
				}

				//---- check the node coordinate ----

				node_ptr = node_data.Get (id);
				if (node_ptr == NULL) continue;

				select = In_Polygon (UnRound (node_ptr->X ()), UnRound (node_ptr->Y ()), &select_subarea.points);

				if (ie_flag) {
					if (select) mid_flag = true;
					if (first) {
						org_flag = select;
						first = false;
					}
					des_flag = select;
				} else {
					if (select) break;
				}
			}
			if (ie_flag) {
				if (!mid_flag) continue;
				if (org_flag) {
					if (des_flag) {
						num_ii++;
					} else {
						num_ie++;
					}
				} else {
					if (des_flag) {
						num_ei++;
					} else {
						num_ee++;
					}
				}
			} else if (num_path < 0) {
				continue;
			}
		}

		//---- check the nodes groups ----

		if (path_flag) {
			select = false;
			Data_Range *range;

			if (plan_file.Mode () == TRANSIT_MODE) {
				if (!transit_flag) continue;

				driver_ptr = driver_data.Get (plan_file.Route ());

				if (driver_ptr == NULL) {
					Warning ("Traveler %d Route %d is not in the Driver File", plan_file.Traveler (), plan_file.Route ());
					continue;
				}
				stop_ptr = stop_data.Get (plan_file.Start_ID ());
				if (stop_ptr == NULL) continue;

				board = stop_ptr->Link ();
				if (stop_ptr->Dir () > 0) board = -board;

				stop_ptr = stop_data.Get (plan_file.End_ID ());
				if (stop_ptr == NULL) continue;

				alight = stop_ptr->Link ();
				if (stop_ptr->Dir () > 0) alight = -alight;

				//---- check for selected nodes ----

				for (range = (Data_Range *) nodes.First (); range != NULL && !select; range = (Data_Range *) nodes.Next ()) {
						
					num = range->Num_Ranges ();
					n1 = n2 = 0;
					on_route = false;

					for (int i=1; i <= driver_ptr->Links (); i++) {
						id = driver_ptr->Link (i);
						link_ptr = link_data.Get (id);

						if (link_ptr == NULL) continue;

						if (driver_ptr->Dir (i) > 0) id = -id;

						//---- check for stops ----

						if (board == id) on_route = true;
						if (!on_route) continue;
						if (alight == id) break;

						//---- check the node sequence ----

						if (id < 0) {
							n2 = range->In_Index (link_ptr->Anode ());
						} else {
							n2 = range->In_Index (link_ptr->Bnode ());
						}
						if (!n2) continue;
						if (n2 != n1 + 1) break;

						if (n2 == num) {
							select = true;
							break;
						}
						n1 = n2;
					}
				}

			} else {

				for (range = (Data_Range *) nodes.First (); range != NULL && !select; range = (Data_Range *) nodes.Next ()) {
						
					path = plan_file.Path (&num_path);	
					num = range->Num_Ranges ();
					n1 = n2 = 0;

					while (num_path-- > 0) {
						id = *path++;

						//---- link list path ----

						if (!type_flag) {
							if (id < 0) {
								dir = 1;
								id = -id;
							} else {
								dir = 0;
							}
							link_ptr = link_data.Get (id);
							if (link_ptr == NULL) continue;

							if (dir == 0) {
								id = link_ptr->Bnode ();
							} else {
								id = link_ptr->Anode ();
							}
						}

						//---- check the node sequence ----

						n2 = range->In_Index (id);

						if (!n2) continue;

						if (n2 != n1 + 1) {
							n2 = 0;
							break;
						}
						if (n2 == num) {
							select = true;
							break;
						}
						n1 = n2;
					}
				}
			}
			if (!select) continue;
		}

		//---- check the link groups ----

		if (link_flag) {
			select = false;
			Data_Range *range;

			if (plan_file.Mode () == TRANSIT_MODE) {
				if (!transit_flag) continue;

				driver_ptr = driver_data.Get (plan_file.Route ());

				if (driver_ptr == NULL) {
					Warning ("Traveler %d Route %d is not in the Driver File", plan_file.Traveler (), plan_file.Route ());
					continue;
				}
				stop_ptr = stop_data.Get (plan_file.Start_ID ());
				if (stop_ptr == NULL) continue;

				board = stop_ptr->Link ();
				if (stop_ptr->Dir () > 0) board = -board;

				stop_ptr = stop_data.Get (plan_file.End_ID ());
				if (stop_ptr == NULL) continue;

				alight = stop_ptr->Link ();
				if (stop_ptr->Dir () > 0) alight = -alight;

				//---- check for selected links ----

				for (range = (Data_Range *) links.First (); range != NULL && !select; range = (Data_Range *) links.Next ()) {
						
					num = range->Num_Ranges ();
					n1 = n2 = 0;
					on_route = false;

					for (int i=1; i <= driver_ptr->Links (); i++) {
						id = driver_ptr->Link (i);
						if (driver_ptr->Dir (i) > 0) id = -id;

						//---- check for stops ----

						if (board == id) on_route = true;
						if (!on_route) continue;
						if (alight == id) on_route = false;

						//---- check the link sequence ----

						n2 = range->In_Index (id);

						if (!n2) continue;
						if (n2 != n1 + 1) break;

						if (n2 == num) {
							select = true;
							break;
						}
						if (!on_route) break;
						n1 = n2;
					}
				}

			} else {

				for (range = (Data_Range *) links.First (); range != NULL && !select; range = (Data_Range *) links.Next ()) {
						
					path = plan_file.Path (&num_path);	
					num = range->Num_Ranges ();
					n1 = n2 = id0 = 0;

					while (num_path-- > 0) {
						id = *path++;

						//---- node list path ----

						if (type_flag) {
							if (id0 == 0) {
								id0 = id;
								continue;
							}
							ab_ptr = ab_key.Get (id0, id);
							id0 = id;

							if (ab_ptr == NULL) continue;

							dir_ptr = dir_data [ab_ptr->dir];
							id = dir_ptr->Link ();
							if (dir_ptr->Dir () > 0) id = -id;
						}

						//---- check the link sequence ----

						n2 = range->In_Index (id);

						if (!n2) continue;

						if (n2 != n1 + 1) {
							n2 = 0;
							break;
						}
						if (n2 == num) {
							select = true;
							break;
						}
						n1 = n2;
					}
				}
			}
			if (!select) continue;
		}

		//---- check the facility types ----

		if (factype_flag && plan_file.Start_Type () == PARKING_ID && plan_file.End_Type () == PARKING_ID) {

			select = false;

			//---- get the parking links ----

			if (parking_flag) {
				parking_ptr = parking_data.Get (plan_file.Start_ID ());

				if (parking_ptr != NULL) {
					link_ptr = link_data.Get (parking_ptr->Link ());

					if (link_ptr != NULL) {
						if (facility_flag [link_ptr->Type ()]) {
							select = true;
						}
					}
				}
				if (!select) {
					parking_ptr = parking_data.Get (plan_file.End_ID ());

					if (parking_ptr != NULL) {
						link_ptr = link_data.Get (parking_ptr->Link ());

						if (link_ptr != NULL) {
							if (facility_flag [link_ptr->Type ()]) {
								select = true;
							}
						}
					}
				}
			}

			//---- check the path links ----

			n1 = 0;
			path = plan_file.Path (&num_path);

			while (!select && num_path-- > 0) {
				n2 = *path++;

				//---- get the link direction ----

				if (type_flag) {
					link_ptr = NULL;

					if (n1 != 0) {
						ab_ptr = ab_key.Get (n1, n2);

						if (ab_ptr != NULL) {
							ttime_ptr = ttime_data [ab_ptr->dir];

							if (ttime_ptr != NULL) {
								link_ptr = link_data.Get (ttime_ptr->Link ());
							}
						}
					}
					n1 = n2;
				} else {
					link_ptr = link_data.Get (abs (n2));
				}
				if (link_ptr != NULL) {
					if (facility_flag [link_ptr->Type ()]) {
						select = true;
					}
				}
			}
			if (!select) continue;
		}

		//---- apply the path-based selection criteria to drive legs ----

		if (vc_flag || diff_flag || ratio_flag) {
			if (plan_file.Start_Type () != PARKING_ID ||
				plan_file.End_Type () != PARKING_ID) continue;

			time = Round (plan_file.Time ());

			skim = n1 = in_link_dir = 0;
			path = plan_file.Path (&num_path);
			first = true;

			select_vc = !vc_flag;
			select_ratio = !ratio_flag;

			//---- get the travel time from the origin parking lot ----

			if (parking_flag) {
				parking_ptr = parking_data.Get (plan_file.Start_ID ());

				if (parking_ptr != NULL) {
					link_ptr = link_data.Get (parking_ptr->Link ());

					if (link_ptr != NULL) {
						if ((type_flag && *path == link_ptr->Anode ()) || *path < 0) {
							offset = parking_ptr->Offset ();
							dir = link_ptr->BA_Dir ();
						} else {
							offset = link_ptr->Length () - parking_ptr->Offset ();
							dir = link_ptr->AB_Dir ();
						}
						if (dir > 0) {
							ttime_ptr = ttime_data [dir];

							num = ttime_data.Period (Resolve (time));

							ttime = ttime_ptr->TTime (num) * offset / link_ptr->Length ();

							skim += ttime;
							time += ttime;
							first = false;

							in_link_dir = ttime_ptr->Link_Dir ();
						}
					}
				}
			}

			//---- re-skim the path with current travel times ----

			while ((!select_vc || !select_ratio || diff_flag) && num_path-- > 0) {
				n2 = *path++;

				if (n1 != 0) {

					//---- get the link direction ----

					if (type_flag) {
						ab_ptr = ab_key.Get (n1, n2);

						if (ab_ptr == NULL) {
							Error ("Anode-Bnode %d-%d was Not Found in the Link File", n1, n2);
						}
						dir = ab_ptr->dir;
					} else {
						link_ptr = link_data.Get (abs (n2));

						if (link_ptr == NULL) {
							Error ("Link %d was Not Found in the Link File", abs (n2));
						}
						if (n2 < 0) {
							dir = link_ptr->BA_Dir ();
						} else {
							dir = link_ptr->AB_Dir ();
						}
					}
					if (dir == 0) {
						n1 = n2;
						continue;
					}

					//---- get the link travel time ----

					ttime_ptr = ttime_data [dir];

					num = ttime_data.Period (Resolve (time));

					ttime = ttime_ptr->TTime (num);

					//---- check the time ratio ----

					if (ratio_flag) {
						share = (double) ttime / ttime_ptr->Time0 ();
						if (share > time_ratio) select_ratio = true;
					}

					//---- add turning movement delay ----

					if (turn_flag) {
						connect_ptr = connect_time.Get (in_link_dir, ttime_ptr->Link_Dir ());
						in_link_dir = ttime_ptr->Link_Dir ();

						if (connect_ptr != NULL && connect_ptr->Periods () > 0) {
							ttime += connect_ptr->TTime (num);
							if (ttime < 1) ttime = 1;
						}
					}

					//---- check the V/C ratio ----

					if (vc_flag) {
						cap = (int) (ttime_ptr->Capacity () * cap_factor + 0.5);

						if (cap > 0) {
							list = ttime_ptr->TOD_List ();

							for (; list > 0; list = use_ptr->TOD_List ()) {
								use_ptr = link_use_data [list]; 

								if (use_ptr->Start () <= time && time < use_ptr->End ()) {
									lanes = ttime_ptr->Thru ();
									if (lanes < 1) lanes = 1;

									tod_cap = (cap * use_ptr->Lanes () + lanes / 2) / lanes;
									if (tod_cap == 0) tod_cap = cap / 2;
									if (tod_cap < 1) tod_cap = 1;
									cap = tod_cap;
									break;
								}
							}
							ratio = (1000 * ttime_ptr->Volume (num) + (cap >> 1)) / cap;
							if (ratio > min_vc) select_vc = true;
						}
					}
					if (first) {
						first = false;
						ttime += (ttime >> 1);
					}
					skim += ttime;
					time += ttime;
				}
				n1 = n2;
			}
			if (!select_vc || !select_ratio) continue;

			//---- travel time difference ----

			if (diff_flag) {

				//---- get the travel time to the destination parking lot ----

				if (parking_flag) {
					parking_ptr = parking_data.Get (plan_file.End_ID ());

					if (parking_ptr != NULL) {
						link_ptr = link_data.Get (parking_ptr->Link ());

						if (link_ptr != NULL) {
							if (type_flag) {
								if (n1 == link_ptr->Anode ()) {
									offset = parking_ptr->Offset ();
									dir = link_ptr->AB_Dir ();
								} else {
									offset = link_ptr->Length () - parking_ptr->Offset ();
									dir = link_ptr->BA_Dir ();
								}
								if (dir > 0) {

									//---- get the link travel time ----

									ttime_ptr = ttime_data [dir];

									num = ttime_data.Period (Resolve (time));

									ttime = ttime_ptr->TTime (num) * offset / link_ptr->Length ();

									//---- add turning movement delay ----

									if (turn_flag) {
										connect_ptr = connect_time.Get (in_link_dir, ttime_ptr->Link_Dir ());

										if (connect_ptr != NULL && connect_ptr->Periods () > 0) {
											ttime += connect_ptr->TTime (num);
											if (ttime < 1) ttime = 1;
										}
									}
								}

							} else {

								skim -= ttime;
								time -= ttime;

								if (n1 > 0) {
									offset = parking_ptr->Offset ();
								} else {
									offset = link_ptr->Length () - parking_ptr->Offset ();
								}
								ttime = ttime * offset / link_ptr->Length ();
							}
							skim += ttime;
							time += ttime;
							first = true;
						}
					}
				}
				if (!first) {
					skim += (ttime >> 1);
				}
				skim = Resolve (skim);

				//---- compare to the plan duration ----

				time = plan_file.Duration ();

				//---- check the selection criteria ----

				diff = abs (time - skim);

				if (diff < min_time) continue;

				if (diff < max_time) {
					if (time > 0) {
						share = (double) diff / (double) time;
					} else {
						share = 1.0;
					}
					if (share < percent_diff) continue;
				}
			}
		}

		//---- add the household to the list ----

		if (hhrec_flag) {
			hh_rec.Household (hhold);
			hh_rec.Person (person);
			hh_rec.Record (trip);

			if (!hhrec_data.Get_Index (&hh_rec)) {
				num_sel++;
				if (!hhrec_data.Add (&hh_rec)) {
					Error ("Adding Record to the Selection List");
				}
			}
		} else {
			if (!hhold_list.Get_Index (hhold)) {
				num_sel++;
				if (!hhold_list.Add (hhold)) {
					Error ("Adding Household to the List");
				}
			}
		}
	}
	End_Progress ();

	plan_file.Close ();

	rec_type = ((hhrec_flag) ? "Trips" : "Households");

	Print (2, "Plan File: %s", plan_file.Filename ());
	Print (1, "\tTotal Number of %s = %d", rec_type, num_hh);

	if (num_hh < 1) num_hh = 1;

	Print (1, "\tNumber of %s Selected = %d (%.1lf%%)", rec_type,
		num_sel, (100.0 * num_sel / num_hh));

	//---- select the households to re-route ----

	select = select_flag;
	share = percent;

	//---- check the maximum selection ----

	if (num_sel * percent / num_hh > max_percent) {
		select = true;
		share = max_percent * num_hh / num_sel;
	}

	//---- write the selected household ids ----

	num = 0;

	if (hhrec_flag) {
		for (hh_ptr = hhrec_data.First_Key (); hh_ptr; hh_ptr = hhrec_data.Next_Key ()) {
			if (select) {
				if (random.Probability () > share) continue;
			}
			hhrec_file.Household (hh_ptr->Household ());
			hhrec_file.Person (hh_ptr->Person ());
			hhrec_file.Record (hh_ptr->Record ());

			if (!hhrec_file.Write ()) {
				Error ("Writing %s", hhrec_file.Filename ());
			}
			num_hhold++;
			num++;
		}
	} else {
		for (hh = hhold_list.First (); hh != 0; hh = hhold_list.Next ()) {
			if (select) {
				if (random.Probability () > share) continue;
			}
			fprintf (hhlist_file.File (), "%d\n", hh);
			num_hhold++;
			num++;
		}
	}

	Print (1, "\tNumber of %s Written = %d (%.1lf%%)", rec_type,
		num, (100.0 * num / num_hh));
}
