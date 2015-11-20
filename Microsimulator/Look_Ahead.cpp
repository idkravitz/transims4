//*********************************************************
//	Look_Ahead.cpp - Look at Path Options Ahead
//*********************************************************

#include "Microsimulator.hpp"

//---------------------------------------------------------
//	Look_Ahead
//---------------------------------------------------------

void Microsimulator::Look_Ahead (Msim_Vehicle *vehicle_ptr)
{
	int i, cell, lane, new_lane, sum_change, sum_stay, speed, vehicle;
	
	Use_Type use;
	Grid_Data *grid_ptr;
	Veh_Type_Data *veh_type_ptr;

	//---- check the target lane ----

	grid_ptr = grid_data [vehicle_ptr->Dir ()];
	if (grid_ptr == NULL) return;

	cell = vehicle_ptr->Cell ();
	lane = vehicle_ptr->Lane ();

	if (cell < grid_ptr->Cell_In () || cell >= grid_ptr->Cell_Out ()) return;

	if ((step & 1) == 1) {
		new_lane = lane + 1;
	} else {
		new_lane = lane - 1;
	}
	if (new_lane < 1 || new_lane > grid_ptr->Lanes ()) return;

	if (grid_ptr->Data (cell, new_lane) != 0 || grid_ptr->To_Data (cell, new_lane) != 0) return;

	veh_type_ptr = veh_type_data [vehicle_ptr->Type ()];

	use = veh_type_ptr->Use ();
	sum_change = 0;

	//---- check for a special use lane ----

	if (use == BUS || use == HOV2 || use == HOV3 || use == HOV4) {
		if (!Cell_Use (cell, lane, grid_ptr, use, true)) {
			for (i=1; i <= grid_ptr->Lanes (); i++) {
				if (Cell_Use (cell, i, grid_ptr, use, true)) {
					vehicle_ptr->Lane (i);
					sum_change = Sum_Path (vehicle_ptr) - abs (lane - i) * lane_factor;
					vehicle_ptr->Lane (lane);

					if ((i < lane && new_lane > lane) || (i > lane && new_lane < lane)) {
						sum_change = -sum_change;
					}
					break;
				}
			}
		}
	}
	if (!Cell_Use (cell, new_lane, grid_ptr, use)) return;

	//---- check the best lane range ----

	if ((grid_ptr->Cells () - cell) < plan_follow * 2 / NUM_PRIORITY_LEVELS) {
		if (!vehicle_ptr->Priority ()) {
		//if (vehicle_ptr->Waiting () < max_wait_time - 10) {
			Travel_Leg_Data *leg_ptr = vehicle_ptr->Travel_Plan ()->Travel_Leg (vehicle_ptr->Leg ());
			if (leg_ptr == 0) return;
			if (vehicle_ptr->Waiting () > min_wait_time / 2) {
			//if (vehicle_ptr->Priority ()) {
				if (lane <= leg_ptr->Out_Lane_Low () && new_lane < lane) return;
				if (lane >= leg_ptr->Out_Lane_High () && new_lane > lane) return;
			} else {
				if (lane <= leg_ptr->Out_Best_Low () && new_lane < lane) return;
				if (lane >= leg_ptr->Out_Best_High () && new_lane > lane) return;
			}
		}
	}

	//---- check the merge ----

	vehicle_ptr->Lane (new_lane);

	if (!Check_Behind (vehicle_ptr) || !Check_Ahead (vehicle_ptr)) {
		vehicle_ptr->Lane (lane);
		return;
	}

	if (permit_none && grid_ptr->Data (cell-1, new_lane) > 0) return;

	speed = vehicle_ptr->Speed ();

	//---- get the relative path weights ----

	sum_change += Sum_Path (vehicle_ptr) - lane_factor;

	vehicle_ptr->Lane (lane);

	sum_stay = Sum_Path (vehicle_ptr);

	if (sum_change <= sum_stay) return;

	//---- check the permissive behavior ----	

	vehicle = grid_ptr->Data (cell-1, new_lane);

	if (vehicle > 0) {
		if (!permit_all) {
			if (random.Probability () > permission_prob [grid_ptr->Type ()]) return;
		}
		vehicle_ptr->Reserve (vehicle);
	}

	//---- change lanes ----

	if (vehicle_ptr->Num_Cells () > 1) {
		vehicle_ptr->To_Lane (new_lane);

		Move_Veh_Cells (vehicle_ptr, true);
	} else {
		grid_ptr->Data (cell, lane, 0);
		grid_ptr->Data (cell, new_lane, vehicle_ptr->Travel_Plan ()->Vehicle ());

		vehicle_ptr->Lane (new_lane);
		vehicle_ptr->To_Lane (new_lane);
	}
	noptional++;
	vehicle_ptr->Move_Flag (true);

	return;
}
