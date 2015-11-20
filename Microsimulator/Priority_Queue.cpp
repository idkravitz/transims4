//*********************************************************
//	Priority_Queue.cpp - Process Priority Queue
//*********************************************************

#include "Microsimulator.hpp"

//---------------------------------------------------------
//	Priority_Queue
//---------------------------------------------------------

void Microsimulator::Priority_Queue (void)
{
	int vehicle;

	Msim_Vehicle *vehicle_ptr;

	//---- check the vehicles waiting in the priority queue ----

	for (vehicle = priority_queue.First (); vehicle != 0; vehicle = priority_queue.Next ()) {

		//---- get the vehicle ----

		vehicle_ptr = vehicle_data [vehicle];

		if (!vehicle_ptr->Priority () || vehicle_ptr->Move_Flag () || 
			vehicle_ptr->Dir () == 0 || vehicle_ptr->Status () >= 4) {

			if (vehicle_ptr->Sub_Type () == 0 ||
				vehicle_ptr->Dir () == 0 || vehicle_ptr->Status () >= 4) {
				
				priority_queue.Remove ();
				continue;
			}
		}

		//--- check for lane change ----

		if (vehicle_ptr->Lane_Change () > 0) {

			Lane_Change (vehicle_ptr);
			vehicle_ptr->Lane_Change (0);

		} else {

			//---- try to move forward ----

			Process_Vehicle (vehicle_ptr);

		}

		//---- remove the vehicle from the queue ----

		if (vehicle_ptr->Move_Flag () && vehicle_ptr->Sub_Type () == 0) {
			npriority++;
			priority_queue.Remove ();

			vehicle_ptr->Priority (0);
			vehicle_ptr->Waiting (0);
		}
	}
}
