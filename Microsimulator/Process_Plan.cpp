//*********************************************************
//	Process_Plan.cpp - Process a Plan Record
//*********************************************************

#include "Microsimulator.hpp"

//---------------------------------------------------------
//	Process_Plan
//---------------------------------------------------------

void Microsimulator::Process_Plan (void)
{
	int i, *rec, num_rec, an, bn, dir, traveler, first_dir, in_link_dir, out_link_dir, mode, index, id, board;
	bool load_flag, flag;

	AB_Data *ab_ptr;
	Grid_Data *grid_ptr;
	Link_Data *link_ptr;
	Parking_Data *parking_ptr;
	Msim_Vehicle *vehicle_ptr;
	Traveler_Data *traveler_ptr;
	Connect_Data *connect_ptr;
	Control_Data *control_ptr;
	Travel_Leg_Data leg_rec;
	Travel_Plan_Data *plan_ptr;
	Rider_Data *rider_ptr;

	index = in_link_dir = out_link_dir = 0;

	//---- deal with non-simulation legs ----

	mode = plan_file.Mode ();

	if (mode == ACTIVITY_MODE || mode == OTHER_MODE || mode == MAGIC_MOVE) return;

	if (plan_file.Driver_Flag () == 1) {
		if (mode == TRANSIT_MODE) return;
	} else {
		if (mode == AUTO_MODE) mode = CARPOOL_MODE;
	}
	if (!transit_flag && (mode == TRANSIT_MODE || mode == WALK_MODE)) return;

	//---- find the traveler ----

	traveler = plan_file.Traveler ();
	load_flag = true;

	traveler_ptr = traveler_data.Get (traveler);

	if (traveler_ptr == NULL) {
		Traveler_Data traveler_rec;

		traveler_rec.Traveler (traveler);

		if (!traveler_data.Add (&traveler_rec)) {
			Error ("Adding Traveler Data");
		}
		traveler_ptr = traveler_data.Record ();
	} else {
		if (traveler_ptr->Active () || traveler_ptr->End_Time () > step) {
			load_flag = false;
		}
	}

	//---- create a new plan record ----

	plan_ptr = new Travel_Plan_Data ();

	if (plan_ptr == NULL) {
		Error ("Insufficient Memory to Create a New Plan Record");
	}
	plan_ptr->Traveler (traveler_data.Record_Index ());
	plan_ptr->Trip (plan_file.Trip ());
	plan_ptr->Leg (plan_file.Leg ());
	plan_ptr->Mode (mode);
	plan_ptr->Start_Time (time_step.Step (plan_file.Time ()));
	plan_ptr->End_Time (time_step.Step (plan_file.Time () + plan_file.Duration ()));

	//---- convert the trip ends ----

	plan_ptr->Start_Type (plan_file.Start_Type ());
	plan_ptr->End_Type (plan_file.End_Type ());

	id = plan_file.Start_ID ();

	switch (plan_ptr->Start_Type ()) {
		case PARKING_ID:
			index = parking_data.Get_Index (id);
			if (index == 0) goto parking_error;
			break;
		case STOP_ID:
			index = stop_data.Get_Index (id);
			if (index == 0) goto stop_error;
			break;
		case LOCATION_ID:
			index = location_data.Get_Index (id);
			if (index == 0) goto location_error;
			break;
		default:
			Error ("Traveler %d Unrecognized Start Type %d", traveler, plan_ptr->Start_Type ());
			break;
	}
	plan_ptr->Start_Index (index);

	id = plan_file.End_ID ();

	switch (plan_ptr->End_Type ()) {
		case PARKING_ID:
			index = parking_data.Get_Index (id);
			if (index == 0) goto parking_error;
			break;
		case STOP_ID:
			index = stop_data.Get_Index (id);
			if (index == 0) goto stop_error;
			break;
		case LOCATION_ID:
			index = location_data.Get_Index (id);
			if (index == 0) goto location_error;
			break;
		default:
			Error ("Traveler %d Unrecognized End Type %d", traveler, plan_ptr->End_Type ());
			break;
	}
	plan_ptr->End_Index (index);

	//---- get the vehicle ----
	
	if (mode == AUTO_MODE) {
		if (plan_ptr->Start_Type () != PARKING_ID || plan_ptr->End_Type () != PARKING_ID) {
			Print (1, "Traveler %d Leg %d has an Drive Mode without two Parking Lots",
				traveler, plan_file.Leg ());
			delete plan_ptr;
			return;
		}
		vehicle_ptr = vehicle_data.Get (plan_file.Vehicle ());

		if (vehicle_ptr == NULL) {
			Problem_Plan (Set_Problem (ACCESS_PROBLEM), plan_ptr, 0, 0, 0);

			if (error_flag) {
				Print (1, "Traveler %d Vehicle %d was Not Found in the Vehicle File", traveler, plan_file.Vehicle ());
			}
			delete plan_ptr;
			return;
		}
		plan_ptr->Vehicle (vehicle_data.Record_Index ());

		if (vehicle_ptr->Driver () != 0) {
			Problem_Plan (Set_Problem (TIME_PROBLEM), plan_ptr, 0, 0, 0);

			if (error_flag) {
				if (vehicle_ptr->Driver () != traveler) {
					Print (1, "Traveler %d is waiting for Traveler %d to park Vehicle %d", traveler, 
						vehicle_ptr->Driver (), vehicle_ptr->Vehicle ());
				} else {
					Print (1, "Traveler %d has not completed the previous trip", traveler);
				}
			}
			load_flag = false;
		} else {
			if (!vehicle_ptr->Initialize ()) {
				Error ("Insufficient Memory for Vehicle Position Data");
			}
			vehicle_ptr->Dir (0);
			vehicle_ptr->Status (0);
		}

		//---- convert the path to directional links ----

		rec = plan_file.Path (&num_rec);
		first_dir = 0;

		//---- node-based paths ----

		if (node_flag) {
			if (!plan_ptr->Num_Travel_Legs (num_rec+1)) goto leg_error;

			//---- get the first link ----

			parking_ptr = parking_data [plan_ptr->Start_Index ()];
				
			link_ptr = link_data [parking_ptr->Link ()];

			//---- parking lots on the same link ----

			if (num_rec == 0) {
				dir = parking_ptr->Offset ();

				parking_ptr = parking_data [plan_ptr->End_Index ()];

				if (parking_ptr->Link () != link_data.Record_Index ()) {
					dir = 0;
				} else if (parking_ptr->Offset () > dir) {
					dir = link_ptr->AB_Dir ();
				} else if (parking_ptr->Offset () < dir) {
					dir = link_ptr->BA_Dir ();
				} else {
					dir = MAX (link_ptr->AB_Dir (), link_ptr->BA_Dir ());
				}
				if (dir == 0) {
					Problem_Plan (Set_Problem (PATH_PROBLEM), plan_ptr, dir, 0, 0);

					if (error_flag) {
						Print (1, "Traveler %d No Path between Parking Lots %d and %d", 
							traveler, plan_file.Start_ID (), parking_ptr->ID ());
					}
					delete plan_ptr;
					return;
				}
				first_dir = dir;

				leg_rec.Dir (dir);
				leg_rec.Max_Speed (0);
				leg_rec.Control (0);

				Access_Lanes (&leg_rec, plan_ptr, true);
				Access_Lanes (&leg_rec, plan_ptr, false);

				if (!plan_ptr->Travel_Leg (&leg_rec)) goto leg_error;

			} else {

				//---- identify the parking link ----

				an = *rec++;

				i = node_data.Get_Index (an);

				if (link_ptr->Anode () == i) {
					dir = link_ptr->BA_Dir ();
				} else if (link_ptr->Bnode () == i) {
					dir = link_ptr->AB_Dir ();
				} else {
					dir = 0;
				}
				if (dir == 0) {
					Problem_Plan (Set_Problem (ACCESS_PROBLEM), plan_ptr, dir, 0, 0);

					if (error_flag) {
						Print (1, "Traveler %d No Access from Parking Lot %d to Node %d", traveler, parking_ptr->ID (), an);
					}
					delete plan_ptr;
					return;
				}
				grid_ptr = grid_data [dir];

				in_link_dir = grid_ptr->Link_Dir ();

				first_dir = dir;

				leg_rec.Dir (dir);

				Access_Lanes (&leg_rec, plan_ptr, true);

				leg_rec.Out_Lane_Low (0);
				leg_rec.Out_Lane_High (0);
				leg_rec.Max_Speed (grid_ptr->Speed ());

				//---- process each node on the path ----

				for (i=1; i < num_rec; i++, rec++) {
					bn = *rec;

					ab_ptr = ab_key.Get (an, bn);

					if (ab_ptr == NULL) {
						Problem_Plan (Set_Problem (PATH_PROBLEM), plan_ptr, 0, 0, 0);

						if (error_flag) {
							Print (1, "Traveler %d Nodes %d and %d were Not Found in the Link File", traveler, an, bn);
						}
						delete plan_ptr;
						return;
					}

					//---- get the connection to the next link ----

					grid_ptr = grid_data [ab_ptr->dir];

					out_link_dir = grid_ptr->Link_Dir ();

					connect_ptr = connect_data.Get (in_link_dir, out_link_dir);

					if (connect_ptr == NULL) goto connect_problem;

					leg_rec.Out_Lane_Low (connect_ptr->In_Lane_Low ());
					leg_rec.Out_Lane_High (connect_ptr->In_Lane_High ());

					leg_rec.Out_Best_Low (connect_ptr->In_Thru_Low ());
					leg_rec.Out_Best_High (connect_ptr->In_Thru_High ());

					if (connect_ptr->Speed () > 0) {
						leg_rec.Max_Speed (connect_ptr->Speed ());
					} else {
						leg_rec.Max_Speed (grid_ptr->Speed ());
					}
					leg_rec.Connect (connect_data.Record_Index ());

					//---- get the traffic control ----

					control_ptr = control_data.Get (in_link_dir, out_link_dir);

					if (control_ptr == NULL) {
						leg_rec.Control (0);
					} else {
						leg_rec.Control (control_data.Record_Index ());
					}

					//---- save the approach leg ----

					if (!plan_ptr->Travel_Leg (&leg_rec)) goto leg_error;

					//---- initialize the departure leg ----

					leg_rec.Dir (ab_ptr->dir);
					leg_rec.In_Lane_Low (connect_ptr->Out_Lane_Low ());
					leg_rec.In_Lane_High (connect_ptr->Out_Lane_High ());
					leg_rec.Max_Speed (grid_ptr->Speed ());

					in_link_dir = out_link_dir;
					an = bn;
				}

				//---- get the last link ----

				parking_ptr = parking_data [plan_ptr->End_Index ()];

				link_ptr = link_data [parking_ptr->Link ()];

				i = node_data.Get_Index (an);

				if (link_ptr->Anode () == i) {
					dir = link_ptr->AB_Dir ();
				} else if (link_ptr->Bnode () == i) {
					dir = link_ptr->BA_Dir ();
				} else {
					dir = 0;
				}
				if (dir == 0) {
					Problem_Plan (Set_Problem (ACCESS_PROBLEM), plan_ptr, dir, 0, 0);

					if (error_flag) {
						Print (1, "Traveler %d No Access from Node %d to Parking Lot %d", traveler, an, parking_ptr->ID ());
					}
					delete plan_ptr;
					return;
				}

				//---- get the connection to the parking link ----

				grid_ptr = grid_data [dir];

				out_link_dir = grid_ptr->Link_Dir ();

				connect_ptr = connect_data.Get (in_link_dir, out_link_dir);

				if (connect_ptr == NULL) goto connect_problem;

				leg_rec.Out_Lane_Low (connect_ptr->In_Lane_Low ());
				leg_rec.Out_Lane_High (connect_ptr->In_Lane_High ());

				leg_rec.Out_Best_Low (connect_ptr->In_Thru_Low ());
				leg_rec.Out_Best_High (connect_ptr->In_Thru_High ());

				if (connect_ptr->Speed () > 0) {
					leg_rec.Max_Speed (connect_ptr->Speed ());
				} else {
					leg_rec.Max_Speed (grid_ptr->Speed ());
				}
				leg_rec.Connect (connect_data.Record_Index ());

				//---- get the traffic control ----

				control_ptr = control_data.Get (in_link_dir, out_link_dir);

				if (control_ptr == NULL) {
					leg_rec.Control (0);
				} else {
					leg_rec.Control (control_data.Record_Index ());
				}

				//---- save the approach leg ----

				if (!plan_ptr->Travel_Leg (&leg_rec)) goto leg_error;

				//---- save the parking leg ----

				leg_rec.Dir (dir);
				leg_rec.In_Lane_Low (connect_ptr->Out_Lane_Low ());
				leg_rec.In_Lane_High (connect_ptr->Out_Lane_High ());
				leg_rec.Max_Speed (grid_ptr->Speed ());
				leg_rec.Control (0);
				leg_rec.Connect (0);

				Access_Lanes (&leg_rec, plan_ptr, false);

				if (!plan_ptr->Travel_Leg (&leg_rec)) goto leg_error;
			}

		} else {	//---- link-based paths ----

			if (!plan_ptr->Num_Travel_Legs (num_rec)) goto leg_error;

			for (i=0; i < num_rec; i++, rec++) {
				dir = abs (*rec);

				link_ptr = link_data.Get (dir);

				if (link_ptr == NULL) {
					Problem_Plan (Set_Problem (PATH_PROBLEM), plan_ptr, 0, 0, 0);

					if (error_flag) {
						Print (1, "Traveler %d Link %d was Not Found in the Link File", traveler, dir);
					}
					delete plan_ptr;
					return;
				}
				if (*rec > 0) {
					dir = link_ptr->AB_Dir ();
				} else {
					dir = link_ptr->BA_Dir ();
				}
				if (dir == 0) {
					Problem_Plan (Set_Problem (PATH_PROBLEM), plan_ptr, dir, 0, 0);

					if (error_flag) {
						Print (1, "Traveler %d %s Direction on Link %d is Not Available", 
							traveler, ((*rec > 0) ? "AB" : "BA"), link_ptr->Link ());
					}
					delete plan_ptr;
					return;
				}
				grid_ptr = grid_data [dir];

				//---- get the connection to the next link ----

				out_link_dir = grid_ptr->Link_Dir ();

				if (i > 0) {
					connect_ptr = connect_data.Get (in_link_dir, out_link_dir);

					if (connect_ptr == NULL) goto connect_problem;

					leg_rec.Out_Lane_Low (connect_ptr->In_Lane_Low ());
					leg_rec.Out_Lane_High (connect_ptr->In_Lane_High ());

					leg_rec.Out_Best_Low (connect_ptr->In_Thru_Low ());
					leg_rec.Out_Best_High (connect_ptr->In_Thru_High ());

					if (connect_ptr->Speed () > 0) {
						leg_rec.Max_Speed (connect_ptr->Speed ());
					} else {
						leg_rec.Max_Speed (grid_ptr->Speed ());
					}
					leg_rec.Connect (connect_data.Record_Index ());

					//---- get the traffic control ----

					control_ptr = control_data.Get (in_link_dir, out_link_dir);

					if (control_ptr == NULL) {
						leg_rec.Control (0);
					} else {
						leg_rec.Control (control_data.Record_Index ());
					}

					//---- save the approach leg ----

					if (!plan_ptr->Travel_Leg (&leg_rec)) goto leg_error;

					leg_rec.In_Lane_Low (connect_ptr->Out_Lane_Low ());
					leg_rec.In_Lane_High (connect_ptr->Out_Lane_High ());

				} else {
					first_dir = dir;

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
			leg_rec.Connect (0);

			if (!plan_ptr->Travel_Leg (&leg_rec)) goto leg_error;
		}
		ntrips++;

		//---- determine the best lanes ----

		Best_Lanes (plan_ptr);

		//---- check for traveler output ----

		if (traveler_output.In_Range (traveler, step, first_dir)) {
			int offset = plan_ptr->Start_Cell () * cell_size - plan_ptr->Start_Offset ();

			traveler_output.Traveler (traveler);
			traveler_output.Vehicle (0);
			traveler_output.Time (step);
			traveler_output.From_Dir (first_dir);
			traveler_output.From_Offset (offset);
			traveler_output.To_Dir (first_dir);
			traveler_output.To_Offset (offset);
			traveler_output.Lane (0);
			traveler_output.Speed (0);

			traveler_output.Output ();
		}

		//---- consider adding the vehicle to the network ----

		if (load_flag) {
			if (Load_Vehicle (plan_ptr)) {
				traveler_ptr->Travel_Plan (plan_ptr);
				return;
			}
		}
		if (!travel_queue.Add (plan_ptr)) goto queue_error;

	} else if (mode == TRANSIT_MODE) {

		ntransit++;

		rider_ptr = rider_data.Get (plan_file.Route ());

		if (rider_ptr == NULL) {
			Problem_Plan (Set_Problem (PATH_PROBLEM), plan_ptr, 0, 0, 0);

			if (error_flag) {
				Print (1, "Traveler %d Route %d was Not Found in the Route File", traveler, plan_file.Route ());
			}
			delete plan_ptr;
			return;
		}
		plan_ptr->Vehicle (rider_data.Record_Index ());

		//---- convert stop index to route stop number ----

		flag = false;
		board = plan_ptr->Start_Index ();

		for (i=1; i <= rider_ptr->Stops (); i++) {
			if (flag) {
				if (rider_ptr->Stop (i) == plan_ptr->End_Index ()) {
					plan_ptr->End_Index (i);
					break;
				}

				//---- check for loop routes ----

				if (rider_ptr->Stop (i) == board) {
					plan_ptr->Start_Index (i);
				}
			} else {
				if (rider_ptr->Stop (i) == board) {
					plan_ptr->Start_Index (i);
					flag = true;
				}
			}
		}
		if (i > rider_ptr->Stops ()) {
			Problem_Plan (Set_Problem (PATH_PROBLEM), plan_ptr, 0, 0, 0);

			if (error_flag) {
				Print (1, "Traveler %d Stops %d and %d was Not Found on Route %d", 
					traveler, plan_file.Start_ID (), plan_file.End_ID (), rider_ptr->Route ());
			}
			delete plan_ptr;
			return;
		}
		plan_ptr->Num_Travel_Legs (0);

		if (load_flag) {
			traveler_ptr->Travel_Plan (plan_ptr);
			traveler_ptr->Next_Rider (0);
			traveler_ptr->Active (true);

			//---- add to the boarding queue ----

			i = plan_ptr->Start_Index ();

			if (rider_ptr->On_Rider (i) == 0) {
				rider_ptr->On_Rider (i, plan_ptr->Traveler ());
			} else {
				for (traveler = rider_ptr->On_Rider (i); traveler; traveler = traveler_ptr->Next_Rider ()) {
					traveler_ptr = traveler_data [traveler];
				}
				traveler_ptr->Next_Rider (plan_ptr->Traveler ());
			}
		} else {
			if (!travel_queue.Add (plan_ptr)) goto queue_error;
		}
	} else if (mode == WALK_MODE) {
		if (load_flag) {
			Walk_Plan (traveler_ptr, plan_ptr);
		} else {
			if (!travel_queue.Add (plan_ptr)) goto queue_error;
		}
	} else {
		delete plan_ptr;
	}
	return;

parking_error:
	Problem_Plan (Set_Problem (ACCESS_PROBLEM), plan_ptr, 0, 0, 0);

	if (error_flag) {
		Print (1, "Traveler %d Parking %d was Not Found in the Parking File", traveler, id);
	}
	delete plan_ptr;
	return;

stop_error:
	Problem_Plan (Set_Problem (ACCESS_PROBLEM), plan_ptr, 0, 0, 0);

	if (error_flag) {
		Print (1, "Traveler %d Stop %d was Not Found in the Transit Stop File", traveler, id);
	}
	delete plan_ptr;
	return;

location_error:
	Problem_Plan (Set_Problem (ACCESS_PROBLEM), plan_ptr, 0, 0, 0);

	if (error_flag) {
		Print (1, "Traveler %d Location %d was Not Found in the Activity Location File", traveler, id);
	}
	delete plan_ptr;
	return;

leg_error:
	Error ("Insufficient Memory for Travel Leg");

queue_error:
	Error ("Adding Traveler %d to the Processing Queue", traveler);

connect_problem:
	Problem_Plan (Set_Problem (CONNECT_PROBLEM), plan_ptr, in_link_dir, 0, 0);

	if (error_flag) {
		link_ptr = link_data [in_link_dir >> 1];
		if (link_ptr != NULL) {
			in_link_dir = link_ptr->Link ();

			link_ptr = link_data [out_link_dir >> 1];

			if (link_ptr != NULL) {
				out_link_dir = link_ptr->Link ();

				Print (1, "Traveler %d Connection was Not Found between Links %d and %d", 
					traveler, in_link_dir, out_link_dir);
			}
		}
	}
	delete plan_ptr;
	return;
}
