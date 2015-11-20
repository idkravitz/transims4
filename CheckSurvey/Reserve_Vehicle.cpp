//*********************************************************
//	Reserve_Vehicle.cpp - reserve vehicle use time period
//*********************************************************

#include "CheckSurvey.hpp"

//---------------------------------------------------------
//	Reserve_Vehicle
//---------------------------------------------------------

void CheckSurvey::Reserve_Vehicle (int first, int last, int start, int end, int driver)
{
	int i, vehicle;

	Activity_Data *act_ptr;
	Veh_Use_Data *veh_use_ptr, veh_use_rec, *best_ptr;

	//---- find the best vehicle ----

	best_ptr = NULL;

	for (veh_use_ptr = veh_use_data.First (); veh_use_ptr; veh_use_ptr = veh_use_data.Next ()) {

		if (veh_use_ptr->Start () <= start && veh_use_ptr->End () >= end) {

			if (best_ptr == NULL) {
				best_ptr = veh_use_ptr;
			} else {
				if (best_ptr->Driver () > 0) {
					if (best_ptr->Driver () != driver && veh_use_ptr->Driver () == driver) {
						best_ptr = veh_use_ptr;
					}
				} else if (veh_use_ptr->Driver () > 0 && veh_use_ptr->Driver () == driver) {
					best_ptr = veh_use_ptr;
				}
			}
		}
	}

	//---- update the vehicle availability ----

	if (best_ptr == NULL) {
		vehicle = -1;
	} else {
		vehicle = best_ptr->Vehicle ();

		best_ptr->Driver (driver);

		memcpy (&veh_use_rec, best_ptr, sizeof (veh_use_rec));

		best_ptr->End (start);

		veh_use_rec.Start (end);

		if (!veh_use_data.Add (&veh_use_rec)) {
			Error ("Adding Vehicle Use Record");
		}
	}

	//---- assign the vehicle to each activity ----

	for (i=first; i <= last; i++) {
		act_ptr = hhold_act_data [i];

		if (act_ptr->Vehicle () > 0 &&
			(i != first || act_ptr->Purpose () != 0)) {
			act_ptr->Vehicle (vehicle);
		}
	}
}
