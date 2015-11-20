//*********************************************************
//	Check_Behind.cpp - Check Speed Constraints from Behind
//*********************************************************

#include "Microsimulator.hpp"

#include <math.h>

//---------------------------------------------------------
//	Check_Behind
//---------------------------------------------------------

bool Microsimulator::Check_Behind (Msim_Vehicle *vehicle_ptr, int seconds)
{
	int c, l, dir, cell, lane, speed, offset, decel, decel2, length, stop_dist, last_cell, react;
	int vehicle, veh_speed, veh_decel2, veh_dist, gap, gap_dist, gap_cells, to_speed, min_dist;

	Msim_Vehicle *veh_ptr;
	Veh_Type_Data *veh_type_ptr;
	Grid_Data *grid_ptr;

	decel = decel2 = stop_dist = min_dist = 0;

	//---- get the vehicle position ----

	dir = vehicle_ptr->Dir ();
	if (dir == 0) return (false);

	cell = vehicle_ptr->Cell ();
	lane = vehicle_ptr->Lane ();
	offset = vehicle_ptr->Offset ();
	speed = vehicle_ptr->Speed ();

	grid_ptr = grid_data [dir];

	if (grid_ptr->To_Data (cell, lane) > 0) {
		return (false);
	}
	veh_type_ptr = veh_type_data [vehicle_ptr->Type ()];

	length = veh_type_ptr->Length ();

	if (speed_method) {

		//---- vehicle performance ----

		decel = veh_type_ptr->Max_Decel ();
		decel2 = 2 * decel;

		stop_dist = speed * speed / decel2;

		min_speed = speed - decel;
		if (min_speed < 0) min_speed = 0;

		//---- stopping distance at link speed ----

		veh_speed = grid_ptr->Speed ();

		min_dist = (seconds - 1) * veh_speed;

		veh_dist = min_dist + veh_speed * veh_speed / decel2;

		if (reaction_flag) {
			double spd = reaction_time [grid_ptr->Type ()] * veh_speed;
			react = (int) spd;
			spd -= react;
			if (spd > random.Probability ()) react++;
			veh_dist += react;
		}

		gap_dist = veh_dist + length - stop_dist;

		gap_cells = (gap_dist + offset + cell_round) / cell_size;

	} else {

		min_speed = 0;

		gap_cells = seconds * grid_ptr->Speed () + length;

	}

	//---- search for upstream vehicles ----

	vehicle = 0;

	for (gap=1, c=cell, l=lane; gap <= gap_cells; gap++) {

		//---- end of the link ----

		if (--c == 0) {

			//---- get the primary input lane ----

			dir = grid_ptr->Connect_Dir (l);
			if (dir == 0) break;

			l = grid_ptr->Connect_Lane (l);

			grid_ptr = grid_data [dir];

			c = grid_ptr->Cells ();
		}

		vehicle = grid_ptr->Data (c, l);

		if (vehicle != 0) break;
	}

	//---- check the vehicle stopping distance ----

	if (vehicle > 0) {

		//---- check for your own tail ----

		if (vehicle == vehicle_ptr->Travel_Plan ()->Vehicle ()) return (true);

		veh_ptr = vehicle_data [vehicle];

		if (veh_ptr->Dir () == 0) return (true);

		if (speed_method) {
			veh_speed = veh_ptr->Speed ();

			if (veh_ptr->Type () != vehicle_ptr->Type ()) {
				veh_decel2 = 2 * veh_type_data [veh_ptr->Type ()]->Max_Decel ();
			} else {
				veh_decel2 = decel2;
			}
			react = 0;

			if (veh_ptr->Status () == 1) {
				to_speed = veh_ptr->To_Speed ();

				veh_dist = min_dist + (veh_speed + to_speed) / 2 + to_speed * to_speed / veh_decel2;
			} else {
				veh_dist = min_dist + veh_speed * veh_speed / veh_decel2;

				if (reaction_flag) {
					double spd = reaction_time [grid_ptr->Type ()] * veh_speed;
					react = (int) spd;
					spd -= react;
					if (spd > random.Probability ()) react++;
					veh_dist += react;
				}
			}
			gap_dist = gap * cell_size - offset + veh_ptr->Offset ();

			veh_dist += length - gap_dist;

			if (veh_dist > stop_dist) {

				//--- try without the reaction time ----

				if (veh_dist - react > stop_dist) return (false);

				//---- calculate minimum speed ----

				min_speed = ((int) ((sqrt ((double) (decel * decel + 4 * decel * (2 * veh_dist - speed))) + 0.5)) - decel) / 2;
			}

		} else {

			if (veh_ptr->Status () == 1) {

				veh_speed = veh_ptr->To_Speed ();

			} else {

				veh_speed = veh_ptr->Speed ();
				last_cell = grid_ptr->Cell_Out ();

				//---- check for a traffic control ----

				if (veh_speed > 0 && c >= (last_cell - veh_speed) && c <= last_cell) {
					Travel_Plan_Data *plan_ptr = veh_ptr->Travel_Plan ();
					if (plan_ptr == NULL) return (true);

					Travel_Leg_Data *leg_ptr = plan_ptr->Travel_Leg (veh_ptr->Leg ());
					if (leg_ptr == NULL) return (true);

					if (leg_ptr->Control () > 0) {
						Control_Data *control_ptr;

						control_ptr = control_data [leg_ptr->Control ()];

						if (control_ptr->Control_Status () == RED_LIGHT ||
							(veh_speed > 0 && (control_ptr->Control_Status () == STOP_SIGN || 
								control_ptr->Control_Status () == STOP_GREEN))) {

							return (true);
						}
					}
				}
			}

			if (seconds * veh_speed + length > gap && veh_speed > 0) {
				return (false);
			}
		}
	}
	return (true);
}
