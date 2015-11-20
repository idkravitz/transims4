//*********************************************************
//	Connection_List.cpp - build connection lists
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Connection_List
//---------------------------------------------------------

void TransimsNet::Connection_List (void)
{
	int dir, change, best, diff1, diff2, num_in, from_type, to_type, dir_type;
	bool flag;

	Node_List *node_ptr;
	Dir_Connect *dir_ptr, *to_dir, *from_dir;
	Connection connection_rec, *connection_ptr, *thru_connect;

	Show_Message ("Building Connection Lists - Record");
	Set_Progress ();

	//---- build the connection lists ----

	for (node_ptr = node_data.First(); node_ptr; node_ptr = node_data.Next ()) {
		Show_Progress ();

		if (node_ptr->Type () == CHANGE_POINT) {
			for (dir = node_ptr->To_Dir (); dir; dir = to_dir->To_List ()) {
				to_dir = dir_data [dir];
				best = dir;

				to_type = to_dir->Type ();
				if (to_type == BRIDGE) to_type = MAJOR;

				for (dir = node_ptr->From_Dir (); dir; dir = from_dir->From_List ()) {
					from_dir = dir_data [dir];

					if (to_dir->Link () == from_dir->Link ()) continue;

					from_type = from_dir->Type ();
					if (from_type == BRIDGE) from_type = MAJOR;

					connection_rec.Dir (dir);
					connection_rec.Facility (from_type);
					connection_rec.Change (0);
					connection_rec.Set_Thru ();

					if (!to_dir->To_Connection (&connection_rec)) goto connect_error;

					connection_rec.Dir (best);
					connection_rec.Facility (to_type);

					if (!from_dir->From_Connection (&connection_rec)) goto connect_error;
				}
			}
			continue;
		}

		if (node_ptr->Type () != INTERSECTION && node_ptr->Type () != INTERCHANGE &&
			!(connect_flag && node_ptr->Type () == ZONE_CENTROID)) continue;

		//---- set connections for each entry link ----

		num_in = 0;

		for (dir = node_ptr->To_Dir (); dir; dir = to_dir->To_List ()) {
			to_dir = dir_data [dir];
			num_in++;

			to_type = to_dir->Type ();
			if (to_type == BRIDGE) to_type = MAJOR;

			for (dir = node_ptr->From_Dir (); dir; dir = from_dir->From_List ()) {
				from_dir = dir_data [dir];

				change = compass.Change (to_dir->Out_Bearing (), from_dir->In_Bearing ());

				if (abs (change) > max_angle) continue;

				if (turn_flag) {
					if (turn_data.Get (to_dir->Link_Dir (), from_dir->Link_Dir ()) != NULL) continue;
				}
				from_type = from_dir->Type ();
				if (from_type == BRIDGE) from_type = MAJOR;

				connection_rec.Dir (dir);
				connection_rec.Facility (from_type);
				connection_rec.Change (change);

				if (to_dir->Link () == from_dir->Link ()) {
					connection_rec.Set_UTurn ();
				} else {
					connection_rec.Set_Other ();
				}
				if (!to_dir->To_Connection (&connection_rec)) goto connect_error;
			}

			//---- set the thru link ----

			dir = 0;
			best = MAX_INTEGER;
		
			for (connection_ptr = to_dir->To_First (); connection_ptr; connection_ptr = to_dir->To_Next ()) {
				change = abs (connection_ptr->Change ());

				if (change > thru_diff) continue;

				if (dir == 0) {
					dir = connection_ptr->Dir ();
					dir_ptr = dir_data [dir];
					best = change;

					dir_type = dir_ptr->Type ();
					if (dir_type == BRIDGE) dir_type = MAJOR;
				} else {
					from_dir = dir_data [connection_ptr->Dir ()];

					from_type = from_dir->Type ();
					if (from_type == BRIDGE) from_type = MAJOR;

					//---- check the facility types ----

					diff1 = abs (from_type - to_type);
					diff2 = abs (dir_type - to_type);

					if (diff1 > diff2) continue;

					if (diff1 == diff2) {

						//---- check the number of lanes ----

						diff1 = abs (from_dir->Thru () - to_dir->Thru ());
						diff2 = abs (dir_ptr->Thru () - to_dir->Thru ());

						if (diff1 > diff2) continue;

						if (diff1 == diff2) {

							//---- check speeds ----

							diff1 = abs (from_dir->Speed () - to_dir->Speed ());
							diff2 = abs (dir_ptr->Speed () - to_dir->Speed ());

							if (diff1 > diff2) continue;

							if (diff1 == diff2) {
								if (change > best) continue;
							}
						}
					}
					dir = connection_ptr->Dir ();
					dir_ptr = from_dir;
					best = change;
					dir_type = from_type;
				}
			}

			if (dir) {
				connection_ptr = to_dir->To_Dir (dir);
				connection_ptr->Set_Thru ();
			}

			//---- set the link relationships ----

			flag = false;

			for (connection_ptr = to_dir->To_First (); connection_ptr; connection_ptr = to_dir->To_Next ()) {
				if (dir > 0) {
					if (connection_ptr->Dir () == dir) flag = true;
				} else {
					flag = (connection_ptr->Change () >= 0);
				}
				if (!connection_ptr->Is_Other ()) continue;

				if (flag) {
					connection_ptr->Set_Right ();
				} else {
					connection_ptr->Set_Left ();
				}
			}
		}

		//---- set connections for each exit link ----

		for (dir = node_ptr->From_Dir (); dir; dir = from_dir->From_List ()) {
			from_dir = dir_data [dir];

			from_type = from_dir->Type ();
			if (from_type == BRIDGE) from_type = MAJOR;

			for (dir = node_ptr->To_Dir (); dir; dir = to_dir->To_List ()) {
				to_dir = dir_data [dir];

				change = compass.Change (to_dir->Out_Bearing (), from_dir->In_Bearing ());

				if (abs (change) > max_angle) continue;

				if (turn_flag) {
					if (turn_data.Get (to_dir->Link_Dir (), from_dir->Link_Dir ()) != NULL) continue;
				}
				to_type = to_dir->Type ();
				if (to_type == BRIDGE) to_type = MAJOR;

				connection_rec.Dir (dir);
				connection_rec.Facility (to_type);
				connection_rec.Change (change);

				if (from_dir->Link () == to_dir->Link ()) {
					connection_rec.Set_UTurn ();
				} else {
					connection_rec.Set_Other ();
				}
				if (!from_dir->From_Connection (&connection_rec)) goto connect_error;
			}

			//---- set the thru link ----

			dir = 0;
			best = MAX_INTEGER;
			
			for (connection_ptr = from_dir->From_First (); connection_ptr; connection_ptr = from_dir->From_Next ()) {
				change = abs (connection_ptr->Change ());

				if (change > thru_diff) continue;

				if (dir == 0) {
					dir = connection_ptr->Dir ();
					dir_ptr = dir_data [dir];
					best = change;

					dir_type = dir_ptr->Type ();
					if (dir_type == BRIDGE) dir_type = MAJOR;
				} else {
					to_dir = dir_data [connection_ptr->Dir ()];

					to_type = to_dir->Type ();
					if (to_type == BRIDGE) to_type = MAJOR;

					//---- check the facility types ----

					diff1 = abs (to_type - from_type);
					diff2 = abs (dir_type - from_type);

					if (diff1 > diff2) continue;

					if (diff1 == diff2) {

						//---- check the number of lanes ----

						diff1 = abs (to_dir->Thru () - from_dir->Thru ());
						diff2 = abs (dir_ptr->Thru () - from_dir->Thru ());

						if (diff1 > diff2) continue;

						if (diff1 == diff2) {

							//---- check speeds ----

							diff1 = abs (to_dir->Speed () - from_dir->Speed ());
							diff2 = abs (dir_ptr->Speed () - from_dir->Speed ());

							if (diff1 > diff2) continue;

							if (diff1 == diff2) {
								if (change > best) continue;
							}
						}
					}
					dir = connection_ptr->Dir ();
					dir_ptr = to_dir;
					best = change;
					dir_type = to_type;
				}
			}

			if (dir) {
				connection_ptr = from_dir->From_Dir (dir);
				connection_ptr->Set_Thru ();
			}

			//---- set the link relationships ----

			flag = false;

			for (connection_ptr = from_dir->From_First (); connection_ptr; connection_ptr = from_dir->From_Next ()) {
				if (dir > 0) {
					if (connection_ptr->Dir () == dir) flag = true;
				} else {
					flag = (connection_ptr->Change () >= 0);
				}
				if (!connection_ptr->Is_Other ()) continue;

				if (flag) {
					connection_ptr->Set_Right ();
				} else {
					connection_ptr->Set_Left ();
				}
			}
		}

		//---- coordinate the approach thru movements ----

		for (dir = node_ptr->To_Dir (); dir; dir = to_dir->To_List ()) {
			to_dir = dir_data [dir];

			connection_ptr = to_dir->To_Thru ();
			if (connection_ptr == NULL) continue;

			from_dir = dir_data [connection_ptr->Dir ()];

			thru_connect = from_dir->From_Thru ();

			if (thru_connect == NULL || thru_connect->Dir () != dir) {
				thru_connect = from_dir->From_Dir (dir);

				if (thru_connect != NULL) {
					if (thru_connect->Is_Left ()) {
						connection_ptr->Set_L_Merge ();
						thru_connect->Set_L_Merge ();
					} else {
						connection_ptr->Set_R_Merge ();
						thru_connect->Set_R_Merge ();
					}
				}
			}
		}
		
		//---- coordinate the departure thru movements ----

		for (dir = node_ptr->From_Dir (); dir; dir = from_dir->From_List ()) {
			from_dir = dir_data [dir];

			connection_ptr = from_dir->From_Thru ();
			if (connection_ptr == NULL) continue;

			to_dir = dir_data [connection_ptr->Dir ()];

			thru_connect = to_dir->To_Thru ();

			if (thru_connect == NULL || thru_connect->Dir () != dir) {
				thru_connect = to_dir->To_Dir (dir);

				if (thru_connect != NULL) {
					if (thru_connect->Is_Left ()) {
						connection_ptr->Set_L_Split ();
						thru_connect->Set_L_Split ();
					} else {
						connection_ptr->Set_R_Split ();
						thru_connect->Set_R_Split ();
					}
				}
			}
		}
	}
	End_Progress ();
	return;

connect_error:
	Error ("Adding Connection Record");
}
