//*********************************************************
//	Load_Vehicle.cpp - Load a Vehicle to the Network
//*********************************************************

#include "Microsimulator.hpp"

//---------------------------------------------------------
//	Load_Vehicle
//---------------------------------------------------------

bool Microsimulator::Load_Vehicle (Travel_Plan_Data *plan_ptr)
{
	int i, dir, cell, lane, speed, min_lane, max_lane;
	int best_speed, best_lane, run, off_num, traveler, next_rider;
	bool extern_flag, reserve_flag, stop_flag;

	Travel_Leg_Data *leg_ptr;
	Msim_Vehicle *vehicle_ptr, *veh2_ptr;
	Veh_Type_Data *veh_type_ptr;
	Grid_Data *grid_ptr;
	Rider_Data *rider_ptr;
	Traveler_Data *traveler_ptr;
	Travel_Plan_Data *transit_ptr;

	cell = plan_ptr->Start_Cell ();

	if (cell == 0) {
		//---- place the vehicle on the destination parking lot ----
		return (true);
	}

	//---- check the vehicle location ----
			
	vehicle_ptr = vehicle_data [plan_ptr->Vehicle ()];

	if (plan_ptr->Start_Index () != vehicle_ptr->Location ()) {
		if (fix_flag) {
			vehicle_ptr->Location (plan_ptr->Start_Index ());
		} else {
			Problem_Plan (Set_Problem (ACCESS_PROBLEM), plan_ptr, plan_ptr->Travel_Leg (1)->Dir (), 0, 0);

			if (error_flag) {
				Print (1, "Traveler %d Vehicle %d is Not Located at Parking Lot %d", 
					traveler_data [plan_ptr->Traveler ()]->Traveler (),
					vehicle_ptr->Vehicle (), parking_data [plan_ptr->Start_Index ()]->ID ());
				if (vehicle_ptr->Driver ()) {
					Print (1, "Vehicle %d is Currently being Used by Traveler %d", 
						vehicle_ptr->Vehicle (), vehicle_ptr->Driver ());
				} else {
					Print (1, "Vehicle %d is at Parking Lot %d", 
						vehicle_ptr->Vehicle (), vehicle_ptr->Location ());
				}
			}
			delete plan_ptr;
			return (true);
		}
	}

	//---- get the loading link ----

	vehicle_ptr->Travel_Plan (plan_ptr);
	vehicle_ptr->Leg (1);

	leg_ptr = plan_ptr->Travel_Leg (1);

	dir = leg_ptr->Dir ();

	grid_ptr = grid_data [dir];

	if (grid_ptr == NULL) {
		Problem_Plan (Set_Problem (LINK_ACCESS_PROBLEM), plan_ptr, dir, vehicle_ptr->Lane (), vehicle_ptr->Cell ());

		if (error_flag && plan_ptr->Mode () == AUTO_MODE) {
			Print (1, "Loading Link for Traveler %d was Not Found", 
				(traveler_data [plan_ptr->Traveler ()])->Traveler ());
		}
		delete plan_ptr;
		return (true);
	}

	//---- check for reserved cells ----

	reserve_flag = false;

	if (vehicle_ptr->Status () == 3) {
		min_lane = leg_ptr->In_Lane_Low ();

		for (lane = leg_ptr->In_Lane_High (); lane >= min_lane; lane--) {
			if (grid_ptr->Data (cell, lane) == -2) {
				grid_ptr->Data (cell, lane, 0);
				vehicle_ptr->Status (0);
				reserve_flag = true;
			}
		}
	} 

	//---- prepare the vehicle data ----

	vehicle_ptr->Dir (dir);
	vehicle_ptr->Cell (cell);

	//---- calculate the speed ----

	veh_type_ptr = veh_type_data [vehicle_ptr->Type ()];

	stop_flag = (vehicle_ptr->Sub_Type () == 1);

	if (stop_flag) {
		extern_flag = false;
		reserve_flag = true;
	} else {
		extern_flag = (parking_data [plan_ptr->Start_Index ()]->Type () == BOUNDARY);
	}
	if (extern_flag) {
		if (boundary_flag) {
			speed = leg_ptr->Max_Speed ();
		} else {
			speed = grid_ptr->Speed ();
		}
	} else {
		speed = veh_type_ptr->Max_Accel ();
	}
	if (speed > veh_type_ptr->Max_Speed ()) {
		speed = veh_type_ptr->Max_Speed ();
	}
	if (speed > grid_ptr->Speed ()) {
		speed = grid_ptr->Speed ();
	}
	vehicle_ptr->Speed (speed);

	//---- check the best loading lane ----

	best_speed = 0;
	best_lane = 0;

	min_lane = leg_ptr->In_Best_Low ();
	max_lane = leg_ptr->In_Best_High ();

	for (lane = max_lane; lane >= min_lane; lane--) {
		if (grid_ptr->Data (cell, lane) == 0 && 		
			Cell_Use (cell, lane, grid_ptr, veh_type_ptr->Use ())) {

			vehicle_ptr->Lane (lane);

			if (extern_flag) {
				min_speed = 0;

				if (Check_Ahead (vehicle_ptr)) {
					if (max_speed > best_speed) {
						best_speed = max_speed;
						best_lane = lane;
					} else if (reserve_flag && best_lane == 0) {
						best_lane = lane;
					}
				}
			} else {
				if (Check_Behind (vehicle_ptr) && Check_Ahead (vehicle_ptr)) {
					if (max_speed > best_speed) {
						best_speed = max_speed;
						best_lane = lane;
					} else if (reserve_flag && best_lane == 0) {
						best_lane = lane;
					}
				}
			}
		}
	}

	if (best_lane == 0) {

		min_lane = leg_ptr->In_Lane_Low ();
		max_lane = leg_ptr->In_Lane_High ();

		//---- check transit stop lane -----

		if (plan_ptr->Mode () == DRIVER_MODE) {
			if (min_lane > 1 && min_lane == leg_ptr->In_Best_Low ()) {
				if (!Cell_Use (cell, min_lane, grid_ptr, veh_type_ptr->Use ())) {
					min_lane--;
				}
			}
		}
		if (min_lane < leg_ptr->In_Best_Low () || max_lane > leg_ptr->In_Best_High ()) {

			//---- located the first available lane ----

			for (lane = max_lane; lane >= min_lane; lane--) {
				if (grid_ptr->Data (cell, lane) == 0 && 		
					Cell_Use (cell, lane, grid_ptr, veh_type_ptr->Use ())) {

					vehicle_ptr->Lane (lane);

					if (extern_flag) {
						min_speed = 0;

						if (Check_Ahead (vehicle_ptr)) {
							if (max_speed > best_speed) {
								best_speed = max_speed;
								best_lane = lane;
							} else if (reserve_flag && best_lane == 0) {
								best_lane = lane;
							}
						}
					} else {
						if (Check_Behind (vehicle_ptr) && Check_Ahead (vehicle_ptr)) {
							if (max_speed > best_speed) {
								best_speed = max_speed;
								best_lane = lane;
							} else if (reserve_flag && best_lane == 0) {
								best_lane = lane;
							}
						}
					}
				}
			}
		}

		//---- check first unrestricted lane ----

		if (best_lane == 0 && (max_lane < grid_ptr->Lanes () || min_lane > 1)) {
			for (lane = grid_ptr->Lanes (); lane >= 1; lane--) {
				if (grid_ptr->Data (cell, lane) > -1 && 
					Cell_Use (cell, lane, grid_ptr, veh_type_ptr->Use ())) break;
			}
			if (grid_ptr->Data (cell, lane) == 0) {
				vehicle_ptr->Lane (lane);

				if (extern_flag) {
					min_speed = 0;

					if (Check_Ahead (vehicle_ptr)) {
						if (max_speed > best_speed) {
							best_speed = max_speed;
							best_lane = lane;
						} else if (reserve_flag && best_lane == 0) {
							best_lane = lane;
						}
					}
				} else {
					if (Check_Behind (vehicle_ptr) && Check_Ahead (vehicle_ptr)) {
						if (max_speed > best_speed) {
							best_speed = max_speed;
							best_lane = lane;
						} else if (reserve_flag && best_lane == 0) {
							best_lane = lane;
						}
					}
				}
			}
		}
		if (best_lane == 0) {
			if (vehicle_ptr->Status () == 0) {

				//---- try to reserve the cell ----

				min_lane = leg_ptr->In_Best_Low ();

				for (lane = leg_ptr->In_Best_High (); lane >= min_lane; lane--) {
					if (Cell_Use (cell, lane, grid_ptr, veh_type_ptr->Use ())) {
						int vehicle = grid_ptr->Data (cell, lane);

						if (vehicle > 0) {
							veh2_ptr = vehicle_data [vehicle];
							if (veh2_ptr->Speed () == 0 && veh2_ptr->Reserve () == 0) {
								veh2_ptr->Reserve (plan_ptr->Vehicle ());
								vehicle_ptr->Status (3);
								break;
							}
						}
					}
				}
			}
			return (false);
		}
	}
	lane = best_lane;
	speed = best_speed;
	vehicle_ptr->Speed (best_speed);

	//---- load the vehicle ----
	
	grid_ptr->Data (cell, lane, plan_ptr->Vehicle ());

	if (!network_traffic.Add (plan_ptr)) {
		Error ("Adding Traveler %d to the Network", (traveler_data [plan_ptr->Traveler ()])->Traveler ());
	}

	//---- set the traveler status flag ----

	if (plan_ptr->Mode () == AUTO_MODE) {
		Traveler_Data *traveler_ptr = traveler_data [plan_ptr->Traveler ()];

		traveler_ptr->Active (true);
		vehicle_ptr->Driver (traveler_ptr->Traveler ());

		if (veh_type_ptr->Use () == HOV2 || veh_type_ptr->Use () == HOV3 || veh_type_ptr->Use () == HOV4) {
			vehicle_ptr->Passengers (veh_type_ptr->Use () - HOV2 + 1);
		} else {
			vehicle_ptr->Passengers (0);
		}
	} else if (plan_ptr->Mode () == DRIVER_MODE) {

		//---- load passengers to transit ----

		rider_ptr = rider_data [plan_ptr->Traveler ()];	
		run = rider_data.Run (vehicle_ptr->Vehicle ());

		//---- board rider ----

		for (traveler = rider_ptr->On_Rider (1); traveler; traveler = next_rider) {
			traveler_ptr = traveler_data [traveler];
			next_rider = traveler_ptr->Next_Rider ();

			transit_ptr = traveler_ptr->Travel_Plan ();

			if (transit_ptr == NULL) {
				Error ("Travel Plans for Traveler %d have been deleted", traveler_ptr->ID ());
			}

			//---- check the vehicle capacity ----

			if (rider_ptr->Load (run) >= veh_type_ptr->Capacity ()) {
				Problem_Plan (Set_Problem (BOARDING_PROBLEM), transit_ptr, dir, 0, 0);
				continue;
			}

			//---- board the route and set the alighting stop number ----

			off_num = transit_ptr->End_Index ();
					
			traveler_ptr->Active (true);
			traveler_ptr->Next_Rider (rider_ptr->Off_Rider (run, off_num));
			rider_ptr->Off_Rider (run, off_num, traveler);
			rider_ptr->On_Rider (1, next_rider);
			rider_ptr->Board (run, 1, 1);
			nboard++;

			//---- start time event ----

			if (event_output.In_Range (transit_ptr->Start_Time (), START_EVENT, transit_ptr->Mode ())) {
				int offset = cell * cell_size - vehicle_ptr->Offset ();

				Link_Data *link_ptr = link_data [grid_ptr->Link ()];

				if (offset > link_ptr->Length ()) {
					offset = link_ptr->Length ();
				} else if (offset < 0) {
					offset = 0;
				}
				event_output.Household (traveler_ptr->Traveler () / Traveler_Scale ());
				event_output.Person (traveler_ptr->Traveler () % Traveler_Scale ());
				event_output.Trip (transit_ptr->Trip ());
				event_output.Leg (transit_ptr->Leg ());
				event_output.Mode (transit_ptr->Mode ());
				event_output.Event (START_EVENT);
				event_output.Schedule (transit_ptr->Start_Time ());
				event_output.Actual (step);
				if (grid_ptr->Dir () > 0) {
					event_output.Link (-link_ptr->Link ());
				} else {
					event_output.Link (link_ptr->Link ());
				}
				event_output.Offset (Resolve (offset));

				event_output.Output ();
			}
		}
		rider_ptr->Time (run, 1, step);
		vehicle_ptr->Passengers (rider_ptr->Load (run));
	}

	//---- set the vehicle location ----

	vehicle_ptr->Lane (lane);

	if (speed_method) {
		vehicle_ptr->Offset (plan_ptr->Start_Offset ());
	} else {
		vehicle_ptr->Offset (0);
	}

	//---- set other vehicle cells ----

	if (vehicle_ptr->Num_Cells () > 1) {
		for (i=2; i <= vehicle_ptr->Num_Cells (); i++) {
			vehicle_ptr->Dir_Num (i, dir);
			vehicle_ptr->Cell_Num (i, cell);
			vehicle_ptr->Lane_Num (i, lane);
		}
	}

	//---- initialize the to position ----

	vehicle_ptr->Status (0);
	vehicle_ptr->To_Dir (vehicle_ptr->Dir ());
	vehicle_ptr->To_Cell (vehicle_ptr->Cell ());
	vehicle_ptr->To_Lane (vehicle_ptr->Lane ());
	vehicle_ptr->To_Offset (vehicle_ptr->Offset ());
	vehicle_ptr->To_Speed (vehicle_ptr->Speed ());
	vehicle_ptr->To_Leg (vehicle_ptr->Leg ());

	num_vehicles++;

	if (plan_ptr->Mode () == DRIVER_MODE) {
		nrun_start++;

		vehicle_ptr->Priority (true);

		if (!priority_queue.Add (plan_ptr->Vehicle ())) {
			Error ("Insufficient Memory for Priority Queue");
		}
	} else {
		nstarted++;
	}

	//---- check for traveler output ----

	if (traveler_output.In_Range (vehicle_ptr->Driver (), step, vehicle_ptr->To_Dir ())) {
		int offset, to_offset, to_speed;

		offset = plan_ptr->Start_Cell () * cell_size - plan_ptr->Start_Offset ();
		to_offset = vehicle_ptr->To_Cell () * cell_size;
		to_speed = vehicle_ptr->To_Speed ();

		if (speed_method) {
			to_offset -= vehicle_ptr->To_Offset ();
		} else {
			to_speed *= cell_size;
		}
		traveler_output.Traveler (vehicle_ptr->Driver ());
		traveler_output.Vehicle (vehicle_ptr->Vehicle ());
		traveler_output.Time (step);
		traveler_output.From_Dir (vehicle_ptr->Dir ());
		traveler_output.From_Offset (offset);
		traveler_output.To_Dir (vehicle_ptr->To_Dir ());
		traveler_output.To_Offset (to_offset);
		traveler_output.Speed (to_speed);
		traveler_output.Lane (vehicle_ptr->To_Lane ());

		traveler_output.Output ();
	}

	//---- check for summarize output ----

	if (summary_output.In_Range (step, vehicle_ptr->To_Dir (), veh_type_map [vehicle_ptr->Type ()])) {
		int offset, to_offset, to_speed, pce;

		offset = plan_ptr->Start_Cell () * cell_size - plan_ptr->Start_Offset ();
		to_offset = vehicle_ptr->To_Cell () * cell_size;
		to_speed = vehicle_ptr->To_Speed ();

		if (speed_method) {
			to_offset -= vehicle_ptr->To_Offset ();
		} else {
			to_speed *= cell_size;
		}
		if (pce_cells == 1) {
			pce = vehicle_ptr->Num_Cells ();
		} else {
			pce = (vehicle_ptr->Num_Cells () + pce_cells / 2) / pce_cells;
		}
		if (pce < 1) pce = 1;

		summary_output.From_Dir (vehicle_ptr->Dir ());
		summary_output.From_Offset (offset);
		summary_output.From_Speed (0);
		summary_output.From_Connect (-1);
		summary_output.To_Dir (vehicle_ptr->To_Dir ());
		summary_output.To_Offset (to_offset);
		summary_output.To_Speed (to_speed);
		summary_output.To_Connect (0);
		summary_output.Vehicle_PCE (pce);
		summary_output.Occupancy (vehicle_ptr->Passengers () + 1);

		summary_output.Summarize (step);
	}

	//---- start time event ----

	if (event_output.In_Range (plan_ptr->Start_Time (), START_EVENT, plan_ptr->Mode ())) {
		int offset = cell * cell_size - vehicle_ptr->Offset ();

		Link_Data *link_ptr = link_data [grid_ptr->Link ()];

		if (offset > link_ptr->Length ()) {
			offset = link_ptr->Length ();
		} else if (offset < 0) {
			offset = 0;
		}
		int hhold, person;

		if (plan_ptr->Mode () == DRIVER_MODE) {
			hhold = rider_data.Route (vehicle_ptr->Vehicle ());
			person = 0;
		} else {
			hhold = vehicle_ptr->Driver () / Traveler_Scale ();
			person = vehicle_ptr->Driver () % Traveler_Scale ();
		}
		event_output.Household (hhold);
		event_output.Person (person);
		event_output.Trip (plan_ptr->Trip ());
		event_output.Leg (plan_ptr->Leg ());
		event_output.Mode (plan_ptr->Mode ());
		event_output.Event (START_EVENT);
		event_output.Schedule (plan_ptr->Start_Time ());
		event_output.Actual (step);
		if (grid_ptr->Dir () > 0) {
			event_output.Link (-link_ptr->Link ());
		} else {
			event_output.Link (link_ptr->Link ());
		}
		event_output.Offset (Resolve (offset));

		event_output.Output ();
	}
	return (true);
}
