//*********************************************************
//	Traffic_Control.cpp - create the control structures
//*********************************************************

#include "Microsimulator.hpp"

//---------------------------------------------------------
//	Traffic_Control 
//---------------------------------------------------------

void Microsimulator::Traffic_Control (void)
{
	int timing, nmovement, index, r, ring, group, timing_plan, index0, timing0;
	int i, low, high, dir, in_dir, out_dir, change, node;

	bool detector_flag, actuated_flag, ring_flag [MAX_RING];

	Link_Data *link_ptr;
	Grid_Data *grid_ptr;
	Connect_Data *connect_ptr;
	Signal_Data *signal_ptr;
	Phasing_Data *phasing_ptr, *ring_ptr;
	Timing_Data timing_rec, *timing_ptr = 0;
	Control_Data control_rec, *control_ptr;
	Controller_Data controller_rec, *controller_ptr = 0;
	Sign_Data *sign_ptr;
	Detector_Data *detector_ptr;

	detector_flag = (detector_data.Num_Records () > 0);
	actuated_flag = false;
	timing0 = 0;

	//---- validate the timing plans ----

	for (signal_ptr = signal_data.First (); signal_ptr; signal_ptr = signal_data.Next ()) {
		timing = signal_ptr->Timing ();

		timing_rec.Timing (timing);
		timing_rec.Phase (1);

		timing_ptr = timing_data.Get_GE (&timing_rec); 
		memset (ring_flag, '\0', sizeof (ring_flag));

		//---- find the first phase for each ring ----

		while (timing_ptr != NULL && timing_ptr->Timing () == timing) {
			ring = timing_ptr->Ring () - 1;

			if (ring >= 0) {
				if (ring_flag [ring]) goto ring_error;
				ring_flag [ring] = true;
			}
			if (signal_ptr->Type () == ACTUATED) {
				if (detector_flag) {
					if (timing_ptr->Max_Green () <= 0) {
						timing_ptr->Max_Green (timing_ptr->Min_Green () + timing_ptr->Ext_Green ());
					}
				} else {
					signal_ptr->Type (TIMED);
					actuated_flag = true;
				}
			}
			timing_ptr = timing_data.Next_Key ();
		}

		//---- set the number of rings ----

		signal_ptr->Rings (0);

		for (ring = 0; ring < MAX_RING; ring++) {
			if (ring_flag [ring]) {
				if (signal_ptr->Rings () != ring) goto ring_error;
				signal_ptr->Rings (ring+1);
			}
		}
		if (signal_ptr->Rings () == 0) goto ring_error;
	}
	if (actuated_flag) {
		Write (1);
		Warning ("Actuated Controllers have been Disabled");
		Show_Message (2);
	}

	//---- process the phasing information ----

	memset (&control_rec, '\0', sizeof (control_rec));
	memset (&controller_rec, '\0', sizeof (controller_rec));

	control_rec.Control_Status (RED_LIGHT);

	for (phasing_ptr = phasing_data.First (); phasing_ptr; phasing_ptr = phasing_data.Next ()) {

		//---- locate the timing plan ----

		timing_rec.Timing (phasing_ptr->Timing ());
		timing_rec.Phase (phasing_ptr->Phase ());

		index = timing_data.Get_Index (&timing_rec);

		if (index == 0) {
			Error ("Timing Plan %d-%d was Not Found for Phasing Plan at Node %d", 
				phasing_ptr->Timing (), phasing_ptr->Phase (), 
				node_data [phasing_ptr->Node ()]->Node ());
		}
		phasing_ptr->Timing_Index (index);

		//---- create movement control records ----

		in_dir = phasing_ptr->In_Link_Dir ();
		out_dir = phasing_ptr->Out_Link_Dir ();

		if (in_dir == 0 || out_dir == 0) {
			Error ("Links are Zero for Phasing Plan at Node %d", 
				node_data [phasing_ptr->Node ()]->Node ());
		}

		control_ptr = control_data.Get (in_dir, out_dir);

		if (control_ptr == NULL) {
			control_rec.In_Link_Dir (in_dir);
			control_rec.Out_Link_Dir (out_dir);

			if (!control_data.Add (&control_rec)) {
				Error ("Adding Signal Control Data to the Control List");
			}
			control_ptr = control_data.Record ();

			//---- get the movement orientation ----

			grid_ptr = grid_data.Get (in_dir);

			control_ptr->Approach_Dir (grid_data.Record_Index ());

			in_dir = grid_ptr->In_Bearing ();

			grid_ptr = grid_data.Get (out_dir);

			out_dir = grid_ptr->In_Bearing ();

			change = compass.Change (in_dir, out_dir);

			//---- check for a left turn ----

			if (change >= -135 && change <= -45) {
				dir = node_in [phasing_ptr->Node ()];

				//---- search for a thru link in the opposite direction ----

				in_dir = compass.Flip (in_dir);

				for (; dir > 0; dir = dir_in [dir]) {
					grid_ptr = grid_data [dir];

					change = compass.Change (in_dir, grid_ptr->Out_Bearing ());

					if (change >= -45 && change <= 45) {
						if (control_ptr->Conflict_Dir (1) == 0) {
							control_ptr->Conflict_Dir (1, dir);
						} else if (control_ptr->Conflict_Dir (2) == 0) {
							control_ptr->Conflict_Dir (2, dir);
							break;
						}
					}
				}
			}
		}
		phasing_ptr->Control_Index (control_data.Record_Index ());
		phasing_ptr->Phase_List (0);

		//---- create signal controller records ----

		controller_ptr = controller_data.Get (phasing_ptr->Node ());

		if (controller_ptr == NULL) {
			controller_rec.Node (phasing_ptr->Node ());

			signal_ptr = signal_data.Get (controller_rec.Node (), 0);

			if (signal_ptr == NULL) {
				Error ("Node %d does not have Signal Data for Midnight", node_data [phasing_ptr->Node ()]->Node ());
			}
			if (!controller_data.Add (&controller_rec)) {
				Error ("Adding Signal Controller Data to the Controller List");
			}
			controller_ptr = controller_data.Record ();

			phasing_ptr->Node_List (0);
		} else {
			phasing_ptr->Node_List (controller_ptr->Movement ());
		}
		controller_ptr->Movement (phasing_data.Record_Index ());
	}
	Print (2, "Number of Signalized Intersections = %d", controller_data.Num_Records ());
	Print (1, "Number of Signal Controlled Movements = %d", control_data.Num_Records ());

	//---- sort the phase list for each ring in each plan ---

	for (signal_ptr = signal_data.First (); signal_ptr; signal_ptr = signal_data.Next ()) {
		timing_plan = signal_ptr->Timing ();

		controller_ptr = controller_data.Get (signal_ptr->Node ());

		if (controller_ptr == NULL) {
			Error ("Controller was Not Found for Signal at Node %d", signal_ptr->Node ());
		}

		//---- identify the first phase in each ring ---

		for (r=1; r <= controller_ptr->Max_Rings (); r++) {
			controller_ptr->Phase (r, 0);
		}
		index = controller_ptr->Movement ();

		while (index > 0) {
			phasing_ptr = phasing_data [index];

			timing_ptr = timing_data [phasing_ptr->Timing_Index ()];

			if (timing_ptr->Timing () == timing_plan) {

				//---- check for duplicate plans ----

				if (phasing_ptr->Phase_List () > 0) break;

				ring = timing_ptr->Ring ();

				if (ring > 0) {
					controller_ptr->Phase (ring, index);
				}
				group = timing_ptr->Group ();

				if (group > signal_ptr->Groups ()) {
					signal_ptr->Groups (group);
				}
			}
			index = phasing_ptr->Node_List ();
		}

		//---- sort the phase movements within each ring ----

		for (r=1; r <= controller_ptr->Max_Rings (); r++) {

			index0 = ring = controller_ptr->Phase (r);
			timing = 0;

			while (ring > 0) {
				ring_ptr = phasing_data [ring];
				if (timing == 0) {
					timing0 = timing = ring_ptr->Timing_Index ();
				}
				index = controller_ptr->Movement ();

				while (index > 0) {
					phasing_ptr = phasing_data [index];

					if (index != ring) {
						if (phasing_ptr->Timing_Index () == timing) {
							if (phasing_ptr->Phase_List () != 0) goto phase_error;
							ring_ptr->Phase_List (index);
							ring_ptr = phasing_ptr;
						}
					}
					index = phasing_ptr->Node_List ();
				}

				//---- get the next phase ----

				timing_ptr = timing_data [timing];

				timing_rec.Timing (timing_ptr->Timing ());
				timing_rec.Phase (timing_ptr->Next_Phase ());

				timing = timing_data.Get_Index (&timing_rec);
				if (timing == 0) goto next_error;

				//---- complete the ring ----

				if (timing == timing0) {
					ring_ptr->Phase_List (index0);
					break;
				}

				//---- find the first movement in the next phase ----

				index = controller_ptr->Movement ();

				while (index > 0) {
					phasing_ptr = phasing_data [index];

					if (phasing_ptr->Timing_Index () == timing) {
						if (phasing_ptr->Phase_List () != 0) goto phase_error;
						ring = index;
						break;
					}
					index = phasing_ptr->Node_List ();
				}
				if (index == 0) goto next_error;

				ring_ptr->Phase_List (ring);
			}
		}
	}

	//---- convert detectors locations to cells ----

	for (detector_ptr = detector_data.First (); detector_ptr; detector_ptr = detector_data.Next ()) {
		grid_ptr = grid_data [detector_ptr->Link_Dir ()];

		if (grid_ptr->Dir ()) {
			link_ptr = link_data [grid_ptr->Link ()];

			high = link_ptr->Length () - detector_ptr->Offset ();
		} else {
			high = detector_ptr->Offset ();
		}
		high = MAX (((high + half_cell) / cell_size), 1);
		if (high > grid_ptr->Cells ()) {
			high = grid_ptr->Cells ();
		}
		low = high - MAX (((detector_ptr->Length () + half_cell) / cell_size), 1) + 1;
		if (low > grid_ptr->Cells ()) {
			low = grid_ptr->Cells ();
		} else if (low < 1) {
			low = 1;
		}
		detector_ptr->Low_Cell (low);
		detector_ptr->High_Cell (high);

		//---- flag lanes with detectors ----

		for (i = detector_ptr->Low_Lane (); i <= detector_ptr->High_Lane (); i++) {
			if (grid_ptr->Data (grid_ptr->Cells (), i) == 0) {
				grid_ptr->Data (grid_ptr->Cells (), i, 1);
			}
		}
	}

	//---- check signal movements ----

	index = 0;
	
	for (signal_ptr = signal_data.First (); signal_ptr; signal_ptr = signal_data.Next ()) {
		if (signal_ptr->Node () == index) continue;

		index = signal_ptr->Node ();

		for (dir = node_in [index]; dir > 0; dir = dir_in [dir]) {
			grid_ptr = grid_data [dir];

			//---- find each turn option ----

			in_dir = grid_ptr->Link_Dir ();

			connect_ptr = connect_data.Get_GE (in_dir, 0);

			while (connect_ptr != NULL && connect_ptr->In_Link_Dir () == in_dir) {

				//---- check for signal control ----

				control_ptr = control_data.Get (in_dir, connect_ptr->Out_Link_Dir ());

				if (control_ptr == NULL) {
					link_ptr = link_data [connect_ptr->In_Link ()];

					if (link_ptr->Type () != WALKWAY) {
						int in_link = link_ptr->Link ();

						link_ptr = link_data [connect_ptr->Out_Link ()];

						if (link_ptr->Type () != WALKWAY) {
							Warning ("Signal Control missing at Node %d from %d to %d",
								node_data [index]->Node (), in_link, link_ptr->Link ());
						}
					}
				}
				connect_ptr = connect_data.Next_Key ();
			}

			//---- check for detectors ----

			if (signal_ptr->Type () == ACTUATED) {
				for (i=1; i <= grid_ptr->Lanes (); i++) {
					if (grid_ptr->Data (grid_ptr->Cells (), i) == 0) {
						Warning ("No Detector on Link %d Lane %d for Actuated Signal at Node %d", 
							link_data [grid_ptr->Link ()]->Link (), i, node_data [index]->Node ());
					}
				}
			}
		}
	}

	//---- clear the detector flags ----

	if (detector_flag) {
		grid_data.Zero_Grids ();
	}

	//---- add signs to control data ----

	nmovement = 0;

	for (sign_ptr = sign_data.First (); sign_ptr; sign_ptr = sign_data.Next ()) {

		if (sign_ptr->Sign () == YIELD_SIGN) continue;

		control_rec.Control_Status (sign_ptr->Sign ());

		dir = sign_ptr->Link_Dir ();

		//---- get the input orientation ----

		grid_ptr = grid_data.Get (dir);

		in_dir = grid_ptr->Out_Bearing ();

		link_ptr = link_data [grid_ptr->Link ()];

		node = (grid_ptr->Dir () > 0) ? link_ptr->Anode () : link_ptr->Bnode ();

		//---- find each turn option ----

		connect_ptr = connect_data.Get_GE (dir, 0);

		while (connect_ptr != NULL && connect_ptr->In_Link_Dir () == dir) {

			//---- check for sign-signal conflicts ----

			control_ptr = control_data.Get (dir, connect_ptr->Out_Link_Dir ());

			if (control_ptr != NULL) {
				Error ("Link Movement %d to %d has Sign and Signal Records", 
					link_data [connect_ptr->In_Link ()]->Link (),
					link_data [connect_ptr->Out_Link ()]->Link ());
			}

			//---- create movement control records ----

			control_rec.In_Link_Dir (dir);
			control_rec.Out_Link_Dir (connect_ptr->Out_Link_Dir ());

			if (!control_data.Add (&control_rec)) {
				Error ("Adding Sign Control Data to the Control List");
			}
			control_ptr = control_data.Record ();

			//---- check for a thru or left turn ----

			if (connect_ptr->Type () == LEFT || connect_ptr->Type () == THRU) {

				grid_ptr = grid_data.Get (control_ptr->Out_Link_Dir ());

				out_dir = grid_ptr->In_Bearing ();

				change = compass.Change (in_dir, out_dir);

				//---- search for a left side and right side approach ----

				for (index = node_in [node]; index > 0; index = dir_in [index]) {
					grid_ptr = grid_data [index];

					out_dir = compass.Change (in_dir, grid_ptr->Out_Bearing ());

					if (connect_ptr->Type () == THRU) {
						//if (change >= -45 && change <= 45) {
						if (out_dir >= -135 && out_dir <= -45) {
							if (control_ptr->Conflict_Dir (2) == 0 || 
								(out_dir >= -100 && out_dir <= -80)) {
								control_ptr->Conflict_Dir (2, index);
							}
						}
					}
					if (out_dir >= 45 && out_dir <= 135) {
						if (control_ptr->Conflict_Dir (1) == 0 || 
							(out_dir >= 80 && out_dir <= 100)) {
							control_ptr->Conflict_Dir (1, index);
						}
					}
				}
			}

			//---- get the next connection ----

			connect_ptr = connect_data.Next_Key ();
			nmovement++;
		}
	}
	Print (2, "Number of Traffic Control Signs = %d", sign_data.Num_Records ());
	Print (1, "Number of Sign Controlled Movements = %d", nmovement);

	//---- add uncontrolled left turns ----

	nmovement = 0;

	control_rec.Control_Status (PERMITTED_GREEN);
	control_rec.Conflict_Dir (2, 0);

	for (connect_ptr = connect_data.First (); connect_ptr; connect_ptr = connect_data.Next ()) {

		if (connect_ptr->Type () != LEFT) continue;

		//---- must be a two way roadway ----

		dir = connect_ptr->In_Link_Dir ();

		link_ptr = link_data [connect_ptr->In_Link ()];

		if (link_ptr->AB_Dir () == 0 || link_ptr->BA_Dir () == 0) continue;

		//---- check for existing controls ----

		control_ptr = control_data.Get (connect_ptr->In_Link_Dir (), connect_ptr->Out_Link_Dir ());

		if (control_ptr != NULL) continue;

		index0 = connect_data.Record_Index ();
		node = (connect_ptr->In_Dir () > 0) ? link_ptr->Anode () : link_ptr->Bnode ();
		out_dir = connect_ptr->In_Link ();

		control_rec.In_Link_Dir (dir);
		control_rec.Out_Link_Dir (connect_ptr->Out_Link_Dir ());

		//---- search for an opposing thru ----

		for (index = node_in [node]; index > 0; index = dir_in [index]) {
			grid_ptr = grid_data [index];

			connect_ptr = connect_data.Get_GE (grid_ptr->Link_Dir (), 0);

			while (connect_ptr && connect_ptr->In_Link_Dir () == grid_ptr->Link_Dir ()) {

				if (connect_ptr->Type () == THRU && connect_ptr->Out_Link () == out_dir) break;

				connect_ptr = connect_data.Next_Key ();
			}

			//---- save the control record ----

			if (connect_ptr && connect_ptr->In_Link_Dir () == grid_ptr->Link_Dir ()) {
				control_rec.Conflict_Dir (1, index);

				if (!control_data.Add (&control_rec)) {
					Error ("Adding Unsignalized Left Turn Control Record");
				}
				nmovement++;
				break;
			}
		}

		//---- reset the connect record position ----

		connect_data.Record_Index (index0);
	}
	Print (2, "Number of Uncontrolled Left Turn Movements = %d", nmovement);

	//---- identify the number of lanes associated with each conflict ----

	for (control_ptr = control_data.First (); control_ptr; control_ptr = control_data.Next ()) {

		for (r=1; r <= control_ptr->Max_Conflicts (); r++) {
			index = control_ptr->Conflict_Dir (r);
			if (index == 0) continue;
			low = high = 0;

			//---- get the input orientation ----

			grid_ptr = grid_data [index];

			dir = grid_ptr->Link_Dir ();

			//---- find each turn option ----

			connect_ptr = connect_data.Get_GE (dir, 0);

			while (connect_ptr != NULL && connect_ptr->In_Link_Dir () == dir) {

				//---- check for a thru movement ----

				if (connect_ptr->Type () == THRU) {
					if (low == 0 || connect_ptr->In_Lane_Low () < low) {
						low = connect_ptr->In_Lane_Low ();
					}
					if (connect_ptr->In_Lane_High () > high) {
						high = connect_ptr->In_Lane_High ();
					}
				}

				//---- get the next connection ----

				connect_ptr = connect_data.Next_Key ();
			}

			//---- set the conflicting lane range ----

			control_ptr->Low_Lane (r, low);
			control_ptr->High_Lane (r, high);
		}
	}
	return;

ring_error:
	Error ("Ring Coding for Timing Plan %d", timing);

phase_error:
	Error ("Ring Phasing Order at Node %d", node_data [controller_ptr->Node ()]->Node ());

next_error:
	Error ("Next Phase %d was Not Found at Node %d", timing_ptr->Next_Phase (), node_data [controller_ptr->Node ()]->Node ());
}
