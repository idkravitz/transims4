//*********************************************************
//	Check_Queue.cpp - Load Vehicles in the Travel Queue
//*********************************************************

#include "Microsimulator.hpp"

//---------------------------------------------------------
//	Check_Queue
//---------------------------------------------------------

void Microsimulator::Check_Queue (void)
{
	int dir;

	Travel_Plan_Data *plan_ptr;
	Traveler_Data *traveler_ptr, *queue_ptr = 0;
	Rider_Data *rider_ptr;
	Msim_Vehicle *vehicle_ptr;
	Stop_Data *stop_ptr;
	Link_Data *link_ptr;

	int traveler, queue, route;

	//---- check the vehicles waiting to load ----

	for (plan_ptr = travel_queue.First (); plan_ptr != NULL; plan_ptr = travel_queue.Next ()) {

		//---- check the max start variance ----

		if (max_start_variance > 0) {
			if (plan_ptr->Start_Time () + max_start_variance < step) {

				if (plan_ptr->Mode () == AUTO_MODE || plan_ptr->Mode () == DRIVER_MODE) {
					dir = plan_ptr->Travel_Leg (1)->Dir ();
				} else if (plan_ptr->Mode () == TRANSIT_MODE) {
					route = plan_ptr->Vehicle ();
					rider_ptr = rider_data [route];

					stop_ptr = stop_data [rider_ptr->Stop (plan_ptr->Start_Index ())];

					link_ptr = link_data [stop_ptr->Link ()];

					if (stop_ptr->Dir () == 0) {
						dir = link_ptr->AB_Dir ();
					} else {
						dir = link_ptr->BA_Dir ();
					}
				} else {
					dir = 0;
				}
				Problem_Plan (Set_Problem (DEPARTURE_PROBLEM), plan_ptr, dir, 0, 0);

				//---- reset the vehicle data ----

				if (plan_ptr->Mode () == AUTO_MODE || plan_ptr->Mode () == DRIVER_MODE) {
					vehicle_ptr = vehicle_data [plan_ptr->Vehicle ()];
					if (vehicle_ptr != NULL) {
						vehicle_ptr->Dir (0);
						vehicle_ptr->Driver (0);
						vehicle_ptr->Travel_Plan (NULL);
						vehicle_ptr->Location (plan_ptr->End_Index ());
					}
				}

				//---- set the traveler off plan ----

				travel_queue.Remove ();
				continue;
			}
		}
		
		//---- attempt to activate the plan ----

		if (plan_ptr->Mode () == DRIVER_MODE) {
				
			//---- attempt to load the vehicle ----

			if (Load_Vehicle (plan_ptr)) {
				travel_queue.Remove (false);
			}

		} else {

			//---- check the status of the traveler ----

			traveler = plan_ptr->Traveler ();

			traveler_ptr = traveler_data [traveler];

			if (traveler_ptr->Active ()) continue;
			if (traveler_ptr->End_Time () > step) continue;
				
			//---- get the mode ----

			if (plan_ptr->Mode () == WALK_MODE) {

				Walk_Plan (traveler_ptr, plan_ptr);
					
				travel_queue.Remove (false);

			} else if (plan_ptr->Mode () == AUTO_MODE) {

				//---- attempt to load the vehicle ----

				if (Load_Vehicle (plan_ptr)) {
					travel_queue.Remove (false);
				} else {
					traveler_ptr->End_Time (step + plan_ptr->End_Time () - plan_ptr->Start_Time ());
				}

			} else if (plan_ptr->Mode () == TRANSIT_MODE) {

				//---- add the person to stop queue ----

				route = plan_ptr->Vehicle ();

				rider_ptr = rider_data [route];

				queue = rider_ptr->On_Rider (plan_ptr->Start_Index ());

				if (queue == 0) {
					rider_ptr->On_Rider (plan_ptr->Start_Index (), traveler);
				} else {
					for (; queue; queue = queue_ptr->Next_Rider ()) {
						queue_ptr = traveler_data [queue];
					}
					queue_ptr->Next_Rider (traveler);
				}
				traveler_ptr->Travel_Plan (plan_ptr);
				traveler_ptr->Next_Rider (0);
				traveler_ptr->Active (true);

				travel_queue.Remove (false);
			}
		}
	}
}
