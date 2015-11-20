//*********************************************************
//	Assign_Vehicles.cpp - assign vehicles to drivers
//*********************************************************

#include "ActGen.hpp"

//---------------------------------------------------------
//	Assign_Vehicles
//---------------------------------------------------------

void ActGen::Assign_Vehicles (void)
{
	int i, j, i1, i2, nact, vehicle, start, end, person, loc1, loc2;
	bool home_flag;

	Activity_Data *act_ptr, *driver_ptr;
	Vehicle_Data *vehicle_ptr;
	Veh_Use_Data veh_use_rec;

	//---- initial household vehicles ----

	veh_use_data.Reset ();

	veh_use_rec.Start (0);
	veh_use_rec.End (MAX_INTEGER);
	veh_use_rec.Driver (0);

	for (act_ptr = hhold_act_data.First_Key (); act_ptr; act_ptr = hhold_act_data.Next_Key ()) {

		vehicle = act_ptr->Vehicle ();
		if (vehicle < 1) continue;

		for (; vehicle > 0; vehicle = vehicle_ptr->Household ()) {
			vehicle_ptr = vehicle_data [vehicle];

			veh_use_rec.Vehicle (vehicle);

			if (!veh_use_data.Add (&veh_use_rec)) {
				Error ("Adding Vehicle Use Record");
			}
		}
		break;
	}

	//---- assign vehicles to drivers ----

	home_flag = true;
	person = vehicle = 0;
	nact = hhold_act_data.Num_Records ();
	start = end = i1 = loc1 = 0;

	for (i=1; i <= nact; i++) {

		act_ptr = hhold_act_data [i];

		if (person != act_ptr->Person () || act_ptr->Purpose () == 0 || i == nact) {

			//---- process the tour ----

			if (!home_flag && person > 0) {

				//---- check for a driver ----

				if (vehicle > 0) {
					if (person == act_ptr->Person () && act_ptr->Purpose () == 0) {
						end = act_ptr->Start_Time ();
						i2 = i;
					} else {
						end = MAX_INTEGER;
						i2 = i - 1;
					}
					Reserve_Vehicle (i1, i2, start, end, person);
				}
			}

			//---- reset the data ----

			i1 = i;
			person = act_ptr->Person ();
			vehicle = act_ptr->Vehicle ();

			if (act_ptr->Purpose () == 0) {
				start = act_ptr->End_Time ();
				home_flag = true;
			} else {
				start = act_ptr->Start_Time ();
				home_flag = false;
			}

		} else {
			home_flag = false;
			if (act_ptr->Vehicle () > 0) vehicle = act_ptr->Vehicle ();
		}
	}

	//---- assign passengers to vehicles ----

	for (i=1; i <= nact; i++) {
		act_ptr = hhold_act_data [i];

		//---- check for a shared ride ----

		if (act_ptr->Vehicle () == -2 && act_ptr->Mode () == DRIVE_ALONE) {
			start = act_ptr->Start_Time ();
			loc2 = act_ptr->Location ();

			home_flag = false;

			for (j=1; j <= nact; j++) {
				driver_ptr = hhold_act_data [j];

				vehicle = driver_ptr->Vehicle ();

				if (vehicle > 0 && home_flag && 
					driver_ptr->Start_Time () == start && 
					driver_ptr->Location () == loc2) {

					act_ptr->Mode (CARPOOL2);
					act_ptr->Vehicle (vehicle);
					break;
				}
				home_flag = (driver_ptr->End_Time () == end && driver_ptr->Location () == loc1);
			}
		}
		end = act_ptr->End_Time ();
		loc1 = act_ptr->Location ();
	}
}
