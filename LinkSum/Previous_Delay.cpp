//*********************************************************
//	Previous_Delay.cpp - Read the Previous Link Delay File
//*********************************************************

#include "LinkSum.hpp"
#include "Time_Step.hpp"

//---------------------------------------------------------
//	Link_Delay
//---------------------------------------------------------

void LinkSum::Previous_Delay (void)
{
	int i, n, num, period, nperiod, link, dir, volume, time, vol, iteration;
	int start, end, seconds, out_link_dir, in_link_dir; 
	bool connect_flag, version3_flag, flag;
	double ttim, max_time, time_ratio, vht;

	int enter, exit, max_queue, num_fail;
	double ttime, distance, density, max_density, queue;

	Time_Step time_step (1, "SECONDS");

	Link_Data *link_ptr;
	Performance_Data *perf_ptr;
	Connect_Time *connect_ptr;

	version3_flag = (previous_delay.Dbase_Format () == VERSION3);
	connect_flag = (Network_Data_Flag (LANE_CONNECTIVITY) && !version3_flag);

	//---- store the link delay data ----

	Show_Message ("Reading %s -- Record", previous_delay.File_Type ());
	Set_Progress (10000);

	vht = 0.0;
	iteration = previous_delay.Iteration ();

	while (previous_delay.Read (false)) {
		Show_Progress ();

		in_link_dir = 0;
	
		//---- get the time period ----

		if (version3_flag) {
			end = previous_delay.Time ();
			if (end == 0) goto nest;
		} else {
			start = time_step.Step (previous_delay.Start_Time ());
			end = time_step.Step (previous_delay.End_Time ());
			if (end == 0) goto nest;

			if (performance_data.Period_Size () == 0) {
				if (performance_data.Period_Size (end - start, performance_data.Min_Size (), performance_data.Max_Size ())) {
					if (((start - performance_data.Min_Size ()) % performance_data.Period_Size ()) != 0) {
						Warning ("Link Delay Period Size %d and Start Time %d do not Align", end - start, start);
					}
				}
			}
		}
		period = Time_Period (end - 1);		//---- subtract 1 to adjust end time ---

		if (period == 0) goto nest;

		//---- get the link record ----

		link = previous_delay.Link ();
		dir = previous_delay.Dir ();

		if (previous_delay.LinkDir_Type () == LINK_SIGN) {
			if (link < 0) {
				link = -link;
				dir = 1;
			}
		}
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
		perf_ptr = performance_data [dir];

		if (perf_ptr == NULL) {
			Error_Check ();
			goto nest;
		}

		//---- store the data ----

		in_link_dir = perf_ptr->Link_Dir ();

		if (previous_delay.Dbase_Format () == VERSION3) {
			enter = exit = previous_delay.Volume ();
			ttime = previous_delay.TTime ();
			distance = ttime * previous_delay.Speed ();
			density = max_density = previous_delay.Vehicles ();

			if (density > 0.0) {
				distance /= density;
			}
			queue = max_queue = num_fail = 0;

			time_ratio = enter * perf_ptr->Time0 ();
			if (time_ratio <= 0.0) time_ratio = 1.0;

			time_ratio = ttime / time_ratio;
		} else {
			enter = previous_delay.Enter ();
			exit = previous_delay.Exit ();
			distance = previous_delay.VMT ();
			density = previous_delay.Density ();
			max_density = previous_delay.Max_Density ();
			queue = previous_delay.Avg_Queue ();
			max_queue = previous_delay.Max_Queue ();
			num_fail = previous_delay.Cycle_Failure ();
			time_ratio = previous_delay.Time_Ratio ();

			//##### smoothing makes speed and VMT/VHT different #####
			vht += previous_delay.VHT ();
			ttime = previous_delay.Speed ();
			if (ttime < 0.5) ttime = 0.5;
			ttime = distance / ttime;
		}

		if (perf_ptr->Periods () == 0) {
			if (!perf_ptr->Periods (performance_data.Periods (), performance_data.Layers (), performance_data.Ratio_Flag ())) {
				Error ("Insufficient Memory for Link Delay Data");
			}
			perf_ptr->Enter2 (period, enter);
			perf_ptr->Exit2 (period, exit);
			perf_ptr->Time2 (period, ttime);
			perf_ptr->Distance2 (period, distance);
			perf_ptr->Density2 (period, density);
			perf_ptr->Max_Density2 (period, max_density);
			perf_ptr->Queue2 (period, queue);
			perf_ptr->Max_Queue2 (period, max_queue);
			perf_ptr->Failure2 (period, num_fail);
			perf_ptr->Count2 (period, 1);
			perf_ptr->Length (UnRound (link_ptr->Length ()));

			if (performance_data.Ratio_Flag () && time_ratio >= performance_data.Congested_Ratio ()) {
				perf_ptr->Ratio_Count2 (period, 1);
				perf_ptr->Ratio_VMT2 (period, distance);
				perf_ptr->Ratio_VHT2 (period, ttime);
			}
		} else {
			perf_ptr->Add_Enter2 (period, enter);
			perf_ptr->Add_Exit2 (period, exit);
			perf_ptr->Add_Time2 (period, ttime);
			perf_ptr->Add_Distance2 (period, distance);
			perf_ptr->Add_Density2 (period, density);
			perf_ptr->Add_Max_Density2 (period, max_density);
			perf_ptr->Add_Queue2 (period, queue);
			perf_ptr->Add_Max_Queue2 (period, max_queue);
			perf_ptr->Add_Failure2 (period, num_fail);
			perf_ptr->Add_Count2 (period, 1);

			if (performance_data.Ratio_Flag () && time_ratio >= performance_data.Congested_Ratio ()) {
				perf_ptr->Add_Ratio_Count2 (period, 1);
				perf_ptr->Add_Ratio_VMT2 (period, distance);
				perf_ptr->Add_Ratio_VHT2 (period, ttime);
			}
		}

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
			volume = previous_delay.Out_Turn ();
			if (volume <= 0) volume = 1;

			ttim *= volume;
			max_time = volume * MIDNIGHT;

			if (ttim < 0.0 || ttim > max_time) {
				ttim = max_time;
			}
			seconds = Round (ttim);
			if (seconds == 0) continue;

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
			if (connect_time.Period_Size () == 0) {
				connect_time.Period_Size (performance_data.Period_Size ());
			}
			if (connect_ptr->Periods () == 0) {
				if (!connect_ptr->Periods (connect_time.Periods (), connect_time.Layers ())) {
					Error ("Insufficient Memory for Connection Delay Data");
				}
			}
			if (connect_ptr->TTime2 (period) == 0) {
				connect_ptr->TTime2 (period, seconds);
				connect_ptr->Volume2 (period, volume);
			} else {
				connect_ptr->Add_TTime2 (period, seconds);
				connect_ptr->Add_Volume2 (period, volume);
			}
		}
	}
	End_Progress ();

	previous_delay.Close ();

	Print (2, "Number of %s Records = %d", previous_delay.File_Type (), Progress_Count ());

	//---- calculate the travel times ----

	link = dir = volume = period  = 0;

	for (perf_ptr = performance_data.First (); perf_ptr; perf_ptr = performance_data.Next ()) {

		//---- process each time period ----

		flag = false;
		nperiod = perf_ptr->Periods ();

		for (i=1; i <= nperiod; i++) {
			vol = perf_ptr->Volume2 (i);
            if (vol > 0) {
				volume++;
				flag = true;
			}
			period++;
		}
		if (flag) dir++;
		link++;
	}
	Print (1, "Percent of Link Directions with Travel Time Data = %.1lf%%", 
				((link) ? dir * 100.0 / link : 0.0));
	Print (1, "Percent of Link Time Periods with Travel Time Data = %.1lf%%", 
				((period) ? volume * 100.0 / period : 0.0));

	//---- adjust connection times ----

	if (connect_flag) {
		link = dir = volume = period = 0;

		for (connect_ptr = connect_time.First (); connect_ptr; connect_ptr = connect_time.Next ()) {
			flag = false;
			nperiod = connect_ptr->Periods ();

			perf_ptr = performance_data.Get (connect_ptr->In_Link_Dir ());
			if (perf_ptr == NULL) {
				Error ("Link Connection");
			}

			for (i=1; i <= nperiod; i++) {
				vol = connect_ptr->Volume2 (i);
				if (vol > 0) {
					time = connect_ptr->TTime2 (i);
					if (time > 0) {
						time = (time + (vol >> 1)) / vol;
						if (time < 1) time = 1;

						time -= Round (perf_ptr->TTime2 (i));

						connect_ptr->TTime2 (i, time);

						volume++;
						flag = true;
					}
				}
				period++;
			}
			if (flag) dir++;
			link++;
		}
		Print (1, "Percent of Link Connections with Time Penalties = %.1lf%%", 
					((link) ? dir * 100.0 / link : 0.0));
		Print (1, "Percent of Connection Periods with Time Penalties = %.1lf%%", 
					((period) ? volume * 100.0 / period : 0.0));
	}
	Print (2, "Total Vehicle Hours of Travel = %.1lf", vht / 3600.0);
}

//---------------------------------------------------------
//	Error_Check ()
//---------------------------------------------------------

void LinkSum::Error_Check (void)
{
	if (++nerror > Max_Delay_Errors ()) {
		Error ("Number of Link Delay Errors Exceeded (%d > %d)", nerror, Max_Delay_Errors ());
	}
}
