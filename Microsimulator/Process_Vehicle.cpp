//*********************************************************
//	Process_Vehicle.cpp - Process a Vehicle
//*********************************************************

#include "Microsimulator.hpp"

//---------------------------------------------------------
//	Process_Vehicle
//---------------------------------------------------------

bool Microsimulator::Process_Vehicle (Msim_Vehicle *vehicle_ptr)
{
	int stat;
	Travel_Plan_Data *plan_ptr;

	if (vehicle_ptr->Status () >= 4 || vehicle_ptr->Dir () == 0) return (false); 

	plan_ptr = vehicle_ptr->Travel_Plan ();
	if (plan_ptr == NULL) return (false);

	//---- check the max end variance ----

	if (max_end_variance > 0) {
		if (plan_ptr->End_Time () + max_end_variance < step) {
			Problem_Plan (Set_Problem (ARRIVAL_PROBLEM), plan_ptr, vehicle_ptr->Dir (), vehicle_ptr->Lane (), vehicle_ptr->Cell ());
			stat = 5;
			goto park_vehicle;
		}
	}

	//---- check the max waiting time ----

	if (vehicle_ptr->Waiting () > max_wait_time) {
		Problem_Plan (Set_Problem (WAIT_PROBLEM), plan_ptr, vehicle_ptr->Dir (), vehicle_ptr->Lane (), vehicle_ptr->Cell ());
		stat = 5;
		goto park_vehicle;
	}

	//---- identify the optimum path ahead ----

	if (look_ahead_flag) {
		Look_Ahead (vehicle_ptr);
	}

	//---- attempt to move the vehicle ----

	if (!Check_Move (vehicle_ptr)) {
		if (problem_code == LINK_ACCESS_PROBLEM) {
			stat = 5;
		} else {
			stat = 4;
			if (vehicle_ptr->Travel_Plan ()->Mode () == DRIVER_MODE) {
				nrun_end++;
			} else {
				ncompleted++;
			}
		}
		goto park_vehicle;
	}
	return (true);

park_vehicle:
	vehicle_ptr->Status (stat);

	if (vehicle_ptr->Reserve () > 0) {
		vehicle_ptr = vehicle_data [vehicle_ptr->Reserve ()];
		vehicle_ptr->Waiting (vehicle_ptr->Waiting () - 10 * steps_per_second);
	}
	return (false);
}
