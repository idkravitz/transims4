//*********************************************************
//	Sum_Path.cpp - Sum the Cell Speeds on the Path Ahead
//*********************************************************

#include "Microsimulator.hpp"

//---------------------------------------------------------
//	Sum_Path
//---------------------------------------------------------

int Microsimulator::Sum_Path (Msim_Vehicle *vehicle_ptr)
{
	int gap, c, l, cell, lane, speed, sum_speed, num_lanes, vehicle;
	int num_cells, last_cell, index;

	Travel_Plan_Data *plan_ptr;
	Travel_Leg_Data *leg_ptr, *from_leg;
	Grid_Data *grid_ptr;
	Veh_Type_Data *veh_type_ptr;
	Msim_Vehicle *veh_ptr;

	num_cells = look_ahead;
	sum_speed = num_lanes = 0;

	//---- get the current position ----

	plan_ptr = vehicle_ptr->Travel_Plan ();
	cell = vehicle_ptr->Cell ();
	index = vehicle_ptr->Leg ();
	grid_ptr = grid_data [vehicle_ptr->Dir ()];

	if (index == plan_ptr->Num_Travel_Legs ()) {
		last_cell = plan_ptr->End_Cell ();
		if (cell >= last_cell) {
			return (0);
		}
	} else {
		last_cell = grid_ptr->Cells ();
	}
	lane = vehicle_ptr->Lane ();
	speed = vehicle_ptr->Speed ();

	veh_type_ptr = veh_type_data [vehicle_ptr->Type ()];

	//---- sum number of cells ahead ----

	leg_ptr = plan_ptr->Travel_Leg (index);
	vehicle = 0;

	for (gap=1, c=cell, l=lane; gap <= num_cells; gap++) {

		//---- end of the link ----

		if (++c > last_cell) {

			//---- set the exit speed ----

			if (leg_ptr->Max_Speed () < speed) {
				speed = leg_ptr->Max_Speed ();
			}

			//---- check the exit lane ----

			if (l < leg_ptr->Out_Lane_Low ()) {
				num_lanes += leg_ptr->Out_Lane_Low () - l;
				l = leg_ptr->Out_Lane_Low ();
				speed = 0;
			} else if (l > leg_ptr->Out_Lane_High ()) {
				num_lanes += l - leg_ptr->Out_Lane_High ();
				l = leg_ptr->Out_Lane_High ();
				speed = 0;
			}

			if (index == plan_ptr->Num_Travel_Legs ()) break;

			from_leg = leg_ptr;

			//---- next link ----

			leg_ptr = plan_ptr->Travel_Leg (++index);

			grid_ptr = grid_data [leg_ptr->Dir ()];

			if (index == plan_ptr->Num_Travel_Legs ()) {
				last_cell = plan_ptr->End_Cell ();
			} else {
				last_cell = grid_ptr->Cells ();
			}
			c = 1;
			l = Entry_Lane (l, from_leg, leg_ptr);
		}
		vehicle = grid_ptr->Data (c, l);

		//---- check the use restrictions ----

		if (!Cell_Use (c, l, grid_ptr, veh_type_ptr->Use ())) {

			//---- get out of the restricted lane ----

			if (l > 1) {
				if (Cell_Use (c, l-1, grid_ptr, veh_type_ptr->Use ())) {
					l--;
					num_lanes++;
				}
			} else if (l < grid_ptr->Lanes ()) {
				if (Cell_Use (c, l+1, grid_ptr, veh_type_ptr->Use ())) {
					l++;
					num_lanes++;
				}
			}
			speed = 0;

		} else if (vehicle >= 0) {

			//---- accelerate ----

			speed += veh_type_ptr->Max_Accel ();

			if (speed > veh_type_ptr->Max_Speed ()) {
				speed = veh_type_ptr->Max_Speed ();
			}
			if (speed > grid_ptr->Speed ()) {
				speed = grid_ptr->Speed ();
			}

			//---- check the vehicle speed ----

			if (vehicle > 0) {
				veh_ptr = vehicle_data [vehicle];

				if (veh_ptr->Speed () < speed) {
					speed = veh_ptr->Speed ();
				}
			}
		} else {

			//---- process a pocket lane ----
			
			if (l <= grid_ptr->Left ()) {
				for (; grid_ptr->Data (c, l) < 0; l++) {
					num_lanes++;
				}
			} else {
				for (; grid_ptr->Data (c, l) < 0; l--) {
					num_lanes++;
				}
			}
			speed = 0;
		}
		sum_speed += speed;
	}
	return (time_factor * sum_speed - lane_factor * num_lanes);
}
