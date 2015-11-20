//*********************************************************
//	Problem_Time.cpp - write the problem time file
//*********************************************************

#include "ProblemSum.hpp"

//---------------------------------------------------------
//	Problem_Time
//---------------------------------------------------------

void ProblemSum::Problem_Time (void)
{
	int i, j, time, type, num, total, num_inc, *num_time, nout;
	int type_field, time_field, start_field;
	char buffer [FIELD_BUFFER];

	Range_Data *range_ptr;

	//---- summarize the problem times ----

	type_field = problem_db.Required_Field ("PROBLEM", "STATUS");
	time_field = problem_db.Optional_Field ("TIME", "TOD");
	start_field = problem_db.Optional_Field ("START", "START_TIME");
	
	Show_Message ("Writing %s -- Record", time_file.File_Type ());
	Set_Progress (10000);
	nout = 0;

	num_inc = times.Num_Ranges ();
	num_time = new int [num_inc];

	i = ((num_types > 1) ? 0 : 1);

	for (; i < MAX_PROBLEM; i++) {
		if (num_problems [i] == 0) continue;

		num = 0;
		memset (num_time, '\0', num_inc * sizeof (int));

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

		total = num_problems [i];

		for (j=0; j < num_inc; j++) {
			Show_Progress ();

			range_ptr = times [j+1];

			time_file.Put_Field (1, i);
			time_file.Put_Field (2, times.Format_Step (range_ptr->Low ()));
			time_file.Put_Field (3, times.Format_Step (range_ptr->High () + 1));
			time_file.Put_Field (4, num_time [j]);
			time_file.Put_Field (5, ((100.0 * num_time [j]) / total));

			if (!time_file.Write ()) {
				Error ("Writing %s", time_file.File_Type ());
			}
			nout++;
		}
	}
	End_Progress ();

	time_file.Close ();

	delete [] num_time;

	Print (2, "Number of %s Records = %d", time_file.File_Type (), nout);
}
