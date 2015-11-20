//*********************************************************
//	Synthetic_OD.cpp - Synthethic OD Table
//*********************************************************

#include "TripSum.hpp"

#include "Vehicle_File.hpp"

#include <math.h>

//---------------------------------------------------------
//	Synthetic_OD
//---------------------------------------------------------

void TripSum::Synthetic_OD (void)
{
	int i, org, des;
	double trips, factor, error;

	Vehicle_Data *veh_ptr;
	Vehicle_File *new_file;

	veh_ptr = NULL;

	//---- initialize the seed matrix ----

	memset (org_total, '\0', nzone * sizeof (double));
	error = 0.0;

	for (org=1; org < nzone; org++) {
		error += org_target [org];

		for (des=1; des < nzone; des++) {
			trips = veh_count [org] [des];

			synod [org] [des] = trips;
			org_total [org] += trips;
		}
	}
	error /= 10000.0;
	if (error < 1.0) error = 1.0;

	//---- maximum of 1000 iterations ----

	Show_Message ("Processing Synthetic OD -- Iteration");
	Set_Progress (1);

	for (i=0; i < 1000; i++) {
		Show_Progress ();

		//---- factor by origin zone ----

		memset (des_total, '\0', nzone * sizeof (double));

		for (org=1; org < nzone; org++) {
			if (org_total [org] <= 0.0) continue;

			factor = org_target [org] / org_total [org];

			for (des=1; des < nzone; des++) {
				trips = synod [org] [des] * factor;
				if (trips > veh_count [org] [des]) trips = veh_count [org] [des];
				des_total [des] += synod [org] [des] = trips;
			}
		}
		trips = 0.0;

		for (des=1; des < nzone; des++) {
			trips += fabs (des_total [des] - des_target [des]);
		}
		if (trips < error) break;

		//---- factor by destination zone ----

		memset (org_total, '\0', nzone * sizeof (double));

		for (des=1; des < nzone; des++) {
			if (des_total [des] <= 0.0) continue;

			factor = des_target [des] / des_total [des];

			for (org=1; org < nzone; org++) {
				trips = synod [org] [des] * factor;
				if (trips > veh_count [org] [des]) trips = veh_count [org] [des];
				org_total [org] += synod [org] [des] = trips;
			}
		}
		trips = 0.0;

		for (org=1; org < nzone; org++) {
			trips += fabs (org_total [org] - org_target [org]);
		}
		if (trips < error) break;
	}
	End_Progress ();

	Print (2, "Number of Iterations = %d, Convergence Error = %.2lf", i, trips);

	//---- select the vehicles to types ----

	trips = 0.0;

	for (org=1; org < nzone; org++) {
		for (des=1; des < nzone; des++) {
			if (veh_count [org] [des] > 0) {
				factor = synod [org] [des] / veh_count [org] [des];

				if (factor > 0.0) {
					for (i=first_veh [org] [des]; i; i=next_veh [i]) {
						if (random.Probability () <= factor) {
							veh_ptr = vehicle_data [i];
							veh_ptr->Type (2);
							veh_ptr->Sub_Type (0);
							trips++;
						}
					}
				}
			}
		}
	}
	Print (2, "Number of Vehicles Modified = %.0lf", trips);

	//---- output the vehicle data ----
	
	new_file = (Vehicle_File *) Demand_Db_File (NEW_VEHICLE);

	Show_Message ("Writing %s -- Record", new_file->File_Type ());
	Set_Progress ();

	i = 0; 

	for (veh_ptr = vehicle_data.First_Key (); veh_ptr; veh_ptr = vehicle_data.Next_Key ()) {
		Show_Progress ();

		new_file->Vehicle (veh_ptr->Vehicle ());
		new_file->Household (veh_ptr->Household ());
		new_file->Location (veh_ptr->Location ());
		new_file->Type (veh_ptr->Type ());
		new_file->Sub_Type (veh_ptr->Sub_Type ());

		if (veh_ptr->Type () == 2) i++;

		if (!new_file->Write ()) {
			Error ("Writing %s", new_file->File_Type ());
		}
	}
	End_Progress ();

	Print (2, "Number of Truck Trips = %d", i);

	new_file->Close ();
}
