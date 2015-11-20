//*********************************************************
//	Connection_List.cpp - build connection lists
//*********************************************************

#include "EnhanceNet.hpp"

//---------------------------------------------------------
//	Connection_List
//---------------------------------------------------------

void EnhanceNet::Connection_List (void)
{
	int dir, change, best, diff1, diff2, type, thru_dir;
	bool flag, thru_flag;
	
	Node_List *node_ptr;
	Dir_Connect *dir_ptr, *to_dir, *from_dir;
	Connection connection_rec, *connection_ptr, *thru_connect;
	Connect_Data *connect_ptr;
	
	int thru_diff = compass.Num_Points () * 50 / 360;

	Show_Message ("Building Connection Lists - Record");
	Set_Progress ();

	//---- build the connection lists ----

	for (node_ptr = node_data.First(); node_ptr; node_ptr = node_data.Next ()) {
		Show_Progress ();

		//---- set connections for each entry link ----

		for (dir = node_ptr->To_Dir (); dir; dir = to_dir->To_List ()) {
			to_dir = dir_data [dir];

			thru_flag = false;
			thru_dir = 0;

			for (dir = node_ptr->From_Dir (); dir; dir = from_dir->From_List ()) {
				from_dir = dir_data [dir];

				connect_ptr = connect_data.Get (to_dir->Link_Dir (), from_dir->Link_Dir ());
				if (connect_ptr == NULL) continue;

				type = connect_ptr->Type ();

				change = compass.Change (to_dir->Out_Bearing (), from_dir->In_Bearing ());

				connection_rec.Dir (dir);
				connection_rec.Index (connect_data.Record_Index ());
				connection_rec.Change (change);

				if (type == NO_TYPE) {
					if (to_dir->Link () == from_dir->Link ()) {
						connection_rec.Set_UTurn ();
					} else {
						connection_rec.Set_Other ();
					}
				} else {
					if (type == THRU) {
						thru_flag = true;
						thru_dir = dir;
					}
					connection_rec.Type (type);
				}
				if (!to_dir->To_Connection (&connection_rec)) goto connect_error;
			}

			//---- set the thru link ----

			if (!thru_flag) {
				best = MAX_INTEGER;
			
				for (connection_ptr = to_dir->To_First (); connection_ptr; connection_ptr = to_dir->To_Next ()) {
					change = abs (connection_ptr->Change ());

					if (change > thru_diff) continue;

					if (thru_dir == 0) {
						thru_dir = connection_ptr->Dir ();
						dir_ptr = dir_data [thru_dir];
						best = change;
					} else {
						from_dir = dir_data [connection_ptr->Dir ()];

						//---- check the number of lanes ----

						diff1 = abs (from_dir->Thru () - to_dir->Thru ());
						diff2 = abs (dir_ptr->Thru () - to_dir->Thru ());

						if (diff1 > diff2) continue;

						if (diff1 == diff2) {

							//---- check the facility types ----

							diff1 = abs (from_dir->Type () - to_dir->Type ());
							diff2 = abs (dir_ptr->Type () - to_dir->Type ());

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
						thru_dir = connection_ptr->Dir ();
						dir_ptr = from_dir;
						best = change;
					}
				}
				if (thru_dir) {
					connection_ptr = to_dir->To_Dir (thru_dir);
					connection_ptr->Set_Thru ();
				}
			}

			//---- set the link relationships ----

			flag = false;

			for (connection_ptr = to_dir->To_First (); connection_ptr; connection_ptr = to_dir->To_Next ()) {
				if (thru_dir > 0) {
					if (connection_ptr->Dir () == thru_dir) flag = true;
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

			thru_flag = false;
			thru_dir = 0;

			for (dir = node_ptr->To_Dir (); dir; dir = to_dir->To_List ()) {
				to_dir = dir_data [dir];

				connect_ptr = connect_data.Get (to_dir->Link_Dir (), from_dir->Link_Dir ());
				if (connect_ptr == NULL) continue;
				
				type = connect_ptr->Type ();

				change = compass.Change (to_dir->Out_Bearing (), from_dir->In_Bearing ());

				connection_rec.Dir (dir);
				connection_rec.Index (connect_data.Record_Index ());
				connection_rec.Change (change);

				if (type == NO_TYPE) {
					if (from_dir->Link () == to_dir->Link ()) {
						connection_rec.Set_UTurn ();
					} else {
						connection_rec.Set_Other ();
					}
				} else {
					if (type == THRU) {
						thru_flag = true;
						thru_dir = dir;
					}
					connection_rec.Type (type);
				}
				if (!from_dir->From_Connection (&connection_rec)) goto connect_error;
			}

			//---- set the thru link ----

			if (!thru_flag) {
				best = MAX_INTEGER;
				
				for (connection_ptr = from_dir->From_First (); connection_ptr; connection_ptr = from_dir->From_Next ()) {
					change = abs (connection_ptr->Change ());

					if (change > thru_diff) continue;

					if (thru_dir == 0) {
						thru_dir = connection_ptr->Dir ();
						dir_ptr = dir_data [thru_dir];
						best = change;
					} else {
						to_dir = dir_data [connection_ptr->Dir ()];

						//---- check the number of lanes ----

						diff1 = abs (to_dir->Thru () - from_dir->Thru ());
						diff2 = abs (dir_ptr->Thru () - from_dir->Thru ());

						if (diff1 > diff2) continue;

						if (diff1 == diff2) {

							//---- check the facility types ----

							diff1 = abs (to_dir->Type () - from_dir->Type ());
							diff2 = abs (dir_ptr->Type () - from_dir->Type ());

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
						thru_dir = connection_ptr->Dir ();
						dir_ptr = to_dir;
						best = change;
					}
				}
				if (thru_dir) {
					connection_ptr = from_dir->From_Dir (thru_dir);
					connection_ptr->Set_Thru ();
				}
			}

			//---- set the link relationships ----

			flag = false;

			for (connection_ptr = from_dir->From_First (); connection_ptr; connection_ptr = from_dir->From_Next ()) {
				if (thru_dir > 0) {
					if (connection_ptr->Dir () == thru_dir) flag = true;
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

		//---- set the connection type ----

		thru_dir = 0;

		for (dir = node_ptr->To_Dir (); dir; dir = to_dir->To_List ()) {
			to_dir = dir_data [dir];
			thru_dir++;

			for (connection_ptr = to_dir->To_First (); connection_ptr; connection_ptr = to_dir->To_Next ()) {
				connect_ptr = connect_data [connection_ptr->Index ()];

				connect_ptr->Type (connection_ptr->Type ());
			}
		}

		//---- check for split movements ----

		if (thru_dir == 1) {
			for (dir = node_ptr->From_Dir (); dir; dir = from_dir->From_List ()) {
				from_dir = dir_data [dir];

				for (connection_ptr = from_dir->From_First (); connection_ptr; connection_ptr = from_dir->From_Next ()) {

					if (connection_ptr->Is_L_Split () || connection_ptr->Is_R_Split ()) {
						connect_ptr = connect_data [connection_ptr->Index ()];

						connect_ptr->Type (connection_ptr->Type ());
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
