//*********************************************************
//	Connect.cpp - Lane Connectivity
//*********************************************************

#include "TransimsNet.hpp"

#include "Connect_File.hpp"

char *turn_text [] = {"", "Thru Lane", "Right Split", "Left Split", "Right Merge", "Left Merge", 
						"Right Turn", "Left Turn", "U-Turn"};

//---------------------------------------------------------
//	Lane_Connect
//---------------------------------------------------------

void TransimsNet::Lane_Connect (int in_dir, Node_List *node_ptr)
{
	int type, lane, leg, num_leg, node, num_thru, out_dir;
	int thru_dir, thru_leg, left_leg, right_leg, uturn_dir, uturn_leg;
	int in_link, in_lane, in_thru, in_left, in_right, in_extra, in_max;
	int out_link, out_lane, out_thru, out_left, out_right, out_extra, out_max, out_lanes;
	int left_split, right_split, merge_left, merge_right, tot_left, tot_right;
	bool flag, lane_flag;

	Dir_Connect *in_ptr, *out_ptr, *dir_ptr;
	Connection *connect_ptr;
	Link_Detail_Data *detail_ptr;

	Connect_File *file = (Connect_File *) Network_Db_Base (NEW_LANE_CONNECTIVITY);

	bool lane_map [20] [20] [20];

	memset (lane_map, '\0', sizeof (lane_map));

	node = node_ptr->Node ();

	in_ptr = dir_data [in_dir];
	in_link = in_ptr->Link ();
	in_thru = in_ptr->Thru ();
	in_max = in_ptr->Left () + in_thru + in_ptr->Right_Turn ();
	in_left = in_right = 1;

	lane_flag = false;

	if (detail_flag) {
		detail_ptr = detail_data.Get (in_ptr->Link_Dir ());
		if (detail_ptr != NULL) {
			lane_flag = ((detail_ptr->Left () + detail_ptr->Left_Thru () + detail_ptr->Thru () + detail_ptr->Right_Thru () + detail_ptr->Right ()) > 0);
		}
	}

	//---- process a dead end link ----

	if (node_ptr->Type () == DEAD_END) {
		if (node_ptr->From_Dir () == 0) return;

		in_lane = in_ptr->Left () - in_ptr->Left_Turn () + 1;;	

		out_ptr = dir_data [node_ptr->From_Dir ()];
		out_link = out_ptr->Link ();

		out_lane = out_ptr->Left () - out_ptr->Left_Merge () + 1;

		nconnect++;

		file->Node (node);
		file->In_Link (in_link);
		file->In_Low (in_lane);
		file->In_High (in_lane);
		file->Out_Link (out_link);
		file->Out_Low (out_lane);
		file->Out_High (out_lane);
		file->Notes (turn_text [UTURN]);

		if (!file->Write ()) goto write_error;
		return;
	}

	//---- calculate movement totals ----

	out_lanes = out_max = out_thru = out_left = out_right = uturn_dir = 0;
	thru_leg = thru_dir = left_leg = right_leg = uturn_leg = 0;
	left_split = right_split = tot_left = tot_right = 0;

	for (leg=1, connect_ptr = in_ptr->To_First (); connect_ptr; leg++, connect_ptr = in_ptr->To_Next ()) {
		out_dir = connect_ptr->Dir ();
		out_ptr = dir_data [out_dir];

		if (connect_ptr->Is_Left ()) {
			out_left += out_ptr->Thru () + out_ptr->Left_Merge ();
			left_leg = leg;
		} else if (connect_ptr->Is_Right ()) {
			out_right += out_ptr->Thru () + out_ptr->Right_Merge ();
			if (right_leg == 0) right_leg = leg;
		} else if (connect_ptr->Is_L_Split ()) {
			left_split += out_ptr->Right_Merge () + out_ptr->Thru () + out_ptr->Left_Merge ();
			left_leg = leg;
		} else if (connect_ptr->Is_R_Split ()) {
			right_split += out_ptr->Left_Merge () + out_ptr->Thru () + out_ptr->Right_Merge ();
			if (right_leg == 0) right_leg = leg;
		} else if (connect_ptr->Is_Thru () || connect_ptr->Is_L_Merge () || connect_ptr->Is_R_Merge ()) {
			thru_leg = leg;
			thru_dir = out_dir;
			out_thru = out_ptr->Thru ();
			out_max = out_ptr->Left () + out_thru + out_ptr->Right_Merge ();
			out_lanes = out_ptr->Left_Merge () + out_thru + out_ptr->Right_Merge ();
		} else if (connect_ptr->Is_UTurn ()) {
			uturn_leg = leg;
			uturn_dir = out_dir;
		}
	}

	//---- check for odd conditions ----

	if (leg > 20) {
		Warning ("Too Many Legs at Node %d for Lane Connectivity Processing", node);
		return;
	}
	num_leg = leg - 1;

	tot_left = out_left + left_split;
	tot_right = out_right + right_split;

	if ((in_ptr->Left_Turn () && !tot_left) || (in_ptr->Right_Turn () && !tot_right)) {
		Warning ("Link %d at Node %d has Turn Pocket but No Road", in_link, node);
	}

	if ((out_thru + tot_left + tot_right) == 0) {
		if (uturn_dir == 0) {
			if (node > high_zone) {
				Warning ("Link %d at Node %d has No Exit Links", in_link, node);
			}
			return;
		} else {
			Warning ("Link %d at Node %d must U-Turn to Exit", in_link, node);
		}
	}

	//---- process thru lanes ----

	if (out_lanes) {
		in_lane = in_ptr->Left () + 1;
		in_extra = out_extra = 0;

		out_ptr = dir_data [thru_dir];
		out_lane = out_ptr->Left () + 1;

		//---- adjust the output lanes based on merges ----

		merge_left = merge_right = 0;
		flag = false;

		for (connect_ptr = out_ptr->From_First (); connect_ptr; connect_ptr = out_ptr->From_Next ()) {
			if (connect_ptr->Dir () == in_dir) {
				flag = true;
				continue;
			}
			if (!connect_ptr->Is_L_Merge () && !connect_ptr->Is_R_Merge () && !connect_ptr->Is_Thru ()) continue;

			dir_ptr = dir_data [connect_ptr->Dir ()];

			if (flag) {
				merge_right += dir_ptr->Right_Turn () + dir_ptr->Thru () + dir_ptr->Left_Turn ();
			} else {
				merge_left += dir_ptr->Left_Turn () + dir_ptr->Thru () + dir_ptr->Right_Turn ();
			}
		}
		if (merge_left || merge_right) {
			out_lane = out_ptr->Left () - out_ptr->Left_Merge () + 1;
			out_thru = out_lanes - merge_left - merge_right;

			if (in_thru > out_thru) {
				out_thru = in_thru - out_thru;

				merge_left -= (out_thru + out_ptr->Left_Merge ()) / 2;
				if (merge_left < 0) merge_left = 0;

				merge_right -= (out_thru + out_ptr->Right_Merge ()) / 2;
				if (merge_right < 0) merge_right = 0;

				out_thru = out_lanes - merge_left - merge_right;
				if (out_thru < 1) out_thru = 1;
				if (in_thru > out_thru) out_thru++;

				if (out_ptr->Left ()) out_lanes++;

				if (merge_left > 0 && (out_lane + merge_left + out_thru - 1) > out_lanes) {
					merge_left = out_lanes - out_lane - out_thru + 1;
					if (merge_left < 0) merge_left = 0;
				}
			}
			out_lane += merge_left;

			//---- prohibit cross over movements ----

			if (node_ptr->Type () == INTERCHANGE) {
				if (merge_left > 1) {
					tot_left = out_left = 0;
					if (in_ptr->Left_Turn ()) {
						in_lane = in_ptr->Left () - in_ptr->Left_Turn () + 1;
						in_thru++;
					}
				}
				if (merge_right > 1) {
					tot_right = out_right = 0;
					if (in_ptr->Right_Turn ()) {
						in_thru++;
					}
				}
			}
		}

		//---- adjust input thru lanes based on splits ----

		if (left_split > 0) {
			if (in_thru > out_thru) {
				in_left = MIN (left_split, in_thru + in_ptr->Left_Turn () - out_thru);
			} else if (left_split > 1 && in_ptr->Left_Turn ()) {
				in_left = in_ptr->Left () + 1;
			}
			if (in_left > in_lane && in_thru > out_thru) {
				in_thru += in_lane - in_left;
				in_lane = in_left;
			}
		}
		if (right_split > 0) {
			if (in_thru > out_thru) {
				in_right = MIN (right_split, in_thru + in_ptr->Right_Turn () - out_thru);
			} else if (right_split > 1 && in_ptr->Right_Turn ()) {
				in_right = in_ptr->Right_Turn () + 1;
			}
			if (in_thru - in_right < out_thru) {
				in_thru = out_thru;
			}
		}

		//---- set lane alignment ----

		if (in_thru > out_thru) {
			if (in_ptr->Left_Turn () && out_left > 1 && in_thru > 2) {
				in_left = in_ptr->Left () + 1;

				if (in_left > in_lane) {
					in_lane = in_left;
					in_thru--;
				} else {
					in_extra = 1;
				}
			}
			if (in_ptr->Right_Turn () && out_right > 1 && in_thru > 2) {
				in_right = in_ptr->Right () + 1;
			}
			if (in_thru - in_extra > out_thru) {
				if (out_ptr->Left_Merge () && out_lane > 1) {
					out_thru++;
					out_lane--;
				}
				if (in_thru > out_thru && out_ptr->Right_Merge ()) {
					out_thru++;
				}
				if (in_thru > out_thru) {
					if (tot_left > in_left) {
						in_left++;
						in_extra = 1;
					}
					if (in_thru - in_extra > out_thru) {
						if (tot_right > in_right) {
							in_right++;
						}
					}
				}
			}
		} else if (out_thru > in_thru) {
			out_extra = 1;
		}

		//---- set the thru lanes -----

		num_thru = (out_thru > in_thru) ? out_thru : in_thru;

		in_extra = ((in_thru + in_extra) >= num_thru);
		out_extra = ((out_thru + out_extra) >= num_thru);

		in_thru += in_lane - 1;
		if (in_thru > in_max) in_thru = in_max;

		out_thru += out_lane - 1;
		if (out_thru > out_max) out_thru = out_max;

		for (lane=0; lane < num_thru; lane++) {
			lane_map [thru_leg] [in_lane] [out_lane] = true;

			if (in_lane < in_thru && (lane || in_extra)) in_lane++;
			if (out_lane < out_thru && (lane || out_extra)) out_lane++;
		}

	} else if (out_left || out_right || left_split || right_split) {		//---- no thru lanes ----
		lane = 1;
		if (tot_left && in_ptr->Left_Turn () == 0) {
			in_thru--;
		} else if (tot_right && in_ptr->Right_Turn () == 0) {
			in_thru--;
		}
		while (in_thru > 0) {
			if (tot_left > lane || tot_right > lane) {
				if (tot_left > lane) {
					in_left++;
					in_thru--;
				}
				if (in_thru > 0 && tot_right > lane) {
					in_right++;
					in_thru--;
				}
			} else {
				if (tot_left > 0) {
					in_left++;
					in_thru--;
				}
				if (in_thru > 0 && tot_right > 0) {
					in_right++;
					in_thru--;
				}
			}
			lane++;
		}
	}

	//---- process left turns ----

	if (tot_left) {
		in_lane = in_ptr->Left () - in_ptr->Left_Turn () + 1;	

		connect_ptr = in_ptr->To_First ();

		//---- process turns to a single roadway ----

		if (left_leg == 1) {
			out_ptr = dir_data [connect_ptr->Dir ()];

			out_lane = out_ptr->Left () - out_ptr->Left_Merge () + 1;

			if (in_left == 1 && in_ptr->Left_Turn () > 1) in_left = in_ptr->Left_Turn ();

			for (lane=1; lane <= in_left; lane++, in_lane++) {
				lane_map [left_leg] [in_lane] [out_lane] = true;

				if (out_ptr->Thru () > lane || (out_ptr->Left_Merge () && out_ptr->Thru () >= lane)) {
					out_lane++;

					if (lane == in_left) {
						lane_map [left_leg] [in_lane] [out_lane] = true;
					}
				}
			}

		} else {	//---- turns to multiple roadways ----

			//---- process a single turn lane to multiple roadways ----

			if (in_left == 1) {

				for (leg=1; leg <= left_leg && connect_ptr; leg++, connect_ptr = in_ptr->To_Next ()) {
					out_ptr = dir_data [connect_ptr->Dir ()];

					out_lane = out_ptr->Left () - out_ptr->Left_Merge () + 1;

					lane_map [leg] [in_lane] [out_lane] = true;

					if (out_ptr->Thru () > 1 || (out_ptr->Left_Merge () && out_ptr->Thru () == 1)) {
						lane_map [leg] [in_lane] [out_lane + 1] = true;
					}
				}

			} else {	//---- process multiple turns ----

				lane = 1;
				flag = (in_left <= left_leg);

				for (leg=1; leg <= left_leg && connect_ptr; leg++, connect_ptr = in_ptr->To_Next ()) {
					out_ptr = dir_data [connect_ptr->Dir ()];

					out_lane = out_ptr->Left () - out_ptr->Left_Merge () + 1;

					lane_map [leg] [in_lane] [out_lane] = true;

					if (flag && leg == 1) continue;

					if (++lane <= in_left) {
						in_lane++; 
					} else {
						continue;
					}

					if (out_ptr->Left_Merge () || out_ptr->Thru () > 1) out_lane++;

					lane_map [leg] [in_lane] [out_lane] = true;

					//---- assign extra in lanes ----

					if (leg == left_leg && leg < in_left) {
						if (out_ptr->Thru () > 2 || (out_ptr->Left_Merge () && out_ptr->Thru () > 1)) {
							out_lane++;

							if (lane == in_left) {
								lane_map [leg] [in_lane] [out_lane] = true;
							}
						}
						while (++lane <= in_left) {
							in_lane++;

							lane_map [leg] [in_lane] [out_lane] = true;

							if (out_lane < (out_ptr->Left () + out_ptr->Thru () + out_ptr->Right_Merge ())) {
								out_lane++;

								if (lane == in_left) {
									lane_map [leg] [in_lane] [out_lane] = true;
								}
							}
						}
					}
				}
			}
		}
	}

	//---- process right turns ----

	if (tot_right) {
		in_lane = in_ptr->Left () + in_ptr->Thru () + in_ptr->Right_Turn ();

		connect_ptr = in_ptr->To_Last ();

		//---- process turns to a single roadway ----

		if (right_leg == num_leg) {

			out_ptr = dir_data [connect_ptr->Dir ()];

			out_lane = out_ptr->Left () + out_ptr->Thru () + out_ptr->Right_Merge ();

			if (in_right == 1 && in_ptr->Right_Turn () > 1) in_right = in_ptr->Right_Turn ();

			for (lane=1; lane <= in_right; lane++, in_lane--) {
				lane_map [right_leg] [in_lane] [out_lane] = true;

				if (out_ptr->Thru () > lane || (out_ptr->Right_Merge () && out_ptr->Thru () >= lane)) {
					out_lane--;

					if (lane == in_right) {
						lane_map [right_leg] [in_lane] [out_lane] = true;
					}
				}
			}

		} else {	//---- turns to multiple roadways ----

			//---- process a single turn lane to multiple roadways ----

			if (in_right == 1) {

				for (leg=num_leg; leg >= right_leg && connect_ptr; leg--, connect_ptr = in_ptr->To_Previous ()) {
					out_ptr = dir_data [connect_ptr->Dir ()];

					out_lane = out_ptr->Left () + out_ptr->Thru () + out_ptr->Right_Merge ();

					lane_map [leg] [in_lane] [out_lane] = true;

					if (out_ptr->Thru () > 1 || (out_ptr->Right_Merge () && out_ptr->Thru () == 1)) {
						lane_map [leg] [in_lane] [out_lane - 1] = true;
					}
				}

			} else {	//---- process multiple turns ----

				lane = 1;
				flag = (in_right < (num_leg - right_leg + 1));

				for (leg=num_leg; leg >= right_leg && connect_ptr; leg--, connect_ptr = in_ptr->To_Previous ()) {
					out_ptr = dir_data [connect_ptr->Dir ()];

					out_lane = out_ptr->Left () + out_ptr->Thru () + out_ptr->Right_Merge ();

					lane_map [leg] [in_lane] [out_lane] = true;

					if (flag && leg == num_leg) continue;

					if (right_split > 0 && leg == num_leg) {
						continue;
					} else if (++lane <= in_right) {
						in_lane--;
					} else {
						continue;
					}
					if (out_ptr->Right_Merge () || out_ptr->Thru () > 1) out_lane--;

					lane_map [leg] [in_lane] [out_lane] = true;

					//---- assign extra in lanes ----

					if (leg == right_leg && lane < in_right) {
						if (out_ptr->Thru () > 2 || (out_ptr->Right_Merge () && out_ptr->Thru () > 1)) {
							out_lane--;
							
							if (lane == in_right) {
								lane_map [leg] [in_lane] [out_lane] = true;
							}
						}
						while (++lane <= in_right) {
							in_lane--;

							lane_map [leg] [in_lane] [out_lane] = true;

							if (out_lane > 2 || out_ptr->Left_Merge () || 
								(out_lane == 2 && !out_ptr->Left_Turn ())) {

								out_lane--;

								if (lane == in_right) {
									lane_map [leg] [in_lane] [out_lane] = true;
								}
							}
						}
					}
				}
			}			
		}
	}

	//---- uturn ----

	if (uturn_dir) {
		in_lane = in_ptr->Left () - in_ptr->Left_Turn () + 1;

		out_ptr = dir_data [uturn_dir];
		out_lane = out_ptr->Left () - out_ptr->Left_Merge () + 1;
									
		lane_map [uturn_leg] [in_lane] [out_lane] = true;
	}

	//---- process the lane map ----

	for (leg=1, connect_ptr = in_ptr->To_First (); connect_ptr; leg++, connect_ptr = in_ptr->To_Next ()) {

		out_ptr = dir_data [connect_ptr->Dir ()];
		out_link = out_ptr->Link ();

		out_max = out_ptr->Left () + out_ptr->Thru () + out_ptr->Right_Merge ();

		type = connect_ptr->Type ();

		for (in_lane=1; in_lane <= in_max; in_lane++) {
			for (out_lane=1; out_lane <= out_max; out_lane++) {

				if (lane_map [leg] [in_lane] [out_lane]) {
					nconnect++;
					file->Node (node);
					file->In_Link (in_link);
					file->In_Low (in_lane);
					file->In_High (in_lane);
					file->Out_Link (out_link);
					file->Out_Low (out_lane);
					file->Out_High (out_lane);
					file->Type (Connect_Code ((Network_Code::Connect_Type) type));
					file->Notes (turn_text [type]);

					if (!file->Write ()) goto write_error;
				}
			}
		}
	}
	return;

write_error:
	Error ("Writing Lane Connectivity Record %d", nconnect);
}
