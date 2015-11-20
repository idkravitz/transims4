//*********************************************************
//	Time_Report.cpp - Print the Problem Time Summary
//*********************************************************

#include "ProblemSum.hpp"

//---------------------------------------------------------
//	Time_Report
//---------------------------------------------------------

void ProblemSum::Time_Report (void)
{
	int i, j, time, type, num, total, num_inc, *num_time;
	int type_field, time_field, start_field;
	char buffer [FIELD_BUFFER];

	//---- summarize the problem times ----
	
	Header_Number (TIME_REPORT);

	type_field = problem_db.Required_Field ("PROBLEM", "STATUS");
	time_field = problem_db.Optional_Field ("TIME", "TOD");
	start_field = problem_db.Optional_Field ("START", "START_TIME");

	num_inc = times.Num_Ranges ();
	num_time = new int [num_inc];

	i = ((num_types > 1) ? 0 : 1);

	for (; i < MAX_PROBLEM; i++) {
		if (num_problems [i] == 0) continue;

		num = 0;
		memset (num_time, '\0', num_inc * sizeof (int));
		header_code = i;

		problem_db.Rewind ();

		while (problem_db.Read_Record ()) {
			if (i > 0) {
				problem_db.Get_Field (type_field, &type);
				if (i != type) continue;
			}
			if (time_field > 0) {
				problem_db.Get_Field (time_field, buffer);

				time = times.Step (buffer);
			} else {
				time = 0;
			}
			if (time == 0) {
				problem_db.Get_Field (start_field, buffer);

				time = times.Step (buffer);
			}
			time = times.In_Increment (time) - 1;

			if (num_time [time] == 0) num++;
			num_time [time]++;
		}
		if (num == 0) continue;

		//---- print the report ----

		if (!Break_Check (num + 6)) {
			Print (1);
			Time_Header ();
		}
		total = num_problems [i];

		for (j=0; j < num_inc; j++) {
			if (num_time [j] == 0) continue;

			Print (1, "%-24.24s   %10d   %6.2lf", times.Range_Format (j+1),
				num_time [j], ((100.0 * num_time [j]) / total));
		}
		Print (2, "Total                      %10d   100.00", total);
	}
	Header_Number (0);

	delete [] num_time;
}

//---------------------------------------------------------
//	Time_Header
//---------------------------------------------------------

void ProblemSum::Time_Header (void)
{
	Print (1, "Time of Day Summary for %s Problems", Problem_Code ((Problem_Type) header_code));
	Print (2, "Time of Day                     Count   Percent");
	Print (1);
}
	 
/*********************************************|***********************************************

	Time or Day Summary for ssssssssss Problems

	Time of Day                     Count   Percent

	ssssssssssssssssssssssss   dddddddddd   ddd.dd

	Total                      dddddddddd   ddd.dd

**********************************************|***********************************************/ 
