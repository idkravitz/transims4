//*********************************************************
//	Transit_Plan.cpp - create the route legs
//*********************************************************

#include "Microsimulator.hpp"

//---------------------------------------------------------
//	Transit_Plan
//---------------------------------------------------------

void Microsimulator::Transit_Plan (void)
{
	int i, j, k, k1, dir, in_link_dir, out_link_dir, offset, num_legs, stop, last_veh;
	char transit_veh [20], buffer [10];
	bool exit_flag = false;

	Driver_Data *driver_ptr;
	Travel_Plan_Data plan_rec, *plan_ptr;
	Travel_Leg_Data leg_rec, *leg_ptr;
	Link_Data *link_ptr;
	Grid_Data *grid_ptr;
	Connect_Data *connect_ptr;
	Control_Data *control_ptr;
	Msim_Vehicle *vehicle_ptr;
	Veh_Type_Data *veh_type_ptr;
	Stop_Data *stop_ptr;
	Rider_Data *rider_ptr;

	in_link_dir = 0;

	//---- convert stop offsets to cells ----

	for (stop_ptr = stop_data.First (); stop_ptr != NULL; stop_ptr = stop_data.Next ()) {
		link_ptr = link_data [stop_ptr->Link ()];
		offset = stop_ptr->Offset ();

		if (stop_ptr->Dir () == 1) {
			dir = link_ptr->BA_Dir ();
			offset = link_ptr->Length () - offset;
		} else {
			dir = link_ptr->AB_Dir ();
		}
		if (dir == 0) {
			Warning ("Link %d is Not Available in the %s Direction", link_ptr->Link (), 
				((stop_ptr->Dir () == 0) ? "AB" : "BA"));
			exit_flag = true;
			continue;
		}
		grid_ptr = grid_data [dir];

		offset = MIN (MAX (((offset + half_cell) / cell_size), grid_ptr->Cell_In ()), grid_ptr->Cell_Out ());

		stop_ptr->Cell (offset);
	}

	//---- initialize the vehicle ID ----

	vehicle_ptr = vehicle_data.Last_Key ();

	if (vehicle_ptr == NULL) {
		last_veh = 0;
	} else {
		last_veh = vehicle_ptr->Vehicle ();
	}
	transit_id = rider_data.Vehicle_ID (last_veh);

	str_fmt (transit_veh, sizeof (transit_veh), "%d", transit_id);

	i = (int) strlen (transit_veh);

	str_fmt (buffer, sizeof (buffer), "%d", rider_data.Max_Runs ());

	j = (int) strlen (buffer);
	j = i-j;

	for (k=j; k < i; k++) {
		transit_veh [k] = '#';
	}
	rider_ptr = rider_data.Last_Key ();

	str_fmt (buffer, sizeof (buffer), "%d", rider_ptr->Route ());

	k1 = (int) strlen (buffer);

	for (k=j-k1; k < j; k++) {
		transit_veh [k] = 'x';
	}
	Print (2, "Transit Vehicle ID = %s", transit_veh);
	Print (1, "Number of Transit Vehicles = %d", rider_data.Num_Runs ());

	//---- process each transit route ----

	leg_rec.Stop_Number (0);

	for (driver_ptr = driver_data.First (); driver_ptr; driver_ptr = driver_data.Next ()) {

		//---- get the route record ----

		rider_ptr = rider_data.Get (driver_ptr->Route ());

		if (rider_ptr == NULL) {
			Warning ("Transit Route was Not Found for Driver Route %d", driver_ptr->Route ());
			exit_flag = true;
			continue;
		}

		//---- convert the vehicle type ----

		veh_type_ptr = veh_type_data.Get (driver_ptr->Type (), driver_ptr->Sub_Type ());

		if (veh_type_ptr == NULL) {
			Warning ("Transit Vehicle Type %d-%d was Not Found", driver_ptr->Type (), driver_ptr->Sub_Type ());
			exit_flag = true;
			continue;
		}
		driver_ptr->Type (veh_type_data.Record_Index ());
		driver_ptr->Sub_Type (1);
		
		//---- create the transit vehicles ----

		stop = rider_ptr->Stop (1);

		for (i=1; i <= rider_ptr->Runs (); i++) {
			vehicle_ptr = vehicle_data.New_Record (true);

			if (vehicle_ptr == NULL) goto mem_error;

			vehicle_ptr->Vehicle (rider_data.Vehicle_ID (rider_ptr->Route (), i));

			//---- set vehicle data ----

			vehicle_ptr->Type (driver_ptr->Type ());
			vehicle_ptr->Sub_Type (1);
			vehicle_ptr->Location (stop);

			if (!vehicle_ptr->Num_Cells (veh_type_ptr->Length ())) goto mem_error;

			if (!vehicle_data.Add ()) goto mem_error;

			//---- convert the schedule times ----

			for (j=1; j <= rider_ptr->Stops (); j++) {
				rider_ptr->Schedule (i, j, time_step.Step (Resolve (rider_ptr->Schedule (i, j))));
			}
		}

		//---- create a new travel plan ----

		if (!route_plan.Add (&plan_rec)) goto mem_error;

		if (!exit_flag && route_plan.Record_Index () != rider_ptr->Driver ()) {
			Warning ("Route Plans are Out of Order (%d vs %d)", route_plan.Record_Index (), rider_ptr->Driver ());
			exit_flag = true;
			continue;
		}
		plan_ptr = route_plan.Record ();

		plan_ptr->Mode (DRIVER_MODE);
		plan_ptr->Leg (0);
		plan_ptr->Traveler (rider_data.Record_Index ());
		plan_ptr->Vehicle (vehicle_data.Record_Index ());

		num_legs = driver_ptr->Links ();

		//---- find the start index ----

		plan_ptr->Start_Type (STOP_ID);
		plan_ptr->Start_Index (stop);

		stop_ptr = stop_data [stop];

		for (i=1; i <= num_legs; i++) {
			if (stop_ptr->Link_Dir () == driver_ptr->Link_Dir (i)) break;
		}
		if (i > num_legs) {
			Warning ("Stop %d was Not Found on Driver Path for Route %d", stop_ptr->ID (), driver_ptr->Route ());
			exit_flag = true;
			continue;
		}
		k1 = i;

		//---- find the end index ----

		stop = rider_ptr->Stop (rider_ptr->Stops ());

		plan_ptr->End_Type (STOP_ID);
		plan_ptr->End_Index (stop);

		stop_ptr = stop_data [stop];

		for (i=num_legs; i >= k1; i--) {
			if (stop_ptr->Link_Dir () == driver_ptr->Link_Dir (i)) break;
		}
		if (i < k1) {
			Warning ("Stop %d was Not Found on Driver Path for Route %d", stop_ptr->ID (), driver_ptr->Route ());
			exit_flag = true;
			continue;
		}
		num_legs = i - k1 + 1;

		if (!plan_ptr->Num_Travel_Legs (num_legs)) goto mem_error;

		//---- create travel legs ----

		for (i=1, k=k1; i <= num_legs; i++, k++) {

			link_ptr = link_data [driver_ptr->Link (k)];

			if (driver_ptr->Dir (k) == 0) {
				dir = link_ptr->AB_Dir ();
			} else {
				dir = link_ptr->BA_Dir ();
			}
			if (dir == 0) {
				Warning ("Link %d is Not Available in the %s Direction", 
					link_ptr->Link (), ((driver_ptr->Dir (k) == 0) ? "AB" : "BA"));
				exit_flag = true;
				continue;
			}
			grid_ptr = grid_data [dir];

			//---- get the connection to the next link ----

			out_link_dir = grid_ptr->Link_Dir ();

			if (i > 1) {
				connect_ptr = connect_data.Get (in_link_dir, out_link_dir);

				if (connect_ptr == NULL) {
					link_ptr = link_data [in_link_dir >> 1];

					if ((in_link_dir & 1) == 0) {
						j = link_ptr->Bnode ();
					} else {
						j = link_ptr->Anode ();
					}
					in_link_dir = link_ptr->Link ();

					link_ptr = link_data [out_link_dir >> 1];

					Warning ("Transit Route %d Connection between Links %d-%d at Node %d", 
							driver_ptr->Route (), in_link_dir, link_ptr->Link (), 
							node_data [j]->Node ());

					in_link_dir = out_link_dir;
					exit_flag = true;
					continue;
				}
				leg_rec.Out_Lane_Low (connect_ptr->In_Lane_Low ());
				leg_rec.Out_Lane_High (connect_ptr->In_Lane_High ());

				leg_rec.Out_Best_Low (connect_ptr->In_Thru_Low ());
				leg_rec.Out_Best_High (connect_ptr->In_Thru_High ());

				if (connect_ptr->Speed () > 0) {
					leg_rec.Max_Speed (connect_ptr->Speed ());
				} else {
					leg_rec.Max_Speed (grid_ptr->Speed ());
				}

				//---- get the traffic control ----

				control_ptr = control_data.Get (in_link_dir, out_link_dir);

				if (control_ptr == NULL) {
					leg_rec.Control (0);
				} else {
					leg_rec.Control (control_data.Record_Index ());
				}

				//---- save the approach leg ----

				if (!plan_ptr->Travel_Leg (&leg_rec)) goto mem_error;

				leg_rec.In_Lane_Low (connect_ptr->Out_Lane_Low ());
				leg_rec.In_Lane_High (connect_ptr->Out_Lane_High ());
			} else {
				leg_rec.Dir (dir);

				Access_Lanes (&leg_rec, plan_ptr, true);

				leg_rec.Out_Lane_Low (0);
				leg_rec.Out_Lane_High (0);
			}
			in_link_dir = out_link_dir;

			leg_rec.Dir (dir);
			leg_rec.Max_Speed (grid_ptr->Speed ());
		}
		Access_Lanes (&leg_rec, plan_ptr, false);

		leg_rec.Control (0);

		if (!plan_ptr->Travel_Leg (&leg_rec)) goto mem_error;
	
		//---- set the stop number ----

		for (i=j=1, k=k1; i <= rider_ptr->Stops (); i++) {
			stop_ptr = stop_data [rider_ptr->Stop (i)];

			for (; j <= num_legs; j++, k++) {
				if (stop_ptr->Link_Dir () == driver_ptr->Link_Dir (k)) break;
			}
			if (j > num_legs) {
				Warning ("Route %d Stops and Driver Links are Incompatible", rider_ptr->Route ());
				exit_flag = true;
				break;
			}

			//---- add the stop to the route leg ----

			leg_ptr = plan_ptr->Travel_Leg (j);

			if (leg_ptr == NULL) {
				if (exit_flag) continue;
				Error ("Transit Leg Pointer is Null");
			}
			if (leg_ptr->Stop_Number () == 0) {
				leg_ptr->Stop_Number (i);
			}
		}

		//---- determine the best lanes ----

		Best_Lanes (plan_ptr);
	}
	if (exit_flag) {
		Error ("Transit Route Coding Errors");
	}
	return;

mem_error:
	Error ("Insufficient Memory for Transit Data");
}
