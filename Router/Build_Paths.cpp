//*********************************************************
//	Build_Paths.cpp - Build Specified Paths
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Build_Paths
//---------------------------------------------------------

void Router::Build_Paths (void)
{
	int hhold, mode, origin, destination, org_rec, des_rec, nloc;

	Location_Data *org_ptr, *des_ptr;
	Vehicle_Data *veh_ptr;
	Range_Data *range_ptr;
	Trip_Data trip_rec;

	//---- initialize the data ----

	Show_Message ("Building Specified Paths -- Record");
	Set_Progress (1000);

	hhold = 0;

	trip_rec.Household (hhold);
	trip_rec.Person (1);
	trip_rec.Trip (1);
	trip_rec.Purpose (1);
	trip_rec.Vehicle (1);

	veh_ptr = &default_vehicle;

	if (mode_flag) {
		mode = new_mode;
	} else {
		mode = DRIVE_ALONE;
	}
	trip_rec.Mode (mode);

	nloc = location_data.Num_Records ();

	//---- loop through each origin location ----

	for (org_rec=1; org_rec <= nloc; org_rec++) {
		org_ptr = location_data [org_rec];
		origin = org_ptr->Location ();

		if (select_org && !org_range.In_Range (origin)) continue;
		if (!select_org && org_ptr->Zone () == 0) continue;

		trip_rec.Origin (origin);

		//---- loop through each destination location ----

		for (des_rec=1; des_rec <= nloc; des_rec++) {
			if (org_rec == des_rec) continue;

			des_ptr = location_data [des_rec];
			destination = des_ptr->Location ();

			if (select_des && !des_range.In_Range (destination)) continue;
			if (!select_des && des_ptr->Zone () == 0) continue;

			trip_rec.Destination (destination);

			//---- loop through each time increment ----

			for (range_ptr = time_range.First (); range_ptr; range_ptr = time_range.Next ()) {
				Show_Progress ();
				nprocess++;
				hhold++;

				trip_rec.Household (hhold);
				trip_rec.Start_Time (range_ptr->Low ());
				trip_rec.End_Time (MIDNIGHT);

				//---- build the path ----

				Plan_Build (&trip_rec);
			}
		}
	}
	End_Progress ();
}
