//*********************************************************
//	Move_Veh_Cells.cpp - Move Multi-Cell Vehicles
//*********************************************************

#include "Microsimulator.hpp"

//---------------------------------------------------------
//	Move_Veh_Cells
//---------------------------------------------------------

void Microsimulator::Move_Veh_Cells (Msim_Vehicle *vehicle_ptr, bool in_place)
{
	int i, from_dir, from_cell, from_lane, to_dir, to_cell, to_lane, vehicle, num_cells;
	int index, last_cell;
	bool one_cell;

	Grid_Data *from_grid_ptr, *to_grid_ptr = 0;
	Travel_Plan_Data *plan_ptr;
	Position *position_ptr;
	Travel_Leg_Data *leg_ptr, *from_leg;

	//---- initialize the movement ----

	num_cells = vehicle_ptr->Num_Cells ();

	from_dir = vehicle_ptr->Dir ();
	from_cell = vehicle_ptr->Cell ();
	from_lane = vehicle_ptr->Lane ();

	to_dir = vehicle_ptr->To_Dir ();
	to_cell = vehicle_ptr->To_Cell ();
	to_lane = vehicle_ptr->To_Lane ();

	plan_ptr = vehicle_ptr->Travel_Plan ();

	vehicle = plan_ptr->Vehicle ();

	//---- check for same cell movements -----

	if (from_dir == to_dir && from_cell == to_cell && from_lane == to_lane) {
		if (in_place) return;
		index = 0;

		for (i=1; i <= num_cells; i++) {
			to_dir = vehicle_ptr->Dir_Num (i);
			to_cell = vehicle_ptr->Cell_Num (i);
			to_lane = vehicle_ptr->Lane_Num (i);

			if (to_dir != index) {
				to_grid_ptr = grid_data [to_dir];
				index = to_dir;
			}
			to_grid_ptr->To_Data (to_cell, to_lane, vehicle);
		}
		return;
	}

	//---- check for one cell movements ----

	one_cell = false;

	if (from_dir == to_dir && from_dir > 0) {
		if (from_cell == to_cell) {
			if (from_lane == to_lane + 1) {
				one_cell = true;
			} else if (from_lane == to_lane - 1) {
				one_cell = true;
			}
		} else if (from_lane == to_lane) {
			if (from_cell == to_cell - 1) {
				one_cell = true;
			}
		}
	}

	//---- implement one cell movement ----

	if (one_cell || in_place) {
		if (!one_cell) ndiagonal++;

		to_grid_ptr = grid_data [to_dir];

		for (i=1; i <= num_cells; i++) {
			from_dir = vehicle_ptr->Dir_Num (i);
			from_cell = vehicle_ptr->Cell_Num (i);
			from_lane = vehicle_ptr->Lane_Num (i);

			if (in_place || i > 1) {
				vehicle_ptr->Dir_Num (i, to_dir);
				vehicle_ptr->Cell_Num (i, to_cell);
				vehicle_ptr->Lane_Num (i, to_lane);
			}

			if (from_dir > 0) {
				from_grid_ptr = grid_data [from_dir];
			
				if (in_place && from_grid_ptr->Data (from_cell, from_lane) == vehicle) {
					from_grid_ptr->Data (from_cell, from_lane, 0);
				}
			} else {
				from_grid_ptr = NULL;
			}
			if (to_dir > 0) {
				if (in_place) {
					to_grid_ptr->Data (to_cell, to_lane, vehicle);
				} else {
					to_grid_ptr->To_Data (to_cell, to_lane, vehicle);
				}
			}
			to_dir = from_dir;
			to_cell = from_cell;
			to_lane = from_lane;
			to_grid_ptr = from_grid_ptr;
		}
		return;
	}

	//---- initialize the position list with current cells ----

	position_data.Reset ();

	for (i=num_cells; i > 0; i--) {
		position_ptr = position_data.New_Record ();
		if (position_ptr == NULL) goto mem_error;

		position_ptr->Dir (vehicle_ptr->Dir_Num (i));
		position_ptr->Cell (vehicle_ptr->Cell_Num (i));
		position_ptr->Lane (vehicle_ptr->Lane_Num (i));

		if (!position_data.Add ()) goto mem_error;
	}

	//---- build the cell path to the destination ----

	from_grid_ptr = grid_data [from_dir];

	index = vehicle_ptr->Leg ();

	if (index == plan_ptr->Num_Travel_Legs ()) {
		last_cell = plan_ptr->End_Cell ();
	} else {
		last_cell = from_grid_ptr->Cells ();
	}

	leg_ptr = plan_ptr->Travel_Leg (index);

	while (from_dir != to_dir || (from_dir == to_dir && from_cell < to_cell)) {

		//---- end of the link ----

		if (++from_cell > last_cell) {

			if (index == plan_ptr->Num_Travel_Legs ()) break;

			if (from_lane < leg_ptr->Out_Lane_Low () || from_lane > leg_ptr->Out_Lane_High ()) break;

			from_leg = leg_ptr;

			//---- next link ----

			leg_ptr = plan_ptr->Travel_Leg (++index);

			from_dir = leg_ptr->Dir ();

			from_grid_ptr = grid_data [from_dir];

			if (index == plan_ptr->Num_Travel_Legs ()) {
				last_cell = plan_ptr->End_Cell ();
			} else {
				last_cell = from_grid_ptr->Cells ();
			}
			from_cell = 1;
			from_lane = Entry_Lane (from_lane, from_leg, leg_ptr);
		}

		//---- add the cell to the position list ----

		position_ptr = position_data.New_Record ();
		if (position_ptr == NULL) goto mem_error;

		position_ptr->Dir (from_dir);
		position_ptr->Cell (from_cell);
		position_ptr->Lane (from_lane);

		if (!position_data.Add ()) goto mem_error;
	}

	//---- adjust the vehicle cells ---

	for (i=1, position_ptr = position_data.Last (); i <= num_cells; i++, position_ptr = position_data.Previous ()) {

		from_dir = vehicle_ptr->Dir_Num (i);
		from_cell = vehicle_ptr->Cell_Num (i);
		from_lane = vehicle_ptr->Lane_Num (i);

		if (from_dir > 0) {
			from_grid_ptr = grid_data [from_dir];
		
			if (in_place && from_grid_ptr->Data (from_cell, from_lane) == vehicle) {
				from_grid_ptr->Data (from_cell, from_lane, 0);
			}
		} else {
			from_grid_ptr = NULL;
		}

		to_dir = position_ptr->Dir ();
		to_cell = position_ptr->Cell ();
		to_lane = position_ptr->Lane ();

		if (in_place || i > 1) {
			vehicle_ptr->Dir_Num (i, to_dir);
			vehicle_ptr->Cell_Num (i, to_cell);
			vehicle_ptr->Lane_Num (i, to_lane);
		}

		if (to_dir > 0) {
			to_grid_ptr = grid_data [to_dir];

			if (in_place) {
				to_grid_ptr->Data (to_cell, to_lane, vehicle);
			} else {
				to_grid_ptr->To_Data (to_cell, to_lane, vehicle);
			}
		}
	}
	return;

mem_error:
	Error ("Insufficient Memory for Vehicle Position List");
}
