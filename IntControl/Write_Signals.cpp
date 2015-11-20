//*********************************************************
//	Write_Signals.cpp - Create the Signalized Node File
//*********************************************************

#include "IntControl.hpp"

#include "Timing_File.hpp"
#include "Phasing_File.hpp"
#include "Detector_File.hpp"
#include "Coordinator_File.hpp"

//---------------------------------------------------------
//	Write_Signals
//---------------------------------------------------------

void IntControl::Write_Signals (void)
{
	int dir, algorithm_field, count, period;
	int detect_offset, detect_length;
	char buffer [FIELD_BUFFER];
	bool type, adjust, flag_n, flag_i;

	int node, link, nphase, phase, related_phase;
	int i, j, n, p1, p2, nnext, out_link, num_in, num_out, detect;
	int lane1, lane2, coord, cap_i, cap_n, thru_lanes_n, thru_lanes_i, list, tod;
	char next_phases [20], *ptr, time_text [TIME_BUFFER];

	int thru_phase [20], protected_left [20], right_on_red [20];
	int left_low [20], left_high [20], left_lanes [20], left_vol [20];
	int thru_low [20], thru_high [20], thru_lanes [20], thru_vol [20];
	int right_low [20], right_high [20], right_lanes [20], right_vol [20];
	int merge_lanes [20], diverge_lanes [20];

	int phase_flag [20], gmin, gmax, gext;
	double phase_time [20], total, bucket, time, wt_i, wt_n;

	Node_List *node_ptr;
	Dir_Connect *dir_ptr, *from_dir, *thru_dir;
	Link_Use_Data *use_ptr;
	Signal_Data *signal_ptr;
	Connection *move_ptr, *thru_move;
	Connect_Time *connect_ptr;

	Signal_Array *signal_list;

	Signal_File *signal_file = (Signal_File *) Network_Db_Base (NEW_SIGNALIZED_NODE);
	Timing_File *timing_file = (Timing_File *) Network_Db_Base (NEW_TIMING_PLAN);
	Phasing_File *phasing_file = (Phasing_File *) Network_Db_Base (NEW_PHASING_PLAN);
	Detector_File *detector_file = (Detector_File *) Network_Db_Base (NEW_DETECTOR);
	Coordinator_File *coord_file = (Coordinator_File *) Network_Db_Base (NEW_SIGNAL_COORDINATOR);

	algorithm_field = signal_file->Optional_Field ("ALGORITHM");

	//---- write the signal files ----

	Show_Message ("Adding %s -- Record", signal_file->File_Type ());
	Set_Progress ();

	//---- write the signalized node file ----

	if (input_signal_flag) {
		signal_list = &input_signal;
	} else {
		signal_list = &(signal_data);
	}
	period = 1;
	time_range.Format_Step (0, time_text);

	//---- add the new signals ----

	for (signal_ptr = signal_list->First_Key (); signal_ptr; signal_ptr = signal_list->Next_Key ()) {
		Show_Progress ();

		node = signal_ptr->Node ();

		node_ptr = node_data.Get (node);

		if (node_ptr == NULL) {
			Warning ("Signalized Node %d is not in the Node File", node);
			continue;
		}
		time_range.Format_Step (signal_ptr->Start (), time_text);

		i = (signal_ptr->Start () + signal_ptr->End ()) / 2;
		tod = Round (i);

		//---- set the period parameters ----

		i = time_range.In_Index (i);
		if (i == 0) i = 1;

		cycle_len = cycle_len_list.Best (i);
		min_phase = min_phase_list.Best (i);
		yellow = yellow_list.Best (i);
		red_clear = red_clear_list.Best (i);
		pocket_factor = pocket_factor_list.Best (i) / 1000.0;
		general_factor = general_factor_list.Best (i) / 1000.0;
		extend_factor = extend_factor_list.Best (i) / 1000.0;
		min_cap = min_cap_list.Best (i);
		max_cap = max_cap_list.Best (i);

		//---- gather the period data ----

		if (regen_flag && connect_time.Periods () > 0) {
			p1 = connect_time.Period (signal_ptr->Start ());
			p2 = connect_time.Period (signal_ptr->End () - 1);
		} else if (time_flag) {
			period = time_range.Span_Index (signal_ptr->Start (), signal_ptr->End ());
		} else {
			tod = 0;
		}

		num_in = num_out = 0;
		memset (left_low, '\0', sizeof (left_low));
		memset (left_high, '\0', sizeof (left_high));
		memset (left_lanes, '\0', sizeof (left_lanes));
		memset (left_vol, '\0', sizeof (left_vol));

		memset (thru_low, '\0', sizeof (thru_low));
		memset (thru_high, '\0', sizeof (thru_high));
		memset (thru_lanes, '\0', sizeof (thru_lanes));
		memset (thru_vol, '\0', sizeof (thru_vol));

		memset (right_low, '\0', sizeof (right_low));
		memset (right_high, '\0', sizeof (right_high));
		memset (right_lanes, '\0', sizeof (right_lanes));
		memset (right_vol, '\0', sizeof (right_vol));

		memset (merge_lanes, '\0', sizeof (merge_lanes));
		memset (diverge_lanes, '\0', sizeof (diverge_lanes));

		//---- process the approach links and lanes ----

		for (n=0, dir = node_ptr->To_Dir (); dir; n++, dir = dir_ptr->To_List ()) {
			dir_ptr = dir_data [dir];
			num_in++;

			for (move_ptr = dir_ptr->To_First (); move_ptr; move_ptr = dir_ptr->To_Next ()) {
				connect_ptr = connect_time [move_ptr->Index ()];

				if (move_ptr->Is_Left ()) {
					if (left_high [n] == 0) {
						left_low [n] = connect_ptr->In_Lane_Low ();
						left_high [n] = connect_ptr->In_Lane_High ();
					} else {
						if (left_low [n] > connect_ptr->In_Lane_Low ()) {
							left_low [n] = connect_ptr->In_Lane_Low ();
						}
						if (left_high [n] < connect_ptr->In_Lane_High ()) {
							left_high [n] = connect_ptr->In_Lane_High ();
						}
					}
					if (turn_flag || delay_flag) {
						if (connect_ptr->Periods () != 0) {
							if (regen_flag) {
								for (period=p1; period <= p2; period++) {
									left_vol [n] += connect_ptr->Volume (period);
								}
							} else {
								left_vol [n] += connect_ptr->Volume (period);
							}
						}
					}
				} else if (move_ptr->Is_Thru () || move_ptr->Is_R_Merge () || 
					move_ptr->Is_L_Merge () || move_ptr->Is_R_Split () || move_ptr->Is_L_Split ()) {

					if (move_ptr->Is_R_Merge () || move_ptr->Is_L_Merge ()) {
						merge_lanes [n] = connect_ptr->In_Lane_High () - connect_ptr->In_Lane_Low () + 1;
					} else if (move_ptr->Is_R_Split () || move_ptr->Is_L_Split ()) {
						diverge_lanes [n] = connect_ptr->In_Lane_High () - connect_ptr->In_Lane_Low () + 1;
					}
					if (thru_high [n] == 0) {
						thru_low [n] = connect_ptr->In_Lane_Low ();
						thru_high [n] = connect_ptr->In_Lane_High ();
					} else {
						if (thru_low [n] > connect_ptr->In_Lane_Low ()) {
							thru_low [n] = connect_ptr->In_Lane_Low ();
						}
						if (thru_high [n] < connect_ptr->In_Lane_High ()) {
							thru_high [n] = connect_ptr->In_Lane_High ();
						}
					}
					if (turn_flag || delay_flag) {
						if (connect_ptr->Periods () != 0) {
							if (regen_flag) {
								for (period=p1; period <= p2; period++) {
									thru_vol [n] += connect_ptr->Volume (period);
								}
							} else {
								thru_vol [n] += connect_ptr->Volume (period);
							}
						}
					}
				} else if (move_ptr->Is_Right ()) {
					if (right_high [n] == 0) {
						right_low [n] = connect_ptr->In_Lane_Low ();
						right_high [n] = connect_ptr->In_Lane_High ();
					} else {
						if (right_low [n] > connect_ptr->In_Lane_Low ()) {
							right_low [n] = connect_ptr->In_Lane_Low ();
						}
						if (right_high [n] < connect_ptr->In_Lane_High ()) {
							right_high [n] = connect_ptr->In_Lane_High ();
						}
					}
					if (turn_flag || delay_flag) {
						if (connect_ptr->Periods () != 0) {
							if (regen_flag) {
								for (period=p1; period <= p2; period++) {
									right_vol [n] += connect_ptr->Volume (period);
								}
							} else {
								right_vol [n] += connect_ptr->Volume (period);
							}
						}
					}
				}
			}
			if (left_high [n] > 0) {
				left_lanes [n] = left_high [n] - left_low [n] + 1;
			}
			if (thru_high [n] > 0) {
				thru_lanes [n] = thru_high [n] - thru_low [n] + 1;
			}
			if (right_high [n] > 0) {
				right_lanes [n] = right_high [n] - right_low [n] + 1;
			}
		}

		//---- check the departure links ----

		for (dir = node_ptr->From_Dir (); dir; dir = dir_ptr->From_List ()) {
			dir_ptr = dir_data [dir];
			num_out++;
		}
		if (num_in == 0 || num_out == 0) {
			if (period == 1) Warning ("Signal Node %d does not have Entry and Exit Links", node);
			continue;
		}
		
		//---- define the phasing plan ----

		type = (signal_ptr->Type () == TIMED);

		nphase = 0;
		memset (thru_phase, '\0', sizeof (thru_phase));
		memset (protected_left, '\0', sizeof (protected_left));
		memset (right_on_red, '\0', sizeof (right_on_red));
		memset (phase_flag, '\0', sizeof (phase_flag));
		memset (phase_time, '\0', sizeof (phase_time));

		for (n=0, dir = node_ptr->To_Dir (); dir; n++, dir = dir_ptr->To_List ()) {
			dir_ptr = dir_data [dir];
		
			if (thru_phase [n]) continue;

			cap_n = dir_ptr->Capacity ();
			if (dir_ptr->Thru () > 1) {
				cap_n = (cap_n + 1) / dir_ptr->Thru ();
			}

			//---- adjust the number of thru lanes ----

			thru_lanes_n = thru_lanes [n];

			if (tod > 0) {
				list = dir_ptr->TOD_List ();

				for (; list > 0; list = use_ptr->TOD_List ()) {
					use_ptr = link_use_data [list]; 

					if (use_ptr->Start () <= tod && tod < use_ptr->End ()) {
						thru_lanes_n = use_ptr->Lanes ();
						break;
					}
				}
			}
			link = dir_ptr->Link ();
			move_ptr = dir_ptr->To_Thru ();

			if (move_ptr != NULL) {
				from_dir = dir_data [move_ptr->Dir ()];
				out_link = from_dir->Link ();
			} else {
				out_link = -1;
			}
			for (i=0, dir = node_ptr->To_Dir (); dir; i++, dir = from_dir->To_List ()) {
				from_dir = dir_data [dir];
				if (from_dir->Link () == out_link) break;

				thru_move = from_dir->To_Thru ();
				if (thru_move != NULL) {
					thru_dir = dir_data [thru_move->Dir ()];
					if (thru_dir->Link () == link) break;
				}
			}

			//---- check for competing thru links ----

			if (dir > 0 && (protected_left [i] != 0 || thru_phase [i] != 0)) {
				if (!left_lanes [n] && !right_lanes [n]) {

					//---- combine with existing thru phase ----

					thru_phase [n] = phase = thru_phase [i];
					wt_n = thru_lanes_n;

					if (method == VOLUME_SPLITS) {
						if (wt_n > 0) {
							total = thru_vol [n];
							if (total > 0) {
								wt_n = total / wt_n;
							} else if (thru_lanes_n > 0) {
								wt_n = cap_n * thru_lanes_n / wt_n;
							} else {
								wt_n = cap_n;
							}
						}
					} else if (method == CAPACITY_SPLITS) {
						wt_n *= cap_n;
					}
					phase--;
					wt_i = phase_time [phase];

					phase_time [phase] = 2.0 * MAX (wt_n, wt_i) * general_factor;
					continue;
				}
				dir = 0;	//---- provide full protection ----
			}

			//---- process conflicting movements ----

			if (dir > 0) {

				cap_i = from_dir->Capacity ();
				if (from_dir->Thru () > 1) {
					cap_i = (cap_i + 1) / from_dir->Thru ();
				}

				//---- adjust the number of thru lanes ----

				thru_lanes_i = thru_lanes [i];

				if (tod > 0) {
					list = from_dir->TOD_List ();

					for (; list > 0; list = use_ptr->TOD_List ()) {
						use_ptr = link_use_data [list]; 

						if (use_ptr->Start () <= tod && tod < use_ptr->End ()) {
							thru_lanes_i = use_ptr->Lanes ();
							break;
						}
					}
				}
				wt_n = left_lanes [n];
				wt_i = left_lanes [i];

				if (method == VOLUME_SPLITS) {
					if (wt_n > 0) {
						if (left_vol [n] > 0) {
							wt_n = left_vol [n] / wt_n;
						} else {
							wt_n = cap_n;
						}
					}
					if (wt_i > 0) {
						if (left_vol [i] > 0) {
							wt_i = left_vol [i] / wt_i;
						} else {
							wt_i = cap_i;
						}
					}
				} else if (method == CAPACITY_SPLITS) {
					wt_n *= cap_n;
					wt_i *= cap_i;
				}


				//---- pocket lane adjustment -----

				flag_n = flag_i = false;

				if (!dir_ptr->Left_Turn () || !from_dir->Left_Turn ()) {
					if (dir_ptr->Left_Turn ()) {
						if (wt_i > 0) {
							flag_i = true;
							wt_i = 0;
						}
					} else if (from_dir->Left_Turn ()) {
						if (wt_n > 0) {
							flag_n = true;
							wt_n = 0;
						}
					} else if (num_in > 2) {
						if (wt_n > 0) {
							flag_n = true;
							wt_n = 0;
						}
						if (wt_i > 0) {
							flag_i = true;
							wt_i = 0;
						}
					}
				}

				//---- set the phase data ----

				if (wt_n > 0 && wt_i > 0) {

					//---- protected lefts ----

					phase_flag [nphase] = 1;
					phase_time [nphase++] = MAX (wt_n, wt_i);
					protected_left [n] = protected_left [i] = nphase;
					thru_phase [n] = thru_phase [i] = nphase + 1;

				} else if (wt_n > 0) {	

					//---- protected left+thru ----

					phase_flag [nphase] = 1;
					phase_time [nphase++] = wt_n;
					protected_left [n] = nphase;
					thru_phase [n] = -nphase;
					thru_phase [i] = nphase + 1;

				} else if (wt_i > 0) {

					//---- protected left+thru ----

					phase_flag [nphase] = 1;
					phase_time [nphase++] = wt_i;
					protected_left [i] = nphase;
					thru_phase [i] = -nphase;
					thru_phase [n] = nphase + 1;

				} else {

					thru_phase [n] = thru_phase [i] = nphase + 1;

				}

				//---- general green phase ----

				wt_n = thru_lanes_n + (dir_ptr->Right_Turn () + ((flag_n) ? left_lanes [n] : 0)) * pocket_factor;
				wt_i = thru_lanes_i + (from_dir->Right_Turn () + ((flag_i) ? left_lanes [i] : 0)) * pocket_factor;

				if (method == VOLUME_SPLITS) {
					if (wt_n > 0) {
						total = thru_vol [n] + right_vol [n] + ((flag_n) ? left_vol [n] : 0);
						if (total > 0) {
							wt_n = total / wt_n;

							//---- subtract the protected left+thru weight ----

							if (thru_phase [n] < 0) {
								wt_n -= phase_time [-1-thru_phase [n]];
								if (wt_n < 0) wt_n = 0;
							}
						} else if (thru_lanes_n > 0) {
							wt_n = cap_n * thru_lanes_n / wt_n;
						} else {
							wt_n = cap_n;
						}
					}
					if (wt_i > 0) {
						total = thru_vol [i] + right_vol [i] + ((flag_i) ? left_vol [i] : 0);
						if (total > 0) {
							wt_i = total / wt_i;

							//---- subtract the protected left+thru weight ----

							if (thru_phase [i] < 0) {
								wt_i -= phase_time [-1-thru_phase [i]];
								if (wt_i < 0) wt_i = 0;
							}
						} else if (thru_lanes_i > 0) {
							wt_i = cap_i * thru_lanes_i / wt_i;
						} else {
							wt_i = cap_i;
						}
					}
				} else if (method == CAPACITY_SPLITS) {
					wt_n *= cap_n;
					wt_i *= cap_i;
				}
				phase_time [nphase++] = 2.0 * MAX (wt_n, wt_i) * general_factor;

			} else {
				
				//---- fully protected phase ----

				if (thru_lanes_n == 0 && left_lanes [n] == 0) {
					thru_lanes_n = right_lanes [n];

					if (thru_lanes_n == 0) {
						if (merge_lanes [n] == 0 && diverge_lanes [n] == 0) {
							if (period == 1) {
								Warning ("No Lane Connectivity for Link %d at Node %d", link, node_ptr->Node ());
							}
							continue;
						} else if (merge_flag) {
							Warning ("Link %d at Node %d has Merge/Diverge Movements", link, node_ptr->Node ());
							thru_lanes_n = MAX (merge_lanes [n], diverge_lanes [n]);
						}
					}
				}
				wt_n = thru_lanes_n + dir_ptr->Right_Turn () * pocket_factor + 
					left_lanes [n] - (1.0 - pocket_factor) * dir_ptr->Left_Turn ();

				if (method == VOLUME_SPLITS) {
					if (wt_n > 0) {
						total = left_vol [n] + thru_vol [n] + right_vol [n];
						if (total > 0) {
							wt_n = total / wt_n;
						} else if (thru_lanes_n > 0) {
							wt_n = cap_n * thru_lanes_n / wt_n;
						} else {
							wt_n = cap_n;
						}
					}
				} else if (method == CAPACITY_SPLITS) {
					wt_n *= cap_n;
				}
				phase_time [nphase++] = wt_n;

				thru_phase [n] = protected_left [n] = nphase;
			}
		}

		//---- set the right on red phases ----

		for (n=0, dir = node_ptr->To_Dir (); dir; n++, dir = dir_ptr->To_List ()) {
			dir_ptr = dir_data [dir];

			for (i=0; i < num_in; i++) {
				if (n != i && thru_phase [n] != thru_phase [i]) {
					if (thru_phase [n] < 0 && thru_phase [i] == (1 - thru_phase [n])) continue;
					if (thru_phase [i] < 0 && thru_phase [n] == (1 - thru_phase [i])) continue;

					right_on_red [n] = (thru_phase [i] < 0) ? (1 - thru_phase [i]) : thru_phase [i];
					break;
				}
			}
		}

		//---- check for one phase signals ----

		if (nphase == 1) {
			if (period == 1) Warning ("Node %d has a One Phase Signal", node);
			continue;
		}

		//---- convert weights to split times ----

		count = 0;
retry:
		total = 0.0;
		phase = 0;

		for (i=0; i < nphase; i++) {
			total += phase_time [i];
			if (!phase_flag [i]) phase++;
		}

		if (total == 0.0) {
			if (period == 1) Warning ("Total Lanes Entering Node %d are Zero", node);
			continue;
		}

		total = (cycle_len - phase * (yellow + red_clear)) / total;

		bucket = 0.45;
		adjust = false;

		for (i=0; i < nphase; i++) {
			time = total * phase_time [i] + bucket;
			phase = (int) time;
			bucket = time - (double) phase;

			if (phase < min_phase) {
				phase = min_phase;
				adjust = true;
			}
			phase_time [i] = (double) phase;
		}

		if (adjust && count++ < 5) {
			goto retry;
		}

		//---- process each link entering the node ----

		for (n=0, dir = node_ptr->To_Dir (); dir; n++, dir = dir_ptr->To_List ()) {
			dir_ptr = dir_data [dir];

			link = dir_ptr->Link ();

			//---- save the timing plan and signal record ----

			if (!n) {
				max_timing++;

				for (i=0, phase=1; i < nphase; i++, phase++) {
					if (type) {
						nnext = 1;
						gmin = (int) (phase_time [i] + 0.5);
						gmax = 0; 
						gext = 0;
					} else {
						nnext = nphase - 1;
//						gmin = min_phase;
//						gmax = (int) (phase_time [i] + 0.5
						gmin = (int) (phase_time [i] + 0.5);
						gmax = 0;
						gext = (int) (phase_time [i] * extend_factor + 0.5);
					}
					related_phase = phase;
					ptr = next_phases;

					for (j=0; j < nnext; j++) {
						related_phase = ((related_phase < nphase) ? related_phase + 1 : 1);
						if (j) {
							*ptr++ = '/';
						}
						*ptr++ = '0' + related_phase;
					}
					*ptr = '\0';

					//---- write the signal timing plan record ----

					timing_file->Timing (max_timing);
					timing_file->Phase (phase);
					timing_file->Next_Phase (next_phases);
					timing_file->Min_Green (gmin);
					timing_file->Max_Green (gmax);
					timing_file->Ext_Green (gext); 
					timing_file->Yellow ((phase_flag [i] == 0) ? yellow : 0);
					timing_file->Red_Clear ((phase_flag [i] == 0) ? red_clear : 0);
					timing_file->Ring ((i == 0) ? 1 : 0);
					timing_file->Group (0);
					timing_file->Notes (str_fmt (buffer, sizeof (buffer), "NODE %d", node));

					if (!timing_file->Write ()) goto timing_error;
					ntiming++;
				}

				//---- write the signal coordinator record ----

				if (coord_file != NULL) {
					coord_file->ID (++max_coord);
					coord_file->Notes ("IntControl Generated");

					if (!coord_file->Write ()) goto coord_error;
					ncoord++;
					coord = max_coord;
				} else {
					coord = node;
				}

				//---- write the signal record ----

				nsignal++;

				signal_file->Node (node);
				if (type) {
					signal_file->Type ("T");
				} else {
					signal_file->Type ("A");
				}
				signal_file->Timing (max_timing);
				signal_file->Offset (0);
				signal_file->Start (time_text);
				signal_file->Coordinator (coord); 
				signal_file->Ring ("S");

				if (algorithm_field > 0) {
					signal_file->Put_Field (algorithm_field, "B");
				}
				signal_file->Notes (str_fmt (buffer, sizeof (buffer), "%d Phase %s", nphase, ((type) ? "Timed" : "Actuated")));

				if (!signal_file->Write ()) goto signal_error;

			}

			//---- process left turn lane ----

			if (left_lanes [n] > 0) {
		
				phase = protected_left [n];
				related_phase = thru_phase [n];

				//---- store detector ----

				if (type) {	
					detect = 0;
				} else {
					lane1 = left_low [n];
					lane2 = left_high [n];

					if (lane2 >= thru_low [n] && thru_low [n] > 0) {
						lane2 = thru_low [n] - 1;
						if (lane2 < lane1 && related_phase < 0) {
							lane2 = lane1;
						}
					}
					if (lane1 <= lane2) {
						detect_offset = dir_ptr->Setback ();

						if (dir_ptr->Left_Turn () > 0) {
							detect_length = (detector_length < dir_ptr->Offset ()) ? detector_length : dir_ptr->Offset ();
						} else {	
							detect_length = (detector_length < dir_ptr->Length ()) ? detector_length : dir_ptr->Length ();
						}
						if (detector_file != NULL) {
							detect = ++max_detector;
							ndetector++;

							detector_file->ID (max_detector);
							detector_file->Node (node);
							detector_file->Link (link);
							detector_file->Offset (UnRound (detect_offset)); 	
							detector_file->Length (UnRound (detect_length));
							detector_file->Low_Lane (lane1);
							detector_file->High_Lane (lane2);
							detector_file->Type ("PRESENCE");
							detector_file->Coordinator (coord);
							detector_file->Notes ("Left Turn");

							if (!detector_file->Write ()) goto detector_error;
						}
					} else {
						detect = 0;
					}
				}

				//---- store phasing plan ----

				for (move_ptr = dir_ptr->To_First (); move_ptr; move_ptr = dir_ptr->To_Next ()) {
					if (!move_ptr->Is_Left () && !move_ptr->Is_UTurn ()) continue;

					from_dir = dir_data [move_ptr->Dir ()];
					out_link = from_dir->Link ();

					if (phase) {
						phasing_file->Node (node);
						phasing_file->Timing (max_timing);
						phasing_file->Phase (phase);
						phasing_file->In_Link (link);
						phasing_file->Out_Link (out_link);
						phasing_file->Protection ("P");
						phasing_file->Detectors (str_fmt (buffer, sizeof (buffer), "%d", detect));
						phasing_file->Notes ("Protected Left");

						if (!phasing_file->Write ()) goto phasing_error;
					}

					if (phase != related_phase) {
						phasing_file->Node (node);
						phasing_file->Timing (max_timing);
						phasing_file->Phase (((related_phase < 0) ? (1 - related_phase) : related_phase));
						phasing_file->In_Link (link);
						phasing_file->Out_Link (out_link);
						phasing_file->Protection ("U");
						if (!phase) {
							phasing_file->Detectors (str_fmt (buffer, sizeof (buffer), "%d", detect));
						} else {
							phasing_file->Detectors ("0");
						}
						phasing_file->Notes ("Unprotected Left");

						if (!phasing_file->Write ()) goto phasing_error;
					}
				}
			}

			//---- store thru lane detector ----

			if (type) {
				detect = 0;
			} else {
				lane1 = thru_low [n];
				lane2 = thru_high [n];

				if (lane1 == 0) {
					lane1 = right_low [n];
				}
				if (right_lanes [n] > 0 && right_high [n] > lane2) {
					lane2 = right_high [n];
				}

				detect_offset = dir_ptr->Setback ();
				detect_length = (detector_length < dir_ptr->Length ()) ? detector_length : dir_ptr->Length ();

				if (detector_file != NULL) {
					detector_file->ID (++max_detector);
					detector_file->Node (node);
					detector_file->Link (link);
					detector_file->Offset (UnRound (detect_offset)); 	
					detector_file->Length (UnRound (detect_length));
					detector_file->Low_Lane (lane1);
					detector_file->High_Lane (lane2);
					detector_file->Type ("PRESENCE");
					detector_file->Coordinator (coord);
					detector_file->Notes ("Thru Lane");

					if (!detector_file->Write ()) goto detector_error;
					ndetector++;
				}
				detect = max_detector;
			}

			//---- store phasing plan ----

			phase = thru_phase [n];
			related_phase = right_on_red [n];

			for (move_ptr = dir_ptr->To_First (); move_ptr; move_ptr = dir_ptr->To_Next ()) {
				if (move_ptr->Is_Left () || move_ptr->Is_UTurn ()) continue;

				from_dir = dir_data [move_ptr->Dir ()];
				out_link = from_dir->Link ();

				nphasing++;
				phasing_file->Node (node);
				phasing_file->Timing (max_timing);
				phasing_file->Phase (((phase < 0) ? (-phase) : phase));
				phasing_file->In_Link (link);
				phasing_file->Out_Link (out_link);
				phasing_file->Protection ("P");
				if (phase < 0) {
					phasing_file->Detectors ("0");
				} else {
					phasing_file->Detectors (str_fmt (buffer, sizeof (buffer), "%d", detect));
				}

				if (move_ptr->Is_Right ()) {
					phasing_file->Notes ("Protected Right");
				} else if (move_ptr->Is_R_Merge ()) {
					phasing_file->Notes ("Protected Right Merge");
				} else if (move_ptr->Is_L_Merge ()) {
					phasing_file->Notes ("Protected Left Merge");
				} else if (move_ptr->Is_R_Split ()) {
					phasing_file->Notes ("Protected Right Split");
				} else if (move_ptr->Is_L_Split ()) {
					phasing_file->Notes ("Protected Left Split");
				} else {
					phasing_file->Notes ("Protected Thru");
				}
				if (!phasing_file->Write ()) goto phasing_error;

				if (phase < 0) {
					nphasing++;
					phasing_file->Node (node);
					phasing_file->Timing (max_timing);
					phasing_file->Phase ((1 - phase));
					phasing_file->In_Link (link);
					phasing_file->Out_Link (out_link);
					phasing_file->Protection ("P");
					phasing_file->Detectors (str_fmt (buffer, sizeof (buffer), "%d", detect));

					if (move_ptr->Is_Right ()) {
						phasing_file->Notes ("Protected Right");
					} else if (move_ptr->Is_R_Merge ()) {
						phasing_file->Notes ("Protected Right Merge");
					} else if (move_ptr->Is_L_Merge ()) {
						phasing_file->Notes ("Protected Left Merge");
					} else if (move_ptr->Is_R_Split ()) {
						phasing_file->Notes ("Protected Right Split");
					} else if (move_ptr->Is_L_Split ()) {
						phasing_file->Notes ("Protected Left Split");
					} else {
						phasing_file->Notes ("Protected Thru");
					}
					if (!phasing_file->Write ()) goto phasing_error;
				}

				if (move_ptr->Is_Right () && related_phase) {
					nphasing++;
					phasing_file->Node (node);
					phasing_file->Timing (max_timing);
					phasing_file->Phase (related_phase);
					phasing_file->In_Link (link);
					phasing_file->Out_Link (out_link);
					phasing_file->Protection ("S");
					phasing_file->Detectors ("0");
					phasing_file->Notes ("Right on Red");

					if (!phasing_file->Write ()) goto phasing_error;
				}
			}
		}
	}
	End_Progress ();

	signal_file->Close ();
	timing_file->Close ();
	phasing_file->Close ();

	if (detector_file != NULL) detector_file->Close ();
	if (coord_file != NULL) coord_file->Close ();
	return;

signal_error:
	Error ("Writing Signalized Node Record %d", nsignal);

timing_error:
	Error ("Writing Timing Plan Record %d", max_timing);

phasing_error:
	Error ("Writing Phasing Plan Record %d", nphasing);

detector_error:
	Error ("Writing Detector Record %d", ndetector);

coord_error:
	Error ("Writing Signal Coordinator Record %d", ncoord);
}
