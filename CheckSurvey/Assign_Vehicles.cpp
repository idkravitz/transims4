//*********************************************************
//	Assign_Vehicles.cpp - assign vehicles to drivers
//*********************************************************

#include "CheckSurvey.hpp"

//---------------------------------------------------------
//	Assign_Vehicles
//---------------------------------------------------------

void CheckSurvey::Assign_Vehicles (Household_Data *household_ptr)
{
	int i, j, i1, i2, nact, vehicle, start, end, person, loc1, loc2, loc3, id;
	bool home_flag;

	Activity_Data *act_ptr, *driver_ptr, *last_ptr;
	Veh_Use_Data veh_use_rec;

	//---- initial household vehicles ----

	veh_use_data.Reset ();

	veh_use_rec.Start (0);
	veh_use_rec.End (MAX_INTEGER);
	veh_use_rec.Driver (0);

	for (i=1; i <= household_ptr->Vehicles (); i++) {
		veh_use_rec.Vehicle (i);

		if (!veh_use_data.Add (&veh_use_rec)) {
			Error ("Adding Vehicle Use Record");
		}
	}

	//---- assign vehicles to drivers ----

	home_flag = true;
	person = vehicle = 0;
	nact = hhold_act_data.Num_Records ();

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
			loc2 = loc3 = act_ptr->Location ();

			home_flag = false;
			last_ptr = NULL;

			for (j=1; j <= nact; j++) {
				driver_ptr = hhold_act_data [j];

				vehicle = driver_ptr->Vehicle ();

				if (vehicle > 0) {
					if (home_flag) {
						if (driver_ptr->Start_Time () == start) {
							if (driver_ptr->Location () == loc2) {
								act_ptr->Mode (CARPOOL2);
								act_ptr->Vehicle (vehicle);
								break;
							} else {
								act_ptr->Vehicle (-3);
								loc3 = driver_ptr->Location ();
							}
						}
					} else if (last_ptr) {
						if (driver_ptr->Start_Time () == start && driver_ptr->Location () == loc2) {
							if (last_ptr->Location () == loc1) {
								act_ptr->Vehicle (-4);
								id = j - 1;
							} else if (last_ptr->End_Time () == end) {
								act_ptr->Vehicle (-5);
								id = j - 1;
							}
						}
					}
				}
				home_flag = (driver_ptr->End_Time () == end && driver_ptr->Location () == loc1);
				last_ptr = driver_ptr;
			}
		}
		end = act_ptr->End_Time ();
		loc1 = act_ptr->Location ();

		if (new_act_flag) {
			if (act_ptr->Vehicle () == -3) {
				act_ptr->Problem (loc3);
			} else if (act_ptr->Vehicle () < -3) {
				driver_ptr->Problem (id);
			}
		}
	}
}
