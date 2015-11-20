//*********************************************************
//	Turn_Stats.cpp - Turning Movement Statistics
//*********************************************************

#include "Validate.hpp"

//---------------------------------------------------------
//	Turn_Stats
//---------------------------------------------------------

void Validate::Turn_Stats (void)
{
	int i, num, in_link_dir, link, dir, node, in_index, in_bearing;
	int out_link, out_index, total, volume, diff;
	int left_obs, thru_obs, right_obs, uturn_obs;
	int left_est, thru_est, right_est, uturn_est;
	double per_left_est, per_thru_est, per_right_est, per_uturn_est, factor;
	double per_left_obs, per_thru_obs, per_right_obs, per_uturn_obs;
	char *str_ptr, buffer [32], bearing [8];
	bool count_flag;

	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Connect_Time *connect_ptr;

	//---- print the page header ----
	
	Header_Number (TURN_MOVEMENT);

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

				Page_Check (5);
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

				//---- estimate ----

				Print (0, "%-20.20s  Estimate   %6d %6d %6d %6d", str_ptr,
					left_est, thru_est, right_est, uturn_est);

				total = left_est + thru_est + right_est + uturn_est;
				if (total < 1) total = 1;
				factor = 100.0 / total;

				per_left_est = left_est * factor;
				per_thru_est = thru_est * factor;
				per_right_est = right_est * factor;
				per_uturn_est = uturn_est * factor;

				Print (0, "  %6.1lf %6.1lf %6.1lf %6.1lf", 
					per_left_est, per_thru_est, per_right_est, per_uturn_est);

				//---- observed ----

				Print (0, "\n%52cObserved   %6d %6d %6d %6d", BLANK,
					left_obs, thru_obs, right_obs, uturn_obs);

				total = left_obs + thru_obs + right_obs + uturn_obs;
				if (total < 1) total = 1;
				factor = 100.0 / total;

				per_left_obs = left_obs * factor;
				per_thru_obs = thru_obs * factor;
				per_right_obs = right_obs * factor;
				per_uturn_obs = uturn_obs * factor;

				Print (0, "  %6.1lf %6.1lf %6.1lf %6.1lf", 
					per_left_obs, per_thru_obs, per_right_obs, per_uturn_obs);

				//---- difference ----

				left_est -= left_obs;
				thru_est -= thru_obs;
				right_est -= right_obs;
				uturn_est -= uturn_obs;

				Print (0, "\n%52cDifference %6d %6d %6d %6d", BLANK, 
					left_est, thru_est, right_est, uturn_est);

				per_left_est -= per_left_obs;
				per_thru_est -= per_thru_obs;
				per_right_est -= per_right_obs;
				per_uturn_est -= per_uturn_obs;

				Print (0, "  %6.1lf %6.1lf %6.1lf %6.1lf", 
					per_left_est, per_thru_est, per_right_est, per_uturn_est);

				//---- percent error ----

				if (per_left_obs != 0.0) {
					per_left_est = 100.0 * per_left_est / per_left_obs;
					if (per_left_est > 999.9) {
						per_left_est = 999.9;
					} else if (per_left_est < -999.9) {
						per_left_est = -999.9;
					}
				}
				if (per_thru_obs != 0.0) {
					per_thru_est = 100.0 * per_thru_est / per_thru_obs;
					if (per_thru_est > 999.9) {
						per_thru_est = 999.9;
					} else if (per_thru_est < -999.9) {
						per_thru_est = -999.9;
					}
				}
				if (per_right_obs != 0.0) {
					per_right_est = 100.0 * per_right_est / per_right_obs;
					if (per_right_est > 999.9) {
						per_right_est = 999.9;
					} else if (per_right_est < -999.9) {
						per_right_est = -999.9;
					}
				}
				if (per_uturn_obs != 0.0) {
					per_uturn_est = 100.0 * per_uturn_est / per_uturn_obs;
					if (per_uturn_est > 999.9) {
						per_uturn_est = 999.9;
					} else if (per_uturn_est < -999.9) {
						per_uturn_est = -999.9;
					}
				}

				//---- volume error ----

				per_left_obs = 100.0 * left_est / ((left_obs) ? left_obs : 1);
				if (per_left_obs > 999.9) {
					per_left_obs = 999.9;
				} else if (per_left_obs < -999.9) {
					per_left_obs = -999.9;
				}
				per_thru_obs = 100.0 * thru_est / ((thru_obs) ? thru_obs : 1);
				if (per_thru_obs > 999.9) {
					per_thru_obs = 999.9;
				} else if (per_thru_obs < -999.9) {
					per_thru_obs = -999.9;
				}
				per_right_obs = 100.0 * right_est / ((right_obs) ? right_obs : 1);
				if (per_right_obs > 999.9) {
					per_right_obs = 999.9;
				} else if (per_right_obs < -999.9) {
					per_right_obs = -999.9;
				}
				per_uturn_obs = 100.0 * uturn_est / ((uturn_obs) ? uturn_obs : 1);
				if (per_uturn_obs > 999.9) {
					per_uturn_obs = 999.9;
				} else if (per_uturn_obs < -999.9) {
					per_uturn_obs = -999.9;
				}

				Print (0, "\n%52c%% Error    %6.1lf %6.1lf %6.1lf %6.1lf", BLANK, 
					per_left_obs, per_thru_obs, per_right_obs, per_uturn_obs);

				Print (0, "  %6.1lf %6.1lf %6.1lf %6.1lf\n", 
					per_left_est, per_thru_est, per_right_est, per_uturn_est);
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
			left_obs = thru_obs = right_obs = uturn_obs = 0;
			left_est = thru_est = right_est = uturn_est = 0;
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
				connect_ptr->Type () == L_MERGE || connect_ptr->Type () == R_MERGE) {
				thru_est += volume;
				thru_obs += total;
			} else if (connect_ptr->Type () == UTURN) {
				uturn_est += volume;
				uturn_obs += total;
			} else {
				if (out_link == 0) {
					out_link = connect_ptr->Out_Link ();
					out_index = link_data.Record_Index () - 1;
				}				
				if (connect_ptr->Type () == LEFT) {
					left_est += volume;
					left_obs += total;
				} else if (connect_ptr->Type () == RIGHT) {
					right_est += volume;
					right_obs += total;
				} else {

					//---- splits ----

					if (connect_ptr->Out_Dir () == 0) {
						dir_ptr = dir_data [link_ptr->AB_Dir ()];
					} else {
						dir_ptr = dir_data [link_ptr->BA_Dir ()];
					}
					diff = compass.Change (in_bearing, dir_ptr->In_Bearing ());

					if (diff < 0) {
						left_est += volume;
						left_obs += total;
					} else {
						right_est += volume;
						right_obs += total;
					}
				}
			}
		}
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Turn_Header
//---------------------------------------------------------

void Validate::Turn_Header (void)
{
	Page_Check (5);
	Print (0, "\n%47cTurning Movement Validation\n", BLANK);
	Print (0, "\n%63c-----------Volume---------- -----------Percent----------", BLANK);
	Print (0, "\nStreet/Link              Dir  Cross Street/Link     Source     ");
	Print (0, "  Left   Thru  Right  Uturn    Left   Thru  Right  Uturn\n");
}

/**********************************************************|***********************************************************

                                               Turning Movement Validation
									
                                                               -----------Volume---------- -----------Percent----------
Street/Link              Dir  Cross Street/Link     Source       Left   Thru  Right  Uturn    Left   Thru  Right  Uturn

sssssssssssssssssssssssss NB  ssssssssssssssssssss  Estimate   dddddd dddddd dddddd dddddd  ffff.f ffff.f ffff.f ffff.f
                                                    Observed   dddddd dddddd dddddd dddddd  ffff.f ffff.f ffff.f ffff.f
                                                    Difference dddddd dddddd dddddd dddddd  ffff.f ffff.f ffff.f ffff.f
													% Error    ffff.f ffff.f ffff.f ffff.f  ffff.f ffff.f ffff.f ffff.f


***********************************************************|***********************************************************/ 
