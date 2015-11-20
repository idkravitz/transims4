//*********************************************************
//	Write_Times.cpp - write the time summary file
//*********************************************************

#include "EventSum.hpp"

//---------------------------------------------------------
//	Write_Times
//---------------------------------------------------------

void EventSum::Write_Times (void)
{
	int i, trips, ttime, diff, error, num_out, num, average;
	char *ptr1, *ptr2, start [16], end [16];

	Event_Time *time_ptr, total;

	Show_Message ("Writing %s -- Period", time_sum_file.File_Type ());
	Set_Progress ();

	num_out = 0;
	num = time_periods.Num_Ranges () + 1;
	ptr1 = ptr2 = "total";

	for (i=1; i <= num; i++) {
		Show_Progress ();

		if (i < num) {
			time_ptr = event_time [i];
			if (time_ptr == NULL) continue;

			total.Trip_Start (total.Trip_Start () + time_ptr->Trip_Start ());
			total.Started (total.Started () + time_ptr->Started ());
			total.Trip_End (total.Trip_End () + time_ptr->Trip_End ());
			total.Ended (total.Ended () + time_ptr->Ended ());
			total.Mid_Trip (total.Mid_Trip () + time_ptr->Mid_Trip ());
			total.Start_Diff (total.Start_Diff () + time_ptr->Start_Diff ());
			total.End_Diff (total.End_Diff () + time_ptr->End_Diff ());
			total.Travel_Time (total.Travel_Time () + time_ptr->Travel_Time ());
			total.Start_Error (total.Start_Error () + time_ptr->Start_Error ());
			total.End_Error (total.End_Error () + time_ptr->End_Error ());
			total.TTime_Diff (total.TTime_Diff () + time_ptr->TTime_Diff ());
			total.TTime_Error (total.TTime_Error () + time_ptr->TTime_Error ());
			total.Sum_Trips (total.Sum_Trips () + time_ptr->Sum_Trips ());
			total.Sum_Error (total.Sum_Error () + time_ptr->Sum_Error ());

			ptr1 = time_periods.Range_Format (i);
			ptr2 = strstr (ptr1, "..");
			*ptr2 = '\0';
			ptr2 += 2;

			if (i == 1) str_cpy (start, sizeof (start), ptr1);
			str_cpy (end, sizeof (end), ptr2);
		} else {
			time_ptr = &total;
			ptr1 = start;
			ptr2 = end;
		}

		time_sum_file.From_Time (ptr1);
		time_sum_file.To_Time (ptr2);

		time_sum_file.Trip_Start (time_ptr->Trip_Start ());

		trips = time_ptr->Started ();

		if (trips > 0) {
			diff = (int) (time_ptr->Start_Diff () / trips + 0.5);
			error = (int) (time_ptr->Start_Error () / trips + 0.5);
		} else {
			diff = error = 0;
		}
		time_sum_file.Started (trips);
		time_sum_file.Start_Diff (time_periods.Format_Duration (diff));
		time_sum_file.Start_Error (time_periods.Format_Duration (error));

		time_sum_file.Trip_End (time_ptr->Trip_End ());

		trips = time_ptr->Ended ();

		if (trips > 0) {
			diff = (int) (time_ptr->End_Diff () / trips + 0.5);
			error = (int) (time_ptr->End_Error () / trips + 0.5);
		} else {
			diff = error = 0;
		}
		time_sum_file.Ended (trips);
		time_sum_file.End_Diff (time_periods.Format_Duration (diff));
		time_sum_file.End_Error (time_periods.Format_Duration (error));

		trips = time_ptr->Mid_Trip ();

		if (trips > 0) {
			ttime = (int) (time_ptr->Travel_Time () / trips + 0.5);
			diff = (int) (time_ptr->TTime_Diff () / trips + 0.5);
			error = (int) (time_ptr->TTime_Error () / trips + 0.5);
			average = (int) (time_ptr->Sum_Error () / trips + 0.5);
		} else {
			ttime = diff = error = average = 0;
		}
		time_sum_file.Mid_Trip (trips);
		time_sum_file.Travel_Time (time_periods.Format_Duration (ttime));
		time_sum_file.TTime_Diff (time_periods.Format_Duration (diff));
		time_sum_file.TTime_Error (time_periods.Format_Duration (error));

		trips = time_ptr->Sum_Trips ();

		if (trips > 0) {
			error = (int) (time_ptr->Sum_Error () / trips + 0.5);
		} else {
			error = 0;
		}
		time_sum_file.Sum_Trips (trips);
		time_sum_file.Sum_Error (time_periods.Format_Duration (error));
		time_sum_file.Avg_Error (time_periods.Format_Duration (average));

		if (!time_sum_file.Write ()) {
			Error ("Writing %s", time_sum_file.File_Type ());
		}
		num_out++;
	}
	End_Progress ();

	time_sum_file.Close ();
	Print (2, "Number of %s Records = %d", time_sum_file.File_Type (), num_out);
}
