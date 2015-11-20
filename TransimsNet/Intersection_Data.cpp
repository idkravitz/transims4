//*********************************************************
//	Intersect.cpp - Create Intersection Data
//*********************************************************

#include "TransimsNet.hpp"

#include "Sign_File.hpp"
#include "Signal_File.hpp"

//---------------------------------------------------------
//	Intersection_Data
//---------------------------------------------------------

void TransimsNet::Intersection_Data (void)
{
	int i, dir, link, num, zone, type, link_num [20], control, control_type;
	int primary, secondary, primary_in, secondary_in, min_in, max_in, signal_type, ring_type;
	bool sign_flag;

	Node_List *node_ptr;
	Zone_Data *zone_ptr;
	Dir_Connect *dir_ptr, *thru_ptr;
	Link_Data *link_ptr;
	Area_Type *type_ptr, default_rec;
	Connection *connect_ptr;
	Link_Detail_Data *detail_ptr;

	sign_flag = Network_File_Flag (UNSIGNALIZED_NODE);
	Sign_File *sign_file = (Sign_File *) Network_Db_Base (NEW_UNSIGNALIZED_NODE);
	Signal_File *signal_file = (Signal_File *) Network_Db_Base (NEW_SIGNALIZED_NODE);

	default_rec.Primary (MAJOR);
	default_rec.Secondary (MINOR);
	default_rec.Signal_Type (ACTUATED);
	default_rec.Ring_Type (SINGLE_RING);
	default_rec.Split_Len (Round (300));
	default_rec.All_Stop (LOCAL);

	detail_ptr = NULL;

	//---- Create Intersection data ----

	Show_Message ("Creating Intersection Data - Record");
	Set_Progress ();

	for (node_ptr = node_data.First (); node_ptr; node_ptr = node_data.Next ()) {
		Show_Progress ();

		if (update_flag || delete_flag) {
			if (repair_nodes.In_Range (node_ptr->Node ())) {

				//---- repair the lane connectivity ----

				for (dir = node_ptr->To_Dir (); dir; dir = dir_ptr->To_List ()) {
					dir_ptr = dir_data [dir];
					Lane_Connect (dir, node_ptr);
				}
				continue;
			}
			if (!update_nodes.In_Range (node_ptr->Node ()) ||
				delete_nodes.In_Range (node_ptr->Node ())) continue;
		}

		if (node_ptr->Type () == CHANGE_POINT) {

			type = NO_SIGN;

		} else if (node_ptr->Type () == INTERCHANGE) {
			num = 0;

			for (dir = node_ptr->To_Dir (); dir; dir = dir_ptr->To_List ()) {
				dir_ptr = dir_data [dir];
				if (dir_ptr->Type () == RAMP) break;
				num++;
			}
			if (dir > 0 || num > 2) {
				type = YIELD_SIGN;
			} else {
				type = NO_SIGN;
			}

		} else if (node_ptr->Type () == INTERSECTION) {

			//---- get the area type ----

			if (zone_flag) {
				zone = Closest_Zone (node_ptr->X (), node_ptr->Y ());

				zone_ptr = zone_data.Get (zone);

				if (zone_ptr == NULL) {
					Error ("Zone %d was Not Found in the Zone file", zone);
				}
				type = zone_ptr->Area_Type ();
			} else {
				type = 2;
			}
			type_ptr = area_type.Best (type);
			if (type_ptr == NULL) {
				type_ptr = &default_rec;
			}

			//---- signal warrants ----

			primary = type_ptr->Primary ();
			secondary = type_ptr->Secondary ();
			signal_type = type_ptr->Signal_Type ();
			ring_type = type_ptr->Ring_Type ();

			control_type = control = 0;
			num = primary_in = secondary_in = min_in = max_in = 0;
			memset (link_num, '\0', sizeof (link_num));

			//---- check the entry links ----

			for (dir = node_ptr->To_Dir (); dir; dir = dir_ptr->To_List ()) {
				dir_ptr = dir_data [dir];
				if (detail_flag) {
					detail_ptr = detail_data.Get (dir_ptr->Link_Dir ());
					if (detail_ptr != NULL) {
						if (detail_ptr->Type () > 0) {
							if (detail_ptr->Type () == 1) {
								if (control_type == 0) control_type = 1;
							} else {
								control_type = detail_ptr->Type ();
								control = detail_ptr->Control ();
							}
						}
					}
				}
				type = dir_ptr->Type ();
				if (type == RAMP) type = MINOR;
				if (type == BRIDGE) type = MAJOR;

				connect_ptr = dir_ptr->To_Thru ();

				if (connect_ptr != NULL) {
					thru_ptr = dir_data [connect_ptr->Dir ()];

					i = thru_ptr->Type ();
					if (i == RAMP) i = MINOR;
					if (i == BRIDGE) i = MAJOR;
					if (i < type) type = i;

					for (i=0; i < num; i++) {
						if (thru_ptr->Link () == link_num [i]) break;
					}
					if (i != num) continue;
				}
				if (type <= primary) primary_in++;
				if (type <= secondary) secondary_in++;
				if (type < min_in || min_in == 0) min_in = type;
				if (type > max_in) max_in = type;

				if (num < 20) {
					link_num [num++] = dir_ptr->Link ();
				}
			}

			//---- check the exit links ----

			for (dir = node_ptr->From_Dir (); dir; dir = dir_ptr->From_List ()) {
				dir_ptr = dir_data [dir];

				for (i=0; i < num; i++) {
					if (dir_ptr->Link () == link_num [i]) break;
				}
				if (i != num) continue;

				type = dir_ptr->Type ();
				if (type == RAMP) type = MINOR;
				if (type == BRIDGE) type = MAJOR;

				connect_ptr = dir_ptr->From_Thru ();

				if (connect_ptr != NULL) {
					thru_ptr = dir_data [connect_ptr->Dir ()];

					i = thru_ptr->Type ();
					if (i == RAMP) i = MINOR;
					if (i == BRIDGE) i = MAJOR;
					if (i < type) type = i;

					for (i=0; i < num; i++) {
						if (thru_ptr->Link () == link_num [i]) break;
					}
					if (i != num) continue;
				}
				if (type <= primary) primary_in++;
				if (type <= secondary) secondary_in++;

				if (num < 20) {
					link_num [num++] = dir_ptr->Link ();
				}
			}

			//---- save the signal warrant ----

			if (control_type > 0) {
				if (control_type == 1) {
					primary_in = secondary_in = 0;
					max_in = min_in = 100;
				} else if (control_type == 2) {
					primary_in = secondary_in = 2;
					signal_type = control;
					ring_type = SINGLE_RING;
				} else if (control_type == 3) {
					primary_in = secondary_in = max_in = 0;
					min_in = 1;
				}
			}
			if (primary_in >= 1 && secondary_in > 1) {
				type = -1;

				//---- create a signal warrant ----

				nsignal++;
				signal_file->Node (node_ptr->Node ());
				signal_file->Type (Signal_Code ((Signal_Type) signal_type));
				signal_file->Offset (0);
				signal_file->Timing (0);
				signal_file->Start ("ALL00:00");
				signal_file->Coordinator (0);
				signal_file->Ring (Ring_Code ((Ring_Type) ring_type));
				signal_file->Notes ("Signal Warrant");

				if (!signal_file->Write ()) {
					Error ("Writing Signalized Node File");
				}
				if (signal_type == ACTUATED) {
					if (ring_type == SINGLE_RING) {
						nactuated1++;
					} else if (ring_type == DUAL_RING) {
						nactuated2++;
					} else {
						nactuated3++;
					}
				} else {
					if (ring_type == SINGLE_RING) {
						nfixed1++;
					} else if (ring_type == DUAL_RING) {
						nfixed2++;
					} else {
						nfixed3++;
					}
				}
			} else if (max_in != min_in || type_ptr->All_Stop () >= min_in) {
				type = STOP_SIGN;
			} else {
				type = NO_SIGN;
			}

		} else if (node_ptr->Type () == DEAD_END && uturn_flag) {
			type = NO_SIGN;

		} else if ((connect_flag || replicate_flag) && node_ptr->Type () == ZONE_CENTROID) {
			type = NO_SIGN;

		} else {
			continue;
		}

		//---- process each link entering the node ----

		for (dir = node_ptr->To_Dir (); dir; dir = dir_ptr->To_List ()) {
			dir_ptr = dir_data [dir];
			link = dir_ptr->Link ();

			//---- set lane connectivity ----

			Lane_Connect (dir, node_ptr);

			//---- adjust the approach type ----

			control = type;

			if (control == STOP_SIGN) {
				if (detail_flag) {
					detail_ptr = detail_data.Get (dir_ptr->Link_Dir ());
				}
				if (detail_ptr != NULL) {
					control = detail_ptr->Control ();
				} else if ((min_in != max_in && dir_ptr->Type () < max_in) || dir_ptr->Type () >= WALKWAY) {
					control = NO_SIGN;
				} else if (num <= 3) {
					link_ptr = link_data.Get (dir_ptr->Link ());

					if (link_ptr->AB_Dir () == 0 || link_ptr->BA_Dir () == 0) {
						connect_ptr = dir_ptr->To_Thru ();

						if (connect_ptr != NULL) {
							thru_ptr = dir_data [connect_ptr->Dir ()];

							link_ptr = link_data.Get (thru_ptr->Link ());

							if (link_ptr->AB_Dir () == 0 || link_ptr->BA_Dir () == 0) {
								control = NO_SIGN;
							}
						}
					}
				}
			} else if (control == YIELD_SIGN) {
				if (dir_ptr->Type () != RAMP || dir_ptr->Thru () > 1) {
					control = NO_SIGN;
				}
			}

			//---- create a sign warrant ----

			if (control == STOP_SIGN || control == YIELD_SIGN) {
				nsign++;
				sign_file->Node (node_ptr->Node ());
				sign_file->Link (link);
				if (control == STOP_SIGN) {
					sign_file->Sign ("S");
					sign_file->Notes ("Stop Sign");
					nstop++;
				} else {
					sign_file->Sign ("Y");
					sign_file->Notes ("Yield Sign");
					nyield++;
				}
				if (!sign_file->Write ()) {
					Error ("Writing Unsignalized Node File");
				}
			}
		}
	}
	End_Progress (); 
}
