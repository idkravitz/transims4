//*********************************************************
//	Check_Move.cpp - Check Vehicle Movement Constraints
//*********************************************************

#include "Microsimulator.hpp"

//---------------------------------------------------------
//	Check_Move
//---------------------------------------------------------

bool Microsimulator::Check_Move (Msim_Vehicle *vehicle_ptr)
{
	int gap, cell, lane, dir, offset, index, to_cell, to_lane, to_dir, to_index, vehicle;
	int speed, step_dist, num_cells, end_cell, veh;

	Travel_Plan_Data *plan_ptr;
	Travel_Leg_Data *leg_ptr, *from_leg;
	Grid_Data *grid_ptr, *to_grid_ptr;
	Msim_Vehicle *veh2_ptr, *veh3_ptr;

	if (vehicle_ptr->Dir () == 0) return (false);

	plan_ptr = vehicle_ptr->Travel_Plan ();
	if (plan_ptr == NULL) return (false);

	//---- check the move conditions ----

	min_speed = 0;

	Check_Ahead (vehicle_ptr, true);

	if (problem_code > 0) {
		Problem_Plan (Set_Problem ((Problem_Type) problem_code), plan_ptr, problem_dir, problem_lane, 0);

		if (problem_code == LINK_ACCESS_PROBLEM) return (false);
	}
	vehicle_ptr->Hold (hold_time);
	vehicle_ptr->Lane_Change (lane_change);
	vehicle_ptr->Blocked (blocked_flag);
	vehicle_ptr->Status (2);

	dir = vehicle_ptr->Dir ();
	grid_ptr = grid_data [dir];
	if (grid_ptr == NULL) return (false);

	//---- apply random slow down ----

	if (slow_down_flag && max_speed > 0 && hold_time == 0) {
		if (slow_down_prob [grid_ptr->Type ()] > random.Probability ()) {
			if (slow_down_percent [grid_ptr->Type ()] > 0) {
				double spd = max_speed * slow_down_percent [grid_ptr->Type ()] / 100.0;
				int slow_down = (int) spd;
				spd -= slow_down;

				if (spd > 0.0 && spd > random.Probability ()) slow_down++;

				max_speed -= slow_down;
			} else {
				max_speed -= veh_type_data [vehicle_ptr->Type ()]->Max_Decel ();
			}
			if (max_speed < 0) max_speed = 0;
		}
	}

	//---- set the speed, offset, and number of cells to move ----

	if (speed_method) {
		speed = vehicle_ptr->Speed ();

		//--- set the speed and calculate the cell movement ---

		if (steps_per_second > 1) {
			max_speed = speed + (max_speed - speed + step_round) / steps_per_second;

			speed = (speed + max_speed + 1) / 2;
			step_dist = (speed + step_round) / steps_per_second;
		} else {
			speed = (speed + max_speed + 1) / 2;
			step_dist = speed;
		}
		offset = vehicle_ptr->Offset ();

		num_cells = (step_dist - offset + cell_round) / cell_size;
		if (num_cells < 0) num_cells = 0;

		offset += num_cells * cell_size - step_dist;

	} else {
		offset = vehicle_ptr->Offset ();

		if (steps_per_second > 1) {
			num_cells = (max_speed + offset) / steps_per_second;
			speed = num_cells * steps_per_second;
			if (speed > grid_ptr->Speed ()) {
				speed = grid_ptr->Speed ();
				num_cells = speed / steps_per_second;
			}
			offset = max_speed - speed;
			max_speed = speed;
		} else {
			speed = max_speed + offset;
			if (speed > grid_ptr->Speed ()) {
				speed = grid_ptr->Speed ();
			}
			offset = max_speed - speed;
			max_speed = num_cells = speed;
		}
	}

	//---- get the current position ----

	dir = vehicle_ptr->Dir ();
	cell = vehicle_ptr->Cell ();
	lane = vehicle_ptr->Lane ();
	index = vehicle_ptr->Leg ();

	grid_ptr = grid_data [dir];
	leg_ptr = plan_ptr->Travel_Leg (index);
	if (grid_ptr == NULL || leg_ptr == NULL) return (false);

	if (num_cells == 0) {

		//---- try to reserve the forward cell ----

		if (lane_change == 0) {
			vehicle = grid_ptr->Data (cell+1, lane);

			if (vehicle > 0) {
				veh2_ptr = vehicle_data [vehicle];

				if (veh2_ptr != NULL && veh2_ptr->Speed () == 0 && veh2_ptr->Dir () > 0) {
					veh = veh2_ptr->Reserve ();
					if (veh > 0) {
						if (veh != vehicle && veh != plan_ptr->Vehicle ()) {
							veh3_ptr = vehicle_data [veh];
							if (veh3_ptr->Dir () == 0) {
								veh2_ptr->Reserve (plan_ptr->Vehicle ());
							} else if (veh3_ptr->Speed () == 0 && veh3_ptr->Reserve () == 0) {
								veh3_ptr->Reserve (plan_ptr->Vehicle ());
							}
						}
					} else {
						veh2_ptr->Reserve (plan_ptr->Vehicle ());
					}
				}
			}
		}
		goto set_to;
	}

	if (index == plan_ptr->Num_Travel_Legs ()) {
		end_cell = plan_ptr->End_Cell ();

		if (cell >= end_cell) {
			if (lane_change != 0) goto set_to;
			return (false);
		}
	} else {
		end_cell = grid_ptr->Cells ();
	}

	//---- move number of cells ahead ----

	to_dir = dir;
	to_cell = cell;
	to_lane = lane;
	to_index = index;
	to_grid_ptr = grid_ptr;

	for (gap=0; gap < num_cells; gap++) {

		//---- end of the link ----

		if (++to_cell > end_cell) {

			if (index == plan_ptr->Num_Travel_Legs ()) break;

			if (to_lane < leg_ptr->Out_Lane_Low () || to_lane > leg_ptr->Out_Lane_High ()) break;

			from_leg = leg_ptr;

			//---- next link ----

			leg_ptr = plan_ptr->Travel_Leg (++to_index);
			if (leg_ptr == NULL) break;

			to_dir = leg_ptr->Dir ();

			to_grid_ptr = grid_data [to_dir];
			if (to_grid_ptr == NULL) break;

			if (to_index == plan_ptr->Num_Travel_Legs ()) {
				end_cell = plan_ptr->End_Cell ();
			} else {
				end_cell = to_grid_ptr->Cells ();
			}
			to_cell = 1;
			to_lane = Entry_Lane (to_lane, from_leg, leg_ptr);
		}

		//---- check the cell ----

		vehicle = to_grid_ptr->To_Data (to_cell, to_lane);

		if (vehicle != 0 && vehicle != plan_ptr->Vehicle ()) break;

		//---- check for turn output ----

		if (dir != to_dir && turn_output.In_Range (step, dir)) {
			turn_output.In_Link (dir);
			turn_output.Out_Link (to_dir);

			turn_output.Summarize (step);
		}
		dir = to_dir;
		cell = to_cell;
		lane = to_lane;
		index = to_index;
		grid_ptr = to_grid_ptr;

		vehicle_ptr->Move_Flag (true);
	}

	//---- move the vehicle to the new location ----

	if (cell >= end_cell && index == plan_ptr->Num_Travel_Legs ()) {
		if (lane_change == 0) return (false);
	}

	//---- check for a complete move ----

	if (gap < num_cells) {
		if (!speed_method) {
			speed = gap * steps_per_second;
			offset = max_speed - speed;
			max_speed = speed;
		}
	}

set_to:
	vehicle_ptr->To_Leg (index);
	vehicle_ptr->To_Dir (dir);
	vehicle_ptr->To_Cell (cell);
	vehicle_ptr->To_Lane (lane);
	vehicle_ptr->To_Speed (max_speed);
	vehicle_ptr->To_Offset (offset);

	if (vehicle_ptr->Num_Cells () > 1) {
		Move_Veh_Cells (vehicle_ptr);
	} else {
		grid_ptr->To_Data (cell, lane, plan_ptr->Vehicle ());
	}
	vehicle_ptr->Status (1);

	//---- check for a reservation -----

	if (vehicle_ptr->Move_Flag () && vehicle_ptr->Reserve () > 0) {
		veh = vehicle_ptr->Reserve ();
		vehicle_ptr->Reserve (0);

		veh2_ptr = vehicle_data [veh];
		if (veh2_ptr == NULL) return (true);

		if (veh2_ptr->Status () == 3) {

			//---- parking lot reservation ----

			dir = vehicle_ptr->Dir ();
			cell = vehicle_ptr->Cell ();
			lane = vehicle_ptr->Lane ();

			grid_ptr = grid_data [dir];
			if (grid_ptr == NULL) return (true);

			if (grid_ptr->To_Data (cell, lane) == 0) {
				grid_ptr->To_Data (cell, lane, -2);
			}

		} else if (veh2_ptr->Status () < 4 && veh2_ptr->Dir () > 0) {
			if (veh2_ptr->Status () == 0 || !veh2_ptr->Move_Flag ()) {
				if (veh2_ptr->Status () == 1) {
					veh2_ptr->Status (0);
					veh2_ptr->Waiting (veh2_ptr->Waiting () - 10 * steps_per_second);

					dir = veh2_ptr->Dir ();
					cell = veh2_ptr->Cell ();
					lane = veh2_ptr->Lane ();

					grid_ptr = grid_data [dir];
					if (grid_ptr == NULL) return (true);

					grid_ptr->To_Data (cell, lane, 0);
				}

				if (veh2_ptr->Lane_Change () > 0) {
					Lane_Change (veh2_ptr);

					if (veh2_ptr->Move_Flag ()) {
						nreserve++;
					}
					next_vehicle = veh;
				} else { 
					next_vehicle = veh;
				}
			} else if (veh2_ptr->Lane_Change () == 1) {
				if (veh2_ptr->To_Dir () == dir && veh2_ptr->To_Cell () == cell && 
					(veh2_ptr->To_Lane () == lane + 1 || veh2_ptr->To_Lane () == lane - 1)) {

					if (cell > 1 && grid_ptr->To_Data (cell - 1, lane) == 0 && 
						veh2_ptr->Cell () <= (cell - 1)) {
						grid_ptr->To_Data (cell, veh2_ptr->To_Lane (), 0);
						grid_ptr->To_Data (cell - 1, lane, vehicle_ptr->Reserve ());

						veh2_ptr->To_Cell (cell - 1);
						veh2_ptr->To_Lane (lane);
						veh2_ptr->Lane_Change (0);
						veh2_ptr->Move_Flag (true);
						nreserve++;
					}
				}
			}
		}
	}
	return (true);
}
