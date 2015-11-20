//*********************************************************
//	Combine_Delay.cpp - combine link delay data
//*********************************************************

#include "LinkDelay.hpp"

void LinkDelay::Combine_Delay (void)
{
	int i, n, num, period, link, dir, vol1, vol2, time1, time2;
	int enter1, enter2, exit1, exit2, density1, density2, max_density1, max_density2;
	int queue1, queue2, max_queue1, max_queue2, count1, count2, vht1, vht2, vmt1, vmt2;
	int start, end, out_link_dir, in_link_dir, num_fail1, num_fail2; 
	int turn_time [20], turn_vol [20];
	bool connect_flag, version3_flag, factor_flag;
	double ttim, factor1;

	Link_Data *link_ptr;
	TTime_Data *ttime_ptr;
	Connect_Time *connect_ptr, *turn_ptr [20];
	AB_Data *ab_ptr;

	version3_flag = (previous_delay.Dbase_Format () == VERSION3);
	connect_flag = (Network_Data_Flag (LANE_CONNECTIVITY) && !version3_flag);

	//---- store the link delay data ----

	Show_Message ("Reading %s -- Record", previous_delay.File_Type ());
	Set_Progress (10000);

	if (method == SIMPLE_AVERAGE) {
		factor = 1.0;
		factor1 = 2.0;
		factor_flag = false;
	} else if (method == WEIGHTED_AVERAGE) {
		factor_flag = (factor == 0.0);
		if (!factor_flag) {
			factor1 = factor + 1.0;
		}
	}

	while (previous_delay.Read (false)) {
		Show_Progress ();

		in_link_dir = 0;
	
		//---- get the time period and travel time ----

		ttim = previous_delay.TTime ();
		vol1 = previous_delay.Volume ();

		if (version3_flag) {
			end = previous_delay.Time ();
			if (end == 0) goto nest;
			ttim /= vol1;
		} else {
			start = time_step.Step (previous_delay.Start_Time ());
			end = time_step.Step (previous_delay.End_Time ());
			if (end == 0) goto nest;

			if (ttime_data.Period_Size () == 0) {
				if (ttime_data.Period_Size (end - start, ttime_data.Min_Size (), ttime_data.Max_Size ())) {
					if (((start - ttime_data.Min_Size ()) % ttime_data.Period_Size ()) != 0) {
						Warning ("Link Delay Period Size %d and Start Time %d do not Align", end - start, start);
					}
				}
			}
		}
		period = Time_Period (end - 1);		//---- subtract 1 to adjust end time ---
		if (period == 0) goto nest;

		if (ttim < 0.0 || ttim > MIDNIGHT) {
			ttim = MIDNIGHT;
		}
		time1 = Round (ttim);
		if (time1 == 0) goto nest;

		//---- get the link record ----

		link = previous_delay.Link ();
		dir = previous_delay.Dir ();

		if (previous_delay.LinkDir_Type () == LINK_SIGN) {
			if (link < 0) {
				link = -link;
				dir = 1;
			}
		}

		//----- match a previous link file ----

		if (link_flag) {
			link_ptr = previous_data.Get (link);
			if (link_ptr == NULL) {
				Error_Check ();
				goto nest;
			}
			ab_ptr = ab_key.Get (link_ptr->Anode (), link_ptr->Bnode ());
			if (ab_ptr == NULL) {
				ab_ptr = ab_key.Get (link_ptr->Bnode (), link_ptr->Anode ());
			}
			if (ab_ptr == NULL) goto nest;
			ttime_ptr = ttime_data [ab_ptr->dir];
			if (ttime_ptr == NULL) {
				Error_Check ();
				goto nest;
			}
			link = ttime_ptr->Link ();
		}

		//---- get the link record ----

		link_ptr = link_data.Get (link);
		if (link_ptr == NULL) {
			Error_Check ();
			goto nest;
		}

		if (previous_delay.LinkDir_Type () == LINK_NODE) {

			//---- from node!!! ----

			if (dir == link_ptr->Anode ()) {
				dir = 0;
			} else if (dir == link_ptr->Bnode ()) {
				dir = 1;
			} else {
				Error_Check ();
				goto nest;
			}
		}

		if (dir == 0) {
			dir = link_ptr->AB_Dir ();
		} else {
			dir = link_ptr->BA_Dir ();
		}
		if (dir == 0) {
			Error_Check ();
			goto nest;
		}
		ttime_ptr = ttime_data [dir];

		if (ttime_ptr == NULL) {
			Error_Check ();
			goto nest;
		}

		//---- check the combination method ----

		vol2 = ttime_ptr->Volume (period);

		if (vol2 == 0 || method != REPLACE_LINKS) {
			in_link_dir = ttime_ptr->Link_Dir ();
		}

		//---- get the remaining data fields ----

		enter1 = previous_delay.Enter ();
		exit1 = previous_delay.Exit ();
		density1 = Round (previous_delay.Density () * 10.0);
		max_density1 = Round (previous_delay.Max_Density () * 10.0);
		queue1 = Round (previous_delay.Avg_Queue () * 10.0);
		max_queue1 = previous_delay.Max_Queue ();
		num_fail1 = previous_delay.Cycle_Failure ();
		count1 = 1;
		vht1 = Round (previous_delay.VHT ());
		vmt1 = Round (previous_delay.VMT ());

		//---- process each nested record ----
nest:
		num = previous_delay.Num_Connect ();
		n=0;

		for (i=1; i <= num; i++) {
			if (!previous_delay.Read (true)) {
				Error ("Number of Nested Records for Link %d", previous_delay.Link ());
			}
			Show_Progress ();

			if (!connect_flag || in_link_dir == 0) continue;

			//---- process a nested record ----

			ttim = previous_delay.Out_Time ();

			if (ttim < 0.0 || ttim > MIDNIGHT) {
				ttim = MIDNIGHT;
			}
			time2 = Round (ttim);
			if (time2 == 0) continue;

			Dir_Data dir_rec;

			//---- process a connection record ----

			link = previous_delay.Out_Link ();
			dir = previous_delay.Out_Dir ();

			if (previous_delay.LinkDir_Type () == LINK_SIGN) {
				if (link < 0) {
					link = -link;
					dir = 1;
				}
			}

			//----- convert the previous link number ----

			if (link_flag) {
				link_ptr = previous_data.Get (link);
				if (link_ptr == NULL) {
					Error_Check ();
					continue;
				}
				if (dir) {
					ab_ptr = ab_key.Get (link_ptr->Bnode (), link_ptr->Anode ());
				} else {
					ab_ptr = ab_key.Get (link_ptr->Anode (), link_ptr->Bnode ());
				}
				if (ab_ptr == NULL) continue;
				Dir_Data *dir_ptr = dir_data [ab_ptr->dir];
				if (dir_ptr == NULL) {
					Error_Check ();
					continue;
				}
				link = dir_ptr->Link ();
				dir = dir_ptr->Dir ();
			}

			link_ptr = link_data.Get (link);
			if (link_ptr == NULL) {
				Error_Check ();
				continue;
			}

			if (previous_delay.LinkDir_Type () == LINK_NODE) {

				//---- from node!!!! ----

				if (dir == link_ptr->Anode ()) {
					dir = 0;
				} else if (dir == link_ptr->Bnode ()) {
					dir = 1;
				} else {
					Error_Check ();
					continue;
				}
			}
			dir_rec.Link (link);
			dir_rec.Dir (dir);

			out_link_dir = dir_rec.Link_Dir ();

			connect_ptr = connect_time.Get (in_link_dir, out_link_dir);

			if (connect_ptr == NULL) {
				Error_Check ();
				continue;
			}
			if (n < 20) {
				turn_ptr [n] = connect_ptr;
				turn_time [n] = time2;
				turn_vol [n++] = previous_delay.Out_Turn ();
			}
		}

		//----- combine the results ----

		if (in_link_dir == 0) continue;

		if (vol1 == 0 || time1 < (ttime_ptr->Time0 () >> 1)) {
			time1 = ttime_ptr->Time0 ();
		}

		//---- adjust turn penalties ----

		num = n;
		if (num > 0) {
			for (n=0; n < num; n++) {
				if (turn_time [n] > 0) {
					turn_time [n] -= time1;
				}
			}
		}

		//---- get the new volume and travel time data ----

		vol2 = ttime_ptr->Volume (period);
		time2 = ttime_ptr->TTime (period);
		enter2 = ttime_ptr->Data (period, 3);
		exit2 = ttime_ptr->Data (period, 4);
		density2 = ttime_ptr->Data (period, 5);
		max_density2 = ttime_ptr->Data (period, 6);
		queue2 = ttime_ptr->Data (period, 7);
		max_queue2 = ttime_ptr->Data (period, 8);
		num_fail2 = ttime_ptr->Data (period, 9);
		count2 = ttime_ptr->Data (period, 10);
		vht2 = ttime_ptr->Data (period, 11);
		vmt2 = ttime_ptr->Data (period, 12);

		//---- implement the combination method ----

		if (method != REPLACE_LINKS) {
			if (factor_flag) {
				if (vol2 > 0) {
					factor = vol1 / vol2;
				} else {
					factor = vol1;
				}
				factor1 = factor + 1.0;
			}
			vol1 = (int) ((vol1 * factor + vol2) / factor1 + 0.5);
			time1 = (int) ((time1 * factor + time2) / factor1 + 0.5);
			enter1 = (int) ((enter1 * factor + enter2) / factor1 + 0.5);
			exit1 = (int) ((exit1 * factor + exit2) / factor1 + 0.5);
			density1 = (int) ((density1 * factor + density2) / factor1 + 0.5);
			max_density1 = (int) ((max_density1 * factor + max_density2) / factor1 + 0.5);
			queue1 = (int) ((queue1 * factor + queue2) / factor1 + 0.5);
			max_queue1 = (int) ((max_queue1 * factor + max_queue2) / factor1 + 0.5);
			num_fail1 = (int) ((num_fail1 * factor + num_fail2) / factor1 + 0.5);
			count1 = (int) ((count1 * factor + count2) / factor1 + 0.5);
			vht1 = (int) ((vht1 * factor + vht2) / factor1 + 0.5);
			vmt1 = (int) ((vmt1 * factor + vmt2) / factor1 + 0.5);
		}

		//---- save the results ----

		if (ttime_ptr->Periods () == 0) {
			if (!ttime_ptr->Periods (ttime_data.Periods (), ttime_data.Layers ())) {
				Error ("Insufficient Memory for Link Delay Data");
			}
		}
		ttime_ptr->Volume (period, vol1);
		ttime_ptr->TTime (period, time1);
		ttime_ptr->Data (period, 3, enter1);
		ttime_ptr->Data (period, 4, exit1);
		ttime_ptr->Data (period, 5, density1);
		ttime_ptr->Data (period, 6, max_density1);
		ttime_ptr->Data (period, 7, queue1);
		ttime_ptr->Data (period, 8, max_queue1);
		ttime_ptr->Data (period, 9, num_fail1);
		ttime_ptr->Data (period, 10, count1);
		ttime_ptr->Data (period, 11, vht1);
		ttime_ptr->Data (period, 12, vmt1);

		//---- save the turn penalties ----

		if (num > 0) {

			for (n=0; n < num; n++) {
				connect_ptr = turn_ptr [n];

				vol1 = turn_vol [n];
				time1 = turn_time [n];

				vol2 = connect_ptr->Volume (period);
				time2 = connect_ptr->TTime (period);

				//---- implement the combination method ----

				if (method != REPLACE_LINKS) {
					if (factor_flag) {
						if (vol2 > 0) {
							factor = vol1 / vol2;
						} else {
							factor = vol1;
						}
						factor1 = factor + 1.0;
					}
					vol1 = (int) ((vol1 * factor + vol2) / factor1 + 0.5);
					time1 = (int) ((time1 * factor + time2) / factor1 + 0.5);
				}

				//---- save the results ----

				if (connect_time.Period_Size () == 0) {
					connect_time.Period_Size (ttime_data.Period_Size (), ttime_data.Min_Size (), ttime_data.Max_Size ());
				}
				if (connect_ptr->Periods () == 0) {
					if (!connect_ptr->Periods (connect_time.Periods (), connect_time.Layers ())) {
						Error ("Insufficient Memory for Connection Delay Data");
					}
				}
				connect_ptr->Volume (period, vol1);
				connect_ptr->TTime (period, time1);
			}
		}
	}
	End_Progress ();

	previous_delay.Close ();

	Print (2, "Number of %s Records = %d", previous_delay.File_Type (), Progress_Count ());
}

//---------------------------------------------------------
//	Error_Check ()
//---------------------------------------------------------

void LinkDelay::Error_Check (void)
{
	if (++nerror > Max_Delay_Errors ()) {
		if (Demand_File_Flag (LINK_DELAY)) Error ("Number of Link Delay Errors Exceeded (%d > %d)", nerror, Max_Delay_Errors ());
	}
}
