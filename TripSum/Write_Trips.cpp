//*********************************************************
//	Write_Trips.cpp - Write Trip file
//*********************************************************

#include "TripSum.hpp"

//---------------------------------------------------------
//	Write_Trips
//---------------------------------------------------------

void TripSum::Write_Trips (void)
{
	int org, des, period, trips, total;

	Matrix_Data *trip_ptr;

	//---- write the records ----

	Show_Message ("Writing %s -- Record", trip_file.File_Type ());
	Set_Progress (10000);

	total = 0;

	for (trip_ptr = trip_table.First_Key (); trip_ptr; trip_ptr = trip_table.Next_Key ()) {
		Show_Progress ();

		trips = trip_ptr->Data ();

		if (trips == 0) continue;

		trip_table.Key (trip_ptr->ID (), &org, &des, &period);

		trip_file.Origin (org);
		trip_file.Destination (des);
		trip_file.Period (period);
		trip_file.Data (trips);

		total += trips;

		if (!trip_file.Write ()) {
			Error ("Writing %s", trip_file.File_Type ());
		}
	}
	End_Progress ();

	trip_file.Close ();

	Print (2, "Number of %s Records = %d", trip_file.File_Type (), Progress_Count ());
	Print (1, "Number of %s Trips = %d", trip_file.File_Type (), total);
} 
