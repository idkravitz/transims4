//*********************************************************
//	Update_Signals.cpp - Update signal timing plans
//*********************************************************

#include "IntControl.hpp"

#include "Timing_File.hpp"

#define MAX_PHASE_DIR	3

//---------------------------------------------------------
//	Update_Signals
//---------------------------------------------------------

void IntControl::Update_Signals (void)
{
	int i, j, k, l, last_timing, nphase, total, count, cycle, phase, next, first;
	int period, p1, p2, volume, share, maximum, timing;
	int rings, groups, group_green [5][5], group_demand [5] [5];
	int split [20], demand [20], lanes [20], critical [20];
	int dir_split [MAX_PHASE_DIR], dir_demand [MAX_PHASE_DIR], dir_lanes [MAX_PHASE_DIR];
	double bucket, factor, time, bucket2, factor2;
	char buffer [FIELD_BUFFER];
	bool adjust;

	Phasing_Data *phasing_ptr;
	Timing_Data *timing_ptr, timing_rec, *timing_list [20], **list, *list_ptr;
	Signal_Data *signal_ptr, *old_ptr;
	Connect_Time *connect_ptr;
	Signal_Array *signal_list;
	Dir_Connect *dir_ptr;
	
	Index_Array update_list;
	Data_Array phase_demand;

	typedef struct {
		int timing;
		int signal;
	} Update_List;

	typedef struct {
		int link_dir;
		int left;
		int thru;
		int right;
	} Approach;

	typedef struct {
		Approach approach [MAX_PHASE_DIR];
	} Phase_Demand;

	Phase_Demand *phase_ptr, *phase_list [20], *other_ptr;
	Update_List *update_ptr;
	Approach *approach_ptr, *app_ptr;

	phase_demand.Data_Size (sizeof (Phase_Demand));
	update_list.Data_Size (sizeof (Update_List));

	Timing_File *timing_file = (Timing_File *) Network_Db_File (NEW_TIMING_PLAN);

	//---- set the signal list ----

	if (input_signal_flag) {
		signal_list = &input_signal;
	} else {
		signal_list = &(signal_data);
	}

	//---- set the timing list ----

	for (signal_ptr = signal_list->First (); signal_ptr; signal_ptr = signal_list->Next ()) {
		timing = signal_ptr->Timing ();

		update_ptr = (Update_List *) update_list.New_Record (true);

		if (timing == 0 && input_signal_flag) {
			old_ptr = signal_data.Get (signal_ptr);

			if (old_ptr != NULL) {
				timing = old_ptr->Timing ();
				signal_ptr->Timing (timing);
			}
		}
		update_ptr->timing = timing;
		update_ptr->signal = signal_list->Record_Index ();
		
		if (!update_list.Add ()) {
			Error ("Adding Timing List Data");
		}
	}

	//---- allocate space for phase demand ----

	if (!phase_demand.Num_Records (timing_data.Num_Records ())) {
		Error ("Insufficient Memory for Phase Demand Data");
	}

	//---- gather phase volumes ----

	Show_Message ("Calculating Phase Demand -- Record");
	Set_Progress ();

	for (phasing_ptr = phasing_data.First (); phasing_ptr; phasing_ptr = phasing_data.Next ()) {
		Show_Progress ();

		if (phasing_ptr->Protection () == STOP_PERMIT) continue;
		
		timing = phasing_ptr->Timing ();

		update_ptr = (Update_List *) update_list.Get (&timing);
		if (update_ptr == NULL) continue;

		connect_ptr = connect_time.Get (phasing_ptr->In_Link_Dir (), phasing_ptr->Out_Link_Dir ());
		if (connect_ptr == NULL || (connect_ptr->Periods () == 0 && !update_report)) continue;

		//---- find the start time ----

		signal_ptr = signal_list->Record (update_ptr->signal);

		if (range_flag) {
			p1 = time_range.Span_Index (signal_ptr->Start (), signal_ptr->End () - 1);
			p2 = time_range.Span_Index (signal_ptr->End (), MIDNIGHT);
		} else {
			p1 = connect_time.Period (signal_ptr->Start ());
			p2 = connect_time.Period (signal_ptr->End () - 1);
		}

		//---- save the turn volume ----

		volume = 0;

		for (period=p1; period <= p2; period++) {
			volume += connect_ptr->Volume (period);
		}
		volume *= 10;
		if (volume == 0) volume = 1;

		timing_rec.Timing (timing);
		timing_rec.Phase (phasing_ptr->Phase ());

		timing = timing_data.Get_Index (&timing_rec);

		if (timing != 0) {
			phase_ptr = (Phase_Demand *) phase_demand [timing];

			approach_ptr = phase_ptr->approach;

			for (i=0; i < MAX_PHASE_DIR; i++, approach_ptr++) {
				if (approach_ptr->link_dir == 0 || approach_ptr->link_dir == connect_ptr->In_Link_Dir ()) {
					approach_ptr->link_dir = connect_ptr->In_Link_Dir ();
					if (connect_ptr->Type () == LEFT || connect_ptr->Type () == UTURN) {
						approach_ptr->left += volume;
					} else if (connect_ptr->Type () == RIGHT) {
						approach_ptr->right += volume;
					} else {
						approach_ptr->thru += volume;
					}
					break;
				}
			}
		}
	}
	End_Progress ();

	//---- update the timing data ----

	Show_Message ("Updating Phase Timing -- Record");
	Set_Progress ();

	last_timing = 0;

	if (update_report) {
		Header_Number (TIMING_UPDATE);

		if (!Break_Check (signal_list->Num_Records () * 6)) {
			Print (1);
			Update_Header ();
		}
	}

	for (timing_ptr = timing_data.First_Key (); ; timing_ptr = timing_data.Next_Key ()) {

		if (timing_ptr == NULL || timing_ptr->Timing () != last_timing) {
			if (last_timing > 0) {

				//---- set the split, demand, and lane data ----

				for (i=0; i < nphase; i++, phase_ptr++) {
					split [i] = demand [i] = lanes [i] = critical [i] = 0;
					phase_ptr = phase_list [i];

					approach_ptr = phase_ptr->approach;

					for (j=0; j < MAX_PHASE_DIR; j++, approach_ptr++) {
						if (approach_ptr->link_dir == 0) break;

						dir_ptr = dir_data.Get (approach_ptr->link_dir);

						//---- scan the other phases for duplicate approach links ----

						for (k=i+1, other_ptr = phase_ptr + 1; k < nphase; k++, other_ptr++) {
							app_ptr = other_ptr->approach;

							for (l=0; l < MAX_PHASE_DIR; l++, app_ptr++) {
								if (app_ptr->link_dir == 0) break;

								if (app_ptr->link_dir == approach_ptr->link_dir) {
									if (app_ptr->left > 0 && approach_ptr->left > 0) {
										volume = (approach_ptr->left + app_ptr->left) / 2;

										if (dir_ptr->Left_Turn ()) {
											share = (int) (volume * 0.9 + 0.5);
										} else {
											share = (int) (volume * 0.6 + 0.5);
										}
										if (approach_ptr->thru == 0 && approach_ptr->right == 0) {
											approach_ptr->left = share;
											app_ptr->left = volume - share;
										} else {
											app_ptr->left = share;
											approach_ptr->left = volume - share;
										}
									}
									if (app_ptr->thru > 0 && approach_ptr->thru > 0) {
										volume = (approach_ptr->thru + app_ptr->thru) / 2;
										share = (int) (volume * 0.75 + 0.5);

										if (j > 0 || phase_ptr->approach [1].link_dir > 0) {
											approach_ptr->thru = share;
											app_ptr->thru = volume - share;
										} else if (l > 0 || other_ptr->approach [1].link_dir > 0) {
											app_ptr->thru = share;
											approach_ptr->thru = volume - share;
										} else if (approach_ptr->left == 0) {
											approach_ptr->thru = share;
											app_ptr->thru = volume - share;
										} else {
											app_ptr->thru = share;
											approach_ptr->thru = volume - share;
										}
									}
									if (app_ptr->right > 0 && approach_ptr->right > 0) {
										volume = (approach_ptr->right + app_ptr->right) / 2;
										share = (int) (volume * 0.75 + 0.5);

										if (j > 0 || phase_ptr->approach [1].link_dir > 0) {
											approach_ptr->right = share;
											app_ptr->right = volume - share;
										} else if (l > 0 || other_ptr->approach [1].link_dir > 0) {
											app_ptr->right = share;
											approach_ptr->right = volume - share;
										} else if (approach_ptr->left == 0) {
											approach_ptr->right = share;
											app_ptr->right = volume - share;
										} else {
											app_ptr->right = share;
											approach_ptr->right = volume - share;
										}
									}
									break;
								}
							}
							if (l < MAX_PHASE_DIR) break;
						}
						
						//---- calculate the approach split ----

						dir_split [j] = dir_lanes [j] = dir_demand [j] = 0;

						if (approach_ptr->thru > 0) {
							dir_lanes [j] += 10 * dir_ptr->Thru ();
							dir_demand [j] += approach_ptr->thru / 10;
						}
						if (approach_ptr->left > 0) {
							if (dir_lanes [j] == 0) {
								dir_lanes [j] += 10 * MAX (dir_ptr->Left_Turn (), 1);
							} else {
								dir_lanes [j] += (int) (10 * dir_ptr->Left_Turn () * pocket_factor + 0.5);
							}
							dir_demand [j] += approach_ptr->left / 10;
						}
						if (approach_ptr->right > 0) {
							dir_lanes [j] += (int) (10 * dir_ptr->Right_Turn () * pocket_factor + 0.5);
							dir_demand [j] += approach_ptr->right / 10;
						}
						if (dir_lanes [j] > 0) {
							dir_split [j] = 10 * dir_demand [j] / dir_lanes [j];
						} else {
							dir_split [j] = dir_demand [j];
						}
					}
					volume = k = 0;

					for (l=0; l < j; l++) {
						if (dir_split [l] > volume) {
							volume = dir_split [l];
							k = l;
						}
					}
					volume *= 2;
					for (l=0; l < j; l++) {
						if (l != k) {
							volume += dir_split [l];
						}
						demand [i] += dir_demand [l];
						lanes [i] += dir_lanes [l];
					}
					critical [i] = split [i] = volume / (j + 1);
				}

				//---- scan for multiple rings and turning movements ----

				rings = groups = total = 0;

				for (i=0, list = timing_list; i < nphase; i++, list++) {
					list_ptr = *list;
					if (list_ptr->Ring () > rings) {
						rings = list_ptr->Ring ();
					}
					if (list_ptr->Group () > groups) {
						groups = list_ptr->Group ();
					}
					total += split [i];
				}

				//---- no turning movements ----

				if (total == 0) goto next_signal;

				//---- check the index range ----

				if (rings > 4 || groups > 4) {
					Write (1, "Too Many Rings %d or Groups %d for Update Processing", rings, groups);
					goto next_signal;
				}

				//---- multi-ring processing ----

				if (rings > 1) {

					//---- set the ring and group codes ----

					for (j=1; j <= rings; j++) {
						for (i=0, list = timing_list; i < nphase; i++, list++) {
							list_ptr = *list;
							if (list_ptr->Ring () == j) break;
						}
						if (i == nphase) continue;

						first = list_ptr->Phase ();
						next = list_ptr->Next_Phase ();
						count = list_ptr->Group ();
						if (count == 0) count = 1;

						//---- follow the phase sequence ----

						for (; next != first; next = list_ptr->Next_Phase ()) {
							for (i=0, list = timing_list; i < nphase; i++, list++) {
								list_ptr = *list;
								if (list_ptr->Phase () == next) break;
							}
							if (i == nphase) break;

							//---- set the codes ----

							list_ptr->Ring (j);
							if (list_ptr->Group () == 0) {
								list_ptr->Group (count);
							} else {
								count = list_ptr->Group ();
							}
						}
					}
					memset (group_green, '\0', sizeof (group_green));
					memset (group_demand, '\0', sizeof (group_demand));

					//---- sum data by ring and group ----

					for (i=0, list = timing_list; i < nphase; i++, list++) {
						list_ptr = *list;
						cycle = MAX (list_ptr->Min_Green (), list_ptr->Max_Green ());

						group_green [list_ptr->Ring ()] [list_ptr->Group ()] += cycle;
						group_demand [list_ptr->Ring ()] [list_ptr->Group ()] += split [i];
					}

					//---- calculate the cycle length ----

					cycle = total = 0;

					for (j=1; j <= groups; j++) {
						maximum = count = 0;

						for (i=1; i <= rings; i++) {
							if (group_green [i] [j] > maximum) {
								maximum = group_green [i] [j];
							}
							if (group_demand [i] [j] > count) {
								count = group_demand [i] [j];
							}
						}
						group_green [0] [j] = maximum;
						group_demand [0] [j] = count;

						cycle += maximum;
						total += count;
					}

					//---- adjust the group allocation using max demand ----

					for (count=0; count < 5; count++) {
						if (total > 0) {
							factor = (double) cycle / total;
							total = 0;
						} else {
							factor = 1.0;
						}
						bucket = 0.45;
						adjust = false;

						//---- adjust each group separately ----

						for (j=1; j <= groups; j++) {
							maximum = group_demand [0] [j];

							time = factor * maximum + bucket;
							phase = (int) time;
							bucket = time - (double) phase;

							if (phase < min_phase) {
								phase = min_phase;
								adjust = true;
							}

							for (i=0; i <= rings; i++) {
								group_demand [i] [j] = 0;
							}
							if (maximum > 0) {
								factor2 = (double) phase / maximum;
							} else {
								factor2 = 1.0;
							}
							bucket2 = 0.45;

							//---- adjust the phase demands within the group ----

							for (i=0, list = timing_list; i < nphase; i++, list++) {
								list_ptr = *list;
								if (list_ptr->Group () == j) {
									time = factor2 * split [i] + bucket2;
									phase = (int) time;
									bucket2 = time - (double) phase;

									if (phase < min_phase) {
										phase = min_phase;
										adjust = true;
									}
									split [i] = phase;
									group_demand [list_ptr->Ring ()] [j] += phase;
								}
							}

							//---- calculate the new maximum for the group ----

							maximum = 0;

							for (i=1; i <= rings; i++) {
								if (group_demand [i] [j] > maximum) {
									maximum = group_demand [i] [j];
								}
							}
							group_demand [0] [j] = maximum;
							total += maximum;
						}
						if (!adjust) break;
					}

					//---- expand phase demand to group maximum ----

					for (phase=0, list = timing_list; phase < nphase; phase++, list++) {
						list_ptr = *list;
						i = list_ptr->Ring ();
						j = list_ptr->Group ();

						if (group_demand [i] [j] > 0 && group_demand [i] [j] < group_demand [0] [j]) {
							if (group_green [0] [j] > 0) {
								factor2 = (double) group_green [i] [j] / group_green [0] [j];
							} else {
								factor2 = 1.0;
							}
							factor = group_demand [0] [j] * factor2 / group_demand [i] [j];

							split [phase] = (int) (split [phase] * factor + 0.5);
						}
					}

				} else {		//---- single ring ----

					//---- get the cycle length ----

					cycle = 0;

					for (i=0, list = timing_list; i < nphase; i++, list++) {
						list_ptr = *list;
						cycle += MAX (list_ptr->Min_Green (), list_ptr->Max_Green ());
					}

					//---- adjust phasing by demand ----

					for (count=0; count < 5; count++) {

						if (total > 0) {
							factor = (double) cycle / total;
							total = 0;
						} else {
							factor = 1.0;
						}
						bucket = 0.45;
						adjust = false;

						for (i=0; i < nphase; i++) {
							time = factor * split [i] + bucket;
							phase = (int) time;
							bucket = time - (double) phase;

							if (phase < min_phase) {
								phase = min_phase;
								adjust = true;
							}
							split [i] = phase;
							total += phase;
						}
						if (!adjust) break;
					}
				}

				//---- update the green times ----

				for (i=0, list = timing_list; i < nphase; i++, list++) {
					list_ptr = *list;

					if (update_report) {
						if (i == 0) {
							if (!Break_Check (nphase + 1)) {
								Print (1);
							}
							if (signal_ptr != NULL) {
								str_fmt (buffer, sizeof (buffer), "%s-", time_range.Format_Step (signal_ptr->Start ()));
								j = signal_ptr->End ();
								if (j != MIDNIGHT) j++;
								str_cat (buffer, sizeof (buffer), time_range.Format_Step (j));

								Print (1, "%10d  %-11.11s %6d", signal_ptr->Node (), buffer, list_ptr->Timing ());
							}
						} else {
							Print (1, "%30c", BLANK);
						}
						Print (0, "   %3d  %7d  %4.1lf  %5d   %3d  %3d  %3d", 
							list_ptr->Phase (), demand [i], (double) lanes [i] / 10.0, critical [i],
							list_ptr->Min_Green (), list_ptr->Max_Green (), list_ptr->Ext_Green ());
					}

					phase = MAX (list_ptr->Min_Green (), list_ptr->Max_Green ());
					if (phase > 0) {
						factor = split [i] / (double) phase;
					} else {
						factor = 1.0;
					}
					phase = (int) (list_ptr->Min_Green () * factor + 0.5);
					if (phase < min_phase) phase = min_phase;
					list_ptr->Min_Green (phase);

					phase = (int) (list_ptr->Max_Green () * factor + 0.5);
					list_ptr->Max_Green (phase);

					phase = (int) (list_ptr->Ext_Green () * factor + 0.5);
					list_ptr->Ext_Green (phase);

					if (update_report) {
						Print (0, "   %3d  %3d  %3d", list_ptr->Min_Green (), 
							list_ptr->Max_Green (), list_ptr->Ext_Green ());
					}
				}
			}
next_signal:
			if (timing_ptr == NULL) break;

			//---- initialize the next timing plan ----

			last_timing = timing_ptr->Timing ();
			nphase = 0;

			update_ptr = (Update_List *) update_list.Get (&last_timing);

			if (update_ptr != NULL) {
				signal_ptr = signal_list->Record (update_ptr->signal);

				i = (signal_ptr->Start () + signal_ptr->End ()) / 2;
				i = time_range.In_Index (i);
				if (i == 0) i = 1;

				pocket_factor = pocket_factor_list.Best (i) / 1000.0;

			} else {
				signal_ptr = NULL;
			}
		}
		Show_Progress ();

		//---- accumulate phase data for the timing plan----

		if (nphase == 20) {
			Error ("Too Many Signal Phases");
		}
		phase_ptr = (Phase_Demand *) phase_demand [timing_data.Record_Index ()];

		phase_list [nphase] = phase_ptr;
		timing_list [nphase] = timing_ptr;
		nphase++;
	}
	End_Progress ();

	Header_Number (0);

	//---- write the timing records ----

	if (!delete_flag) {
		Show_Message ("Writing %s -- Record", timing_file->File_Type ());
		Set_Progress ();

		timing_db.Rewind ();

		while (timing_db.Read_Record ()) {
			Show_Progress ();

			timing_file->Copy_Fields (&timing_db);

			timing = timing_file->Timing ();

			if (update_list.Get_Index (&timing) == 0) {
				if (!timing_file->Write ()) {
					Error ("Writing %s", timing_file->File_Type ());
				}
			}
		}
		End_Progress ();
	}

	//---- save the changes ----

	Show_Message ("Updating %s -- Record", timing_file->File_Type ());
	Set_Progress ();

	timing_db.Rewind ();

	while (timing_db.Read_Record ()) {
		Show_Progress ();

		timing_file->Copy_Fields (&timing_db);

		timing = timing_file->Timing ();

		if (update_list.Get_Index (&timing) != 0) {
			timing_rec.Timing (timing);
			timing_rec.Phase (timing_file->Phase ());

			timing_ptr = timing_data.Get (&timing_rec);

			if (timing_ptr != NULL) {
				timing_file->Min_Green (timing_ptr->Min_Green ());
				timing_file->Max_Green (timing_ptr->Max_Green ());
				timing_file->Ext_Green (timing_ptr->Ext_Green ());
				ntiming++;
			}
			if (!timing_file->Write ()) {
				Error ("Writing %s", timing_file->File_Type ());
			}
		}
	}
	End_Progress ();
}

//---------------------------------------------------------
//	Update_Header
//---------------------------------------------------------

void IntControl::Update_Header (void)
{
	Print (1, "Signal Timing Updates");
	Print (2, "%24cTiming%23cSplit    Input Greens   Output Greens", BLANK, BLANK);
	Print (1, "    Signal  Time Period   Plan  Phase  Volume Lanes   Lane   Min  Max  Ext   Min  Max  Ext");
	Print (1);
}

/*********************************************|***********************************************

	Signal Timing Updates

	                       Timing                 Net   Split    Input Greens   Output Greens
	    Signal Time Period   Plan  Phase  Volume Lanes   Lane   Min  Max  Ext   Min  Max  Ext

	dddddddddd sssssssssss dddddd   ddd  ddddddd  dd.d  ddddd   ddd  ddd  ddd   ddd  ddd  ddd
	                                ddd  ddddddd  dd.d  ddddd   ddd  ddd  ddd   ddd  ddd  ddd
	                                ddd  ddddddd  dd.d  ddddd   ddd  ddd  ddd   ddd  ddd  ddd
	                                ddd  ddddddd  dd.d  ddddd   ddd  ddd  ddd   ddd  ddd  ddd

**********************************************|***********************************************/ 
