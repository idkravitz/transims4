//*********************************************************
//	Write_Trip_Times.cpp - write trip time file
//*********************************************************

#include "PlanCompare.hpp"

//---------------------------------------------------------
//	Write_Trip_Times
//---------------------------------------------------------

void PlanCompare::Write_Trip_Times (void)
{
	int i, num, num_out, start_diff, end_diff, base;

	Trip_Time *time_ptr;

	FILE *file;

	Show_Message (2, "Writing %s -- Record", time_file.File_Type ());
	Set_Progress ();

	file = time_file.File ();

	//---- print each record ----

	num = MIDNIGHT / 900;
	num_out = 0;

	for (i=1, time_ptr = trip_time; i <= num; i++, time_ptr++) {
		Show_Progress ();

		//---- average start time differences ----

		base = time_ptr->compare_start;
		if (base <= 0) base = 1;

		if (time_ptr->start_diff >= 0) {
			start_diff = (time_ptr->start_diff + (base >> 1)) / base;
		} else {
			start_diff = -((-time_ptr->start_diff + (base >> 1)) / base);
		}

		//---- average end time differences ----

		base = time_ptr->compare_end;
		if (base <= 0) base = 1;

		if (time_ptr->end_diff >= 0) {
			end_diff = (time_ptr->end_diff + (base >> 1)) / base;
		} else {
			end_diff = -((-time_ptr->end_diff + (base >> 1)) / base);
		}
		fprintf (file, "%d:%02d\t%d\t%d\t%d\t%d\t%d\t%d\n", (i / 4), (i % 4) * 15, 
			time_ptr->input_start, time_ptr->compare_start, start_diff,
			time_ptr->input_end, time_ptr->compare_end, end_diff);

		num_out++;
	}
	End_Progress ();

	time_file.Close ();

	Print (2, "Number of %s Records = %d", time_file.File_Type (), num_out);
}
