//*********************************************************
//	Move_Vehicles.cpp - Reposition the Vehicles
//*********************************************************

#include "Microsimulator.hpp"

//---------------------------------------------------------
//	Move_Vehicles
//---------------------------------------------------------

bool Microsimulator::Move_Vehicles (void)
{
	int wait;
	bool active, traveler_flag;

	Travel_Plan_Data *plan_ptr;
	Msim_Vehicle *vehicle_ptr;

	//---- update the vehicle positions ----

	active = traveler_flag = false;

	for (plan_ptr = network_traffic.First (); plan_ptr != NULL; plan_ptr = network_traffic.Next ()) {

		//---- check if the vehicle is still on the network ----

		vehicle_ptr = vehicle_data [plan_ptr->Vehicle ()];
		if (vehicle_ptr == NULL) continue;

		if (vehicle_ptr->Status () >= 4) {
			Park_Vehicle (vehicle_ptr, (vehicle_ptr->Status () > 4));
			network_traffic.Remove ();
			vehicle_ptr->Travel_Plan (NULL);
			continue;
		}
		active = true;
		if (plan_ptr->Mode () != DRIVER_MODE) traveler_flag = true;

		//---- check for traveler output ----

		if (vehicle_ptr->Move_Flag () || vehicle_ptr->Waiting () == 0) {
			int traveler;

			if (vehicle_ptr->Sub_Type () == 0) {
				traveler = vehicle_ptr->Driver ();
			} else {
				traveler = vehicle_ptr->Vehicle ();
			}
			if (traveler_output.In_Range (traveler, step, vehicle_ptr->To_Dir ())) {
				int offset, to_offset, to_speed;

				offset = vehicle_ptr->Cell () * cell_size;
				to_offset = vehicle_ptr->To_Cell () * cell_size;
				to_speed = vehicle_ptr->To_Speed ();

				if (speed_method) {
					offset -= vehicle_ptr->Offset ();
					to_offset -= vehicle_ptr->To_Offset ();
				} else {
					to_speed *= cell_size;
				}
				traveler_output.Traveler (traveler);
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
		}

		//---- check for snapshot output ----

		if (snapshot_output.In_Range (step, vehicle_ptr->To_Dir ())) {
			int offset, speed, accel;

			offset = vehicle_ptr->To_Cell () * cell_size;
			speed = vehicle_ptr->To_Speed ();
			accel = speed - vehicle_ptr->Speed ();

			if (speed_method) {
				offset -= vehicle_ptr->To_Offset ();
			} else {
				speed *= cell_size;
				accel *= cell_size;
			}
			snapshot_output.Vehicle (vehicle_ptr->Vehicle ());
			snapshot_output.Time (step);
			snapshot_output.Dir (vehicle_ptr->To_Dir ());
			snapshot_output.Lane (vehicle_ptr->To_Lane ());
			snapshot_output.Offset (UnRound (offset));
			snapshot_output.Speed (UnRound (speed));
			snapshot_output.Accel (UnRound (accel));
			snapshot_output.VehType (veh_type_data [vehicle_ptr->Type ()]->Type ());
			snapshot_output.Driver (vehicle_ptr->Driver ());
			snapshot_output.Passengers (vehicle_ptr->Passengers ());

			snapshot_output.Output ();
		}

		//---- check for summarize output ----

		if (summary_output.In_Range (step, vehicle_ptr->To_Dir (), veh_type_map [vehicle_ptr->Type ()]) 
			&& vehicle_ptr->Leg () > 0 && vehicle_ptr->To_Leg () > 0) {

			int offset, to_offset, speed, to_speed, connect, to_connect, pce;
			Travel_Leg_Data *leg_ptr;

			offset = vehicle_ptr->Cell () * cell_size;
			to_offset = vehicle_ptr->To_Cell () * cell_size;
			speed = vehicle_ptr->Speed ();
			to_speed = vehicle_ptr->To_Speed ();

			if (pce_cells == 1) {
				pce = vehicle_ptr->Num_Cells ();
			} else {
				pce = (vehicle_ptr->Num_Cells () + pce_cells / 2) / pce_cells;
			}
			if (pce < 1) pce = 1;

			leg_ptr = plan_ptr->Travel_Leg (vehicle_ptr->Leg ());
			if (leg_ptr == NULL) continue;

			connect = leg_ptr->Connect ();

			leg_ptr = plan_ptr->Travel_Leg (vehicle_ptr->To_Leg ());
			if (leg_ptr == NULL) continue;

			to_connect = leg_ptr->Connect ();

			if (speed_method) {
				offset -= vehicle_ptr->Offset ();
				to_offset -= vehicle_ptr->To_Offset ();
			} else {
				speed *= cell_size;
				to_speed *= cell_size;
			}
			summary_output.From_Dir (vehicle_ptr->Dir ());
			summary_output.From_Offset (offset);
			summary_output.From_Speed (speed);
			summary_output.From_Connect (connect);
			summary_output.To_Dir (vehicle_ptr->To_Dir ());
			summary_output.To_Offset (to_offset);
			summary_output.To_Speed (to_speed);
			summary_output.To_Connect (to_connect);
			summary_output.Vehicle_PCE (pce);
			summary_output.Occupancy (vehicle_ptr->Passengers () + 1);

			summary_output.Summarize (step);
		}

		//---- check for speed bin output ----

		if (speed_bin_output.In_Range (step, vehicle_ptr->To_Dir (), vehicle_ptr->Type ())) {
			speed_bin_output.Dir (vehicle_ptr->To_Dir ());
			speed_bin_output.Offset (vehicle_ptr->To_Cell () * cell_size - half_cell);
			speed_bin_output.Veh_Type (vehicle_ptr->Type ());
			speed_bin_output.Speed (vehicle_ptr->To_Speed ());

			speed_bin_output.Summarize (step);
		}

		//---- check if a lane change is needed ----

		if (vehicle_ptr->Lane_Change () > 0) {
			if (!lane_changes [vehicle_ptr->Lane_Change () - 1].Add (plan_ptr->Vehicle ())) {
				Error ("Insufficient Memory for Lane Changing Queue");
			}
		}
		
		//---- check for movement ----

		if (vehicle_ptr->Move_Flag () || vehicle_ptr->Hold () > 0) {

			if (vehicle_ptr->Hold () > 0) {
				vehicle_ptr->Hold_Down ();
				vehicle_ptr->Hold_Flag (true);
			} else {
				vehicle_ptr->Hold_Flag (false);
			}
			vehicle_ptr->Waiting (0);
			if (vehicle_ptr->Sub_Type () == 0) vehicle_ptr->Priority (false);
			vehicle_ptr->Move_Flag (false);
			vehicle_ptr->Dir (vehicle_ptr->To_Dir ());
			vehicle_ptr->Cell (vehicle_ptr->To_Cell ());
			vehicle_ptr->Lane (vehicle_ptr->To_Lane ());
			vehicle_ptr->Leg (vehicle_ptr->To_Leg ());

		} else {

			wait = vehicle_ptr->Waiting () + 1;
		
			//---- check the min wait time ----

			if (!vehicle_ptr->Priority () && wait == min_wait_time) {
				vehicle_ptr->Priority (true);

				if (!priority_queue.Add (plan_ptr->Vehicle ())) {
					Error ("Insufficient Memory for Priority Queue");
				}
			}
			vehicle_ptr->Waiting (wait);
		}
		vehicle_ptr->Status (0);
		vehicle_ptr->Speed (vehicle_ptr->To_Speed ());
		vehicle_ptr->Offset (vehicle_ptr->To_Offset ());
	}
	if (active) {
		grid_data.Switch_Grids ();
	}
	return (traveler_flag);
}
