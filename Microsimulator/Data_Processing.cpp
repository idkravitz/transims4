//*********************************************************
//	Data_Processing.cpp - prepare microsimulator data
//*********************************************************

#include "Microsimulator.hpp"

//---------------------------------------------------------
//	Data_Processing 
//---------------------------------------------------------

void Microsimulator::Data_Processing (void)
{
	int i, j, k, right, left, dir, cells, lanes, lane, length, to, *node, *list;
	int in_dir, in_low, in_high, out_low, out_high, speed, start_tod, in_off, out_off, cell_in, cell_out;
	bool control_flag, connect_flag;
	
	Link_Data *link_ptr, *in_link_ptr;
	Grid_Data *grid_ptr, *in_grid_ptr;
	Link_Use_Data *use_ptr;
	Pocket_Data *pocket_ptr;
	Connect_Data *connect_ptr;
	Veh_Type_Data *type_ptr;

	int use_code = Use_Code ("CAR/TRUCK/BUS/RAIL");

	//---- initialize the node-link lists ----

	if (signal_flag && signal_data.Num_Records () == 0) {
		signal_flag = false;
	}
	control_flag = (signal_flag || sign_data.Num_Records () > 0);

	if (control_flag) {
		if (!node_in.Num_Records (node_data.Num_Records ()) ||
			!dir_in.Num_Records (dir_data.Num_Records ())) {

			Error ("Insufficient Memory for Node-Link Lists");
		}
	}
	start_tod = Round (start_time_step / steps_per_second);

	//---- add cells to links ----

	for (link_ptr = link_data.First (); link_ptr != NULL; link_ptr = link_data.Next ()) {

		//---- add cells to vehicle travel links only ----

		if ((link_ptr->Use () & use_code) == 0) continue;

		cells = MAX (((link_ptr->Length () + half_cell) / cell_size), 1);

		//---- process each direction ----

		for (i=0; i < 2; i++) {

			if (i) {
				dir = link_ptr->BA_Dir ();
				to = link_ptr->Anode ();
				in_off = link_ptr->Boffset ();
				out_off = link_ptr->Aoffset ();
			} else {
				dir = link_ptr->AB_Dir ();
				to = link_ptr->Bnode ();
				in_off = link_ptr->Aoffset ();
				out_off = link_ptr->Boffset ();
			}
			if (dir == 0) continue;

			//---- add data to the node-link lists ----

			if (control_flag) {
				node = node_in.Record (to);
				list = dir_in.Record (dir);

				*list = *node;
				*node = dir;
			}

			//---- allocate cell space ---

			grid_ptr = grid_data [dir];

			left = grid_ptr->Left ();
			right = grid_ptr->Right ();

			lanes = grid_ptr->Thru () + left + right;

			if (!grid_ptr->Data_Size (cells, lanes)) {
				Error ("Insufficient Memory for Link Cells");
			}
			ncells += cells * lanes;

			//---- set the cell offsets ----

			cell_in = MAX (((in_off + half_cell) / cell_size) + 1, 2);
			cell_out = MAX ((cells - MAX (((out_off + half_cell) / cell_size), 1)), 1);

			grid_ptr->Cell_In (cell_in);
			grid_ptr->Cell_Out (cell_out);

			//---- initialize the pocket lanes and access restrictions ----

			grid_ptr->Type (link_ptr->Type ());

			right = lanes - right;

			for (j=1; j <= lanes; j++) {
				if (j <= left || j > right) {
					for (k=1; k <= cells; k++) {
						grid_ptr->Data (k, j, -1);
						grid_ptr->To_Data (k, j, -1);
						ncells--;
					}
				}
				grid_ptr->Lane_Use (j, link_ptr->Use ());
			}
			if (grid_ptr->TOD_List () > 0) {
				tod_flag = true;

				for (j = grid_ptr->TOD_List (); j; j = use_ptr->TOD_List ()) {
					use_ptr = link_use_data [j]; 

					if (use_ptr->Offset () > 0 || use_ptr->Length () > 0) {
						length = use_ptr->Offset () + use_ptr->Length ();

						use_ptr->Low_Cell (MIN (MAX (((use_ptr->Offset () + half_cell) / cell_size), cell_in), cell_out));
						use_ptr->High_Cell (MIN (MAX (((length + half_cell) / cell_size), cell_in), cell_out));
					}
				}
			}

			//---- recalculate the cell-based speed ----

			if (!speed_method) {
				speed = (grid_ptr->Speed () + cell_round) / cell_size;
				if (speed < 1) speed = 1;

				grid_ptr->Speed (speed);
			}
		}
	}

	//----- process the pocket lanes ----

	for (pocket_ptr = pocket_data.First (); pocket_ptr != NULL; pocket_ptr = pocket_data.Next ()) {

		link_ptr = link_data [pocket_ptr->Link ()];

		if (pocket_ptr->Dir () == 0) {
			dir = link_ptr->AB_Dir ();
		} else {
			dir = link_ptr->BA_Dir ();
		}
		if (dir == 0) {
			Error ("Pocket Lane %d cound not be Assigned to Link %d", pocket_ptr->ID (), link_ptr->Link ());
		}
		grid_ptr = grid_data [dir];

		cells = grid_ptr->Cells ();

		if (cells == 0) continue;

		lane = pocket_ptr->Lane ();

		left = grid_ptr->Left ();
		right = grid_ptr->Lanes () - grid_ptr->Right ();

		if ((lane > left && lane < right) || lane > grid_ptr->Lanes ()) {
			Error ("Lane %d on Pocket %d is Out of Range", lane, pocket_ptr->ID ());
		}

		length = MAX (((pocket_ptr->Length () + cell_round) / cell_size), 1);

		if (pocket_ptr->Type () == TURN_LANE) {
			k = cells;
			j = MAX ((grid_ptr->Cell_Out () - length), 0) + 1;
			//j = MAX ((k - length), 0) + 1;
		} else if (pocket_ptr->Type () == MERGE_LANE) {
			j = 1;
			k = MIN ((length + grid_ptr->Cell_In ()), cells);
			//k = MIN (length, cells);
		} else {
			if (pocket_ptr->Dir () == 0) {
				j = MAX (((pocket_ptr->Offset () + half_cell) / cell_size), 1);
			} else {
				j = MAX (cells - MAX (((pocket_ptr->Offset () + half_cell) / cell_size), 1), 0) + 1;
			}
			k = MIN (j + length, cells);
		}

		for (i=j; i <= k; i++) {
			grid_ptr->Data (i, lane, 0);
			grid_ptr->To_Data (i, lane, 0);
			ncells++;
		}
	}
	Print (2, "Number of Link Cells = %d", ncells);

	//---- identify primary input lanes ----

	for (connect_ptr = connect_data.First (); connect_ptr != NULL; connect_ptr = connect_data.Next ()) {

		if (!speed_method && connect_ptr->Speed () > 0) {
			speed = (connect_ptr->Speed () + cell_round) / cell_size;
			if (speed < 1) speed = 1;

			connect_ptr->Speed (speed);
		}

		in_link_ptr = link_data [connect_ptr->In_Link ()];

		if (connect_ptr->In_Dir ()) {
			in_dir = in_link_ptr->BA_Dir ();
		} else {
			in_dir = in_link_ptr->AB_Dir ();
		}
		if (in_dir == 0) continue;

		in_grid_ptr = grid_data [in_dir];

		link_ptr = link_data [connect_ptr->Out_Link ()];

		if (connect_ptr->Out_Dir ()) {
			dir = link_ptr->BA_Dir ();
		} else {
			dir = link_ptr->AB_Dir ();
		}
		if (dir == 0) continue;

		grid_ptr = grid_data [dir];

		//---- check for impossible connections ----
		
		in_low = connect_ptr->In_Lane_Low ();
		out_low = connect_ptr->Out_Lane_Low ();

		in_high = connect_ptr->In_Lane_High ();
		out_high = connect_ptr->Out_Lane_High ();

		connect_flag = false;

		for (; in_low <= in_high; in_low++) {
			if (in_grid_ptr->Data (in_grid_ptr->Cells (), in_low) < 0) continue;

			for (; out_low <= out_high; out_low++) {
				if (grid_ptr->Data (1, out_low) == 0) {
					connect_flag = true;
					break;
				}
			}
			if (connect_flag) break;
		}

		if (!connect_flag) {
			Write (1, "Warning: Connections between Links %d %d-%d and %d %d-%d are Not Possible",
				link_data [in_grid_ptr->Link ()]->Link (),
				connect_ptr->In_Lane_Low (), connect_ptr->In_Lane_High (),
				link_data [grid_ptr->Link ()]->Link (),
				connect_ptr->Out_Lane_Low (), connect_ptr->Out_Lane_High ());
		}

		//---- align the input/output lanes ----

		if (connect_ptr->Type () != NO_TYPE && connect_ptr->Type () != THRU) continue;

		in_low = connect_ptr->In_Thru_Low ();
		out_low = connect_ptr->Out_Thru_Low ();

		in_high = connect_ptr->In_Thru_High ();
		out_high = connect_ptr->Out_Thru_High ();

		//---- assign lanes ----

		for (; in_low <= in_high && out_low <= out_high; in_low++, out_low++) {
			if (connect_ptr->Type () == NO_TYPE && grid_ptr->Connect_Dir (out_low) != 0) {

				//---- check the connection speed -----

				speed = connect_ptr->Speed () * 3 / 2;
				if (speed > 0 && speed < in_grid_ptr->Speed ()) continue;

				//---- check the facility type ----

				if (link_ptr->Type () < in_link_ptr->Type ()) continue;

				//---- check the number of lanes ----

				if (abs (grid_ptr->Thru () - in_grid_ptr->Thru ()) > 1) continue;

				//---- check arterial mismatches ----

				if (in_link_ptr->Type () != FREEWAY && 
					(link_ptr->Type () - in_link_ptr->Type ()) > 1) continue;

				//---- check the main lane throughput ----

				if (in_grid_ptr->Thru () > 
					(connect_ptr->In_Lane_High () - connect_ptr->In_Lane_Low () + 2)) continue;

			}
			grid_ptr->Connect_Dir (out_low, in_dir);
			grid_ptr->Connect_Lane (out_low, in_low);
		}
	}

	//---- create the vehicle type map ----

	num_veh_type = veh_type_data.Num_Records () + 1;

	veh_type_map = new int [num_veh_type];
	num_veh = new int [num_veh_type];
	num_per = new int [num_veh_type];

	memset (veh_type_map, '\0', num_veh_type * sizeof (int));

	for (type_ptr = veh_type_data.First (); type_ptr; type_ptr = veh_type_data.Next ()) {
		veh_type_map [veh_type_data.Record_Index ()] = type_ptr->Type ();
	}

	//---- process the traffic control data ----

	if (control_flag) {
		Traffic_Control ();

		//---- free the memory ----

		node_in.Clear ();
		dir_in.Clear ();
	}

	//---- process transit routes ----

	if (transit_flag) {
		Transit_Plan ();
	}

	//---- initialize occupancy output ----

	occupancy_output.Initialize_Cells ();
}
