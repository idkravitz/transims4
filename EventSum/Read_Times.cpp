//*********************************************************
//	Read_Times.cpp - read the comparison travel times
//*********************************************************

#include "EventSum.hpp"

//---------------------------------------------------------
//	Read_Times
//---------------------------------------------------------

void EventSum::Read_Times (void)
{
	int hhold, num_rec, mode;

	Event_Data *event_ptr;
	
	num_rec = time_in_file.Estimate_Records ();
	if (!event_data.Max_Records (num_rec)) {
		Error ("Insufficient Memory for %d Event Records", num_rec);
	}
	Show_Message ("Reading %s -- Record", time_in_file.File_Type ());
	Set_Progress ();

	num_rec = 0;

	//---- store the travel time data ----

	while (time_in_file.Read ()) {
		Show_Progress ();

		//---- check the household id ----

		hhold = time_in_file.Household ();
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
		mode = time_in_file.Mode ();
		if (mode > 0 && !trip_modes [mode]) continue;

		//---- save a new travel time ----

		event_ptr  = event_data.New_Record (true);
		if (event_ptr == NULL) goto mem_error;

		event_ptr->Household (hhold);
		event_ptr->Person (time_in_file.Person ());
		event_ptr->Trip (time_in_file.Trip ());
		event_ptr->Leg (time_in_file.Leg ());
		event_ptr->Mode (mode);
		event_ptr->Purpose (time_in_file.Purpose ());
		event_ptr->Constraint (time_in_file.Constraint ());
		event_ptr->Start_Link (time_in_file.Start_Link ());
		event_ptr->End_Link (time_in_file.End_Link ());
		event_ptr->Trip_Start (time_periods.Step (time_in_file.Trip_Start ()));
		event_ptr->Trip_End (time_periods.Step (time_in_file.Trip_End ()));
		event_ptr->Base_Start (time_periods.Step (time_in_file.Started ()));
		event_ptr->Base_End (time_periods.Step (time_in_file.Ended ()));

		if (!event_data.Add ()) goto mem_error;
		num_rec++;
	}
	End_Progress ();

	time_in_file.Close ();

	event_data.Max_Records (event_data.Num_Records ());
	
	Print (2, "Number of %s Records = %d", time_in_file.File_Type (), num_rec);
	return;

mem_error:
	Error ("Insufficient Memory for Event Data");
}
