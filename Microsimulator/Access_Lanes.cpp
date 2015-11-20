//*********************************************************
//	Access_Lanes.cpp - Entry/Exit Lane Options
//*********************************************************

#include "Microsimulator.hpp"

//---------------------------------------------------------
//	Access_Lanes
//---------------------------------------------------------

bool Microsimulator::Access_Lanes (Travel_Leg_Data *leg_ptr, Travel_Plan_Data *plan_ptr, bool start_flag)
{
	int i, cell, dir, offset, low_lane, high_lane, index, time;
	bool limit_flag, stop_flag;

	Grid_Data *grid_ptr;

	dir = offset = time = 0;
	limit_flag = stop_flag = false;

	//---- get the parking data ----

	grid_ptr = grid_data [leg_ptr->Dir ()];

	if (start_flag) {
		index = plan_ptr->Start_Index ();

		if (plan_ptr->Start_Type () == PARKING_ID) {
			Parking_Data *parking_ptr = parking_data [index];

			dir = parking_ptr->Dir ();
			offset = parking_ptr->Offset ();
			limit_flag = (parking_ptr->Type () != BOUNDARY);
			stop_flag = false;
			time = plan_ptr->Start_Time ();

		} else if (plan_ptr->Start_Type () == STOP_ID) {
			Stop_Data *stop_ptr = stop_data [index];

			dir = grid_ptr->Dir ();
			offset = stop_ptr->Offset ();
			limit_flag = (stop_ptr->Type () != EXTLOAD);
			stop_flag = true;
		}
		leg_ptr->In_Lane_Low (0);
		leg_ptr->In_Lane_High (0);
		plan_ptr->Start_Cell (0);
		plan_ptr->Start_Offset (0);

	} else {
		index = plan_ptr->End_Index ();

		if (plan_ptr->End_Type () == PARKING_ID) {
			Parking_Data *parking_ptr = parking_data [index];

			dir = parking_ptr->Dir ();
			offset = parking_ptr->Offset ();
			limit_flag = (parking_ptr->Type () != BOUNDARY);
			stop_flag = false;
			time = plan_ptr->End_Time ();

		} else if (plan_ptr->End_Type () == STOP_ID) {
			Stop_Data *stop_ptr = stop_data [index];

			dir = grid_ptr->Dir ();
			offset = stop_ptr->Offset ();
			limit_flag = (stop_ptr->Type () != EXTLOAD);
			stop_flag = true;
		}
		leg_ptr->Out_Lane_Low (0);
		leg_ptr->Out_Lane_High (0);
		plan_ptr->End_Cell (0);
		plan_ptr->End_Offset (0);
	}

	//---- get the cell number ----

	if (stop_flag) {
		cell = offset;
		offset = 0;
	} else {
		if (grid_ptr->Dir () == 1) {
			Link_Data *link_ptr = link_data [grid_ptr->Link ()];

			offset = link_ptr->Length () - offset;
		}
		cell = MIN (MAX (((offset + half_cell) / cell_size), grid_ptr->Cell_In ()), grid_ptr->Cell_Out ());

		offset = cell * cell_size - offset;
	}

	//---- determine the lane options ----

	low_lane = grid_ptr->Lanes () + 1;
	high_lane = 0;

	for (i=1; i <= grid_ptr->Lanes (); i++) {
		if (grid_ptr->Data (cell, i) != -1) {
			if (i < low_lane) low_lane = i;
			if (i > high_lane) high_lane = i;
		}
	}
	if (high_lane == 0) return (false);

	if (limit_flag) {
		Msim_Vehicle *vehicle_ptr;
		Veh_Type_Data *veh_type_ptr;

		vehicle_ptr = vehicle_data [plan_ptr->Vehicle ()];
		veh_type_ptr = veh_type_data [vehicle_ptr->Type ()];

		leg_ptr->Max_Speed (veh_type_ptr->Max_Decel ());

		if (leg_ptr->Max_Speed () > veh_type_ptr->Max_Speed ()) {
			leg_ptr->Max_Speed (veh_type_ptr->Max_Speed ());
		}
		if (leg_ptr->Max_Speed () > grid_ptr->Speed ()) {
			leg_ptr->Max_Speed (grid_ptr->Speed ());
		}
		if (grid_ptr->Dir () == dir) {
			//if (stop_flag) {
			//	low_lane = high_lane;
			//} else {
				low_lane = MAX (high_lane - 1, low_lane);
			//}
		} else {
			//if (stop_flag) {
			//	high_lane = low_lane;
			//} else {
				high_lane = MIN (low_lane + 1, high_lane);
			//}
		}
	} else if (boundary_flag) {
		Boundary_Data *boundary_ptr;

		boundary_ptr = boundary_data.Get (index);

		if (boundary_ptr != NULL) {
			int speed;
			Msim_Vehicle *vehicle_ptr;
			Veh_Type_Data *veh_type_ptr;

			vehicle_ptr = vehicle_data [plan_ptr->Vehicle ()];
			veh_type_ptr = veh_type_data [vehicle_ptr->Type ()];

			time = boundary_data.Period (time / time_step.Step_Size ());
			speed = (boundary_ptr->Speed (time) + cell_round) / cell_size;
			if (speed < 1) speed = 1;

			if (slow_down_flag) {
				if (slow_down_prob [grid_ptr->Type ()] > random.Probability ()) {
					if (slow_down_percent [grid_ptr->Type ()] > 0) {
						double spd = speed * slow_down_percent [grid_ptr->Type ()] / 100.0;
						int slow_down = (int) spd;
						spd -= slow_down;

						if (spd > 0.0 && spd > random.Probability ()) slow_down++;

						speed -= slow_down;
					} else {
						speed -= veh_type_ptr->Max_Decel ();
					}
					if (speed < 1) speed = 1;
				}
			}
			leg_ptr->Max_Speed (speed);

			if (leg_ptr->Max_Speed () > veh_type_ptr->Max_Speed ()) {
				leg_ptr->Max_Speed (veh_type_ptr->Max_Speed ());
			}
			if (leg_ptr->Max_Speed () > grid_ptr->Speed ()) {
				leg_ptr->Max_Speed (grid_ptr->Speed ());
			}
		}
	}

	if (start_flag) {
		leg_ptr->In_Lane_Low (low_lane);
		leg_ptr->In_Lane_High (high_lane);
		plan_ptr->Start_Cell (cell);
		plan_ptr->Start_Offset (offset);
	} else {
		leg_ptr->Out_Lane_Low (low_lane);
		leg_ptr->Out_Lane_High (high_lane);
		leg_ptr->Out_Best_Low (low_lane);
		leg_ptr->Out_Best_High (high_lane);
		plan_ptr->End_Cell (cell);
		plan_ptr->End_Offset (offset);
	}
	return (true);
}
