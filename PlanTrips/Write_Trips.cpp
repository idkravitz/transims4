//*********************************************************
//	Write_Trips.cpp - write the trip file
//*********************************************************

#include "PlanTrips.hpp"

//---------------------------------------------------------
//	Write_Trips
//---------------------------------------------------------

void PlanTrips::Write_Trips (void)
{
	Trip_Data *trip_ptr;

	Show_Message ("Writing %s", trip_file->File_Type ());
	Set_Progress (10000);

	//---- read each plan record ----

	for (trip_ptr = trip_data.First_Key (); trip_ptr; trip_ptr = trip_data.Next_Key ()) {
		Show_Progress ();

		trip_file->Household (trip_ptr->Household ());
		trip_file->Person (trip_ptr->Person ());
		trip_file->Trip (trip_ptr->Trip ());
		trip_file->Purpose (trip_ptr->Purpose ());
		trip_file->Mode (trip_ptr->Mode ());
		trip_file->Vehicle (trip_ptr->Vehicle ());
		trip_file->Origin (trip_ptr->Origin ());
		trip_file->Destination (trip_ptr->Destination ());
		trip_file->Start (trip_time.Format_Time (trip_ptr->Start_Time ()));
		trip_file->Arrive (trip_time.Format_Time (trip_ptr->End_Time ()));
		trip_file->Constraint (trip_ptr->Constraint ());

		if (!trip_file->Write ()) {
			Error ("Writing %s", trip_file->File_Type ());
		}
		num_trips++;
	}
	End_Progress ();

	trip_file->Close ();
}
