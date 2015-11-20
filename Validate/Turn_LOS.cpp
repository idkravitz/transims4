//*********************************************************
//	Turn_LOS.cpp - Turning Movement LOS Statistics
//*********************************************************

#include "Validate.hpp"

//---------------------------------------------------------
//	Turn_LOS
//---------------------------------------------------------

void Validate::Turn_LOS (void)
{
	int i, j, num, in_link_dir, link, dir, node, in_index, in_bearing;
	int out_link, out_index, total, volume, diff, tod;
	int timing, record, green, cycle, yellow, rings;
	double cap, lanes, share;

	bool left_lane [20];
	int left_est_vol, left_obs_vol, left_diff_vol;
	double left_est_vc, left_obs_vc, left_diff_per, left_green;
	char left_est_los, left_obs_los, left_diff_los [8];

	bool thru_lane [20];
	int thru_est_vol, thru_obs_vol, thru_diff_vol;
	double thru_est_vc, thru_obs_vc, thru_diff_per, thru_green;
	char thru_est_los, thru_obs_los, thru_diff_los [8];

	char *str_ptr, buffer [32], bearing [8];
	bool count_flag, new_line, left_flag, pass_flag;

	double los_ratio [5] = {0.55, 0.66, 0.775, 0.885, 0.985};

	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Connect_Time *connect_ptr;
	Signal_Data *signal_ptr;
	Timing_Data *timing_ptr, timing_rec;
	Phasing_Data *phasing_ptr;

	//---- estimate percent greens by phase ----

	timing = rings = 0;
	pass_flag = false;

	for (timing_ptr = timing_data.First_Key (); timing_ptr; timing_ptr = timing_data.Next_Key ()) {
		if (timing_ptr->Timing () != timing) {
			if (timing > 0) {
				if (!pass_flag) {
					timing_ptr = timing_data [record];
					pass_flag = true;

					//---- estimate the cycle length ----

					green = MAX (timing_rec.Min_Green (), timing_rec.Max_Green ());
					yellow = timing_rec.Yellow () + timing_rec.Red_Clear ();

					if (rings > 1) {
						green = (green + rings / 2) / rings;
						yellow = (yellow + rings / 2) / rings;
					}
					cycle = green + yellow;

					if (cycle < 60) {
						share = 1.0 - (yellow / 60.0);
					} else {
						share = 1.0 - ((double) yellow / cycle);
					}
					if (share < 0.9) {
						share = 0.9;
					}
					cycle = (int) (green / share + 0.5);

					//---- calculate percent green ----

					green = MAX (timing_ptr->Min_Green (), timing_ptr->Max_Green ());
					timing_ptr->Max_Green ((green * 100 + cycle / 2) / cycle);
					continue;
				} else {
					pass_flag = false;
				}
			}
			timing = timing_ptr->Timing ();
			rings = timing_ptr->Ring ();
			record = timing_data.Record_Index ();
			memcpy (&timing_rec, timing_ptr, sizeof (timing_rec));
		} else {
			if (pass_flag) {
				green = MAX (timing_ptr->Min_Green (), timing_ptr->Max_Green ());
				timing_ptr->Max_Green ((green * 100 + cycle / 2) / cycle);				
			} else {
				timing_rec.Min_Green (timing_rec.Min_Green () + timing_ptr->Min_Green ());
				timing_rec.Max_Green (timing_rec.Max_Green () + timing_ptr->Max_Green ());
				timing_rec.Yellow (timing_rec.Yellow () + timing_ptr->Yellow ());
				timing_rec.Red_Clear (timing_rec.Red_Clear () + timing_ptr->Red_Clear ());
				if (timing_ptr->Ring () > rings) rings = timing_ptr->Ring ();
			}
		}
	}

	//---- initialize the percent green ----

	for (dir_ptr = dir_data.First (); dir_ptr; dir_ptr = dir_data.Next ()) {
		dir_ptr->Thru (0);
		dir_ptr->Left (0);
	}

	//---- apply percent green to each approach link ----

	tod = time_periods.First ()->Low ();

	for (phasing_ptr = phasing_data.First (); phasing_ptr; phasing_ptr = phasing_data.Next ()) {

		//---- check the time of day restrictions ----

		signal_ptr = signal_data.Get_LE (phasing_ptr->Node (), tod);
		
		if (signal_ptr == NULL || signal_ptr->Timing () != phasing_ptr->Timing ()) continue;

		timing_rec.Timing (phasing_ptr->Timing ());
		timing_rec.Phase (phasing_ptr->Phase ());

		timing_ptr = timing_data.Get (&timing_rec);
		if (timing_ptr == NULL) continue;

		connect_ptr = connect_time.Get (phasing_ptr->In_Link_Dir (), phasing_ptr->Out_Link_Dir ());
		if (connect_ptr == NULL) continue;

		green = timing_ptr->Max_Green ();
		if (phasing_ptr->Protection () != PROTECTED) {
			green = green / 2;
		}

		//---- get the movement type ----

		link_ptr = link_data.Get (connect_ptr->In_Link ());
		if (link_ptr == NULL) continue;

		if (!connect_ptr->In_Dir ()) {
			dir_ptr = dir_data [link_ptr->AB_Dir ()];
		} else {
			dir_ptr = dir_data [link_ptr->BA_Dir ()];
		}
		if (dir_ptr == NULL) continue;

		if (connect_ptr->Type () == LEFT || connect_ptr->Type () == UTURN) {
			dir_ptr->Left (dir_ptr->Left () + green);
		} else {
			dir_ptr->Thru (dir_ptr->Thru () + green);
		}
	}

	//---- print the page header ----
	
	Header_Number (TURN_LOS);

	if (!Break_Check (connect_time.Num_Records () + 9)) {
		Print (1);
		Turn_Header ();
	}

	//---- print each approach link ----

	in_link_dir = link = dir = node = in_index = in_bearing = out_link = out_index = 0;
	count_flag = false;

	for (connect_ptr = connect_time.First_Key (); ; connect_ptr = connect_time.Next_Key ()) {

		if (connect_ptr == NULL || in_link_dir != connect_ptr->In_Link_Dir ()) {
			if (count_flag) {

				//---- get the link name ----

				if (street_name != NULL) {
					str_ptr = street_name [in_index];
				} else {
					str_ptr = NULL;
				}
				if (str_ptr == NULL) {
					str_ptr = str_fmt (buffer, sizeof (buffer), "Link %d %s", link, ((!dir) ? "AB" : "BA"));
				}

				//---- get the link direction ----
				
				str_cpy (bearing, sizeof (bearing), compass.Point_Text (in_bearing));

				if (bearing [1] == '\0') {
					bearing [1] = 'B';
					bearing [2] = '\0';
				}

				//---- print the street name ----

				Page_Check (2);
				Print (0, "\n%-25.25s %-2.2s  ", str_ptr, bearing);

				//---- get the cross street ----

				if (street_name != NULL) {
					str_ptr = street_name [out_index];
				} else {
					str_ptr = NULL;
				}
				if (str_ptr == NULL) {
					str_ptr = str_fmt (buffer, sizeof (buffer), "Node %d", node);
				}
				Print (0, "%-20.20s", str_ptr);

				//---- left ----

				if (left_est_vol > 0 || left_obs_vol > 0) {
					lanes = 0;

					for (i=0; i < 20; i++) {
						if (left_lane [i]) {
							if (thru_lane [i]) {
								lanes += 0.5;
							} else {
								lanes += 1.0;
							}
						}
					}
					if (lanes > 1.0) {
						cap = hours * 1740 * (1.0 + (0.8 * (lanes - 1.0)));
					} else {
						cap = hours * 1740;
					}
					left_est_vc = left_est_vol * left_green / cap;
					left_obs_vc = left_obs_vol * left_green / cap;

					for (i=0; i < 5; i++) {
						if (left_est_vc <= los_ratio [i]) break;
					}
					left_est_los = 'A' + i;

					for (j=0; j < 5; j++) {
						if (left_obs_vc <= los_ratio [j]) break;
					}
					left_obs_los = 'A' + j;

					left_diff_vol = left_est_vol - left_obs_vol;

					if (left_obs_vol > 0) {
						left_diff_per = left_diff_vol * 100.0 / left_obs_vol;
					} else {
						left_diff_per = 100.0;
					}
					j = abs (i - j);
					for (i=0; i < j; i++) {
						left_diff_los [i] = '*';
					}
					left_diff_los [i] = '\0';

					Print (0, "   Left    %6d %5.2lf  %c   %6d %5.2lf  %c   %6d  %6.1lf  %s", 
						left_est_vol, left_est_vc, left_est_los,
						left_obs_vol, left_obs_vc, left_obs_los,
						left_diff_vol, left_diff_per, left_diff_los);
					new_line = true;
				} else {
					new_line = false;
				}

				//---- thru ----

				if (thru_est_vol > 0 || thru_obs_vol > 0) {
					if (new_line) Print (0, "\n%50c", BLANK);

					lanes = 0;

					for (i=0; i < 20; i++) {
						if (thru_lane [i]) {
							if (left_lane [i]) {
								lanes += 0.5;
							} else {
								lanes += 1.0;
							}
						}
					}
					cap = hours * 1810 * lanes;

					thru_est_vc = thru_est_vol * thru_green / cap;
					thru_obs_vc = thru_obs_vol * thru_green / cap;

					for (i=0; i < 5; i++) {
						if (thru_est_vc <= los_ratio [i]) break;
					}
					thru_est_los = 'A' + i;

					for (j=0; j < 5; j++) {
						if (thru_obs_vc <= los_ratio [j]) break;
					}
					thru_obs_los = 'A' + j;

					thru_diff_vol = thru_est_vol - thru_obs_vol;

					if (thru_obs_vol > 0) {
						thru_diff_per = thru_diff_vol * 100.0 / thru_obs_vol;
					} else {
						thru_diff_per = 100.0;
					}
					j = abs (i - j);
					for (i=0; i < j; i++) {
						thru_diff_los [i] = '*';
					}
					thru_diff_los [i] = '\0';

					Print (0, "   Thru    %6d %5.2lf  %c   %6d %5.2lf  %c   %6d  %6.1lf  %s", 
						thru_est_vol, thru_est_vc, thru_est_los,
						thru_obs_vol, thru_obs_vc, thru_obs_los,
						thru_diff_vol, thru_diff_per, thru_diff_los);
				}
			}

			//---- initialize the next approach ----

			if (connect_ptr == NULL) break;

			in_link_dir = connect_ptr->In_Link_Dir ();
			link = connect_ptr->In_Link ();
			dir = connect_ptr->In_Dir ();

			link_ptr = link_data.Get (link);

			if (!dir) {
				dir_ptr = dir_data [link_ptr->AB_Dir ()];
				node = link_ptr->Bnode ();
			} else {
				dir_ptr = dir_data [link_ptr->BA_Dir ()];
				node = link_ptr->Anode ();
			}
			in_bearing = dir_ptr->Out_Bearing ();
			in_index = link_data.Record_Index () - 1;

			out_link = out_index = 0;

			left_green = dir_ptr->Left () / 100.0;
			memset (left_lane, '\0', sizeof (left_lane));

			left_est_vol = left_obs_vol = left_diff_vol = 0;
			left_est_vc = left_obs_vc = left_diff_per = 0.0;
			left_est_los = left_obs_los = 'A';
			left_diff_los [0] = '\0';

			thru_green = dir_ptr->Thru () / 100.0;
			memset (thru_lane, '\0', sizeof (thru_lane));

			thru_est_vol = thru_obs_vol = thru_diff_vol = 0;
			thru_est_vc = thru_obs_vc = thru_diff_per = 0.0;
			thru_est_los = thru_obs_los = 'A';
			thru_diff_los [0] = '\0';

			count_flag = false;
		}

		//---- sum the time period data ----

		num = connect_ptr->Periods ();
		if (num == 0) continue;

		total = volume = 0;

		for (i=1; i <= num; i++) {
			total += connect_ptr->Count (i);
			volume += connect_ptr->Volume (i);
		}

		if (volume > 0 || total > 0) {
			if (total > 0) count_flag = true;

			//---- find the movement type ----

			link_ptr = link_data.Get (connect_ptr->Out_Link ());

			if (connect_ptr->Type () == THRU || 
				connect_ptr->Type () == R_MERGE || connect_ptr->Type () == L_MERGE) {
				left_flag = false;
			} else if (connect_ptr->Type () == UTURN) {
				left_flag = true;
			} else {
				if (out_link == 0) {
					out_link = connect_ptr->Out_Link ();
					out_index = link_data.Record_Index () - 1;
				}				
				if (connect_ptr->Type () == LEFT) {
					left_flag = true;
				} else if (connect_ptr->Type () == RIGHT) {
					left_flag = false;
				} else {

					//---- diverge ----

					if (connect_ptr->Out_Dir () == 0) {
						dir_ptr = dir_data [link_ptr->AB_Dir ()];
					} else {
						dir_ptr = dir_data [link_ptr->BA_Dir ()];
					}
					diff = compass.Change (in_bearing, dir_ptr->In_Bearing ());

					if (diff < 0) {
						left_flag = true;
					} else {
						left_flag = false;
					}
				}
			}
			if (left_flag) {
				left_est_vol += volume;
				left_obs_vol += total;

				for (i=connect_ptr->In_Lane_Low (); i <= connect_ptr->In_Lane_High (); i++) {
					left_lane [i] = true;
				}
			} else {
				thru_est_vol += volume;
				thru_obs_vol += total;

				for (i=connect_ptr->In_Lane_Low (); i <= connect_ptr->In_Lane_High (); i++) {
					thru_lane [i] = true;
				}
			}
		}
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Turn_LOS_Header
//---------------------------------------------------------

void Validate::Turn_LOS_Header (void)
{
	Page_Check (5);
	Print (0, "\n%45cTurn Level of Service Validation\n", BLANK);
	Print (0, "\n%61c----Estimate----  ----Observed----  ------Difference------", BLANK);
	Print (0, "\nStreet/Link              Dir  Cross Street/Link    Movement  ");
	Print (0, "Volume  V/C  LOS  Volume  V/C  LOS  Volume Percent  LOS\n");
}

/**********************************************************|***********************************************************

                                             Turn Level of Service Validation

													         ----Estimate----  ----Observed----  ------Difference------
Street/Link              Dir  Cross Street/Link    Movement  Volume  V/C  LOS  Volume  V/C  LOS  Volume Percent  LOS

sssssssssssssssssssssssss NB  ssssssssssssssssssss   Left    dddddd ff.ff  c   dddddd ff.ff  c   dddddd  ffff.f  ssssss
                                                     Thru    dddddd ff.ff  c   dddddd ff.ff  c   dddddd  ffff.f  ssssss



***********************************************************|***********************************************************/ 
