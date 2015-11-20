//*********************************************************
//	Read_Input.cpp - read the input data
//*********************************************************

#include "SmoothData.hpp"

//---------------------------------------------------------
//	Read_Input
//---------------------------------------------------------

void SmoothData::Read_Input (void)
{
	int start, end, t, t1;
	double factor, total, share;
	bool flag, active;
	char *str_ptr;

	Diurnal_File *input_file, *initial_file;

	Time_Step initial_time (1, "HOURS");

	Diurnal_Data *diurnal_ptr;

	input_file = group_ptr->Input_File ();
	initial_file = group_ptr->Initial_File ();

	//---- initialize the distribution ----

	if (initial_file != NULL) {
		while (initial_file->Read ()) {
			Show_Progress ();

			str_ptr = initial_file->Start_Time ();
			if (*str_ptr == '\0') continue;
			num_ini++;

			start = initial_time.Step (str_ptr);
			end = initial_time.Step (initial_file->End_Time ());

			if (start >= end) goto time_error;

			//---- read the field ----

			share = initial_file->Share ();

			if (group_ptr->Increment () > 0) {
				total = (end - start) / group_ptr->Increment ();
				if (total < 1.0) total = 1.0;
				share = share / total;
			}

			for (t = start; t < end; t = t1) {
				if (group_ptr->Increment () > 0) {
					t1 = t + group_ptr->Increment ();
				} else {
					t1 = end;
				}
				diurnal_ptr = diurnal_data.New_Record (true);
				if (diurnal_ptr == NULL) goto diurnal_error;

				diurnal_ptr->Start_Time (t);
				diurnal_ptr->End_Time (t1);

				//---- get the share data ----

				if (!diurnal_ptr->Num_Shares (3)) goto diurnal_error;

				diurnal_ptr->Share (3, share);

				if (!diurnal_data.Add ()) goto diurnal_error;
			}
		}
	}

	//---- read the input file ----

	active = false;

	while (input_file->Read ()) {
		Show_Progress ();

		str_ptr = input_file->Start_Time ();
		if (*str_ptr == '\0') continue;
		num_in++;

		start = group_ptr->Input_Time (str_ptr);
		end = group_ptr->Input_Time (input_file->End_Time ());

		if (start >= end) goto time_error;

		//---- check for a new record group ----

		if (start == 0) {
			if (active) {
				Write_Output ();

				if (initial_file == NULL) {
					diurnal_data.Reset ();
				}
			}
			active = true;

			//---- copy replicated fields ----

			if (group_ptr->Replicate_Flag ()) {
				group_ptr->Output_File ()->Copy_Fields (input_file);
			}
		}

		//---- read the field ----

		share = input_file->Share ();

		if (initial_file == NULL) {

			//---- distribute equal shares ----

			if (group_ptr->Increment () > 0) {
				total = (end - start) / group_ptr->Increment ();
				if (total < 1.0) total = 1.0;
				share = share / total;
			}

			for (t = start; t < end; t = t1) {
				if (group_ptr->Increment () > 0) {
					t1 = t + group_ptr->Increment ();
				} else {
					t1 = end;
				}
				diurnal_ptr = diurnal_data.New_Record (true);
				if (diurnal_ptr == NULL) goto diurnal_error;

				diurnal_ptr->Start_Time (t);
				diurnal_ptr->End_Time (t1);

				//---- get the share data ----

				if (!diurnal_ptr->Num_Shares (2)) goto diurnal_error;

				diurnal_ptr->Share (1, share);

				if (!diurnal_data.Add ()) goto diurnal_error;
			}

		} else {

			//---- factor the initial distribution -----

			flag = false;
			t = t1 = 0;
			total = 0.0;

			for (diurnal_ptr = diurnal_data.First (); diurnal_ptr; diurnal_ptr = diurnal_data.Next ()) {
				if (!flag) {
					if (diurnal_ptr->Start_Time () <= start && diurnal_ptr->End_Time () > start) {
						t = diurnal_data.Record_Index ();
						flag = true;
					}
				}
				if (flag) {
					total += diurnal_ptr->Share (3);
				}
				if (diurnal_ptr->Start_Time () < end && diurnal_ptr->End_Time () >= end) {
					t1 = diurnal_data.Record_Index ();
					break;
				}
			}
			if (!flag) continue;

			//---- calculate the adjustment factor ----

			if (total > 0.0) {
				factor = share / total;
			} else {
				share = share / (t1 - t + 1);
				flag = false;
			}

			//---- adjust the distribution ----

			for (; t <= t1; t++) {
				diurnal_ptr = diurnal_data [t];

				if (flag) {
					share = diurnal_ptr->Share (3) * factor;
				}
				diurnal_ptr->Share (1, share);
			}
		}
	}

	//---- write the last record ----

	if (active) Write_Output ();

	return;

time_error:
	Error ("Time Range is Inappropriate (%d >= %d)", start, end);

diurnal_error:
	Error ("Adding Time Point to Diurnal Distribution");
}
