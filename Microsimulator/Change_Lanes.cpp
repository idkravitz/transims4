//*********************************************************
//	Change_Lanes.cpp - Process Priority Lane Changing List
//*********************************************************

#include "Microsimulator.hpp"

//---------------------------------------------------------
//	Change_Lanes
//---------------------------------------------------------

void Microsimulator::Change_Lanes (void)
{
	int i, vehicle;

	Integer_List *list;
	Msim_Vehicle *vehicle_ptr;

	//---- process each list in priority order ----

	list = lane_changes;

	for (i=0; i < NUM_PRIORITY_LEVELS; i++, list++) {

		//---- check the vehicles waiting to change lanes ----

		for (vehicle = list->First (); vehicle != 0; vehicle = list->Next ()) {

			//---- get the vehicle ----

			vehicle_ptr = vehicle_data [vehicle];

			if (vehicle_ptr->Lane_Change () && vehicle_ptr->Dir () > 0 && vehicle_ptr->Status () < 4) {
				Lane_Change (vehicle_ptr);
			}
		}

		//---- reset the list ----

		list->Reset ();
	}
}
