//*********************************************************
//	Write_Events.cpp - Create Link Event File
//*********************************************************

#include "EventSum.hpp"

//---------------------------------------------------------
//	Write_Events
//---------------------------------------------------------

void EventSum::Write_Events (void)
{
	int num_out;

	Event_Data *event_ptr;

	Show_Message ("Writing %s -- Record", travel_time_file.File_Type ());
	Set_Progress ();

	num_out = 0;

	//---- write travel time file ----

	for (event_ptr = event_data.First_Key (); event_ptr; event_ptr = event_data.Next_Key ()) {
		Show_Progress ();

		if (event_ptr->Mode () > 0 && !trip_modes [event_ptr->Mode ()]) continue;

		//---- check the time range ----

		if (!time_periods.Span_Range (event_ptr->Base_Start (), event_ptr->Base_End ()) &&
			!time_periods.Span_Range (event_ptr->Started (), event_ptr->Ended ())) continue;

		//---- set the data fields ----

		travel_time_file.Household (event_ptr->Household ());
		travel_time_file.Person (event_ptr->Person ());
		travel_time_file.Trip (event_ptr->Trip ());
		travel_time_file.Leg (event_ptr->Leg ());
		travel_time_file.Mode (event_ptr->Mode ());
		travel_time_file.Purpose (event_ptr->Purpose ());
		travel_time_file.Constraint (event_ptr->Constraint ());
		travel_time_file.Start_Link (event_ptr->Start_Link ());
		travel_time_file.End_Link (event_ptr->End_Link ());

		travel_time_file.Trip_Start (time_periods.Format_Step (event_ptr->Trip_Start ()));
		travel_time_file.Base_Start (time_periods.Format_Step (event_ptr->Base_Start ()));
		travel_time_file.Started (time_periods.Format_Step (event_ptr->Started ()));
		travel_time_file.Start_Diff (time_periods.Format_Duration (event_ptr->Base_Start_Diff ()));

		travel_time_file.Trip_End (time_periods.Format_Step (event_ptr->Trip_End ()));
		travel_time_file.Base_End (time_periods.Format_Step (event_ptr->Base_End ()));
		travel_time_file.Ended (time_periods.Format_Step (event_ptr->Ended ()));
		travel_time_file.End_Diff (time_periods.Format_Duration (event_ptr->Base_End_Diff ()));

		travel_time_file.Mid_Trip (time_periods.Format_Step (event_ptr->Mid_Trip ()));
		travel_time_file.Travel_Time (time_periods.Format_Duration (event_ptr->Travel_Time ()));

		if (!travel_time_file.Write ()) {
			Error ("Writing %s", travel_time_file.File_Type ());
		}
		num_out++;
	}
	End_Progress ();

	travel_time_file.Close ();

	Print (2, "Number of %s Records = %d", travel_time_file.File_Type (), num_out);
}
