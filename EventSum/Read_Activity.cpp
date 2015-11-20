//*********************************************************
//	Read_Activity.cpp - read the activity file
//*********************************************************

#include "EventSum.hpp"

#include "Activity_File.hpp"

//---------------------------------------------------------
//	Read_Activity
//---------------------------------------------------------

void EventSum::Read_Activity (void)
{
	int hhold, person, purpose, start, end_min, end_max, last_hhold, last_person, num_rec, link, mode, trip;
	bool home_flag, add_flag;

	Location_Data *location_ptr;
	Activity_File *activity_file;
	Event_Data *event_ptr, event_rec;

	activity_file = (Activity_File *) Demand_Db_Base (ACTIVITY);

	//----- initialize the summary memory ----
	
	if (event_data.Num_Records () == 0) {
		num_rec = activity_file->Estimate_Records ();

		if (!event_data.Max_Records (num_rec)) {
			Error ("Insufficient Memory for %d Event Records", num_rec);
		}
	}
	Show_Message ("Reading %s -- Record", activity_file->File_Type ());
	Set_Progress ();

	trip = 1;
	start = last_hhold = last_person = num_rec = link = 0;
	home_flag = true;

	//---- store the trip data ----

	while (activity_file->Read ()) {
		Show_Progress ();

		//---- check the household id ----

		hhold = activity_file->Household ();
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
		mode = activity_file->Mode ();
		if (mode > 0 && !trip_modes [mode]) continue;

		//---- check for the beginning of a new tour ----

		person = activity_file->Person ();
		purpose = activity_file->Purpose ();

		if (hhold == last_hhold && person == last_person) {

			//---- save a new travel time ----

			add_flag = true;

			if (time_in_flag) {
				event_rec.Household (hhold);
				event_rec.Person (person);
				event_rec.Trip (trip);
				event_rec.Leg (0);

				event_ptr = event_data.Get (&event_rec);
				if (event_ptr != NULL) {
					add_flag = false;
				} else {
					event_ptr = event_data.Get_GE (&event_rec);

					if (event_ptr == NULL || event_ptr->Key1 () != event_rec.Key1 () ||
						event_ptr->Trip () != event_rec.Trip ()) goto skip;
				}
			}
			if (add_flag) {
				event_ptr  = event_data.New_Record (true);
				if (event_ptr == NULL) goto mem_error;

				event_ptr->Household (hhold);
				event_ptr->Person (person);
				event_ptr->Trip (trip);
				event_ptr->Leg (0);

				if (!event_data.Add ()) goto mem_error;

				trip += 2;
			}
			event_ptr->Purpose (purpose);
			event_ptr->Constraint (activity_file->Constraint ());

			event_ptr->Trip_Start (start);
			event_ptr->Start_Link (link);

			end_min = time_periods.Step (activity_file->Start_Min ());
			end_max = time_periods.Step (activity_file->Start_Max ());

			location_ptr = location_data.Get (activity_file->Location ());

			if (location_ptr == NULL) {
				Warning ("Activity Location %d was Not Found in the Activity Location File",
					activity_file->Location ());
				link = 0;
			} else {
				link = location_ptr->Link ();
			}
			event_ptr->Trip_End ((end_min + end_max) / 2);
			event_ptr->End_Link (link);
			num_rec++;
		}
skip:
		if (hhold != last_hhold || person != last_person) {
			trip = 1;
		}
		last_hhold = hhold;
		last_person = person;
		home_flag = (purpose == 0);

		end_min = time_periods.Step (activity_file->End_Min ());
		end_max = time_periods.Step (activity_file->End_Max ());

		start = (end_min + end_max) / 2;

		location_ptr = location_data.Get (activity_file->Location ());

		if (location_ptr == NULL) {
			Warning ("Activity Location %d was Not Found in the Activity Location File",
				activity_file->Location ());
			link = 0;
		} else {
			link = location_ptr->Link ();
		}
	}
	End_Progress ();

	activity_file->Close ();
	
	Print (2, "Number of %s Records = %d", activity_file->File_Type (), num_rec);
	return;

mem_error:
	Error ("Insufficient Memory for Event Data");
}
