//*********************************************************
//	Update_Transit.cpp - update transit routes
//*********************************************************

#include "Microsimulator.hpp"

//---------------------------------------------------------
//	Update_Transit
//---------------------------------------------------------

void Microsimulator::Update_Transit (void)
{
	int i, start, index, vehicle;

	Rider_Data *rider_ptr;
	Travel_Plan_Data *plan_ptr; 
	Msim_Vehicle *vehicle_ptr;

	//---- load new runs ----

	for (rider_ptr = rider_data.First (); rider_ptr != NULL; rider_ptr = rider_data.Next ()) {
		index = rider_data.Record_Index ();

		for (i=1; i <= rider_ptr->Runs (); i++) {
			start = rider_ptr->Schedule (i, 1);

			if (start < step) continue;
			if (start > step) break;

			//---- initialize the plan record ----

			plan_ptr = route_plan.Copy (rider_ptr->Driver ());

			if (plan_ptr == NULL) goto mem_error;

			vehicle = rider_data.Vehicle_ID (rider_ptr->Route (), i);

			vehicle_ptr = vehicle_data.Get (vehicle);

			if (vehicle_ptr == NULL) {
				Error ("Transit Vehicle %d was Not Found", vehicle);
			}
			plan_ptr->Vehicle (vehicle_data.Record_Index ());
			plan_ptr->Trip (i);

			if (!vehicle_ptr->Initialize ()) {
				Error ("Insufficient Memory for Transit Vehicle Data");
			}
			vehicle_ptr->Driver (index);
	
			plan_ptr->Start_Time (start);
			plan_ptr->End_Time (rider_ptr->Schedule (i, rider_ptr->Stops ()));

			vehicle_ptr->Priority (true);
			nruns++;

			//---- consider adding the vehicle to the network ----

			if (!Load_Vehicle (plan_ptr)) {
				if (!travel_queue.Add (plan_ptr)) goto mem_error;
			}
		}
	}
	return;

mem_error:
	Error ("Insufficient Memory for a new Transit Run");
}
