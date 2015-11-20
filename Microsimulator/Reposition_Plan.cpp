//*********************************************************
//	Reposition_Plan.cpp - Move Traveler at End of Plan
//*********************************************************

#include "Microsimulator.hpp"

//---------------------------------------------------------
//	Reposition_Plan
//---------------------------------------------------------

void Microsimulator::Reposition_Plan (void)
{
	if (plan_file.Mode () != AUTO_MODE || plan_file.Driver_Flag () != 1) return;

	int end_lot, traveler;

	Msim_Vehicle *vehicle_ptr;
	Traveler_Data *traveler_ptr;

	traveler = plan_file.Traveler ();

	//---- add the traveler to the list ----

	traveler_ptr = traveler_data.Get (traveler);

	if (traveler_ptr == NULL) {
		Traveler_Data traveler_rec;

		traveler_rec.Traveler (traveler);

		if (!traveler_data.Add (&traveler_rec)) {
			Error ("Adding Traveler Data");
		}
	} else {
		traveler_ptr->Active (false);
	}

	//---- move the vehicle to the end parking lot ----	

	end_lot = parking_data.Get_Index (plan_file.End_ID ());

	if (end_lot == 0) {
		Error ("Traveler %d Parking Lot %d was Not Found in the Parking File", traveler, plan_file.End_ID ());
	}
	vehicle_ptr = vehicle_data.Get (plan_file.Vehicle ());

	if (vehicle_ptr == NULL) {
		Error ("Traveler %d Vehicle %d was Not Found in the Vehicle File", traveler, plan_file.Vehicle ());
	}
	vehicle_ptr->Location (end_lot);
}
