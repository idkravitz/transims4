//*********************************************************
//	Read_Trips.cpp - read the trip file
//*********************************************************

#include "EventSum.hpp"

#include "Trip_File.hpp"

//---------------------------------------------------------
//	Read_Trips
//---------------------------------------------------------

void EventSum::Read_Trips (void)
{
	int hhold, num_rec, mode;
	bool add_flag;

	Location_Data *location_ptr;
	Trip_File *trip_file;
	Event_Data *event_ptr, event_rec;

	trip_file = (Trip_File *) Demand_Db_Base (TRIP);
	
	//----- initialize the summary memory ----
	
	if (event_data.Num_Records () == 0) {
		num_rec = trip_file->Estimate_Records ();
		if (!event_data.Max_Records (num_rec)) {
			Error ("Insufficient Memory for %d Event Records", num_rec);
		}
	}
	Show_Message ("Reading %s -- Record", trip_file->File_Type ());
	Set_Progress ();

	num_rec = 0;

	//---- store the trip data ----

	while (trip_file->Read ()) {
		Show_Progress ();

		//---- check the household id ----

		hhold = trip_file->Household ();
		if (hhold <= 0) continue;
		if (hhold > max_hhold) {
			Error ("Household %d is Out of Range (1..%d)", hhold, max_hhold);
		}
		if (range_flag) {
			if (!hhold_range.In_Range (hhold)) continue;
		}
		if (hhlist_flag) {
			if (hhlist_data.Get (hhold) == NULL) continue;
		}
		mode = trip_file->Mode ();
		if (mode > 0 && !trip_modes [mode]) continue;

		//---- save a new travel time ----

		add_flag = true;

		if (time_in_flag) {
			event_rec.Household (hhold);
			event_rec.Person (trip_file->Person ());
			event_rec.Trip (trip_file->Trip ());
			event_rec.Leg (0);

			event_ptr = event_data.Get (&event_rec);
			if (event_ptr != NULL) {
				add_flag = false;
			} else {
				event_ptr = event_data.Get_GE (&event_rec);

				if (event_ptr == NULL || event_ptr->Key1 () != event_rec.Key1 () ||
					event_ptr->Trip () != event_rec.Trip ()) continue;
			}
		}

		if (add_flag) {
			event_ptr = event_data.New_Record (true);
			if (event_ptr == NULL) goto mem_error;

			event_ptr->Household (hhold);
			event_ptr->Person (trip_file->Person ());
			event_ptr->Trip (trip_file->Trip ());
			event_ptr->Leg (0);

			if (!event_data.Add ()) goto mem_error;
		}
		event_ptr->Mode (mode);
		event_ptr->Purpose (trip_file->Purpose ());
		event_ptr->Constraint (trip_file->Constraint ());
		event_ptr->Trip_Start (time_periods.Step (trip_file->Start ()));
		event_ptr->Trip_End (time_periods.Step (trip_file->Arrive ()));

		//---- find the origin and destination links ----

		location_ptr = location_data.Get (trip_file->Origin ());

		if (location_ptr == NULL) {
			Warning ("Trip Origin %d was Not Found in the Activity Location File", 
				trip_file->Origin ());
			event_ptr->Start_Link (0);
		} else {
			event_ptr->Start_Link (location_ptr->Link ());
		}

		location_ptr = location_data.Get (trip_file->Destination ());

		if (location_ptr == NULL) {
			Warning ("Trip Destination %d was Not Found in the Activity Location File",
				trip_file->Destination ());
			event_ptr->End_Link (0);
		} else {
			event_ptr->End_Link (location_ptr->Link ());
		}
		num_rec++;
	}
	End_Progress ();

	trip_file->Close ();
	
	Print (2, "Number of %s Records = %d", trip_file->File_Type (), num_rec);
	return;

mem_error:
	Error ("Insufficient Memory for Event Data");
}
