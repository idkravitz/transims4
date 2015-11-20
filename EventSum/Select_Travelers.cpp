//*********************************************************
//	Select_Travelers.cpp - select travelers
//*********************************************************

#include "EventSum.hpp"

//---------------------------------------------------------
//	Select_Travelers
//---------------------------------------------------------

void EventSum::Select_Travelers (void)
{
	int diff, time;
	double share;
	bool keep;

	Event_Data *event_ptr;
	Class2_Index diff_rec, *diff_ptr;

	Show_Message ("Selecting Travelers -- Record");
	Set_Progress ();

	//---- select travel time differences ----

	for (event_ptr = event_data.First (); event_ptr; event_ptr = event_data.Next ()) {
		Show_Progress ();

		//---- check the time range ----

		if (event_ptr->Base_Start () >= event_ptr->Base_End () ||
			event_ptr->Started () >= event_ptr->Ended ()) continue;

		if (!time_periods.Span_Range (event_ptr->Base_Start (), event_ptr->Base_End ()) &&
			!time_periods.Span_Range (event_ptr->Started (), event_ptr->Ended ())) continue;

		time = event_ptr->Base_Time ();
		diff = abs (event_ptr->Travel_Time () - time);
		share = 1.0;
		keep = false;

		if (diff >= min_time) {
			if (diff < max_time) {
				if (time > 0) {
					share = (double) diff / (double) time;
				}
				if (share > percent_diff) keep = true;
			} else {
				keep = true;
			}
		}
		if (keep) {
			diff_rec.Key1 ((int) (share * 10000.0 + 0.5));
			diff_rec.Key2 (event_data.Record_Index ());

			if (!difference_sort.Add (&diff_rec)) {
				Error ("Adding Difference Sort Record");
			}
		}
	}
	End_Progress ();

	diff = difference_sort.Num_Records ();
	time = event_data.Num_Records ();
	if (time > 0) {
		share = 100.0 * diff / time;
	} else {
		share = 0.0;
		time = 1;
	}
	Print (2, "Number of Trips Selected = %d (%.1lf%%)", diff, share);

	diff = (int) (diff * percent + 0.5);

	//---- check the maximum selection ----

	if (((double) diff / time) > max_percent) {
		diff = (int) (time * max_percent + 0.5);
	}
	share = 100.0 * diff / time;

	Print (1, "Number of Trips Flagged = %d (%.1lf%%)", diff, share);

	//---- select the event records ----

	for (diff_ptr = difference_sort.Last_Key (); diff_ptr; diff_ptr = difference_sort.Previous_Key ()) {
		event_ptr = event_data [diff_ptr->Key2 ()];

		if (event_ptr != NULL) {
			event_ptr->Select_Flag (true);

			if (new_hhold_flag) {
				HHList_Data *hhlist_ptr;

				hhlist_ptr = hhlist_data.Get (event_ptr->Household ());

				if (hhlist_ptr != NULL) {
					hhlist_ptr->Keep (true);
				}
			}
		}
		if (--diff == 0) break;
	}
}
