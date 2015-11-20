//*********************************************************
//	Top_100_TTime.cpp - top 100 travel time differences
//*********************************************************

#include "EventSum.hpp"

//---------------------------------------------------------
//	Top_100_TTime_Report
//---------------------------------------------------------

void EventSum::Top_100_TTime_Report (void)
{
	int i, diff;
	char base_start [20], base_end [20], base_time [20], started [20], ended [20], ttime [20];
	char difference [20];

	Event_Data *event_ptr;

	typedef struct {
		int diff;
		int index;
	} Top_100;

	Top_100 top_100 [100], *top_ptr;

	Show_Message ("Top 100 Travel Time Differences -- Record");
	Set_Progress (1000);

	memset (top_100, '\0', 100 * sizeof (Top_100));

	//---- find top 100 travel time differences ----

	for (event_ptr = event_data.First (); event_ptr; event_ptr = event_data.Next ()) {
		Show_Progress ();

		//---- check the time range ----

		if (event_ptr->Base_Start () >= event_ptr->Base_End () ||
			event_ptr->Started () >= event_ptr->Ended ()) continue;

		if (!time_periods.Span_Range (event_ptr->Base_Start (), event_ptr->Base_End ()) &&
			!time_periods.Span_Range (event_ptr->Started (), event_ptr->Ended ())) continue;

		diff = abs (event_ptr->Base_Time () - event_ptr->Travel_Time ());

		top_ptr = top_100;

		for (i=0; i < 100; i++, top_ptr++) {
			if (diff > top_ptr->diff) {
				if (top_ptr->diff > 0 && i < 99) {
					memmove (top_ptr+1, top_ptr, (99-i) * sizeof (Top_100));
				}
				top_ptr->diff = diff;
				top_ptr->index = event_data.Record_Index ();
				break;
			}
		}
	}
	End_Progress ();

	//---- print the report ----

	top_ptr = top_100;

	Header_Number (TOP_100_TTIME);

	if (!Break_Check (100 + 7)) {
		Print (1);
		Top_100_TTime_Header ();
	}		

	for (i=0; i < 100; i++, top_ptr++) {
		if (top_ptr->diff == 0) break;
		
		event_ptr = event_data [top_ptr->index];

		diff = event_ptr->Travel_Time () - event_ptr->Base_Time ();

		//---- print the data record ----

		time_periods.Format_Step (event_ptr->Base_Start (), base_start);
		time_periods.Format_Step (event_ptr->Base_End (), base_end);
		time_periods.Format_Duration (event_ptr->Base_Time (), base_time);
		time_periods.Format_Step (event_ptr->Started (), started);
		time_periods.Format_Step (event_ptr->Ended (), ended);
		time_periods.Format_Duration (event_ptr->Travel_Time (), ttime);
		time_periods.Format_Duration (diff, difference);

		Print (1, "%9d %4d %5d %9.9s%9.9s%9.9s %9.9s%9.9s%9.9s %9.9s", 
			event_ptr->Household (), event_ptr->Person (), event_ptr->Trip (),
			base_start, base_end, base_time, started, ended, ttime, difference);
	}
	if (i) {
		Print (2, "Number of Records in the Report = %d", i);
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Top_100_TTime_Header
//---------------------------------------------------------

void EventSum::Top_100_TTime_Header (void)
{
	Print (1, "Top 100 Travel Time Differences");
	Print (2, "%22c-------- Schedule --------  --------- Actual ---------", BLANK);
	Print (1, "Household Person Trip    Start     End  Duration     Start     End  Duration Difference");
	Print (1);
}

/*********************************************|***********************************************

	Top 100 Travel Time Differences

	                      -------- Schedule --------  --------- Actual ----------
	Household Person Trip    Start     End  Duration     Start     End  Duration Difference

	ddddddddd  ddd   ddd  dd:dd:dd dd:dd:dd dd:dd:dd  dd:dd:dd dd:dd:dd dd:dd:dd  dd:dd:dd

	Number of Records in the Report = dddddd

**********************************************|***********************************************/ 
