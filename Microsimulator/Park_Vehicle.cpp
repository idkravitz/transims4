//*********************************************************
//	Park_Vehicle.cpp - Move Vehicle to a Parking Lot
//*********************************************************

#include "Microsimulator.hpp"

//---------------------------------------------------------
//	Park_Vehicle
//---------------------------------------------------------

void Microsimulator::Park_Vehicle (Msim_Vehicle *vehicle_ptr, bool error_flag)
{
	int run, stop, traveler, dir;
	double time;

	Traveler_Data *traveler_ptr;
	Travel_Plan_Data *plan_ptr, *transit_ptr;
	Rider_Data *rider_ptr;
	Grid_Data *grid_ptr;

	plan_ptr = vehicle_ptr->Travel_Plan ();
	if (plan_ptr == NULL) return;

	//---- report the parking move ----

	if (vehicle_ptr->Dir () > 0 && !error_flag) {

		//---- check for traveler output ----

		if (traveler_output.In_Range (vehicle_ptr->Driver (), step, vehicle_ptr->Dir ())) {
			int offset, to_offset, to_speed;
			Travel_Leg_Data *leg_ptr;

			offset = vehicle_ptr->Cell () * cell_size;

			leg_ptr = plan_ptr->Travel_Leg (plan_ptr->Num_Travel_Legs ());
			if (leg_ptr != NULL) {
				to_speed = leg_ptr->Max_Speed ();
				to_offset = plan_ptr->End_Cell () * cell_size - plan_ptr->End_Offset ();

				if (speed_method) {
					offset -= vehicle_ptr->Offset ();
				} else {
					to_speed *= cell_size;
				}
				traveler_output.Traveler (vehicle_ptr->Driver ());
				traveler_output.Vehicle (vehicle_ptr->Vehicle ());
				traveler_output.Time (step);
				traveler_output.From_Dir (vehicle_ptr->Dir ());
				traveler_output.From_Offset (offset);
				traveler_output.To_Dir (leg_ptr->Dir ());
				traveler_output.To_Offset (to_offset);
				traveler_output.Speed (to_speed);
				traveler_output.Lane (0);

				traveler_output.Output ();
			}
		}

		//---- summarize link delay ----

		if (summary_output.In_Range (step, vehicle_ptr->Dir (), veh_type_map [vehicle_ptr->Type ()])) {
			int offset, to_offset, speed, to_speed, connect, pce;
			Travel_Leg_Data *leg_ptr;

			if (vehicle_ptr->Leg () > 0) {
				leg_ptr = plan_ptr->Travel_Leg (vehicle_ptr->Leg ());
				if (leg_ptr != NULL) {
					connect = leg_ptr->Connect ();
				} else {
					connect = 0;
				}
			} else {
				connect = 0;
			}
			offset = vehicle_ptr->Cell () * cell_size;
			speed = vehicle_ptr->Speed ();

			if (pce_cells == 1) {
				pce = vehicle_ptr->Num_Cells ();
			} else {
				pce = (vehicle_ptr->Num_Cells () + pce_cells / 2) / pce_cells;
			}
			if (pce < 1) pce = 1;

			leg_ptr = plan_ptr->Travel_Leg (plan_ptr->Num_Travel_Legs ());
			if (leg_ptr != NULL) {
				to_speed = leg_ptr->Max_Speed ();
				to_offset = plan_ptr->End_Cell () * cell_size - plan_ptr->End_Offset ();

				if (speed_method) {
					offset -= vehicle_ptr->Offset ();
				} else {
					speed *= cell_size;
					to_speed *= cell_size;
				}
				summary_output.From_Dir (vehicle_ptr->Dir ());
				summary_output.From_Offset (offset);
				summary_output.From_Speed (speed);
				summary_output.From_Connect (connect);
				summary_output.To_Dir (leg_ptr->Dir ());
				summary_output.To_Offset (to_offset);
				summary_output.To_Speed (to_speed);
				summary_output.To_Connect (-1);
				summary_output.Vehicle_PCE (pce);
				summary_output.Occupancy (vehicle_ptr->Passengers () + 1);

				summary_output.Summarize (step);
			}
		}

		//---- end time event ----

		if (event_output.In_Range (plan_ptr->End_Time (), END_EVENT, plan_ptr->Mode ())) {
			int offset = plan_ptr->End_Offset ();

			Link_Data *link_ptr;

			offset = plan_ptr->End_Cell () * cell_size - plan_ptr->End_Offset ();

			grid_ptr = grid_data [vehicle_ptr->Dir ()];
			link_ptr = link_data [grid_ptr->Link ()];

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
			event_output.Event (END_EVENT);
			event_output.Schedule (plan_ptr->End_Time ());
			event_output.Actual (step);
			if (grid_ptr->Dir () > 0) {
				event_output.Link (-link_ptr->Link ());
			} else {
				event_output.Link (link_ptr->Link ());
			}
			event_output.Offset (Resolve (offset));

			event_output.Output ();
		}

		//---- run time event ----

		if (event_output.In_Range (plan_ptr->End_Time (), DURATION_EVENT, plan_ptr->Mode ())) {
			int offset = plan_ptr->End_Offset ();

			Link_Data *link_ptr;

			offset = plan_ptr->End_Cell () * cell_size - plan_ptr->End_Offset ();

			grid_ptr = grid_data [vehicle_ptr->Dir ()];
			link_ptr = link_data [grid_ptr->Link ()];

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
			event_output.Event (DURATION_EVENT);
			event_output.Schedule (plan_ptr->End_Time () - plan_ptr->Start_Time ());
			event_output.Actual (step - plan_ptr->Start_Time ());
			if (grid_ptr->Dir () > 0) {
				event_output.Link (-link_ptr->Link ());
			} else {
				event_output.Link (link_ptr->Link ());
			}
			event_output.Offset (Resolve (offset));

			event_output.Output ();
		}
	}

	//---- assign the vehicle to the parking lot ----

	dir = vehicle_ptr->Dir ();

	vehicle_ptr->Driver (0);
	vehicle_ptr->Dir (0);
	vehicle_ptr->Travel_Plan (NULL);
	vehicle_ptr->Location (plan_ptr->End_Index ());

	num_vehicles--;

	//---- set the traveler status flag ----

	if (plan_ptr->Mode () == AUTO_MODE) {
		traveler_ptr = traveler_data [plan_ptr->Traveler ()];
		traveler_ptr->Active (false);
		traveler_ptr->End_Time (step);

		if (vehicle_ptr->Status () == 4) {
			tot_complete++;
			tot_hours += step - plan_ptr->Start_Time ();
		}

	} else if (plan_ptr->Mode () == DRIVER_MODE) {
		bool external_stop;
		Stop_Data *stop_ptr;
		Veh_Type_Data *veh_type_ptr;

		veh_type_ptr = veh_type_data [vehicle_ptr->Type ()];
		rider_ptr = rider_data [plan_ptr->Traveler ()];	
		run = rider_data.Run (vehicle_ptr->Vehicle ());
		stop = rider_ptr->Stops ();
		time = Round (step);
				
		stop_ptr = stop_data [stop];
		external_stop = (stop_ptr->Type () == EXTLOAD);

		//---- let riders off ---

		for (traveler = rider_ptr->Off_Rider (run, stop); traveler; traveler = traveler_ptr->Next_Rider ()) {
			traveler_ptr = traveler_data [traveler];
			
			if (!external_stop) {
				time += veh_type_ptr->Unloading ();
			}
			traveler_ptr->End_Time (Resolve ((int) (time + 0.5)));
			traveler_ptr->Active (false);

			rider_ptr->Alight (run, stop, 1);
			nalight++;

			//---- end time event ----

			if (dir > 0 && !error_flag) {
				transit_ptr = traveler_ptr->Travel_Plan ();

				if (event_output.In_Range (transit_ptr->End_Time (), END_EVENT, transit_ptr->Mode ())) {
					int offset = transit_ptr->End_Offset ();

					grid_ptr = grid_data [dir];
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
					event_output.Event (END_EVENT);
					event_output.Schedule (transit_ptr->End_Time ());
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
		}
		rider_ptr->Off_Rider (run, stop, 0);
		rider_ptr->Time (run, stop, Resolve ((int) (time + 0.5)));
		vehicle_ptr->Passengers (rider_ptr->Load (run));
	}
}
