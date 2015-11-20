//*********************************************************
//	Drive_Path.cpp - Build a Drive Path
//*********************************************************

#include "Router.hpp"

#include "math.h"

//---------------------------------------------------------
//	Drive_Path
//---------------------------------------------------------

int Router::Drive_Path (Use_Type type, bool best_flag)
{
	int lnk, offset, offset_factor, period, approach, dir, use, period_size, half_period, period1;
	unsigned cum_a, cum_b, max_cum, hi_cum;
	int tod_a, tod_b, cost, cost_a, cost_b, time_a, time_b, penalty, pen_imp, pen, pen1;
	int imp, imped, tim, time, variance, best_des, list, time1;
	int x0, y0, x1, y1, x2, y2, dist, max_dist;
	double dx, dy, factor, factor1;
	bool ab_flag, des_flag, hov_flag, hov_lanes, local;
	bool restrict_flag, restrict_in, restrict_org, restrict_des;

	Trip_End_Data *org_ptr, *des_ptr;
	Node_Data *node_ptr;
	Link_Data *link_ptr;
	TTime_Data *ttime_ptr;
	Link_Use_Data *use_ptr;
	TOD_Turn_Data *turn_ptr = 0;
	Connect_Time *connect_ptr;
	Path_Data *path_ptr, path_root;
	Toll_Data *toll_ptr;
	
	if (type == CAR) type = SOV;
	hov_flag = (type == HOV2 || type == HOV3 || type == HOV4);
	local = true;
	period = period1 = x0 = y0 = x1 = y1 = max_dist = 0;
	period_size = ttime_data.Row_Size ();
	half_period = period_size / 2;
	factor = factor1 = 0.5;

	//---- initialize the org data ----

	org_ptr = org_array.First ();

	link_ptr = link_data [org_ptr->Link ()];

	if (!Use_Permission (link_ptr->Use (), type)) {
		use_flag = true;
		return (0);
	}
	restrict_org = Use_Permission (link_ptr->Use (), RESTRICTED);
	restrict_des = restrict_in = false;

	cum_a = org_ptr->Cum ();
	tod_a = org_ptr->TOD ();
	cost_a = org_ptr->Cost ();
	time_a = org_ptr->Time ();

	cost_b = cost_a;
	time_b = time_a;

	if (tod_flag) {
		period = ttime_data.Period (tod_a);
		if (interpolate_flag) {
			period1 = (tod_a % period_size);
			if (period1 < half_period) {
				if (period1 <= 1) {
					factor = factor1 = 0.5;
					period1 = 1;
				} else {
					factor = (double) (half_period + period1) / period_size;
					factor1 = 1.0 - factor;
					period1 = period - 1;
				}
			} else {
				if (period1 >= ttime_data.Periods ()) {
					factor = factor1 = 0.5;
					period1 = period;
				} else {
					factor = (double) (2 * half_period - period1) / period_size;
					factor1 = 1.0 - factor;
					period1 = period + 1;
				}
			}
		}
	}

	best_des = 0;
	max_cum = MAX_INTEGER;

	//---- check for org and des on the same link ----

	des_flag = false;

	for (des_ptr = des_array.First (); des_ptr != NULL; des_ptr = des_array.Next ()) {
		if (org_ptr->Link () != des_ptr->Link ()) continue;

		//---- get direction and distance ----
		
		if (org_ptr->Offset () <= des_ptr->Offset ()) {
			dir = link_ptr->AB_Dir ();
			offset = des_ptr->Offset () - org_ptr->Offset ();
			ab_flag = true;
		} else {
			dir = link_ptr->BA_Dir ();
			offset = org_ptr->Offset () - des_ptr->Offset ();
			ab_flag = false;
		}
		if (dir == 0) {
			zero_flag = true;
			des_ptr->Link (0);
			continue;
		}
		offset_factor = ((offset << offset_roll) + (link_ptr->Length () >> 1)) / link_ptr->Length ();

		ttime_ptr = ttime_data [dir];

		//---- update delay ----

		if (tod_flag) {
			time = ttime_ptr->TTime (period);
			if (interpolate_flag) {
				time1 = ttime_ptr->TTime (period1);
				time = (int) (time * factor + time1 * factor1 + 0.5);
			}
		} else {
			time = ttime_ptr->Time0 ();
		}

		//---- get cost data ----

		cost = 0;

		if (toll_flag) {
			for (list = ttime_ptr->Cost_List (); list; list = toll_ptr->TOD_List ()) {
				toll_ptr = toll_data [list]; 

				if (toll_ptr->Start () <= tod_a && tod_a < toll_ptr->End ()) {
					if (Use_Permission (toll_ptr->Use (), type)) {
						cost = toll_ptr->Toll ();
						break;
					}
				}
			}
		}

		imped = (link_ptr->Length () * value_distance + cost * value_cost + 
				time * value_time + value_round) >> value_roll;
		if (imped < 0) continue;

		if (random_flag) {
			variance = (imped * random_imped + 50) / 100;
			if (variance > 0) {
				imped = (int) (imped + variance * (random.Probability () - 0.5) + 0.5);
			}
		}

		//---- calculate time ---

		tim = (time * offset_factor + offset_round) >> offset_roll;

		tod_b = tod_a + tim;

		if (tod_b < tod_a || tod_b > max_tod) {
			time_flag = true;
			continue;
		}

		//---- adjust the link impedance ----

		imp = (imped * offset_factor + offset_round) >> offset_roll;
		if (imp < 1) {
			if (imp < 0) continue;
			imp = 1;
		}

		////---- get the parking cost ----

		//cost = des_ptr->Parking ();

		//if (cost > 0) {
		//	imp += (cost * value_cost + value_round) >> value_roll;
		//	if (imp < 1) {
		//		if (imp < 0) continue;
		//		imp = 1;
		//	}
		//}

		//---- check the cumulative impedance ----
		
		cum_b = cum_a + imp;

		if (cum_b < 0 || cum_b >= max_cum || (cum_b >= des_ptr->Cum () && des_ptr->Cum ())) continue;

		des_ptr->Cum (cum_b);
		des_ptr->TOD (tod_b);
		des_ptr->Cost (cost_b);
		des_ptr->Time (time_b);
		des_ptr->Prev (dir);

		link_path [dir]->Prev (0);

		des_flag = true;

		if (best_flag) {
			max_cum = cum_b;
			best_des = des_array.Record_Index ();
		}
	}

	//---- update the max cum ----

	if (best_flag) {
		if (best_des > 0) return (best_des);
		if (zero_flag) return (0);
	} else if (des_flag || zero_flag) {
		hi_cum = 0;

		for (des_ptr = des_array.First (); des_ptr != NULL; des_ptr = des_array.Next ()) {
			if (des_ptr->Link () > 0 && des_ptr->Cum () > hi_cum) hi_cum = des_ptr->Cum ();
		}
		if (hi_cum == 0) return (0);
		if (hi_cum < max_cum) max_cum = hi_cum;
	}

	//---- initialize the path building ----

	link_path.Zero_Fill ();

	first_ptr = last_ptr = &path_root;

	first_ptr->Next (0);

	//---- process each direction ----

	for (lnk=0; lnk < 2; lnk++) {

		//---- get direction data ----

		if (lnk == 0) {
			dir = link_ptr->AB_Dir ();
			offset = link_ptr->Length () - org_ptr->Offset ();
		} else {
			dir = link_ptr->BA_Dir ();
			offset = org_ptr->Offset ();
		}
		if (dir == 0) continue;

		offset_factor = ((offset << offset_roll) + (link_ptr->Length () >> 1)) / link_ptr->Length ();

		ttime_ptr = ttime_data [dir];

		//---- check time of day restrictions ----

		hov_lanes = false;

		if (ttime_ptr->TOD_List () > 0) {
			use = link_ptr->Use ();

			for (list = ttime_ptr->TOD_List (); list; list = use_ptr->TOD_List ()) {
				use_ptr = link_use_data [list]; 

				if (use_ptr->Start () <= tod_a && tod_a < use_ptr->End ()) {
					use = use_ptr->Use ();
					hov_lanes = (hov_flag && use_ptr->HOV_Lanes () > 0 && 
								use_ptr->HOV_Lanes () < use_ptr->Lanes ());
					break;
				}
			}
			if (!Use_Permission (use, type)) {
				use_flag = true;
				continue;
			}
		}
		
		//---- update delay ----

		if (tod_flag && !hov_lanes) {
			time = ttime_ptr->TTime (period);
			if (interpolate_flag) {
				time1 = ttime_ptr->TTime (period1);
				time = (int) (time * factor + time1 * factor1 + 0.5);
			}
		} else {
			time = ttime_ptr->Time0 ();
		}

		//---- calculate time ----

		tim = (time * offset_factor + offset_round) >> offset_roll;

		//---- check the time schedule ----

		tod_b = tod_a + tim;

		if (tod_b < tod_a || tod_b > max_tod) {
			time_flag = true;
			continue;
		}

		//---- get cost data ----

		cost = 0;

		if (toll_flag) {
			for (list = ttime_ptr->Cost_List (); list; list = toll_ptr->TOD_List ()) {
				toll_ptr = toll_data [list]; 

				if (toll_ptr->Start () <= tod_a && tod_a < toll_ptr->End ()) {
					if (Use_Permission (toll_ptr->Use (), type)) {
						cost = toll_ptr->Toll ();
						break;
					}
				}
			}
		}

		//---- check the cumulative impedance ----

		imped = (link_ptr->Length () * value_distance + cost * value_cost + 
				time * value_time + value_round) >> value_roll;
		if (imped < 0) 	continue;
	
		imp = (imped * offset_factor + offset_round) >> offset_roll;
		if (imp < 1) {
			if (imp < 0) continue;
			imp = 1;
		}

		cum_b = cum_a + imp;

		path_ptr = link_path [dir];

		if (cum_b < 0 || cum_b >= max_cum || (cum_b >= path_ptr->Cum () && path_ptr->Cum ())) continue;

		if (path_ptr->Next () == 0 && last_ptr != path_ptr) {
			last_ptr->Next (dir);
			last_ptr = path_ptr;
		}			
		path_ptr->Cum (cum_b);
		path_ptr->TOD (tod_b);
		path_ptr->Cost (cost_b);
		path_ptr->Time (time_b);
		path_ptr->Prev (-1);
	}
	if (first_ptr->Next () == 0) return (0);

	//---- calculate the org-des distance ----

	if (distance_flag || local_flag) {
		offset = org_ptr->Offset ();
		offset_factor = ((offset << offset_roll) + (link_ptr->Length () >> 1)) / link_ptr->Length ();

		node_ptr = node_data [link_ptr->Anode ()];

		x0 = node_ptr->X ();
		y0 = node_ptr->Y ();

		node_ptr = node_data [link_ptr->Bnode ()];

		x0 += ((node_ptr->X () - x0) * offset_factor + offset_round) >> offset_roll;
		y0 += ((node_ptr->Y () - y0) * offset_factor + offset_round) >> offset_roll;

		//---- find the furthest destination ----

		max_dist = 0;
		x1 = y0;
		y1 = y0;

		for (des_ptr = des_array.First (); des_ptr != NULL; des_ptr = des_array.Next ()) {
			if (des_ptr->Link () == 0) continue;

			link_ptr = link_data [des_ptr->Link ()];

			offset = des_ptr->Offset ();
			offset_factor = ((offset << offset_roll) + (link_ptr->Length () >> 1)) / link_ptr->Length ();

			node_ptr = node_data [link_ptr->Anode ()];

			x2 = node_ptr->X ();
			y2 = node_ptr->Y ();

			node_ptr = node_data [link_ptr->Bnode ()];

			x2 += ((node_ptr->X () - x2) * offset_factor + offset_round) >> offset_roll;
			y2 += ((node_ptr->Y () - y2) * offset_factor + offset_round) >> offset_roll;

			dx = x0 - x2;
			dy = y0 - y2;

			dist = (int) (sqrt (dx * dx + dy * dy) + 0.5);

			if (dist > max_dist) {
				x1 = x2;
				y1 = y2;
				max_dist = dist;
			}
		}
		dist = max_dist;
		max_dist = (dist * max_ratio + 50) / 100;

		if (max_dist < min_distance) {
			max_dist = min_distance;
		} else if (max_dist > max_distance) {
			max_dist = max_distance;
		}
		max_dist += dist;
	}

	//---- initialize the restricted access ----

	for (des_ptr = des_array.First (); des_ptr; des_ptr = des_array.Next ()) {
		if (des_ptr->Link () > 0) {
			link_ptr = link_data [des_ptr->Link ()];

			if (Use_Permission (link_ptr->Use (), RESTRICTED)) {
				restrict_des = true;
				break;
			}
		}
	}
	restrict_flag = (restrict_org || restrict_des);

	//---- build the path ----

	for (;;) {
		approach = first_ptr->Next ();
		if (approach <= 0) break;

		first_ptr->Next (-1);

		first_ptr = link_path [approach];

		//---- check the cumulative impedance ----

		cum_a = first_ptr->Cum ();

		if (cum_a >= max_cum) continue;

		//---- check the activity schedule ----

		tod_a = first_ptr->TOD ();

		if (tod_a < 0 || tod_a >= max_tod) {
			time_flag = true;
			continue;
		}
		time_a = first_ptr->Time ();
		cost_a = first_ptr->Cost ();

		time_b = time_a;

		if (tod_flag) {
			period = ttime_data.Period (tod_a);
			if (interpolate_flag) {
				period1 = (tod_a % period_size);
				if (period1 < half_period) {
					if (period1 <= 1) {
						factor = factor1 = 0.5;
						period1 = 1;
					} else {
						factor = (double) (half_period + period1) / period_size;
						factor1 = 1.0 - factor;
						period1 = period - 1;
					}
				} else {
					if (period1 >= ttime_data.Periods ()) {
						factor = factor1 = 0.5;
						period1 = period;
					} else {
						factor = (double) (2 * half_period - period1) / period_size;
						factor1 = 1.0 - factor;
						period1 = period + 1;
					}
				}
			}
		}

		//---- check the circuity or restrictions ----

		if (distance_flag || local_flag || restrict_flag) {
			ttime_ptr = ttime_data [approach];

			link_ptr = link_data [ttime_ptr->Link ()];

			if (restrict_flag) {
				restrict_in = Use_Permission (link_ptr->Use (), RESTRICTED);
			}
			if (distance_flag || local_flag) {
				if (ttime_ptr->Dir ()) {
					node_ptr = node_data [link_ptr->Anode ()];
				} else {
					node_ptr = node_data [link_ptr->Bnode ()];
				}

				x2 = node_ptr->X ();
				y2 = node_ptr->Y ();

				dx = x0 - x2;
				dy = y0 - y2;

				dist = (int) (sqrt (dx * dx + dy * dy) + 0.5);

				local = (local_flag && dist <= local_distance);

				dx = x1 - x2;
				dy = y1 - y2;

				dir = (int) (sqrt (dx * dx + dy * dy) + 0.5);

				if (local_flag && dir <= local_distance) local = true;

				dist += dir;

				if (distance_flag && dist > max_dist) {
					dist_flag = true;
					continue;
				}
			}
		}

		//---- process each link leaving the approach link ----

		for (dir = drive_list [approach]; dir; dir = connect_ptr->Next_Rec ()) {

			penalty = pen_imp = 0;
			connect_ptr = connect_time [dir];

			//---- check for time of day prohibitions ----

			for (list = connect_ptr->TOD_List (); list; list = turn_ptr->TOD_List ()) {
				turn_ptr = tod_turn [list]; 

				if (turn_ptr->Start () <= tod_a && tod_a < turn_ptr->End ()) {
					if (turn_ptr->Use () == 0 || Use_Permission (turn_ptr->Use (), type)) break;
				}
			}
			if (list) {
				if (turn_ptr->Penalty () == 0) continue;
				penalty = turn_ptr->Penalty ();
			}

			//--- get the link ---

			dir = connect_ptr->Dir_Index ();

			ttime_ptr = ttime_data [dir];

			lnk = ttime_ptr->Link ();
			ab_flag = (ttime_ptr->Dir () == 0);

			link_ptr = link_data [lnk];

			//---- check the local access restrictions ----

			if (!local && link_ptr->Type () >= local_type && link_ptr->Type () <= LOCAL) {
				//**** problem flag ****
				continue;
			}

			//---- check for time of day restrictions ----

			use = link_ptr->Use ();
			hov_lanes = false;

			for (list = ttime_ptr->TOD_List (); list; list = use_ptr->TOD_List ()) {
				use_ptr = link_use_data [list]; 

				if (use_ptr->Start () <= tod_a && tod_a < use_ptr->End ()) {
					use = use_ptr->Use ();
					hov_lanes = (hov_flag && use_ptr->HOV_Lanes () > 0 && 
								use_ptr->HOV_Lanes () < use_ptr->Lanes ());
					break;
				}
			}

			//---- check the vehicle type ----

			if (!Use_Permission (use, type)) {
				use_flag = true;
				continue;
			}

			//---- get direction data ----

			if (tod_flag && !hov_lanes) {
				time = ttime_ptr->TTime (period);
				if (interpolate_flag) {
					time1 = ttime_ptr->TTime (period1);
					time = (int) (time * factor + time1 * factor1 + 0.5);
				}
			} else {
				time = ttime_ptr->Time0 ();
			}

			//---- update the connection penalty -----

			if (tod_flag) {
				pen = connect_ptr->TTime (period);
				if (interpolate_flag) {
					pen1 = connect_ptr->TTime (period1);
					pen = (int) (pen * factor + pen1 * factor1 + 0.5);
				}
				penalty += pen;
			}
			if (penalty > 0) {
				pen_imp = (penalty * value_time + value_round) >> value_roll;
			} else if (penalty < 0) {
				pen_imp = -((-penalty * value_time + value_round) >> value_roll);
			}

			//---- add turn impedance penalty ----

			if (turn_flag) {
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

					if (toll_ptr->Start () <= tod_a && tod_a < toll_ptr->End ()) {
						if (Use_Permission (toll_ptr->Use (), type)) {
							cost = toll_ptr->Toll ();
							break;
						}
					}
				}
			}

			//---- calculate the impedance ----

			imped = (link_ptr->Length () * value_distance + cost * value_cost + 
					time * value_time + value_round) >> value_roll;
			if (imped < 0) continue;

			imp = imped + pen_imp;
			if (imp < 1) {
				if (imp < 0 && pen_imp >= 0) continue;
				imp = 1;
			}
			if (random_flag) {
				variance = (imp * random_imped + 50) / 100;
				if (variance > 0) {
					variance = (int) (imp + variance * (random.Probability () - 0.5) + 0.5);

					if (pen_imp == 0) {
						imped = variance;
					} else {
						imped = (imped * variance + (imp >> 1)) / imp;
						pen_imp = variance - imped;
					}
				}
			}
			path_ptr = link_path [dir];

			cost_b = cost_a + cost;

			//---- check for the trip end ----

			des_flag = false;

			for (des_ptr = des_array.First (); des_ptr != NULL; des_ptr = des_array.Next ()) {
				if (lnk != des_ptr->Link ()) continue;

				offset = (ab_flag) ? des_ptr->Offset () : link_ptr->Length () - des_ptr->Offset ();

				offset_factor = ((offset << offset_roll) + (link_ptr->Length () >> 1)) / link_ptr->Length ();

				//---- calculate the time ----

				tim = (time * offset_factor + offset_round) >> offset_roll;
				if (tim < 0) {
					time_flag = true;
					continue;
				}
				tim += penalty;
				if (tim < 0 && penalty < 0) tim = 1;

				//---- check the time schedule ----

				tod_b = tod_a + tim;

				if (tod_b < tod_a || tod_b > max_tod) {
					time_flag = true;
					continue;
				}
				
				//---- adjust the link impedance ----

				imp = ((imped * offset_factor + offset_round) >> offset_roll) + pen_imp;
				if (imp < 1) {
					if (imp < 0 && pen_imp >= 0) continue;
					imp = 1;
				}

				//---- check the cumulative impedance ----

				cum_b = cum_a + imp;

				if (cum_b < 0 || cum_b >= max_cum || (cum_b >= des_ptr->Cum () && des_ptr->Cum ())) continue;

				des_ptr->Cum (cum_b);
				des_ptr->TOD (tod_b);
				des_ptr->Cost (cost_b);
				des_ptr->Time (time_b);
				des_ptr->Prev (dir);

				path_ptr->Prev (approach);

				des_flag = true;

				if (best_flag) {
					max_cum = cum_b;
					best_des = des_array.Record_Index ();
				}
			}

			//---- update the max cum ----

			if (des_flag && !best_flag) {
				hi_cum = 0;

				for (des_ptr = des_array.First (); des_ptr != NULL; des_ptr = des_array.Next ()) {
					if (des_ptr->Cum () > hi_cum) hi_cum = des_ptr->Cum ();
				}
				if (hi_cum > 0 && hi_cum < max_cum) max_cum = hi_cum;
			}

			//---- check the restricted access ----

			if (Use_Permission (use, RESTRICTED)) {
				if (restrict_in || restrict_des) {
					if (!restrict_in) {
						restrict_org = false;
					}
				} else {
					use_flag = true;
					continue;
				}
			} else if (restrict_in && !restrict_org) {
				use_flag = true;
				continue;
			}

			//---- check the time ----

			time += penalty;

			if (time < 0 && penalty < 0) {
				time = 1;
			}
			tod_b = tod_a + time;

			if (tod_b < tod_a || tod_b >= max_tod) {
				time_flag = true;
				continue;
			}

			//---- check the cumulative impedance at B ----

			imp = imped + pen_imp;
			if (imp < 1) {
				if (imp < 0 && pen_imp >= 0) continue;
				imp = 1;
			}
			cum_b = cum_a + imp;

			if (cum_b < 0 || cum_b >= max_cum || (cum_b >= path_ptr->Cum () && path_ptr->Cum ())) continue;

			if (path_ptr->Next () == -1) {
				path_ptr->Next (first_ptr->Next ());
				first_ptr->Next (dir);
			} else if (path_ptr->Next () == 0 && last_ptr != path_ptr) {
				last_ptr->Next (dir);
				last_ptr = path_ptr;
			}
			path_ptr->Cum (cum_b);
			path_ptr->TOD (tod_b);
			path_ptr->Cost (cost_b);
			path_ptr->Time (time_b);
			path_ptr->Prev (approach);
		}
	}
	return (best_des);
}
