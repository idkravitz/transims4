//*********************************************************
//	Process_Step.cpp - Process a Time Step
//*********************************************************

#include "Microsimulator.hpp"

//---------------------------------------------------------
//	Process_Step
//---------------------------------------------------------

void Microsimulator::Process_Step (void)
{
	bool first;

	Travel_Plan_Data *plan_ptr;
	Msim_Vehicle *vehicle_ptr;

	//---- process the vehicles on the network -----

	for (plan_ptr = network_traffic.First (); plan_ptr != NULL; plan_ptr = network_traffic.Next ()) {

		next_vehicle = plan_ptr->Vehicle ();
		first = true;

		//---- process the next vehicle -----

		while (next_vehicle > 0) {
			vehicle_ptr = vehicle_data [next_vehicle];
			next_vehicle = 0;

			if (vehicle_ptr != NULL && vehicle_ptr->Status () == 0 && vehicle_ptr->Dir () > 0) {

				Process_Vehicle (vehicle_ptr);

				if (!first && vehicle_ptr->Move_Flag ()) {
					nmove++;
				}
			}
			first = false;
		}
	}
}
