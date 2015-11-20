//*********************************************************
//	Output_Delay.cpp - save the link delay data
//*********************************************************

#include "LinkDelay.hpp"

//---------------------------------------------------------
//	Output_Delay
//---------------------------------------------------------

void LinkDelay::Output_Delay (void)
{
	int i, j, j1, j2, k, k2, n, num, dir, nperiods, vol, volume, nout, time0, increment;
	int t0, t1, t2, tim, save, sum_vol, max_queue, num_fail, enter, exit, count;
	double ttime, length, speed, factor, queue, density, max_density, time, distance;
	bool connect_flag, flag;

	Link_Data *link_ptr;
	TTime_Data *ttime_ptr;
	Connect_Time *connect_ptr, *turn_ptr [20];

	Delay_File *output_delay = (Delay_File *) Demand_Db_Base (NEW_LINK_DELAY);

	connect_flag = Network_Data_Flag (LANE_CONNECTIVITY);

	Show_Message ("Writing %s -- Link", output_delay->File_Type ());
	Set_Progress (1000);
	nout = num = 0;

	//---- default lane and turn information ----

	increment = Time_Increment ();
	if (increment < 1) {
		Error ("Increment %d Out of Range", increment);
	}
	output_delay->Lane (1);
	output_delay->Turn (0);
	output_delay->Increment (increment);

	if (output_delay->Dbase_Format () == VERSION3) {
		output_delay->Create_Header ();
	}

	//---- initialize the sort option ----

	if (sort_flag) {
		k2 = ttime_data.Periods ();
	} else {
		t0 = 0;
		j1 = 1;
		j2 = ttime_data.Periods ();
		k2 = 1;
	}

	//---- time sort loop ----

	for (k=1; k <= k2; k++) {
		if (sort_flag) {
			j1 = j2 = k;
			t0 = (k - 1) * increment;
		}

		//---- link sort loop ----

		for (link_ptr = link_data.First_Key (); link_ptr != NULL; link_ptr = link_data.Next_Key ()) {
			Show_Progress ();

			output_delay->Link (link_ptr->Link ());

			length = UnRound (link_ptr->Length ());

			//---- process each direction ----

			for (i=0; i < 2; i++) {
				dir = (i) ? link_ptr->BA_Dir () : link_ptr->AB_Dir ();
				if (dir == 0) continue;

				ttime_ptr = ttime_data [dir];

				nperiods = ttime_ptr->Periods ();
				if (nperiods == 0) continue;

				//---- collect connection pointers ----

				if (connect_flag) {
					num = 0;
					j = ttime_ptr->Link_Dir ();

					for (connect_ptr = connect_time.Get_GE (j, 0); connect_ptr; connect_ptr = connect_time.Next_Key ()) {
						if (connect_ptr->In_Link_Dir () != j || num == 20) break;
						turn_ptr [num++] = connect_ptr;
					}
				}

				//---- smooth the time of day delays ----

				if (naverage > 0) {
					Smooth_Delay (ttime_ptr, num, turn_ptr);
				}

				//---- save the link direction data ----

				if (output_delay->LinkDir_Type () == Db_Code::LINK_SIGN) {
					if (i) {
						output_delay->Link (-link_ptr->Link ());
					} else {
						output_delay->Link (link_ptr->Link ());
					}
				} else {
					if (output_delay->LinkDir_Type () == Db_Code::LINK_NODE) {

						//---- from node!!!! ----

						output_delay->Dir (((i) ? link_ptr->Bnode () : link_ptr->Anode ()));
					} else {
						output_delay->Dir (i);
					}
				}

				//---- process each time period ----

				t2 = t0;
				time0 = ttime_ptr->Time0 ();

				for (j=j1; j <= j2; j++) {
					t1 = t2;
					t2 += increment;

					//---- get the data ----

					tim = ttime_ptr->TTime (j);
					if (tim == 0) continue;

					volume = ttime_ptr->Volume (j);
					if (volume == 0 && tim == time0) continue;

					ttime = UnRound (tim);
					speed = length / ttime;

					enter = ttime_ptr->Data (j, 3);
					exit = ttime_ptr->Data (j, 4);
					density = UnRound (ttime_ptr->Data (j, 5)) / 10.0;
					max_density = UnRound (ttime_ptr->Data (j, 6)) / 10.0;
					queue = UnRound (ttime_ptr->Data (j, 7)) / 10.0;
					max_queue = ttime_ptr->Data (j, 8);
					num_fail = ttime_ptr->Data (j, 9);
					count = ttime_ptr->Data (j, 10);
					time = UnRound (ttime_ptr->Data (j, 11));
					distance = UnRound (ttime_ptr->Data (j, 12));

					if (count > 1) {
						density = density / count;
						if (density > 0.0 && density < 0.01) density = 0.01;
		
						queue = queue / count;
						if (queue > 0.0 && queue < 0.01) queue = 0.01;
					}

					//---- save the Version 3 fields ----

					if (output_delay->Dbase_Format () == Db_Code::VERSION3) {

						output_delay->Time (t2);
						output_delay->Volume (volume);

						if (volume < 1) volume = 1;
						output_delay->TTime (ttime * volume);
						output_delay->TTime2 (ttime * ttime * volume);

						output_delay->Vehicles ((int) (density + 0.5));
						output_delay->Speed (density * speed);
						output_delay->Speed2 (density * speed * speed);

						if (!output_delay->Write (false)) goto write_error;
						nout++;

					} else {

						output_delay->Start_Time (time_step.Format_Step (t1));
						output_delay->End_Time (time_step.Format_Step (t2));

						output_delay->Volume (volume);
						output_delay->Enter (enter);
						output_delay->Exit (exit);

						output_delay->Speed (speed);
						output_delay->TTime (ttime);

						factor = UnRound (time0);
						if (factor < 0.1) factor = 0.1;

						output_delay->Delay (ttime - factor);
						output_delay->Time_Ratio (ttime / factor);

						if (distance == 0.0) {
							output_delay->VMT (length * volume);
						} else {
							output_delay->VMT (distance);
						}
						if (time == 0.0) {
							output_delay->VHT (ttime * volume);
						} else {
							output_delay->VHT (time);
						}
						output_delay->Density (density);
						output_delay->Max_Density (max_density);

						output_delay->Avg_Queue (queue);
						output_delay->Max_Queue (max_queue);
						output_delay->Cycle_Failure (num_fail);

						if (!connect_flag) {
							output_delay->Num_Connect (0);
							if (!output_delay->Write (false)) goto write_error;
							nout++;
							continue;
						}

						//---- adjust turn penalties ----

						flag = false;

						if (num > 0) {
							sum_vol = 0;

							for (n=0; n < num; n++) {
								connect_ptr = turn_ptr [n];

								vol = connect_ptr->Volume (j);
								if (vol > 0) {
									connect_ptr->Add_TTime (j, tim);
									sum_vol += vol;
									flag = true;
								} else if (connect_ptr->TTime (j) != 0) {
									connect_ptr->Add_TTime (j, tim);
								}
							}
						}
						if (flag) {
							vol = volume * 2 - sum_vol;
							if (vol < 0 || (vol == 1 && sum_vol > 1 && (sum_vol % 2) == 1)) {
								vol = 0;
							}
							output_delay->Enter (vol);
							output_delay->Exit (sum_vol);
						}

						//---- save the connection data ----

						for (n=save=0; n < num; n++) {
							connect_ptr = turn_ptr [n];
							if (connect_ptr->Volume (j) > 0 || connect_ptr->TTime (j) > 0) save++;
						}
						if (save <= 1) {
							output_delay->Num_Connect (0);
							if (!output_delay->Write (false)) goto write_error;
							nout++;
							continue;
						}
						output_delay->Num_Connect (save);

						if (!output_delay->Write (false)) goto write_error;
						nout++;

						for (n=0; n < num; n++) {
							connect_ptr = turn_ptr [n];

							tim = connect_ptr->TTime (j);
							vol = connect_ptr->Volume (j);
							if (vol <= 0 && tim <= 0) continue;

							output_delay->Out_Link (connect_ptr->Out_Link ());
							output_delay->Out_Dir (connect_ptr->Out_Dir ());
							output_delay->Out_Turn (vol);
							output_delay->Out_Time (UnRound (tim));

							if (!output_delay->Write (true)) goto write_error;
							nout++;
						}
					}
				}
			}
		}
	}
	End_Progress ();

	output_delay->Close ();

	Print (2, "Number of %s Records = %d", output_delay->File_Type (), nout);
	return;

write_error:
	exe->Error ("Writing Link Delay File");
}
