//*********************************************************
//	Lane_Change.cpp - Process a Lane Change
//*********************************************************

#include "Microsimulator.hpp"

//---------------------------------------------------------
//	Lane_Change
//---------------------------------------------------------

bool Microsimulator::Lane_Change (Msim_Vehicle *vehicle_ptr)
{
	int dir, cell, lane, new_cell, new_lane, vehicle, best_out;
	bool use_flag, permit_flag;

	Travel_Plan_Data *plan_ptr, *plan2_ptr;
	Msim_Vehicle *veh2_ptr;
	Veh_Type_Data *veh_type_ptr;
	Grid_Data *grid_ptr;
	Travel_Leg_Data *leg_ptr, *leg2_ptr;

	if (vehicle_ptr->Lane_Change () == 0 || vehicle_ptr->Dir () == 0) return (false);

	lane = vehicle_ptr->Lane ();

	//--- check for lane changes ----

	plan_ptr = vehicle_ptr->Travel_Plan ();
	if (plan_ptr == NULL) return (false);

	leg_ptr = plan_ptr->Travel_Leg (vehicle_ptr->Leg ());
	if (leg_ptr == NULL) return (false);

	dir = vehicle_ptr->Dir ();

	grid_ptr = grid_data [dir];
	if (grid_ptr == NULL) return (false);

	cell = new_cell = vehicle_ptr->Cell ();
	
	veh_type_ptr = veh_type_data [vehicle_ptr->Type ()];

	if (vehicle_ptr->Blocked ()) {
		new_lane = (leg_ptr->Out_Best_Low () + leg_ptr->Out_Best_High ()) / 2;
		if (lane <= new_lane) {
			if (Cell_Use (cell+1, lane+1, grid_ptr, veh_type_ptr->Use ())) {
				new_lane = lane + 1;
			} else {
				new_lane = lane - 1;
			}
		} else if (Cell_Use (cell+1, lane-1, grid_ptr, veh_type_ptr->Use ())) {
			new_lane = lane - 1;
		} else {
			new_lane = lane + 1;
		}
	} else if (lane <= leg_ptr->Out_Best_Low () && lane < leg_ptr->Out_Best_High ()) {
		new_lane = lane + 1;
	} else if (lane >= leg_ptr->Out_Best_High () && lane > leg_ptr->Out_Best_Low ()) {
		new_lane = lane - 1;
	} else if (lane <= leg_ptr->Out_Lane_Low () && lane < leg_ptr->Out_Lane_High ()) {
		new_lane = lane + 1;
	}  else if (lane >= leg_ptr->Out_Lane_High () && lane > leg_ptr->Out_Lane_Low ()) {
		new_lane = lane - 1;
	} else if (lane > 1) {
		new_lane = lane - 1;
	} else {
		new_lane = lane + 1;
	}

	//---- check the target lane ----

	if (new_lane < 1 || new_lane > grid_ptr->Lanes ()) return (false);

	vehicle = grid_ptr->Data (cell, new_lane);

	if (vehicle < 0) return (false);
	if (vehicle > 0 && vehicle_ptr->Lane_Change () > 1) return (false);

	use_flag = false;

	if (!Cell_Use (cell, new_lane, grid_ptr, veh_type_ptr->Use ())) {
		if (cell == grid_ptr->Cell_Out ()) {
			use_flag = true;
		} else {
			return (false);
		}
	}

	//---- check the vehicle status ----

	if (vehicle > 0) {

		veh2_ptr = vehicle_data [vehicle];
		if (veh2_ptr == NULL) return (false);

		if (veh2_ptr->Dir () == 0) return (false);

		if (veh2_ptr->Status () == 0 || !veh2_ptr->Move_Flag ()) {
			if (veh2_ptr->Reserve () == 0 && grid_ptr->To_Data (cell, new_lane) == 0) {
				veh2_ptr->Reserve (plan_ptr->Vehicle ());
			}

			//---- check for the front of a multi-cell vehicle ----

			if (veh2_ptr->Lane_Change () == 0) return (false);
			if (veh2_ptr->Num_Cells () > 1 && veh2_ptr->Cell () != cell) return (false);
			if (vehicle_ptr->Num_Cells () > 1 && veh2_ptr->Speed () > 0) return (false);

			//---- check the relative speeds ----

			if (vehicle_ptr->Speed () > max_swap_speed) return (false);
			if (veh2_ptr->Speed () > max_swap_speed) return (false);
			if (abs (vehicle_ptr->Speed () - veh2_ptr->Speed ()) > max_swap_diff) return (false);

			//---- attempt to swap positions ----

			if (vehicle_ptr->Num_Cells () == 1 && veh2_ptr->Num_Cells () == 1) {

				//--- check for lane changes ----

				plan2_ptr = veh2_ptr->Travel_Plan ();
				if (plan2_ptr == NULL) return (false);

				leg2_ptr = plan2_ptr->Travel_Leg (veh2_ptr->Leg ());
				if (leg2_ptr == NULL) return (false);

				if (new_lane <= leg2_ptr->Out_Best_Low () && new_lane < leg2_ptr->Out_Best_High ()) {
					best_out = new_lane + 1;
				} else if (new_lane >= leg2_ptr->Out_Best_High () && new_lane > leg2_ptr->Out_Best_Low ()) {
					best_out = new_lane - 1;
				} else if (new_lane <= leg2_ptr->Out_Lane_Low () && new_lane < leg2_ptr->Out_Lane_High ()) {
					best_out = new_lane + 1;
				}  else if (new_lane >= leg2_ptr->Out_Lane_High () && new_lane > leg2_ptr->Out_Lane_Low ()) {
					best_out = new_lane - 1;
				} else if (new_lane > 1) {
					best_out = new_lane - 1;
				} else {
					best_out = new_lane + 1;
				}
				if (best_out != lane) return (false);

				//--- exchange positions ----

				grid_ptr->Data (cell, lane, plan2_ptr->Vehicle ());
				grid_ptr->Data (cell, new_lane, plan_ptr->Vehicle ());

				veh2_ptr->Lane (lane);
				veh2_ptr->Move_Flag (true);
				veh2_ptr->Lane_Change (0);

				vehicle_ptr->Lane (new_lane);
				vehicle_ptr->Move_Flag (true);
				vehicle_ptr->Lane_Change (0);

				if (veh2_ptr->Reserve () == plan_ptr->Vehicle ()) {
					veh2_ptr->Reserve (0);
				}
				nswap += 2;
				return (true);
			}

			//---- try to move the vehicle diagonally -----

			if (cell == grid_ptr->Cells () || permit_none) return (false);

			vehicle_ptr->Lane (new_lane);

			min_speed = 0;
			if (!Check_Ahead (vehicle_ptr) || max_speed == 0) {
				vehicle_ptr->Lane (lane);
				return (false);	
			}
			new_cell = cell + 1;
		}
	}

	//---- check the merge ----				

	vehicle_ptr->Cell (new_cell);
	vehicle_ptr->Lane (new_lane);

	if (!Check_Behind (vehicle_ptr) || !Check_Ahead (vehicle_ptr) || 
		(use_flag && max_speed == 0)) {

		vehicle_ptr->Cell (cell);
		vehicle_ptr->Lane (lane);
		return (false);
	}

	//---- check the permissive behavior ----	

	vehicle = grid_ptr->Data (new_cell-1, new_lane);
	
	if (vehicle > 0) {
		if (permit_all) {
			permit_flag = true;
		} else if (permit_none) {
			permit_flag = false;
		} else {
			permit_flag = (random.Probability () <= permission_prob [grid_ptr->Type ()]);
		}
		if (permit_flag) {
			vehicle_ptr->Reserve (vehicle);
		} else {
			vehicle_ptr->Cell (cell);
			vehicle_ptr->Lane (lane);

			veh2_ptr = vehicle_data [vehicle];
			if (veh2_ptr == NULL) return (false);

			if (veh2_ptr->Status () == 1 && !veh2_ptr->Move_Flag ()) {

				veh2_ptr->Status (0);
				veh2_ptr->Waiting (veh2_ptr->Waiting () - 10 * steps_per_second);

				dir = veh2_ptr->Dir ();
				cell = veh2_ptr->Cell ();
				lane = veh2_ptr->Lane ();

				grid_ptr = grid_data [dir];
				if (grid_ptr == NULL) return (false);

				grid_ptr->To_Data (cell, lane, 0);

				next_vehicle = vehicle;
			}
			return (false);
		}
	}

	//---- move the vehicle ----

	if (vehicle_ptr->Num_Cells () > 1) {
		vehicle_ptr->Cell (cell);
		vehicle_ptr->Lane (lane);
		vehicle_ptr->To_Dir (dir);
		vehicle_ptr->To_Cell (new_cell);
		vehicle_ptr->To_Lane (new_lane);

		Move_Veh_Cells (vehicle_ptr, true);
	} else {

		grid_ptr->Data (cell, lane, 0);
		grid_ptr->Data (new_cell, new_lane, plan_ptr->Vehicle ());
	}
	vehicle_ptr->Move_Flag (true);
	vehicle_ptr->Lane_Change (0);

	nrequired++;
	return (true);
}
